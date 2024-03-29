/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "panelayoutmanager/panelayoutmanager_impl.H"
#include "panelayoutmanager/panecontainer_impl.H"
#include "panelayoutmanager/panefactory_impl.H"
#include "generic_window_handler.H"
#include "screen_positions_impl.H"
#include "x/w/focusable_container.H"
#include "x/w/panefactory.H"
#include "x/w/pane_layout_appearance.H"
#include "x/w/impl/focus/focusable.H"
#include "x/w/impl/themedim_axis_element.H"
#include "defaulttheme.H"
#include "screen.H"
#include "messages.H"
#include <x/xml/xpath.H>

LIBCXXW_NAMESPACE_START

panelayoutmanagerObj::panelayoutmanagerObj(const ref<implObj> &impl)
	: gridlayoutmanagerObj{impl},
	  impl{impl}
{
}

panelayoutmanagerObj::~panelayoutmanagerObj()=default;

size_t panelayoutmanagerObj::size() const
{
	notmodified();
	return impl->size(grid_lock);
}

size_t panelayoutmanagerObj::restored_size() const
{
	notmodified();
	return impl->restored_size;
}

elementptr panelayoutmanagerObj::get(size_t n) const
{
	notmodified();
	return impl->get_pane_element(grid_lock, n);
}

namespace {
#if 0
}
#endif

