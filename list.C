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
#include "peephole/peephole_impl_element.H"
#include "peepholed_focusable.H"
#include "messages.H"
#include "x/w/impl/background_color.H"
#include "x/w/impl/container.H"
#include "x/w/impl/container_element.H"
#include "x/w/impl/theme_font_element.H"
#include <x/visitor.H>
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
	  focusoff_border{"listfocusoff_border"},
	  focuson_border{"listfocuson_border"},
	  v_padding{"list_v_padding"},
	  h_padding{"list_h_padding"},
	  indent{"list_indent"},
	  horizontal_scrollbar{scrollbar_visibility::automatic},
	  vertical_scrollbar{scrollbar_visibility::automatic},
	  background_color{"list_background_color"},
	  selected_color{"list_selected_color"},
	  highlighted_color{"list_highlighted_color"},
	  current_color{"list_current_color"},
	  list_font{theme_font{"list"}},
	  list_separator_border{"list_separator_border"}
{
}

new_listlayoutmanager::~new_listlayoutmanager()=default;

void new_listlayoutmanager::visible_focusoff_border()
{
	focusoff_border="listvisiblefocusoff_border";
}

focusable_container
new_listlayoutmanager::create(const container_impl &parent_container) const
{
	auto create_list_element_impl=
		make_function< ref<list_elementObj::implObj>
			       (const list_element_impl_init_args &)>
		([]
		 (const auto &init_args)
		 {
			 return ref<list_elementObj::implObj>
				 ::create(init_args);
		 });
	auto create_listlayoutmanager_impl=
		make_function< ref<listlayoutmanagerObj::implObj>
			       (const ref<listcontainer_pseudo_implObj> &,
				const list_element &)>
		([]
		 (const ref<listcontainer_pseudo_implObj> &container_impl,
		  const list_element &list_element_singleton)
		 {
			 return ref<listlayoutmanagerObj::implObj>
				 ::create(container_impl,
					  list_element_singleton);
		 });

	list_create_info lci{create_list_element_impl,
			     create_listlayoutmanager_impl};

	return create_impl(parent_container, synchronized_columns,
			   nullptr, lci);
}

focusable_container
new_listlayoutmanager::create_impl(const container_impl &parent_container,
				   const synchronized_axis
				   &list_synchronized_columns,
				   table_create_info *tci,
				   const list_create_info &lci) const
{

	auto focusable_container_impl=
		ref<peepholed_container_impl_t>::create(parent_container);

	containerptr internal_listcontainer;

	peephole_style list_peephole_style{halign::fill, valign::fill};

	if (width_value)
		list_peephole_style.width_algorithm=*width_value;
	else
	{
		list_peephole_style.width_algorithm=
			peephole_algorithm::stretch_peephole;
	}

	auto vertical_scrollbar_value=vertical_scrollbar;
	std::visit(visitor{
			[&]
			(const std::tuple<size_t, size_t> &height)
			{
				const auto &[min, max]=height;

				if (min <= 0)
					throw EXCEPTION
						(_("Cannot create a list with 0"
						   " visible rows."));

				if (max < min)
					throw EXCEPTION(_("Cannot have maximum"
							  " number of rows "
							  "less than the "
							  "minimum."));

				list_peephole_style.height_algorithm=height;

				if (min < max &&
				    vertical_scrollbar_value==
				    scrollbar_visibility::automatic)
				{
					vertical_scrollbar_value=
						scrollbar_visibility
						::automatic_reserved;
				}
			},
			[&]
			(const dim_axis_arg &height)
			{
				list_peephole_style.height_algorithm=height;
			}},
		height_value);

	auto [peephole_info, lm]=create_peepholed_focusable_with_frame
		({list_border,
		  focusoff_border,
		  focuson_border,
		  0,
		  background_color,
		  focusable_container_impl,
		  list_peephole_style,
		  horizontal_scrollbar,
		  vertical_scrollbar_value
		}, [&, this]
			(const container_impl &peepholed_parent,
			 const container_impl &layout_container_impl)
		   {
			   created_list_container(layout_container_impl, tci);

			   auto peephole_impl=ref<peephole_impl_elementObj<
				   container_elementObj<child_elementObj>>>
				   ::create(peepholed_parent);

			   auto [container_element,
				 peepholed_element,
				 focusable_element,
				 focusable_element_impl
				 ]=this->list_style.create
				   (peephole_impl,
				    *this,
				    lci,
				    list_synchronized_columns);

			   container_element->show();

			   internal_listcontainer=container_element;

			   return std::make_tuple(peephole_impl,
						  peepholed_element,
						  focusable_element,
						  focusable_element_impl);
		 });

	create_table_header_row(lm, tci);

	return ref<listObj>::create(internal_listcontainer, peephole_info,
				    lm->impl);
}

void new_listlayoutmanager::created_list_container(const container_impl &,
						   table_create_info *)
	const
{
}

void new_listlayoutmanager::create_table_header_row(const gridlayoutmanager &,
						    table_create_info *)
	const
{
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

const list_selection_type_cb_t no_selection_type=
	[](ONLY IN_THREAD,
	   const listlayoutmanager &layout_manager,
	   size_t i,
	   const callback_trigger_t &trigger,
	   const busy &mcguffin)
{
};

LIBCXXW_NAMESPACE_END
