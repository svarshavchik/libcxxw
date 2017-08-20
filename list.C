/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/focusable_container.H"
#include "x/w/peepholed_focusableobj.H"
#include "x/w/rgb.H"
#include "listlayoutmanager/listlayoutmanager.H"
#include "listlayoutmanager/listcontainer_impl.H"
#include "focus/focusable.H"
#include "peephole/peepholed.H"
#include "peephole/peephole_impl.H"
#include "peepholed_focusable.H"
#include "peepholed_listcontainer_impl.H"
#include "background_color.H"
#include "container.H"
#include "reference_font_element.H"
#include "container_element.H"
#include "nonrecursive_visibility.H"

#include <tuple>
#include <utility>

LIBCXXW_NAMESPACE_START

//! The container returned by create_list().

//! create_list() returned this subclass of focusable_containerObj.
//!
//! Overrides get_layout_impl() to return the layout manager from the
//! peepholed listcontainer, as the alleged layout manager for this element.
//!
//! The internal listcontainer gets passed to the constructor and saved here.

class LIBCXX_HIDDEN listObj : public peepholed_focusableObj {

 public:

	const listcontainer internal_listcontainer;

	listObj(const listcontainer &internal_listcontainer,
		const ref<peepholed_focusableObj::implObj> &impl,
		const ref<layoutmanagerObj::implObj> &layout_impl)
		: peepholed_focusableObj(impl, layout_impl),
		internal_listcontainer(internal_listcontainer)
		{
		}

	~listObj()=default;

	ref<layoutmanagerObj::implObj> get_layout_impl() const override
	{
		return internal_listcontainer->get_layout_impl();
	}
};

///////////////////////////////////////////////////////////

static void default_selection_changed(list_lock &, const listlayoutmanager &,
				      size_t, bool, const busy &)
{
}

new_listlayoutmanager
::new_listlayoutmanager(const listlayoutstyle &layout_style)
	: layout_style(layout_style),
	  selection_type(single_selection_type),
	  selection_changed(default_selection_changed),
	  rows(4),
	  columns(1),
	  v_padding("list_v_padding"),
	  left_padding("list_left_padding"),
	  inner_padding("list_inner_padding"),
	  right_padding("list_right_padding"),
	  vertical_scrollbar(scrollbar_visibility::automatic),
	  background_color("list_background_color"),
	  selected_color("list_selected_color"),
	  highlighted_color("list_highlighted_color"),
	  current_color("list_current_color")
{
}

new_listlayoutmanager::~new_listlayoutmanager()=default;

// Builder for the peepholed list container. Factored out from
// do_create_list() for readability.

static inline listcontainer
make_peepholed_list(const ref<peepholeObj::implObj> &peephole_parent,
		    const new_listlayoutmanager &style)
{
	auto internal_listcontainer_impl=
		ref<peepholed_listcontainerObj::implObj>
		::create(peephole_parent, style);

	auto internal_listlayoutmanager_impl=
		ref<listlayoutmanagerObj::implObj>::create
		(internal_listcontainer_impl, style);

	auto container=peepholed_listcontainer::create
		(internal_listcontainer_impl,
		 internal_listlayoutmanager_impl);

	internal_listlayoutmanager_impl->style
		.initialize(internal_listlayoutmanager_impl);

	container->show();

	return container;
}

focusable_container
new_listlayoutmanager::create(const ref<containerObj::implObj>
			      &parent_container) const
{
	auto focusable_container_impl=
		ref<peepholed_container_impl_t>::create(parent_container);

	listcontainerptr internal_listcontainer;

	auto [peephole_info, lm]=create_peepholed_focusable_with_frame
		({"list_border",
				"inputfocusoff_border",
				"inputfocuson_border",
				0,
				focusable_container_impl->get_element_impl()
				.create_background_color(background_color),
				focusable_container_impl,
				peephole_style(),
				scrollbar_visibility::never,
				vertical_scrollbar},
		 [&]
		 (const ref<containerObj::implObj> &peepholed_parent)
		 {
			 auto peephole_impl=ref<peepholeObj::implObj>
			 ::create(peepholed_parent);

			 auto container=make_peepholed_list(peephole_impl,
							    *this);

			 internal_listcontainer=container;

			 return std::make_tuple(peephole_impl,
						container,
						container,
						container->impl);
		 });

	return ref<listObj>::create(internal_listcontainer, peephole_info,
				    lm->impl);
}

////////////////////////////////////////////////////////////////////////////


void single_selection_type(list_lock &lock,
			   const listlayoutmanager &layout_manager,
			   size_t i,
			   const busy &mcguffin)
{
	// Selecting the sole selection is going to deselect it.

	if (layout_manager->selected(lock, i))
	{
		layout_manager->selected(lock, i, false);
		return;
	}

	for (auto n=layout_manager->size(); n > 0; )
	{
		--n;

		if (n != i)
			layout_manager->selected(lock, n, false);
	}

	layout_manager->selected(lock, i, true);
}

void multiple_selection_type(list_lock &lock,
			     const listlayoutmanager &layout_manager,
			     size_t i,
			     const busy &mcguffin)
{
	layout_manager->selected(lock, i,
				 !layout_manager->selected(lock, i));
}

LIBCXXW_NAMESPACE_END
