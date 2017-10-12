/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "draw_info.H"
#include "background_color.H"

#include "textlistlayoutmanager/textlistlayoutstyle_impl.H"
#include "textlistlayoutmanager/textlist_impl.H"
#include "textlistlayoutmanager/textlist_celltext.H"
#include "textlistlayoutmanager/textlist_cellimage.H"
#include "peepholed_listcontainer_impl_element.H"
#include "peephole/peephole_impl.H"
#include "textlistlayoutmanager/textlistlayoutmanager_impl.H"
#include "textlistlayoutmanager/textlist_container_impl.H"
#include "textlistlayoutmanager/textlist_impl.H"
#include "textlistlayoutmanager/textlist_cell.H"
#include "richtext/richtextmeta.H"
#include "richtext/richtextstring.H"
#include "icon.H"
#include "messages.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/text_paramfwd.H"

#include <x/visitor.H>

LIBCXXW_NAMESPACE_START

std::tuple<container, peepholed, focusable, focusable_impl>
textlistlayout_style_impl
::create(const ref<peepholeObj::implObj> &peephole_parent,
	 const new_listlayoutmanager &style) const
{
	// A container that uses the textlistlayoutmanager.

	auto internal_listcontainer_impl=
		ref<peepholed_listcontainer_impl_elementObj
		    <textlist_container_implObj>>
		::create(style, peephole_parent);

	// The single element in the container, used to draw the list, and
	// its public object.

	auto t_impl=ref<textlistObj::implObj>
		::create(internal_listcontainer_impl, style, *this);

	auto t=textlist::create(t_impl);

	// Layout manager implementation object...

	auto internal_textlistlayoutmanager_impl=
		ref<textlistlayoutmanagerObj::implObj>::create
		(internal_listcontainer_impl, t);

	/// ... which is registered as the container for the peepholed object.
	auto container=peepholed_listcontainer::create
		(internal_listcontainer_impl,
		 t_impl,
		 internal_listcontainer_impl,
		 internal_textlistlayoutmanager_impl);

	t->show();

	return {container, container, container, t_impl};
}


std::vector<textlist_cell>
textlistlayout_style_impl::create_cells(const std::vector<list_item_param> &t,
					textlistObj::implObj &textlist_element)
	const
{
	const size_t extra=extra_leading_columns();

	// We expect the number of new items to be evenly divisible by the
	// number of columns in the least (less any leading columns we add
	// internally).
	if (textlist_element.columns <= extra ||
	    (t.size() % (textlist_element.columns-extra)))
		throw EXCEPTION(_("Number of text strings does not correspond to the number of columns in the list."));

	std::vector<textlist_cell> newcells;

	// Since its evenly divisible, compute the number of rows, then
	// multiply by the number of columns for each item, to figure out how
	// much space to reserve.
	newcells.reserve(t.size() / (textlist_element.columns - extra)
			 * textlist_element.columns);

	// Default metadata for the text.

	richtextmeta default_meta{
		textlist_element.create_background_color
			("label_foreground_color"),
			textlist_element.create_theme_font
			(textlist_element.label_theme_font())};

	size_t c=0;

	for (const auto &s:t)
	{
		if ((c % textlist_element.columns) == 0)
		{
			// First column on each row. Add leading cells.

			for (size_t i=0; i<extra; ++i)
			{
				newcells.push_back(create_leading_column
						   (textlist_element, i));
				++c;
			}
		}

		halign alignment=halign::left;

		// Was this column's alignment specifically requested?
		auto col_alignment=textlist_element
			.col_alignments.find(c % textlist_element.columns);

		if (col_alignment != textlist_element
		    .col_alignments.end())
			alignment=col_alignment->second;

		// Create the cell.

		auto t=std::visit
			(visitor {
				[&](const text_param &s)
				{
					auto rts=textlist_element
						.create_richtextstring
						(default_meta, s);


					return textlist_cell
						(textlist_celltext
						 ::create(rts, alignment, 0));
				},

				[&](const image_param &s)
				{
					auto i=textlist_element
						.get_window_handler()
						.create_icon_mm(s.c_str(),
								render_repeat
								::none,
								0,0);

					return textlist_cell
						(textlist_cellimage::create
						 (std::vector<icon>{i},
						  alignment));
				}
			},
				s);
		newcells.push_back(t);
		++c;
	}

	return newcells;
}

////////////////////////////////////////////////////////////////////////////
//
// Highlighted list style.

class LIBCXX_HIDDEN highlighted_textlistlayout_style_impl
	: public textlistlayout_style_impl {

	size_t actual_columns(const new_listlayoutmanager &style)
		const override
	{
		return style.columns;
	}

	std::unordered_map<size_t, int>
		actual_col_widths(const new_listlayoutmanager &style)
		const override
	{
		return style.requested_col_widths;
	}

	std::unordered_map<size_t, halign>
		actual_col_alignments(const new_listlayoutmanager &style)
		const override
	{
		return style.col_alignments;
	}

	void set_selected_background(IN_THREAD_ONLY,
				     draw_info &di,
				     const background_color &bgcolor)
		const override
	{
		di.window_background=
			bgcolor->get_current_color(IN_THREAD)->impl;
	}

	size_t extra_leading_columns() const override
	{
		return 0;
	}

	textlist_cell create_leading_column(textlistObj::implObj
					    &textlist_element,
					    size_t column_number)
		const override
	{
		throw EXCEPTION("Should not get here.");
	}

	void selected_changed(textlist_cell *row,
			      bool selected_flag) const override
	{
	}
};

static const
highlighted_textlistlayout_style_impl textlistlayout_style_instance;

const layout_style_t &text_list=textlistlayout_style_instance;

////////////////////////////////////////////////////////////////////////////
//
// Bulleted list style.

class LIBCXX_HIDDEN bulleted_textlistlayout_style_impl
	: public textlistlayout_style_impl {

	// One more column for the bullet.

	size_t actual_columns(const new_listlayoutmanager &style)
		const override
	{
		return style.columns+1;
	}

	// Adjust requested_col_widths accordingly.

	std::unordered_map<size_t, int>
		actual_col_widths(const new_listlayoutmanager &style)
		const override
	{
		std::unordered_map<size_t, int> actual;

		for (const auto &cw:style.requested_col_widths)
		{
			actual.insert({cw.first+1, cw.second});
		}

		return actual;
	}

	// Adjust col_alignments accordingly.
	std::unordered_map<size_t, halign>
		actual_col_alignments(const new_listlayoutmanager &style)
		const override
	{
		std::unordered_map<size_t, halign> actual;

		for (const auto &cw:style.col_alignments)
		{
			actual.insert({cw.first+1, cw.second});
		}

		return actual;
	}

	void set_selected_background(IN_THREAD_ONLY,
				     draw_info &di,
				     const background_color &bgcolor)
		const override
	{
	}


	size_t extra_leading_columns() const override
	{
		return 1;
	}

	textlist_cell create_leading_column(textlistObj::implObj
					    &textlist_element,
					    size_t column_number)
		const override
	{
		return textlist_cellimage::create(std::vector<icon>{
				textlist_element.bullet1,
					textlist_element.bullet2},
			halign::center);
	}

	// Update the bullet element.
	void selected_changed(textlist_cell *row,
			      bool selected_flag) const override
	{
		textlist_cellimage i=*row;

		*mpobj<size_t>::lock{i->n}=selected_flag ? 1:0;
	}
};

static const
bulleted_textlistlayout_style_impl bulleted_style_instance;

const layout_style_t &bulleted_text_list=bulleted_style_instance;

LIBCXXW_NAMESPACE_END
