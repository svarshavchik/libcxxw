/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "booklayoutmanager/booklayoutmanager_impl.H"
#include "booklayoutmanager/bookgridlayoutmanager.H"
#include "booklayoutmanager/pagetabgridcontainer_impl.H"
#include "booklayoutmanager/pagetabgridlayoutmanager_impl.H"
#include "booklayoutmanager/pagetab_impl.H"
#include "booklayoutmanager/pagetabsingletonlayoutmanager_implobj.H"
#include "peephole/peephole.H"
#include "peephole/peephole_impl_element.H"
#include "peephole/peepholed_element.H"
#include "x/w/peephole_style.H"
#include "peephole/peephole_layoutmanager_impl.H"
#include "x/w/impl/layoutmanager.H"
#include "image_button.H"
#include "image_button_internal_impl.H"
#include "x/w/impl/icon.H"
#include "x/w/impl/container_element.H"
#include "x/w/impl/container_visible_element.H"
#include "x/w/impl/child_element.H"
#include "x/w/impl/always_visible.H"
#include "x/w/impl/bordercontainer_element.H"
#include "x/w/impl/borderlayoutmanager.H"
#include "x/w/impl/richtext/richtext.H"
#include "capturefactory.H"
#include "busy.H"
#include "run_as.H"
#include "catch_exceptions.H"
#include "messages.H"
#include "generic_window_handler.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/pagelayoutmanager.H"
#include "x/w/pagefactory.H"
#include "x/w/container.H"
#include "x/w/element.H"
#include "x/w/book_appearance.H"
#include "x/w/image_button_appearance.H"
#include "x/w/bookpagefactory.H"
#include "x/w/canvas.H"
#include "x/w/callback_trigger.H"

#include <x/weakcapture.H>

#include <vector>

LIBCXXW_NAMESPACE_START

// The layoutmanagerObj base class gets a ref to the real layout manager
// implementation object, and we store our impl.

booklayoutmanagerObj
::booklayoutmanagerObj(const ref<implObj> &impl)
: layoutmanagerObj{impl->impl},
  book_pagelayoutmanager{impl->book_pagelayoutmanager()},
  book_pagetabgridlayoutmanager{impl->book_pagetabgridlayoutmanager()},
  impl{impl}
{
	book_pagelayoutmanager->notmodified();
	book_pagetabgridlayoutmanager->notmodified();
}

booklayoutmanagerObj::~booklayoutmanagerObj()=default;

// The internal pagelayoutmanager is the authoritative source for our pages.

size_t booklayoutmanagerObj::pages() const
{
	notmodified();
	return book_pagelayoutmanager->pages();
}

std::optional<size_t> booklayoutmanagerObj::opened() const
{
	notmodified();
	return book_pagelayoutmanager->opened();
}

void booklayoutmanagerObj
::on_opened(const functionref<void (THREAD_CALLBACK,
				    const book_status_info_t &)> &cb)
{
	notmodified();
	impl->impl->layout_container_impl->container_element_impl()
		.get_window_handler().thread()
		->run_as([me_impl=impl, cb]
			 (ONLY IN_THREAD)
			 {
				 auto me=booklayoutmanager::create(me_impl);

				 book_lock lock{me};

				 lock.layout_manager->impl->impl
					 ->callback(lock)=cb;

				 auto opened=me->book_pagelayoutmanager
					 ->opened();

				 if (!opened)
					 return;

				 lock.layout_manager->impl->impl
					 ->invoke_callback
					 (IN_THREAD, lock, *opened, initial{});
			 });
}

void booklayoutmanagerObj::open(size_t n)
{
	notmodified();
	impl->impl->run_as([me_impl=impl, n]
			   (ONLY IN_THREAD)
			   {
				   auto me=booklayoutmanager::create(me_impl);

				   me->open(IN_THREAD, n);
			   });
}

void booklayoutmanagerObj::open(ONLY IN_THREAD, size_t n)
{
	open(IN_THREAD, n, {});
}

