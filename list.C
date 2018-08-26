/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/listlayoutmanager.H"
#include "x/w/focusable_container.H"
#include "x/w/peepholed_focusableobj.H"
#include "x/w/rgb.H"
#include "x/w/synchronized_axis.H"
#include "listlayoutmanager/listlayoutmanager_impl.H"
#include "listlayoutmanager/listlayoutstyle_impl.H"
#include "gridlayoutmanager.H"
#include "x/w/impl/focus/focusable.H"
#include "peephole/peepholed.H"
#include "peephole/peephole_impl.H"
#include "peepholed_focusable.H"
#include "x/w/impl/background_color.H"
#include "x/w/impl/container.H"
#include "x/w/impl/theme_font_element.H"
#include "x/w/impl/container_element.H"
#include "x/w/impl/nonrecursive_visibility.H"

#include <tuple>
#include <utility>

LIBCXXW_NAMESPACE_START

namespace {
#if 0
}
#endif

//! The container returned by create_list().

//! create_list() returned this subclass of focusable_containerObj.
//!
//! Overrides get_layout_impl() to return the layout manager from the
//! peepholed listcontainer, as the alleged layout manager for this element.
//!
//! The internal listcontainer gets passed to the constructor and saved here.

class LIBCXX_HIDDEN listObj : public peepholed_focusableObj {

 public:

	const container internal_listcontainer;

	listObj(const container &internal_listcontainer,
		const ref<peepholed_focusableObj::implObj> &impl,
		const layout_impl &container_layout_impl)
		: peepholed_focusableObj{impl, container_layout_impl},
		internal_listcontainer{internal_listcontainer}
		{
		}

	~listObj()=default;

	layout_impl get_layout_impl() const override
	{
		return internal_listcontainer->get_layout_impl();
	}
};
#if 0
{
#endif
}

///////////////////////////////////////////////////////////

new_listlayoutmanager
::new_listlayoutmanager(const listlayoutstyle_impl &list_style)
	: list_style{list_style},
	  selection_type{single_selection_type},
	  height_value{std::tuple<size_t, size_t>{4,4}},
	  columns{1},
	  synchronized_columns{synchronized_axis::create()},
	  list_border{"list_border"},
	  v_padding{"list_v_padding"},
	  left_padding{"list_left_padding"},
	  inner_padding{"list_inner_padding"},
	  right_padding{"list_right_padding"},
	  vertical_scrollbar{scrollbar_visibility::automatic},
	  background_color{"list_background_color"},
	  selected_color{"list_selected_color"},
	  highlighted_color{"list_highlighted_color"},
	  current_color{"list_current_color"},
	  list_font{theme_font{"list"}}
{
}

new_listlayoutmanager::~new_listlayoutmanager()=default;

focusable_container
new_listlayoutmanager::create(const container_impl
			      &parent_container) const
{
	auto focusable_container_impl=
		ref<peepholed_container_impl_t>::create(parent_container);

	containerptr internal_listcontainer;

	peephole_style list_peephole_style{halign::fill};

	auto [peephole_info, lm]=create_peepholed_focusable_with_frame
		({list_border,
				"inputfocusoff_border",
				"inputfocuson_border",
				0,
				background_color,
				focusable_container_impl,
				list_peephole_style,
				scrollbar_visibility::never,
				vertical_scrollbar},
		 [&, this]
		 (const container_impl &peepholed_parent)
		 {
			 auto peephole_impl=ref<peepholeObj::implObj>
			 ::create(peepholed_parent);

			 auto [container_element,
			       peepholed_element,
			       focusable_element,
			       focusable_element_impl
			       ]=this->list_style.create(peephole_impl, *this);

			 container_element->show();

			 internal_listcontainer=container_element;

			 return std::make_tuple(peephole_impl,
						peepholed_element,
						focusable_element,
						focusable_element_impl);
		 });

	return ref<listObj>::create(internal_listcontainer, peephole_info,
				    lm->impl);
}

////////////////////////////////////////////////////////////////////////////

// Implementation functions for the stock list selection types.

const list_selection_type_cb_t single_selection_type=
	[]
	(ONLY IN_THREAD,
	 const listlayoutmanager &layout_manager,
	 size_t i,
	 const callback_trigger_t &trigger,
	 const busy &mcguffin)
{
	if (layout_manager->selected(i))
		return; // Already selected it.

	layout_manager->unselect(IN_THREAD);
	layout_manager->selected(IN_THREAD, i, true, trigger);
};

const list_selection_type_cb_t single_optional_selection_type=
	[]
	(ONLY IN_THREAD,
	 const listlayoutmanager
	 &layout_manager,
	 size_t i,
	 const callback_trigger_t &trigger,
	 const busy &mcguffin)
{
	// Selecting the sole selection is going to deselect it.

	if (layout_manager->selected(i))
	{
		layout_manager->selected(IN_THREAD, i, false, trigger);
		return;
	}

	layout_manager->unselect(IN_THREAD);
	layout_manager->selected(IN_THREAD, i, true, trigger);
};

const list_selection_type_cb_t multiple_selection_type=
	[](ONLY IN_THREAD,
	   const listlayoutmanager &layout_manager,
	   size_t i,
	   const callback_trigger_t &trigger,
	   const busy &mcguffin)
{
	layout_manager->selected(IN_THREAD, i, !layout_manager->selected(i),
				 trigger);
};


LIBCXXW_NAMESPACE_END
