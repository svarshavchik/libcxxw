/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "panelayoutmanager/panelayoutmanager_impl.H"
#include "panelayoutmanager/panecontainer_impl.H"
#include "panelayoutmanager/panefactory_impl.H"
#include "x/w/focusable_container.H"
#include "x/w/panefactory.H"
#include "x/w/canvas.H"
#include "x/w/impl/focus/focusable.H"
#include "defaulttheme.H"

LIBCXXW_NAMESPACE_START

panelayoutmanagerObj::panelayoutmanagerObj(const ref<implObj> &impl)
	: layoutmanagerObj{impl},
	  impl{impl}
{
}

panelayoutmanagerObj::~panelayoutmanagerObj()=default;

size_t panelayoutmanagerObj::size() const
{
	return impl->size();
}

elementptr panelayoutmanagerObj::get(size_t n) const
{
	return impl->get_pane_element(n);
}

class LIBCXX_HIDDEN append_panefactoryObj : public panefactory_implObj {

 public:

	append_panefactoryObj(const panelayoutmanager &layout)
		: panefactory_implObj{layout}
	{
	}

	void created(const element &e) override
	{
		created_at(e, layout->size());
	}

	~append_panefactoryObj()=default;
};

panefactory panelayoutmanagerObj::append_panes()
{
	return ref<append_panefactoryObj>::create(ref(this));
}

void panelayoutmanagerObj::remove_pane(size_t pane_number)
{
	impl->remove_pane(ref(this), pane_number);
}

class LIBCXX_HIDDEN insert_panefactoryObj : public panefactory_implObj {

 public:

	std::atomic_size_t position;

	insert_panefactoryObj(const panelayoutmanager &layout,
			      size_t position)
		: panefactory_implObj{layout}, position{position}
	{
	}

	void created(const element &e) override
	{
		created_at(e, position++);
	}

	~insert_panefactoryObj()=default;
};

panefactory panelayoutmanagerObj::insert_panes(size_t position)
{
	return ref<insert_panefactoryObj>::create(ref(this), position);
}

class LIBCXX_HIDDEN replace_panefactoryObj : public panefactory_implObj {

 public:

	std::atomic_size_t position;

	replace_panefactoryObj(const panelayoutmanager &layout,
			      size_t position)
		: panefactory_implObj{layout}, position{position}
	{
	}

	void created(const element &e) override
	{
		auto p=position++;

		created_at(e, p);

		// Remove the pane the new one replaced.

		layout->remove_pane(++p);
	}

	~replace_panefactoryObj()=default;
};

panefactory panelayoutmanagerObj::replace_panes(size_t position)
{
	return ref<replace_panefactoryObj>::create(ref(this), position);
}

void panelayoutmanagerObj::remove_all_panes()
{
	grid_map_t::lock lock{impl->grid_map};

	if (impl->size() == 0)
		return;

	// Minimal situation:
	//
	// [pane]
	// [slider]
	//   ...
	// [canvas]

	impl->remove_elements(lock, 2, impl->total_size(lock)-3);
	impl->remove_element(lock, 0);
	impl->request_extra_space_to_canvas();
}

panefactory panelayoutmanagerObj::replace_all_panes()
{
	// The factory acquires the lock, first.

	auto f=append_panes();

	remove_all_panes();
	return f;
}

///////////////////////////////////////////////////////////////////////////
namespace {
#if 0
}
#endif

class LIBCXX_HIDDEN panecontainerObj : public focusable_containerObj {

 public:

	const ref<panelayoutmanagerObj::implObj> layout_impl;

	panecontainerObj(const ref<containerObj::implObj> &impl,
			 const ref<panelayoutmanagerObj::implObj> &layout_impl)
		: focusable_containerObj{impl, layout_impl},
		layout_impl{layout_impl}
		{
		}

	~panecontainerObj()=default;

	focusable_impl get_impl() const override
	{
		grid_map_t::lock lock{layout_impl->grid_map};

		auto n=layout_impl->total_size(lock);

		return layout_impl->get_element(lock, n <= 1 ? 0:1);
	}

	void do_get_impl(const function<internal_focusable_cb> &cb)
		const override
	{
		grid_map_t::lock lock{layout_impl->grid_map};

		auto n=layout_impl->total_size(lock);

		// An element in the container is either a slider or a pane
		// element. Both of them are focusables. The pane element
		// consists of two focusables, the scrollbars.
		//
		// Additional pane elements that are focusable containers will
		// get automatically inserted into the focus order, so we'll
		// just start with a ballpark estimate that's going to be
		// good enought.
		// then number of elements we will have.

		std::vector<focusable_impl> v;

		v.reserve(1 + 2*n);

		for (size_t i=0; i<n; ++i)
		{
			focusable f=layout_impl->get_element(lock, i);

			f->get_impl([&]
				    (const auto &focusable_group)
				    {
					    v.insert(v.end(),
						     focusable_group.impls,
						     focusable_group.impls+
						     focusable_group
						     .internal_impl_count);
				    });
		}
		cb(internal_focusable_group{v.size(), &v.at(0)});
	}
};

#if 0
{
#endif
}

new_panelayoutmanager::new_panelayoutmanager(orientation_t orientation)
	: pane_style{"pane_border", "pane_slider",
		"pane_slider_background"},
	  orientation{orientation}
{
}

new_panelayoutmanager::~new_panelayoutmanager()=default;

focusable_container
new_panelayoutmanager::create(const ref<containerObj::implObj> &parent)
	const
{
	auto impl=ref<panecontainer_implObj>::create(parent);

	// Create the appropriate implementation subclass.

	ref<panelayoutmanagerObj::implObj> lm_impl{
		orientation == orientation_t::vertical ?
			ref<panelayoutmanagerObj::implObj>{
			ref<panelayoutmanagerObj::implObj::orientation
			<panelayoutmanagerObj::implObj::vertical>>
			::create(impl, *this)}
		: ref<panelayoutmanagerObj::implObj>{
			ref<panelayoutmanagerObj::implObj::orientation
			    <panelayoutmanagerObj::implObj::horizontal>>
				::create(impl, *this)}
	};

	auto c=ref<panecontainerObj>::create(impl, lm_impl);

	panelayoutmanager lm=c->get_layoutmanager();

	if (orientation != orientation_t::vertical)
		lm->impl->insert_row(&*lm, 0);

	// Create the canvas element that absorbs any extra space.
	{
		auto f=lm_impl->create_slider_factory(&*lm, 0);

		f->padding(0);
		f->create_canvas([]
				 (const auto &canvas)
				 {
					 canvas->show();
				 },
				 {0, 0, 0},
				 {0, 0, 0});
	}
	// Initial slider.
	lm_impl->create_slider(lm_impl->create_slider_factory(&*lm, 0));
	lm_impl->request_extra_space_to_canvas();

	return c;
}

pane_lock::pane_lock(const panelayoutmanager &lm)
	: grid_map_t::lock{lm->impl->grid_map}
{
}

pane_lock::~pane_lock()=default;

LIBCXXW_NAMESPACE_END