void booklayoutmanagerObj::open(ONLY IN_THREAD, size_t n,
				const callback_trigger_t &trigger)
{
	notmodified();
	book_pagelayoutmanager->notmodified();
	book_pagetabgridlayoutmanager->notmodified();

	book_lock lock{ref{this}};

	// Remember which page is currently open.

	auto opened=book_pagelayoutmanager->opened();

	pagetabptr previous_page;

	if (opened)
		previous_page=impl->get_pagetab(lock, *opened);
	pagetab next_page=impl->get_pagetab(lock, n);

	book_pagelayoutmanager->open(n);

	// Make sure to scroll the opened tab into the view
	// (the center of the peephole), unless it was a
	// button click. Yanking the tab into the center
	// of the peephole, just after clicking on it,
	// is rather rude.
	auto ensure_visibility=trigger.index() ==
		callback_trigger_button_event ? false:true;

	if (previous_page)
	{
		previous_page->impl->initialize_if_needed(IN_THREAD);
		previous_page->impl->set_active(IN_THREAD, false);
	}
	next_page->impl->set_active(IN_THREAD, true);

	if (ensure_visibility)
	{
		next_page->elementObj::impl
			->ensure_entire_visibility(IN_THREAD);
	}

	lock.layout_manager->impl->impl->invoke_callback
		(IN_THREAD, lock, n, trigger);
}

void booklayoutmanagerObj::close()
{
	notmodified();
	impl->impl->run_as([me_impl=impl]
			   (ONLY IN_THREAD)
			   {
				   auto me=booklayoutmanager::create(me_impl);

				   me->close(IN_THREAD);
			   });
}

void booklayoutmanagerObj::close(ONLY IN_THREAD)
{
	notmodified();
	book_pagelayoutmanager->notmodified();
	book_pagetabgridlayoutmanager->notmodified();
	book_lock lock{ ref{this} };

	auto opened=book_pagelayoutmanager->opened();

	pagetabptr previous_page;

	if (opened)
		previous_page=impl->get_pagetab(lock, *opened);

	book_pagelayoutmanager->close();

	// We can officially close the book immediately, but we
	// have to punt the tab visual update to the connection thread.

	if (!previous_page)
		return;

	previous_page->impl->set_active(IN_THREAD, false);
}

element booklayoutmanagerObj::get_page(size_t n) const
{
	notmodified();
	return book_pagelayoutmanager->get(n);
}

//////////////////////////////////////////////////////////////////////////////
//
// Logic shared by both the append and the insert factories.

class LIBCXX_HIDDEN bookpagefactory_implObj : public bookpagefactoryObj {

 protected:

	// These are accessed only while holding a lock

	const booklayoutmanager layout_manager;
	const pagefactory book_pagefactory;

	// The tab factory

	const gridfactory book_tabfactory;

 public:

	bookpagefactory_implObj(const booklayoutmanager &layout_manager,
				const pagefactory &book_pagefactory,
				const gridfactory &book_tabfactory)
		: layout_manager{layout_manager},
		book_pagefactory{book_pagefactory},
		book_tabfactory{book_tabfactory}
	{
	}

	~bookpagefactory_implObj()=default;

	bookpagefactoryObj &halign(LIBCXXW_NAMESPACE::halign a) override
	{
		book_pagefactory->halign(a);
		return *this;
	}

	bookpagefactoryObj &valign(LIBCXXW_NAMESPACE::valign a) override
	{
		book_pagefactory->valign(a);
		return *this;
	}
};


// Factored out for readability.

// callback that gets executed when a tab is selected by pointer or keyboard.

static inline void tab_activate(ONLY IN_THREAD,
				const booklayoutmanager &layout_manager,
				const element &activated_page,
				const callback_trigger_t &trigger)
{
	book_lock lock{layout_manager};

	auto index=layout_manager->book_pagelayoutmanager
		->lookup(activated_page);

	if (!index)
		return;

	layout_manager->open(IN_THREAD, *index, trigger);
}

// Helper for populating a new tab's structure.
//
// The caller provides its factory, our layout manager, and the tab_factory
// parameter.
//
// The caller is responsible for invoking gridfactory->created_internally()
// for the returned element.

