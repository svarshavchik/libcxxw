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
#include "container_element.H"
#include "container_visible_element.H"
#include "child_element.H"
#include "always_visible.H"
#include "capturefactory.H"
#include "run_as.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/pagelayoutmanager.H"
#include "x/w/pagefactory.H"
#include "x/w/container.H"
#include "x/w/element.H"
#include "x/w/bookpagefactory.H"
#include "x/w/canvas.H"

#include <x/weakcapture.H>

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
	return impl->book_pagelayoutmanager->size();
}

void booklayoutmanagerObj::open(size_t n) const
{
	book_lock lock{ const_ref(this) };

	// Remember which page is currently open.

	auto opened=impl->book_pagelayoutmanager->opened();

	pagetabptr previous_page;

	if (opened)
		previous_page=impl->get_pagetab(*opened);
	pagetab next_page=impl->get_pagetab(n);

	// The official switch to the new page can be done immediately, but
	// need to punt the tab visual update to the connection thread.

	impl->book_pagelayoutmanager->open(n);
	impl->book_switchcontainer->elementObj::impl->THREAD
		->run_as([previous_page, next_page]
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
				 next_page->elementObj::impl
					 ->ensure_entire_visibility(IN_THREAD);
			 });

}

void booklayoutmanagerObj::close() const
{
	book_lock lock{ const_ref(this) };

	auto opened=impl->book_pagelayoutmanager->opened();

	pagetabptr previous_page;

	if (opened)
		previous_page=impl->get_pagetab(*opened);

	impl->book_pagelayoutmanager->close();

	// We can officially close the book immediately, but we
	// have to punt the tab visual update to the connection thread.

	if (!previous_page)
		return;

	impl->book_switchcontainer->elementObj::impl->THREAD
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
				const element &activated_page)
{
	book_lock lock{layout_manager};

	auto index=layout_manager->impl->book_pagelayoutmanager
		->lookup(activated_page);

	if (!index)
		return;

	layout_manager->open(*index);
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
					 const factory &)> &tab_factory)
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

	inner_tab_gridcontainer_impl->request_visibility(true);

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
	// It is always_visibleObj, and the initial background color needs
	// to be set.
	impl->get_element_impl().THREAD
		->run_as([impl]
			 (IN_THREAD_ONLY)
			 {
				 impl->request_visibility(IN_THREAD, true);
				 impl->set_active(IN_THREAD, false);
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
						   new_page);
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
					 const factory &)> &factory)
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
					 const factory &)> &factory)
		override;
};

bookpagefactory booklayoutmanagerObj::append()
{
	return ref<append_bookpagefactoryObj>::create(ref(this));
}

void append_bookpagefactoryObj
::do_add(const function<void (const factory &, const factory &)> &f)
{
	grid_map_t::lock lock{layout_manager->impl->impl->grid_map};

	auto [tab_element, hotspot_element, page_element]=
		create_new_tab(book_tabfactory(lock),
			       layout_manager, book_pagefactory, f);

	install_activate_callback(layout_manager, hotspot_element,
				  page_element);
	book_tabfactory(lock)->created_internally(tab_element);
	book_pagefactory->created_internally(page_element);
}

bookpagefactory booklayoutmanagerObj::insert(size_t page_number)
{
	return ref<insert_bookpagefactoryObj>::create(ref(this), page_number);
}

void insert_bookpagefactoryObj
::do_add(const function<void (const factory &, const factory &)> &f)
{
	grid_map_t::lock lock{layout_manager->impl->impl->grid_map};

	auto [tab_element, hotspot_element, page_element]=
		create_new_tab(book_tabfactory(lock),
			       layout_manager, book_pagefactory, f);

	install_activate_callback(layout_manager, hotspot_element,
				  page_element);
	book_tabfactory(lock)->created_internally(tab_element);
	book_pagefactory->created_internally(page_element);

	++page_number;
}

//////////////////////////////////////////////////////////////////////////////

new_booklayoutmanager::new_booklayoutmanager()=default;

new_booklayoutmanager::~new_booklayoutmanager()=default;

// The peepholed page tab container.

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

ref<layoutmanagerObj::implObj>
new_booklayoutmanager::create(const ref<containerObj::implObj> &c) const
{
	// Create the our gridlayoutmanager subclass, the real layout manager
	// for this container.

	auto grid=ref<bookgridlayoutmanagerObj>::create(c);

	// Need to construct the contents of the grid.

	auto gridlm=grid->create_gridlayoutmanager();

	auto factory=gridlm->append_row();

	// Placeholders for the images.

	factory->padding(0).create_canvas([](const auto &){}, {0,0,0},{0,0,0});
	factory->padding(0).create_canvas([](const auto &){}, {0,0,0},{0,0,0});

	factory=gridlm->insert_columns(0, 1);

	////////////////////////////////////////////////////////////////////
	//
	// The page tab element is, first and foremost, a peephole.

	factory->padding(0).valign(valign::bottom).halign(halign::fill);

	auto peephole_impl=
		ref<always_visibleObj<peepholeObj::implObj>>
		::create(factory->get_container_impl());

	peephole_impl->request_visibility(true);

	// And inside this peephole is the actual tab.

	auto pagetab_gridcontainer=pagetabgridcontainer_impl::create
		(peephole_impl,
		 "book_tab_h_padding",
		 "book_tab_v_padding",
		 "book_tab_inactive_color",
		 "book_tab_active_color");

	pagetab_gridcontainer->request_visibility(true);

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

	auto peephole_lm=
		ref<peepholeObj::layoutmanager_implObj>::create
		(peephole_impl,
		 peephole_style{},
		 pagetab_container);

	// And the peephole.

	auto my_peephole=peephole::create(peephole_impl, peephole_lm);

	factory->created_internally(my_peephole);

	// The internal pagelayoutmanager goes into the next row.

	factory=gridlm->append_row();

	auto current_page_container_impl=
		ref<always_visibleObj<container_elementObj<child_elementObj>>>
		::create(c);

	current_page_container_impl->request_visibility(true);

	auto page_lm_impl=new_pagelayoutmanager{}
	    .create(current_page_container_impl);

	auto current_page_container=
		container::create(current_page_container_impl,
				  page_lm_impl);

	factory->colspan(3);
	factory->created_internally(current_page_container);

	return grid;
}

//////////////////////////////////////////////////////////////////////////////

book_lock::book_lock(const const_booklayoutmanager &layout_manager)
	: grid_map_t::lock{layout_manager->impl->impl->grid_map},
	layout_manager{layout_manager}
{
}

book_lock::~book_lock()=default;

LIBCXXW_NAMESPACE_END
