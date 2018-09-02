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
#include "x/w/canvas.H"
#include "listlayoutmanager/listlayoutmanager_impl.H"
#include "listlayoutmanager/listlayoutstyle_impl.H"
#include "gridlayoutmanager.H"
#include "x/w/impl/focus/focusable.H"
#include "peephole/peepholed.H"
#include "peephole/peephole_impl.H"
#include "peepholed_focusable.H"
#include "capturefactory.H"
#include "x/w/impl/background_color.H"
#include "x/w/impl/container.H"
#include "x/w/impl/theme_font_element.H"
#include "x/w/impl/container_element.H"
#include "x/w/impl/always_visible_element.H"
#include "x/w/impl/borderlayoutmanager.H"
#include "x/w/impl/bordercontainer_element.H"

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

//! Override border layout manager, and don't draw anything.

//! We need to do more work to seamlessly align the header rows with
//! the list columns. synchronized_axis takes care of harmonizing their
//! metrics. However the list peephole has a focus frame around it, and
//! we need to account for it.
//!
//! What we do is put the header inside a border layout manager, using the
//! same border on the left and the right side as the list's focus frame,
//! and no top or bottom border.
//!
//! We need to make sure that the border does not get drawn, though, so we
//! use this subclass for that.

class LIBCXX_HIDDEN listheaderinvisibleborderObj
	: public borderlayoutmanagerObj::implObj {

public:

	using implObj::implObj;

	void do_draw(ONLY IN_THREAD,
		     const draw_info &di,
		     clip_region_set &clip,
		     rectangle_set &drawn_areas) override
	{
		// superclass_t is inherited from the parent. This bypasses
		// the parent code.

		superclass_t::do_draw(IN_THREAD, di, clip, drawn_areas);
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
	  vertical_scrollbar{scrollbar_visibility::automatic},
	  background_color{"list_background_color"},
	  selected_color{"list_selected_color"},
	  highlighted_color{"list_highlighted_color"},
	  current_color{"list_current_color"},
	  header_color{"list_header_color"},
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
		  focusoff_border,
		  focuson_border,
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
			       ]=this->list_style.create(peephole_impl,
							 *this,
							 synchronized_columns);

			 container_element->show();

			 internal_listcontainer=container_element;

			 return std::make_tuple(peephole_impl,
						peepholed_element,
						focusable_element,
						focusable_element_impl);
		 });

	// Create the header row, if asked.

	if (header_factory)
	{
		// We need to have the header row lined up with the list
		// columns. synchronized_axis does bulk of the work, but we
		// need to also faithfully reproduce, in the header row, the
		// additional bordering that gets created inside
		// create_peepholed_focusable_with_frame. This bordering
		// consists of:
		//
		// 1. A list_border around the peephole.
		//
		// 2. A focus frame inside it.
		//
		// What we'll do is recreate the same structure in the
		// header row, in order to balance everything out.

		auto f=lm->insert_row(0);

		// We don't need any extra padding from the outer grid.
		f->padding(0);

		// And make sure it's left-aligned
		f->halign(halign::left);

		// Create a replica list border.
		child_element_init_params header_init_params;
		header_init_params.background_color=header_color;

		auto parent_container=f->get_container_impl();

		auto header_border_container_impl=
			ref<bordercontainer_elementObj<
				always_visible_elementObj<container_elementObj<
					child_elementObj>>>>
			::create(list_border, list_border,
				 list_border, "empty", 0, 0,
				 parent_container,
				 header_init_params);

		// Now create a (not quite a) replicable of the focus frame.

		auto header_focusframe_container_impl=
			ref<bordercontainer_elementObj<
				always_visible_elementObj<container_elementObj<
					child_elementObj>>>>
			::create(focusoff_border, focusoff_border,
				 "empty", "empty", 0, 0,
				 header_border_container_impl);

		// We now create the actual header row element.

		new_gridlayoutmanager nglm;

		nglm.synchronized_columns=synchronized_columns;

		// Container implementation object for the header row.

		auto header_container_impl=
			ref<always_visible_elementObj<container_elementObj
						      <child_elementObj>>>
			::create(header_focusframe_container_impl,
				 header_init_params);

		ref<gridlayoutmanagerObj::implObj> gridlayoutmanager_impl=
			nglm.create(header_container_impl);

		auto glm=gridlayoutmanager_impl->create_gridlayoutmanager();

		auto hf=glm->append_row();

		// Must use the padding logic as the
		// actual list.

		for (size_t i=0; i<columns; ++i)
		{
			auto cf=capturefactory::create
				(hf->get_container_impl());

			header_factory(cf, i);

			auto left_padding=h_padding;
			auto right_padding=h_padding;

			auto lr_iter=lr_paddings.find(i);

			if (lr_iter != lr_paddings.end())
			{
				auto &[l, r]=lr_iter->second;
				left_padding=l;
				right_padding=r;
			}

			hf->left_padding(left_padding);
			hf->right_padding(right_padding);

			// Also use same borders.

			if (i > 0)
			{
				auto iter=column_borders.find(i);
				if (iter!=column_borders.end())
					hf->left_border(iter->second);
			}

			// And use the same alignment

			auto align_iter=col_alignments.find(i);

			if (align_iter != col_alignments.end())
				hf->halign(align_iter->second);

			hf->created_internally(cf->get());
		}

		auto header=container::create(header_container_impl,
					      gridlayoutmanager_impl);

		// We can now create the invisible focusframe in the header
		// row that balances the real estate from the real focus frame
		// around the list peephole.

		auto header_focusframe_container_impl_lm=
			ref<listheaderinvisibleborderObj>
			::create(header_focusframe_container_impl,
				 header_focusframe_container_impl,
				 header,
				 halign::fill,
				 valign::fill);

		auto header_focusframe_container=
			container::create(header_focusframe_container_impl,
					  header_focusframe_container_impl_lm);

		// And now the list border frame around it, to balance out
		// the list border around the focus frame.

		auto header_border_container_impl_lm=
			ref<borderlayoutmanagerObj::implObj>
			::create(header_border_container_impl,
				 header_border_container_impl,
				 header_focusframe_container,
				 halign::fill,
				 valign::fill);

		auto header_border_container=
			container::create(header_border_container_impl,
					  header_border_container_impl_lm);

		f->created_internally(header_border_container);

		// Need to create a spacer for the vertical scrollbar.
		f->padding(0);
		f->create_canvas()->show();
	}
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

const list_selection_type_cb_t no_selection_type=
	[](ONLY IN_THREAD,
	   const listlayoutmanager &layout_manager,
	   size_t i,
	   const callback_trigger_t &trigger,
	   const busy &mcguffin)
{
};

LIBCXXW_NAMESPACE_END