static auto create_new_tab(const bookpagefactoryObj &my_factory,
			   const gridfactory &gridfactory,
			   const booklayoutmanager &layout_manager,
			   const pagefactory &book_pagefactory,
			   const function<void (const factory &)> &tab_factory,
			   const function<void (const factory &)> &page_factory,
			   const create_bookpage_args_t &args)
{
	// All elements in the tab strip have no padding, and take up the
	// full height of the tab strip.
	gridfactory->padding(0);
	gridfactory->valign(valign::fill);

	// First-ly, an inner container with the border layout manager, for this
	// new tab's individual borders.

	auto &book_tab_border=
		layout_manager->impl->impl->appearance->tab_border;

	auto tab_parent_container=gridfactory->get_container_impl();

	auto inner_tab_gridcontainer_impl=
		ref<always_visibleObj<container_visible_elementObj<
			bordercontainer_elementObj<
				container_elementObj<child_elementObj>>>>>
		::create(tab_parent_container->get_window_handler(),
			 book_tab_border,
			 book_tab_border,
			 book_tab_border,
			 border_infomm{},
			 richtextptr{},
			 0, 0, 0,
			 tab_parent_container);

	// Create the implementation object for the pagetab.

	auto impl=ref<pagetabObj::implObj>
		::create(inner_tab_gridcontainer_impl,
			 layout_manager->book_pagetabgridlayoutmanager
			 ->impl->my_container,
			 my_factory.appearance);

	// Finish initializing the impl in the connection thread.
	// Need to install the shortcut.
	std::optional<shortcut> default_shortcut;

	const auto &sc=optional_arg_or<shortcut>(args, default_shortcut);
	impl->set_shortcut(sc);

	// Obtain the initial contents of the actual tab label.
	auto tab_capture_factory=capturefactory::create(impl);
	auto page_capture_factory=
		capturefactory::create(book_pagefactory
				       ->get_container_impl());
	tab_factory(tab_capture_factory);
	page_factory(page_capture_factory);

	auto page_element=page_capture_factory->get();

	// And now we can construct the singleton layout manager for the
	// pagetab implementation object.

	auto tab_element=tab_capture_factory->get();
	// tab_element->show_all();

	auto lm=ref<pagetabsingletonlayoutmanager_implObj>
		::create(impl, tab_element);

	auto new_page=pagetab::create(impl, lm);

	// Now we can create the border layout manager for the new page tab.

	auto inner_tab_lm=ref<borderlayoutmanagerObj::implObj>::create
		(inner_tab_gridcontainer_impl,
		 inner_tab_gridcontainer_impl,
		 std::nullopt,
		 new_page,
		 halign::fill,
		 valign::bottom);

	// And the inner container itself.
	auto inner_tab_container=container::create(inner_tab_gridcontainer_impl,
						   inner_tab_lm);

	layout_manager->set_modified();
	return std::tuple{inner_tab_container, new_page, page_element};
}

// Install a callback when the tab hotspot gets activated.
//
// Weakly capture the entire container with the book layout manager,
// and the newly-added tab element.


static
void install_activate_callback(const booklayoutmanager &layout_manager,
			       const pagetab &new_hotspot,
			       const element &new_page)
{
	new_hotspot->on_activate
		([weak_captures=make_weak_capture(layout_manager->impl
						  ->impl->layout_container_impl,
						  new_page)]
		 (ONLY IN_THREAD, const auto &trigger, const auto &busy)
		 {
			 // Recover the weak captures, and invoke
			 // tab_activated().

			 auto got=weak_captures.get();

			 if (!got)
				 return;

			 auto &[container_impl, new_page]=*got;

			 container_impl->invoke_layoutmanager
				 ([&]
				  (const auto &lm_impl)
				  {
					  tab_activate
						  (IN_THREAD, lm_impl
						   ->create_public_object(),
						   new_page,
						   trigger);
				  });
		 });
}

/////////////////////////////////////////////////////////////////////////////
//
// The append factory.

class LIBCXX_HIDDEN append_bookpagefactoryObj
	: public bookpagefactory_implObj {


 public:

	// The constructor prepares the two internal append factories for
	// the internal pagelayoutmanager, and the tab grid layoutmanager.

	append_bookpagefactoryObj(const booklayoutmanager &layout_manager)
		: bookpagefactory_implObj{
		layout_manager,
			layout_manager->book_pagelayoutmanager->append(),
			layout_manager->book_pagetabgridlayoutmanager
			->append_columns(0)}
	{
	}

	// Destructor
	~append_bookpagefactoryObj()=default;

	//! Implement add()
	void do_add(const function<void (const factory &)> &,
		    const function<void (const factory &)> &,
		    const create_bookpage_args_t &) override;
};