class append_panefactoryObj : public panefactory_implObj {

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
#if 0
{
#endif
}

panefactory panelayoutmanagerObj::append_panes()
{
	return ref<append_panefactoryObj>::create(ref(this));
}

void panelayoutmanagerObj::remove_pane(size_t pane_number)
{
	modified=true;

	impl->remove_pane(ref{this}, pane_number, grid_lock);
}

namespace {
#if 0
}
#endif

class insert_panefactoryObj : public panefactory_implObj {

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
#if 0
{
#endif
}

panefactory panelayoutmanagerObj::insert_panes(size_t position)
{
	return ref<insert_panefactoryObj>::create(ref(this), position);
}

namespace {
#if 0
}
#endif

class replace_panefactoryObj : public panefactory_implObj {

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
#if 0
{
#endif
}
panefactory panelayoutmanagerObj::replace_panes(size_t position)
{
	return ref<replace_panefactoryObj>::create(ref(this), position);
}

void panelayoutmanagerObj::remove_all_panes()
{
	modified=true;

	if (impl->size(grid_lock) == 0)
		return;

	// Minimal situation:
	//
	// [pane]
	// [slider]

	impl->remove_elements(grid_lock, 2, impl->total_size(grid_lock)-2);
	impl->remove_element(grid_lock, 0);
}

panefactory panelayoutmanagerObj::replace_all_panes()
{
	modified=true;

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

class panecontainerObj : public focusable_containerObj {

 public:

	const ref<panelayoutmanagerObj::implObj> panelayout_impl;

	panecontainerObj(const container_impl &impl,
			 const ref<panelayoutmanagerObj::implObj>
			 &panelayout_impl)
		: focusable_containerObj{impl, panelayout_impl},
		panelayout_impl{panelayout_impl}
		{
		}

	~panecontainerObj()=default;

	focusable_impl get_impl() const override
	{
		grid_map_t::lock lock{panelayout_impl->grid_map};

		auto n=panelayout_impl->total_size(lock);

		return panelayout_impl->get_element(lock, n <= 1 ? 0:1);
	}

	void do_get_impl(const function<internal_focusable_cb> &cb)
		const override
	{
		grid_map_t::lock lock{panelayout_impl->grid_map};

		auto n=panelayout_impl->total_size(lock);

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
			focusable f=panelayout_impl->get_element(lock, i);

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

//! Pane container implementation object for a vertically-adjustable pane.

//! The vertical pane container's height is specified by explicit dimensions.

class vertical_adjusted_panecontainer_implObj
	: public themedim_axis_heightObj<panecontainer_implObj> {

	typedef themedim_axis_heightObj<panecontainer_implObj> superclass_t;

 public:
	//! Constructor.
	vertical_adjusted_panecontainer_implObj
		(const container_impl &parent,
		 const dim_axis_arg &axis,
		 const child_element_init_params &init_params)
			: superclass_t{axis, parent, init_params}
	{
	}

	//! Re-verify our metrics, once we're initialized.
	void initialize(ONLY IN_THREAD) override
	{
		superclass_t::initialize(IN_THREAD);
		reset(IN_THREAD);
	}

	//! Theme was updated.

	void theme_updated(ONLY IN_THREAD,
			   const const_defaulttheme &new_theme) override
	{
		superclass_t::theme_updated(IN_THREAD, new_theme);
		reset(IN_THREAD);
	}

 private:

	//! Reset only the vertical metrics.
	void reset(ONLY IN_THREAD)
	{
		auto hv=get_horizvert(IN_THREAD);

		hv->set_element_metrics(IN_THREAD, hv->horiz,
					get_height_axis(IN_THREAD));

	}
};

//! Pane container implementation object for a horizontally-adjustable pane.

//! The horizontal pane container's width is specified by explicit dimensions.

class horizontal_adjusted_panecontainer_implObj
	: public themedim_axis_widthObj<panecontainer_implObj> {

	typedef themedim_axis_widthObj<panecontainer_implObj> superclass_t;

 public:
	//! Constructor.
	horizontal_adjusted_panecontainer_implObj
		(const container_impl &parent,
		 const dim_axis_arg &axis,
		 const child_element_init_params &init_params)
			: superclass_t{axis, parent, init_params}
	{
	}

	// Re-verify our metrics, once we're initialized.

	void initialize(ONLY IN_THREAD) override
	{
		superclass_t::initialize(IN_THREAD);
		reset(IN_THREAD);
	}

	//! Theme was updated.

	void theme_updated(ONLY IN_THREAD,
			   const const_defaulttheme &new_theme) override
	{
		superclass_t::theme_updated(IN_THREAD, new_theme);
		reset(IN_THREAD);
	}

 private:

	//! Reset only the horizontal metrics.
	void reset(ONLY IN_THREAD)
	{
		auto hv=get_horizvert(IN_THREAD);

		hv->set_element_metrics(IN_THREAD,
					get_width_axis(IN_THREAD),
					hv->vert);
	}
};

#if 0
{
#endif
}

new_panelayoutmanager::new_panelayoutmanager(const dim_axis_arg &size,
					     orientation_t orientation)
	: orientation{orientation},
	  size{size},
	  appearance{pane_layout_appearance::base::theme()}
{
}

new_panelayoutmanager::~new_panelayoutmanager()=default;

new_panelayoutmanager::new_panelayoutmanager(const new_panelayoutmanager &)
=default;

new_panelayoutmanager &new_panelayoutmanager
::operator=(const new_panelayoutmanager &)=default;

// Helper for loading preserved sizes, factored out for readability.

static inline std::vector<dim_t> load(
	const std::string &name,
	const container_impl &impl,
	const screen_positions_handle &config_handle)
{
	std::vector<dim_t> restored_sizes;

	auto lock=config_handle->config();

	auto xpath=lock->get_xpath("size");

	size_t n=xpath->count();

	restored_sizes.reserve(n);

	try {
		for (size_t i=1; i <= n; ++i)
		{
			xpath->to_node(i);

			restored_sizes.push_back(lock->get_text<dim_t>());
		}
	} catch (const exception &e)
	{
		auto ee=EXCEPTION( "Error restoring pane \""
				   << name << "\": " << e );

		ee->caught();

		restored_sizes.clear();
	}

	return restored_sizes;
}

// For optimal results, precompute the initial metrics of the pane container,
// and construct it.

static inline ref<panecontainer_implObj>
create_panecontainer_impl(const container_impl &parent,
			  const new_panelayoutmanager &plm)
{
	child_element_init_params init_params;

	auto theme=parent->container_element_impl()
		.get_screen()->impl->current_theme.get();

	switch (plm.orientation) {
	case new_panelayoutmanager::orientation_t::horizontal:
		init_params.initial_metrics.horiz=plm.size
			.compute(theme, themedimaxis::width);
		return ref<horizontal_adjusted_panecontainer_implObj>
			::create(parent, plm.size, init_params);
	case new_panelayoutmanager::orientation_t::vertical:
		init_params.initial_metrics.horiz=plm.size
			.compute(theme, themedimaxis::height);
		return ref<vertical_adjusted_panecontainer_implObj>
			::create(parent, plm.size, init_params);
	default:
		throw EXCEPTION("Should not happen");
	}
}

focusable_container
new_panelayoutmanager::create(const container_impl &parent,
			      const function<void
			      (const focusable_container &)>  &creator)
	const
{
	auto impl=create_panecontainer_impl(parent, *this);

	std::vector<dim_t> restored_sizes;
	screen_positions_handleptr config_handleptr;

	if (!name.empty())
	{
		auto &wh=impl->elementObj::implObj::get_window_handler();

		auto config_handle=wh.widget_config_handle(
			libcxx_uri,
			"pane",
			name,
			impl
		);

		restored_sizes=load(name, impl, config_handle);

		config_handleptr=config_handle;
	}

	// Create the appropriate implementation subclass.

	ref<panelayoutmanagerObj::implObj> lm_impl{
		orientation == orientation_t::vertical ?
			ref<panelayoutmanagerObj::implObj>{
			ref<panelayoutmanagerObj::implObj::orientation
			<panelayoutmanagerObj::implObj::vertical>>
			::create(impl, appearance, config_handleptr,
				 restored_sizes)}
		: ref<panelayoutmanagerObj::implObj>{
			ref<panelayoutmanagerObj::implObj::orientation
			    <panelayoutmanagerObj::implObj::horizontal>>
			::create(impl, appearance, config_handleptr,
				 restored_sizes)}
	};

	auto c=ref<panecontainerObj>::create(impl, lm_impl);

	panelayoutmanager lm=lm_impl->create_panelayoutmanager();

	if (orientation != orientation_t::vertical)
		lm->impl->insert_row(&*lm, 0);

	// Initial slider.
	lm_impl->create_slider(lm_impl->create_slider_factory(&*lm, 0));

	creator(c);
	return c;
}

LIBCXXW_NAMESPACE_END
