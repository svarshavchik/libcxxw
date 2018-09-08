/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/tablelayoutmanager.H"
#include "x/w/canvas.H"
#include "x/w/synchronized_axis.H"
#include "gridlayoutmanager.H"
#include "capturefactory.H"
#include "tablelayoutmanager/table_synchronized_axis.H"
#include "tablelayoutmanager/tablelayoutmanager_impl.H"
#include "x/w/impl/always_visible_element.H"
#include "x/w/impl/bordercontainer_element.H"
#include "x/w/impl/container_element.H"
#include "x/w/impl/borderlayoutmanager.H"

LIBCXXW_NAMESPACE_START

namespace {
#if 0
}
#endif

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

new_tablelayoutmanager
::new_tablelayoutmanager(const functionref<void (const factory &, size_t)
			 > &header_factory,
			 const listlayoutstyle_impl &list_style)
	: new_listlayoutmanager{list_style},
	  header_factory{header_factory},
	  header_color{"list_header_color"}
{
	focusoff_border="listvisiblefocusoff_border";
}

new_tablelayoutmanager::~new_tablelayoutmanager()=default;

// A table uses its own internal synchronized axis.
//
// Override create(), and pass through an opaque table_create_info pointer
// to create_impl(), which creates the list portion of the table.
// create_impl() invokes create_table_header_row(), which will grab the
// custom synchronized axis implementation from here, an duse it.

struct LIBCXX_HIDDEN new_listlayoutmanager::table_create_info {

	const table_synchronized_axis axis_impl;

	const synchronized_axis axis;

};

focusable_container
new_tablelayoutmanager::create(const container_impl &parent_container) const
{
	auto axis_impl=table_synchronized_axis::create();
	auto axis=synchronized_axis::create();

	table_create_info tci{axis_impl, axis};

	return create_impl(parent_container,
			   axis, &tci);
}

void new_tablelayoutmanager::create_table_header_row(const gridlayoutmanager
						     &lm,
						     table_create_info *tci)
	const
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

	// And make sure it's filled to its column's width.
	// Same width as the peephole with the list. The header
	// row's width is, therefore, same as the peephole's, and
	// their horizontal axis gets synchronized.
	f->halign(halign::fill);

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

	// Synchronize the horizontal axis with the list's axis.
	nglm.synchronized_columns=tci->axis;

	// Container implementation object for the header row.

	auto header_container_impl=
		ref<always_visible_elementObj<container_elementObj
					      <child_elementObj>>>
		::create(header_focusframe_container_impl,
			 header_init_params);

	ref<gridlayoutmanagerObj::implObj> gridlayoutmanager_impl=
		nglm.create(header_container_impl);

	auto glm=gridlayoutmanager_impl->create_gridlayoutmanager();

	// Have the header row's grid layout manager use same
	// requested column widths as the list.
	for (const auto &requested_col_width:requested_col_widths)
		glm->requested_col_width(requested_col_width.first,
					 requested_col_width.second);
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

	// There's an additional column for the vertical scrollbar.
	// Put a dummy spacer in there.
	f->padding(0);
	f->create_canvas()->show();
}

LIBCXXW_NAMESPACE_END