// The insert factory.

class LIBCXX_HIDDEN insert_bookpagefactoryObj
	: public bookpagefactory_implObj {

	size_t page_number;

 public:

	// The constructor prepares the two internal insert factories for
	// the internal pagelayoutmanager, and the tab grid layoutmanager.
	insert_bookpagefactoryObj(const booklayoutmanager &layout_manager,
				  size_t page_number)
		: bookpagefactory_implObj{layout_manager,
			layout_manager->book_pagelayoutmanager
			->insert(page_number),
			layout_manager->book_pagetabgridlayoutmanager
			->insert_columns(0, page_number)},
		page_number{page_number}
	{
	}

	// Destructor
	~insert_bookpagefactoryObj()=default;

	//! Implement add().
	void do_add(const function<void (const factory &)> &,
		    const function<void (const factory &)> &,
		    const create_bookpage_args_t &) override;
};

bookpagefactory booklayoutmanagerObj::append()
{
	return ref<append_bookpagefactoryObj>::create(ref(this));
}

void append_bookpagefactoryObj
::do_add(const function<void (const factory &)> &tab_factory,
	 const function<void (const factory &)> &page_factory,
	 const create_bookpage_args_t &args)
{
	book_lock lock{layout_manager};

	auto [tab_element, hotspot_element, page_element]=
		create_new_tab(*this, book_tabfactory,
			       layout_manager, book_pagefactory,
			       tab_factory, page_factory, args);

	install_activate_callback(layout_manager, hotspot_element,
				  page_element);
	book_tabfactory->created_internally(tab_element);
	book_pagefactory->created_internally(page_element);
	layout_manager->impl->rebuild_focusables();
}

bookpagefactory booklayoutmanagerObj::insert(size_t page_number)
{
	return ref<insert_bookpagefactoryObj>::create(ref(this), page_number);
}

void booklayoutmanagerObj::remove(size_t page_number)
{
	book_lock lock{ref{this}};

	if (page_number >= pages())
		throw EXCEPTION(gettextmsg(_("Page %1% does not exist"),
					   page_number));
	book_pagetabgridlayoutmanager->remove(0, page_number);
	book_pagelayoutmanager->remove(page_number);
}

void insert_bookpagefactoryObj
::do_add(const function<void (const factory &)> &tab_factory,
	 const function<void (const factory &)> &page_factory,
	 const create_bookpage_args_t &args)
{
	book_lock lock{layout_manager};

	auto [tab_element, hotspot_element, page_element]=
		create_new_tab(*this, book_tabfactory,
			       layout_manager, book_pagefactory,
			       tab_factory, page_factory, args);

	install_activate_callback(layout_manager, hotspot_element,
				  page_element);
	book_tabfactory->created_internally(tab_element);
	book_pagefactory->created_internally(page_element);

	++page_number;
	layout_manager->impl->rebuild_focusables();
}

//////////////////////////////////////////////////////////////////////////////
//
// Creating a new book. Need to create some glue classes for that.

// First, the page tab strip which goes into a peephole, and thusly it must
// be a peepholed_elementObj. Otherwise, it's an ordinary container.

class LIBCXX_HIDDEN pagetabpeepholed_containerObj
	: public peepholed_elementObj<containerObj> {

	typedef peepholed_elementObj<containerObj> superclass_t;

 public:
	using superclass_t::superclass_t;

	~pagetabpeepholed_containerObj()=default;

	// There are no scrollbars, but we need to define these anyway.

	dim_t horizontal_increment(ONLY IN_THREAD) const override { return 0; }
	dim_t vertical_increment(ONLY IN_THREAD) const override { return 0; }

	size_t peepholed_rows(ONLY IN_THREAD) const override
	{
		return 0;
	}
};

// And we need to implement our focusable_container.

