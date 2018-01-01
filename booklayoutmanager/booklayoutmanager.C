/*
** Copyright 2017 Double Precision, Inc.
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
#include "peephole/peephole_impl.H"
#include "peephole/peepholed_element.H"
#include "peephole/peephole_style.H"
#include "peephole/peephole_layoutmanager_impl.H"
#include "layoutmanager.H"
#include "image_button.H"
#include "image_button_internal_impl.H"
#include "icon.H"
#include "container_element.H"
#include "container_visible_element.H"
#include "child_element.H"
#include "always_visible.H"
#include "capturefactory.H"
#include "busy.H"
#include "run_as.H"
#include "catch_exceptions.H"
#include "generic_window_handler.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/pagelayoutmanager.H"
#include "x/w/pagefactory.H"
#include "x/w/container.H"
#include "x/w/element.H"
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
	  impl{impl}
{
}

booklayoutmanagerObj::~booklayoutmanagerObj()=default;

// The internal pagelayoutmanager is the authoritative source for our pages.

size_t booklayoutmanagerObj::pages() const
{
	return impl->book_pagelayoutmanager->pages();
}

std::optional<size_t> booklayoutmanagerObj::opened() const
{
	return impl->book_pagelayoutmanager->opened();
}

void booklayoutmanagerObj
::on_opened(const std::function<void (const book_status_info_t &)> &cb)
{
	book_lock lock{ref(this)};

	lock.layout_manager->impl->impl->callback(lock)=cb;
}

void booklayoutmanagerObj::open(size_t n)
{
	implObj::open(ref(this), n, {});
}

void booklayoutmanagerObj
::implObj::open(const booklayoutmanager &blm, size_t n,
		const callback_trigger_t &trigger)
{
	// Hijack the logger for LOG_ERROR
	const auto &logger=elementObj::implObj::logger;

	book_lock lock{blm};

	// Remember which page is currently open.

	auto opened=blm->impl->book_pagelayoutmanager->opened();

	pagetabptr previous_page;

	if (opened)
		previous_page=blm->impl->get_pagetab(*opened);
	pagetab next_page=blm->impl->get_pagetab(n);

	// The official switch to the new page can be done immediately, but
	// need to punt the tab visual update to the connection thread.

	blm->impl->book_pagelayoutmanager->open(n);

	blm->impl->book_pagecontainer->elementObj::impl->THREAD
		->run_as([previous_page, next_page,

			  // Make sure to scroll the opened tab into the view
			  // (the center of the peephole), unless it was a
			  // button click. Yanking the tab into the center
			  // of the peephole, just after clicking on it,
			  // is rather rude.
			  ensure_visibility=trigger.index() ==
			  callback_trigger_button_event ? false:true]
			 (IN_THREAD_ONLY)
			 {
				 if (previous_page)
				 {
					 previous_page->impl
						 ->initialize_if_needed
						 (IN_THREAD);
					 previous_page->impl
						 ->set_active(IN_THREAD, false);
				 }
				 next_page->impl->set_active(IN_THREAD, true);

				 if (!ensure_visibility)
					 return;
				 next_page->elementObj::impl
					 ->ensure_entire_visibility(IN_THREAD);
			 });

	try {
		auto &cb=lock.layout_manager->impl->impl->callback(lock);

		if (cb)
			cb({lock, n, trigger,
			    busy_impl{lock.layout_manager
						->impl->impl->container_impl
						->container_element_impl()}});
	} CATCH_EXCEPTIONS;
}

void booklayoutmanagerObj::close()
{
	book_lock lock{ ref(this) };

	auto opened=impl->book_pagelayoutmanager->opened();

	pagetabptr previous_page;

	if (opened)
		previous_page=impl->get_pagetab(*opened);

	impl->book_pagelayoutmanager->close();

	// We can officially close the book immediately, but we
	// have to punt the tab visual update to the connection thread.

	if (!previous_page)
		return;

	impl->book_pagecontainer->elementObj::impl->THREAD
		->run_as([previous_page]
			 (IN_THREAD_ONLY)
			 {
				 previous_page->impl
					 ->set_active(IN_THREAD, false);
			 });
}

element booklayoutmanagerObj::get_page(size_t n) const
{
	return impl->book_pagelayoutmanager->get(n);
}

//////////////////////////////////////////////////////////////////////////////
//
// Logic shared by both the append and the insert factories.

class LIBCXX_HIDDEN bookpagefactory_implObj : public bookpagefactoryObj {

 protected:

	// These are accessed only while holding a lock

	const booklayoutmanager layout_manager;
	const pagefactory book_pagefactory;

	// The tab factory is accessed only while a lock is being held.

	gridfactory book_tabfactory_under_lock;

	inline gridfactory &book_tabfactory(grid_map_t::lock &)
	{
		return book_tabfactory_under_lock;
	}

 public:

	bookpagefactory_implObj(const booklayoutmanager &layout_manager,
				const pagefactory &book_pagefactory,
				const gridfactory &book_tabfactory)
		: layout_manager{layout_manager},
		book_pagefactory{book_pagefactory},
		book_tabfactory_under_lock{book_tabfactory}
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

static inline void tab_activate(const booklayoutmanager &layout_manager,
				const element &activated_page,
				const callback_trigger_t &trigger)
{
	book_lock lock{layout_manager};

	auto index=layout_manager->impl->book_pagelayoutmanager
		->lookup(activated_page);

	if (!index)
		return;

	booklayoutmanagerObj::implObj::open(layout_manager,
					    *index,
					    trigger);
}

// Helper for populating a new tab's structure.
//
// The caller provides its factory, our layout manager, and the tab_factory
// parameter.
//
// The caller is responsible for invoking gridfactory->created_internally()
// for the returned element.

static
auto create_new_tab(const gridfactory &gridfactory,
		    const booklayoutmanager &layout_manager,
		    const pagefactory &book_pagefactory,
		    const function<void (const factory &,
					 const factory &)> &tab_factory,
		    const shortcut &sc)
{
	// All elements in the tab strip have no padding, and take up the
	// full height of the tab strip.
	gridfactory->padding(0);
	gridfactory->valign(valign::fill);

	// First-ly, an inner container with the grid layout manager, for this
	// new tab's individual borders.

	auto inner_tab_gridcontainer_impl=
		ref<container_visible_elementObj<
			always_visibleObj<container_elementObj
					  <child_elementObj>>>>
		::create(gridfactory->get_container_impl());

	// Inner container's grid layoutmanager.

	auto inner_tab_lm=
		new_gridlayoutmanager{}.create(inner_tab_gridcontainer_impl);

	// And the inner container itself.
	auto inner_tab_container=container::create(inner_tab_gridcontainer_impl,
						   inner_tab_lm);

	// Now, inside the inner container, we need to put in the pagetab
	// element. First, obtain the gridfactory, and configure the new,
	// single cell.

	gridlayoutmanager inner_tab_glm=
		inner_tab_container->get_layoutmanager();
	auto inner_tab_element_factory=inner_tab_glm->append_row();

	inner_tab_element_factory->padding(0);
	inner_tab_element_factory->valign(valign::bottom);
	inner_tab_element_factory->left_border("book_tab_border");
	inner_tab_element_factory->right_border("book_tab_border");
	inner_tab_element_factory->top_border("book_tab_border");

	// Create the implementation object for the pagetab.

	auto impl=ref<pagetabObj::implObj>
		::create(inner_tab_element_factory->get_container_impl(),
			 layout_manager->impl->book_pagetabgridlayoutmanager
			 ->impl->my_container,
			 "book_tab_warm_color",
			 "book_tab_hot_color");

	// Finish initialize the impl in the connection thread.
	//
	// The initial background color needs
	// to be set. Also, install the shortcut.
	impl->get_element_impl().THREAD
		->run_as([impl, sc]
			 (IN_THREAD_ONLY)
			 {
				 impl->set_active(IN_THREAD, false);
				 impl->set_shortcut(IN_THREAD, sc);
			 });

	// Obtain the initial contents of the actual tab label.
	auto tab_capture_factory=capturefactory::create(impl);
	auto page_capture_factory=
		capturefactory::create(book_pagefactory
				       ->get_container_impl());
	tab_factory(tab_capture_factory, page_capture_factory);

	auto page_element=page_capture_factory->get();

	// And now we can construct the singleton layout manager for the
	// pagetab implementation object.

	auto lm=ref<pagetabsingletonlayoutmanager_implObj>
		::create(impl, tab_capture_factory->get());

	auto new_page=pagetab::create(impl, lm);

	// Now, tell the inner container that we created its element.
	inner_tab_element_factory->created_internally(new_page);

	return std::tuple{inner_tab_container, new_page, page_element};
}

// Install a callback when the tab hotspot gets activated.
//
// Weakly capture the entire container with the book layout manager,
// and the newly-added tab element.


static
void install_activate_callback(const booklayoutmanager &layout_manager,
			       const auto &new_hotspot,
			       const element &new_page)
{
	new_hotspot->on_activate
		([weak_captures=make_weak_capture(layout_manager->impl
						  ->impl->container_impl,
						  new_page)]
		 (const auto &trigger, const auto &busy)
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
						  (lm_impl
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
		: bookpagefactory_implObj{layout_manager,
			layout_manager->impl->book_pagelayoutmanager
			->append(),
			layout_manager->impl->book_pagetabgridlayoutmanager
			->append_columns(0)}
	{
	}

	// Destructor
	~append_bookpagefactoryObj()=default;

	//! Implement add()
	void do_add(const function<void (const factory &,
					 const factory &)> &factory,
		    const shortcut &sc)
		override;
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
			layout_manager->impl->book_pagelayoutmanager
			->insert(page_number),
			layout_manager->impl->book_pagetabgridlayoutmanager
			->insert_columns(0, page_number)},
		page_number{page_number}
	{
	}

	// Destructor
	~insert_bookpagefactoryObj()=default;

	//! Implement add().
	void do_add(const function<void (const factory &,
					 const factory &)> &factory,
		    const shortcut &sc)
		override;
};

bookpagefactory booklayoutmanagerObj::append()
{
	return ref<append_bookpagefactoryObj>::create(ref(this));
}

void append_bookpagefactoryObj
::do_add(const function<void (const factory &, const factory &)> &f,
	 const shortcut &sc)
{
	grid_map_t::lock lock{layout_manager->impl->impl->grid_map};

	auto [tab_element, hotspot_element, page_element]=
		create_new_tab(book_tabfactory(lock),
			       layout_manager, book_pagefactory, f, sc);

	install_activate_callback(layout_manager, hotspot_element,
				  page_element);
	book_tabfactory(lock)->created_internally(tab_element);
	book_pagefactory->created_internally(page_element);
	layout_manager->impl->rebuild_focusables(lock);
}

bookpagefactory booklayoutmanagerObj::insert(size_t page_number)
{
	return ref<insert_bookpagefactoryObj>::create(ref(this), page_number);
}

void insert_bookpagefactoryObj
::do_add(const function<void (const factory &, const factory &)> &f,
	 const shortcut &sc)
{
	grid_map_t::lock lock{layout_manager->impl->impl->grid_map};

	auto [tab_element, hotspot_element, page_element]=
		create_new_tab(book_tabfactory(lock),
			       layout_manager, book_pagefactory, f, sc);

	install_activate_callback(layout_manager, hotspot_element,
				  page_element);
	book_tabfactory(lock)->created_internally(tab_element);
	book_pagefactory->created_internally(page_element);

	++page_number;
	layout_manager->impl->rebuild_focusables(lock);
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

	dim_t horizontal_increment(IN_THREAD_ONLY) const override { return 0; }
	dim_t vertical_increment(IN_THREAD_ONLY) const override { return 0; }
};

// Glue together the buttons on the the sides of the tab strip peephole,
// which we will use with create_image_button().

class LIBCXX_HIDDEN book_tab_imagebuttonObj
	: public image_button_internalObj::implObj {

 public:

	using image_button_internalObj::implObj::implObj;

	~book_tab_imagebuttonObj()=default;

	//! Override temperature_changed()

	//! Flash the scroll icons when clicking on them.

	void temperature_changed(IN_THREAD_ONLY,
				 const callback_trigger_t &trigger) override
	{
		image_button_internalObj::implObj::temperature_changed
			(IN_THREAD, trigger);

		set_image_number(IN_THREAD,
				 trigger,
				 hotspot_temperature(IN_THREAD)
				 == temperature::hot ? 1:0);
	}
};

// And we need to implement our focusable_container.

class LIBCXX_HIDDEN book_focusable_containerObj
	: public focusable_containerObj {

 public:
	using focusable_containerObj::focusable_containerObj;

	~book_focusable_containerObj()=default;

	ref<focusableImplObj> get_impl() const override
	{
		ptr<focusableImplObj> p;

		containerObj::impl->invoke_layoutmanager
			([&]
			 (const ref<gridlayoutmanagerObj::implObj> &g)
			 {
				 p=g->get(0, 0);
			 });

		return p;
	}

	void do_get_impl(const function<internal_focusable_cb> &cb) const
		override
	{
		const_booklayoutmanager lm=get_layoutmanager();

		book_lock lock{lm};

		auto focusables=lm->impl->get_focusables(lock);

		process_focusable_impls_from_focusables(cb, focusables);
	}
};

new_booklayoutmanager::new_booklayoutmanager()
	: background_color{"page_background"},
	  border{"page_border"}
{
}

new_booklayoutmanager::~new_booklayoutmanager()=default;

focusable_container
new_booklayoutmanager::create(const ref<containerObj::implObj> &parent) const
{
	// Our container implementation is nothing special.

	ref<containerObj::implObj> c=
		ref<container_elementObj<child_elementObj>>::create(parent);

	// Create the our gridlayoutmanager subclass, the real layout manager
	// for this container.

	auto grid=ref<bookgridlayoutmanagerObj>::create(c);

	// Start building the contents of the grid.

	auto gridlm=grid->create_gridlayoutmanager();

	auto factory=gridlm->append_row();

	// Left scroll image button.

	auto &wh=c->get_window_handler();

	gridlm->row_alignment(0, valign::bottom);
	gridlm->requested_row_height(1, 100);
	factory->padding(0).border(border);

	// We will not use image button callbacks, instead we'll hook into
	// the activation callbacks.

	ptr<book_tab_imagebuttonObj> left_scroll_impl,
		right_scroll_impl;

	auto left_scroll=create_image_button
		(true,
		 [&]
		 (const auto &container_impl)
		 {
			 // Use our scalable icons, and make them
			 // book_scroll_height tall.

			 auto impl=ref<book_tab_imagebuttonObj>::create
				 (container_impl,
				  std::vector<icon>{
					  wh.create_icon
						  ({"scroll-left1",
						   render_repeat::none,
						   0,
						  "book_scroll_height"}),
					  wh.create_icon
						  ({"scroll-left2",
						   render_repeat::none,
						   0,
						  "book_scroll_height"}),
						  });

			 left_scroll_impl=impl;

			 return impl;
		 }, *factory, valign::bottom,
		 [](const auto &ignore){});

	left_scroll->set_background_color(background_color);
	left_scroll->show();

	////////////////////////////////////////////////////////////////////
	//
	// The page tab element is, first and foremost, a peephole.

	factory->padding(0).halign(halign::fill);

	auto peephole_impl=
		ref<always_visibleObj<peepholeObj::implObj>>
		::create(factory->get_container_impl());

	// And inside this peephole is the actual tab.

	auto pagetab_gridcontainer=pagetabgridcontainer_impl::create
		(peephole_impl,
		 "book_tab_h_padding",
		 "book_tab_v_padding",
		 "book_tab_inactive_color",
		 "book_tab_active_color");

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

	peephole_style style;

	style.scroll=peephole_scroll::centered;

	auto peephole_lm=
		ref<peepholeObj::layoutmanager_implObj>::create
		(peephole_impl,
		 style,
		 pagetab_container);

	// And the peephole.

	auto my_peephole=peephole::create(peephole_impl, peephole_lm);

	factory->created_internally(my_peephole);

	// It's time for the right scroll button.

	factory->padding(0).border(border);

	auto right_scroll=create_image_button
		(true,
		 [&]
		 (const auto &container_impl)
		 {
			 auto impl=ref<book_tab_imagebuttonObj>::create
				 (container_impl,
				  std::vector<icon>{
					  wh.create_icon
						  ({"scroll-right1",
						   render_repeat::none,
						   0,
						   "book_scroll_height"}),
					  wh.create_icon
						  ({"scroll-right2",
						   render_repeat::none,
						   0,
						   "book_scroll_height"}),
						  });

			 right_scroll_impl=impl;

			 return impl;
		 }, *factory, valign::bottom,
		 [](const auto &ignore){});

	right_scroll->set_background_color(background_color);
	right_scroll->show();

	// It's very nice for an image_button to provide callbacks that
	// report when its image changes. But what we really need to do is
	// install callbacks for its internal hotspot, that gets clicked on.

	left_scroll_impl->on_activate
		([c=make_weak_capture(c)]
		 (const auto &trigger,
		  const auto &mcguffin)
		 {
			 auto got=c.get();

			 if (!got)
				 return;

			 auto &[c]=*got;

			 c->invoke_layoutmanager
				 ([]
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

					  lm->open(--next);
				  });
		 });

	right_scroll_impl->on_activate
		([c=make_weak_capture(c)]
		 (const auto &trigger,
		  const auto &mcguffin)
		 {
			 auto got=c.get();

			 if (!got)
				 return;

			 auto &[c]=*got;

			 c->invoke_layoutmanager
				 ([]
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

					  lm->open(next);
				  });
		 });

	// The internal pagelayoutmanager goes into the next row.

	factory=gridlm->append_row();

	auto current_page_container_impl=
		ref<always_visibleObj<container_elementObj<child_elementObj>>>
		::create(c);

	auto page_lm_impl=new_pagelayoutmanager{}
	    .create(current_page_container_impl);

	auto current_page_container=
		container::create(current_page_container_impl,
				  page_lm_impl);

	factory->colspan(3);
	factory->border(border);
	factory->created_internally(current_page_container);
	current_page_container->set_background_color(background_color);

	return ref<book_focusable_containerObj>::create(c, grid);
}

//////////////////////////////////////////////////////////////////////////////

book_lock::book_lock(const const_booklayoutmanager &layout_manager)
	: grid_map_t::lock{layout_manager->impl->impl->grid_map},
	layout_manager{layout_manager}
{
}

book_lock::~book_lock()=default;

LIBCXXW_NAMESPACE_END