class LIBCXX_HIDDEN book_focusable_containerObj
	: public focusable_containerObj {

 public:
	using focusable_containerObj::focusable_containerObj;

	~book_focusable_containerObj()=default;

	focusable_impl get_impl() const override
	{
		ref<bookgridlayoutmanagerObj> bglm=
			get_layout_impl();

		auto blm_impl=ref<booklayoutmanagerObj::implObj>::create(bglm);

		return blm_impl->get_focusable()->get_impl();
	}

	void do_get_impl(const function<internal_focusable_cb> &cb) const
		override
	{
		ref<bookgridlayoutmanagerObj> bglm=
			get_layout_impl();

		auto blm_impl=ref<booklayoutmanagerObj::implObj>::create(bglm);

		auto focusables=blm_impl->get_focusables();

		process_focusable_impls_from_focusables(cb, focusables);
	}
};

new_booklayoutmanager::new_booklayoutmanager()
	: appearance{book_appearance::base::theme()}
{
}

new_booklayoutmanager::~new_booklayoutmanager()=default;
new_booklayoutmanager::new_booklayoutmanager(const new_booklayoutmanager &)
=default;

new_booklayoutmanager &new_booklayoutmanager
::operator=(const new_booklayoutmanager &)=default;

focusable_container
new_booklayoutmanager::create(const container_impl &parent,
			      const function<void
			      (const focusable_container &)>
			      &creator) const
{
	// Our container implementation is nothing special.

	container_impl c=
		ref<container_elementObj<child_elementObj>>::create(parent);

	// Create the our gridlayoutmanager subclass, the real layout manager
	// for this container.

	auto grid=ref<bookgridlayoutmanagerObj>::create(c, appearance);

	// Start building the contents of the grid.

	auto gridlm=grid->create_gridlayoutmanager();

	// Any extra space beyond what we requested will be assigned to
	// column #1, which is the tab peephole, and row #1, which is the
	// page.
	gridlm->requested_col_width(1, 100);
	gridlm->requested_row_height(1, 100);

	// Left scroll image button.

	gridlm->row_alignment(0, valign::bottom);

	auto factory=gridlm->append_row();
	factory->padding(0);

	// We will not use image button callbacks, instead we'll hook into
	// the activation callbacks.

	ptr<image_button_internalObj::implObj> left_scroll_impl,
		right_scroll_impl;

	auto button_appearance=appearance->left_scroll_button;

	create_image_button_info scroll_button_info
		{factory->get_container_impl(), true, button_appearance, true};

	auto left_scroll=create_image_button
		(scroll_button_info,
		 [&]
		 (const auto &container_impl)
		 {
			 // Use our scalable icons, and make them
			 // book_scroll_height tall.

			 auto impl=scroll_imagebutton_specific_height
				 (container_impl,
				  button_appearance->images,
				  appearance->scroll_button_height);

			 left_scroll_impl=impl;

			 return impl;
		 },
		 [](const auto &ignore){});

	factory->created_internally(left_scroll);

	////////////////////////////////////////////////////////////////////
	//
	// The page tab element is, first and foremost, a peephole.

	factory->padding(0).halign(halign::fill).border(appearance->border);

	child_element_init_params peephole_impl_init_params;

	peephole_impl_init_params.background_color=
		appearance->tabs_background_color;

	auto peephole_impl=
		ref<always_visibleObj<peephole_impl_elementObj<
			container_elementObj<child_elementObj>>>>
		::create(factory->get_container_impl(),
			 peephole_impl_init_params);

	// And inside this peephole is the actual tab.

	auto pagetab_gridcontainer=
		pagetabgridcontainer_impl::create(peephole_impl);

	// And the layout manager for the pagetab_gridcontainer

	auto pagetab_container_impl_lm=
		ref<pagetabgridlayoutmanagerObj::implObj>
		::create(pagetab_gridcontainer, peephole_impl);

	// Create the initial, empty, row 0.
	pagetab_container_impl_lm->create_gridlayoutmanager()->append_row();

	auto pagetab_container=
		ref<pagetabpeepholed_containerObj>::create
		(pagetab_gridcontainer,
		 pagetab_container_impl_lm);

	// We can now create the peephole layoutmanager for the
	// peephole_container_impl;

	peephole_style style{peephole_algorithm::automatic,
			     peephole_algorithm::automatic};

	style.scroll=peephole_scroll::centered;

	auto peephole_lm=
		ref<peepholelayoutmanagerObj::implObj>::create
		(peephole_impl,
		 style,
		 pagetab_container);

	// And the peephole.

	auto my_peephole=peephole::create(peephole_impl, peephole_lm);

	factory->created_internally(my_peephole);

	// It's time for the right scroll button.

	factory->padding(0);

	button_appearance=appearance->right_scroll_button;
	scroll_button_info.parent_container_impl=factory->get_container_impl();
	auto right_scroll=create_image_button
		(scroll_button_info,
		 [&]
		 (const auto &container_impl)
		 {
			 auto impl=scroll_imagebutton_specific_height
				 (container_impl,
				  button_appearance->images,
				  appearance->scroll_button_height);

			 right_scroll_impl=impl;

			 return impl;
		 },
		 [](const auto &ignore){});

	factory->created_internally(right_scroll);

	// It's very nice for an image_button to provide callbacks that
	// report when its image changes. But what we really need to do is
	// install callbacks for its internal hotspot, that gets clicked on.

	left_scroll_impl->on_activate
		([c=make_weak_capture(c)]
		 (ONLY IN_THREAD,
		  const auto &trigger,
		  const auto &mcguffin)
		 {
			 auto got=c.get();

			 if (!got)
				 return;

			 auto &[c]=*got;

			 c->invoke_layoutmanager
				 ([&]
				  (const auto &lm_impl)
				  {
					  booklayoutmanager lm=
						  lm_impl->create_public_object();
					  book_lock lock{lm};

					  auto n=lm->pages();

					  if (n == 0)
						  return;

					  size_t next;

					  auto current=lm->opened();

					  if (current)
						  next=*current;
					  else
						  next=n;

					  if (next == 0)
						  return;

					  // Don't forward the button event.
					  // ensure_visibility won't, for a
					  // button event, and we want to make
					  // it visible.
					  lm->open(IN_THREAD, --next,
						   prev_key{});
				  });
		 });

	right_scroll_impl->on_activate
		([c=make_weak_capture(c)]
		 (ONLY IN_THREAD,
		  const auto &trigger,
		  const auto &mcguffin)
		 {
			 auto got=c.get();

			 if (!got)
				 return;

			 auto &[c]=*got;

			 c->invoke_layoutmanager
				 ([&]
				  (const auto &lm_impl)
				  {
					  booklayoutmanager lm=
						  lm_impl->create_public_object();
					  book_lock lock{lm};

					  auto n=lm->pages();

					  if (n == 0)
						  return;

					  size_t next;

					  auto current=lm->opened();

					  if (current)
						  next=*current+1;
					  else
						  next=0;

					  if (next >= n)
						  return;

					  // Don't forward the button event.
					  // ensure_visibility won't, for a
					  // button event, and we want to make
					  // it visible.
					  lm->open(IN_THREAD, next,
						   next_key{});
				  });
		 });

	// The internal pagelayoutmanager goes into the next row.

	factory=gridlm->append_row();

	child_element_init_params current_page_container_init_params;

	current_page_container_init_params.background_color=
		appearance->background_color;

	auto current_page_container_impl=
		ref<always_visibleObj<container_elementObj<child_elementObj>>>
		::create(c, current_page_container_init_params);

	auto page_lm_impl=new_pagelayoutmanager{}
	    .create(current_page_container_impl);

	auto current_page_container=
		container::create(current_page_container_impl,
				  page_lm_impl);

	factory->colspan(3);
	factory->halign(halign::fill);
	factory->valign(valign::fill);
	factory->border(appearance->border);
	factory->created_internally(current_page_container);

	auto new_c=ref<book_focusable_containerObj>::create(c, grid);

	creator(new_c);
	return new_c;
}

//////////////////////////////////////////////////////////////////////////////

book_lock::book_lock(const const_booklayoutmanager &layout_manager)
	: layout_manager{layout_manager},
	  grid_lock{layout_manager->book_pagetabgridlayoutmanager
		    ->impl->grid_map}
{
	layout_manager->notmodified();
}

book_lock::~book_lock()=default;

LIBCXXW_NAMESPACE_END
