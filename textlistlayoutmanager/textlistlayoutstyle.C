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
#include "textlistlayoutmanager/textlistlayoutmanager_impl.H"
#include "textlistlayoutmanager/textlist_container_impl.H"
#include "textlistlayoutmanager/textlist_cell.H"
#include "peepholed_listcontainer_impl_element.H"
#include "peephole/peephole_impl.H"
#include "popup/popup.H"
#include "popup/popup_attachedto_handler.H"
#include "menu/menubarlayoutmanager_impl.H"
#include "richtext/richtextstring.H"
#include "icon.H"
#include "messages.H"
#include "x/w/textlistlayoutmanager.H"
#include "x/w/text_paramfwd.H"
#include "x/w/shortcut.H"
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

void
textlistlayout_style_impl::create_cells(const std::vector<list_item_param> &t,
					textlistObj::implObj &textlist_element,
					std::vector<textlist_cell> &newcells,
					std::vector<textlist_rowinfo> &rowmeta)
	const
{
	const size_t extra_leading=extra_leading_columns();
	const size_t extra_trailing=extra_trailing_columns();
	const size_t extra=extra_leading+extra_trailing;

	size_t n_real_elements=0;

	if (textlist_element.columns <= extra)
		throw EXCEPTION("Internal error: attempting to initialize a list with too few columns");

	const size_t real_columns=textlist_element.columns-extra;

	for (const auto &item:t)
	{
		std::visit(visitor{
				[&](const shortcut &sc)
				{
				},
				[&](const list_item_status_change_callback &cb)
				{
				},
				[&](const menuoption &mo)
				{
				},
				[&](const submenu &sm)
				{
				},
				[&](const separator &)
				{
					if (n_real_elements % real_columns)
						throw EXCEPTION("list separator must be specified by itself");
					n_real_elements += real_columns;
				},
				[&](const text_param &)
				{
					++n_real_elements;
				},
				[&](const image_param &)
				{
					++n_real_elements;
				}},
			static_cast<const list_item_param::variant_t &>(item));
	}

	// We expect the number of new items to be evenly divisible by the
	// number of columns in the least (less any leading columns we add
	// internally).
	if (n_real_elements % real_columns)
		throw EXCEPTION(_("Number of text strings does not correspond to the number of columns in the list."));

	// Since its evenly divisible, compute the number of rows, then
	// multiply by the number of columns for each item, to figure out how
	// much space to reserve.
	newcells.reserve(n_real_elements / real_columns
			 * textlist_element.columns);
	rowmeta.reserve(n_real_elements / real_columns);

	size_t c=0;

	textlist_rowinfo next_rowinfo;
	bool havemeta=false;

	for (const auto &s:t)
	{
		halign alignment=halign::left;

		// Was this column's alignment specifically requested?
		auto col_alignment=textlist_element
			.col_alignments.find(c % textlist_element.columns);

		if (col_alignment != textlist_element
		    .col_alignments.end())
			alignment=col_alignment->second;

		// Create the cell.

		bool created_cell=false;
		bool created_separator=false;

		process_list_item_param
			(s,
			 textlist_element,
			 alignment,
			 next_rowinfo,
			 [&, this]
			 (const auto &new_cell)
			 {
				 bool first_column=
					 (c % textlist_element.columns) == 0;

				 if (first_column)
				 {
					 // First column on each row.
					 // Add leading cells.

					 for (size_t i=0; i<extra_leading; ++i)
					 {
						 newcells.push_back
							 (create_leading_column
							  (textlist_element,
							   i));
						 ++c;
					 }
				 }

				 newcells.push_back(new_cell);
				 created_cell=true;
			 },
			 [&]
			 {
				 bool first_column=
					 (c % textlist_element.columns) == 0;

				 // The preliminary pass that reserved all
				 // elements should've checked this already.
				 if (!first_column)
					 throw EXCEPTION(_("Internal error: "
							   "separator element "
							   "is out of place"));

				 if (havemeta)
					 throw EXCEPTION(_("Cannot specify "
							   "any other "
							   "attributes for a "
							   "separator element")
							 );

				 auto empty_str=textlist_element
					 .create_richtextstring
					 (textlist_element.itemlabel_meta, "");
				 auto empty_cell=
					 textlist_celltext::create(empty_str,
								   halign::left,
								   0);

				 for (size_t i=0; i<textlist_element.columns;
				      ++i)
					 newcells.push_back(empty_cell);

				 // next_rowinfo should be clean
				 rowmeta.push_back(next_rowinfo);
				 c += textlist_element.columns;
				 created_separator=true;
			 });

		if (created_separator)
			continue;

		if (created_cell)
		{
			++c;

			// Processed a column value. If this is the end of
			// the row, save the metadata.

			if ((c % textlist_element.columns) ==
			    (textlist_element.columns - extra_trailing))
			{
				// Last column. Add trailing cells.

				for (size_t i=0; i<extra_trailing; ++i)
				{
					newcells.push_back
						(create_trailing_column
						 (textlist_element, i,
						  next_rowinfo));
					++c;
				}
			}

			if ((c % textlist_element.columns) == 0)
			{
				rowmeta.push_back(next_rowinfo);
				next_rowinfo={};
				havemeta=false;
			}
		}
		else
		{
			// Metadata must be specified only at the beginning
			// of the row.
			if ((c % textlist_element.columns) != 0)
				throw EXCEPTION(_("Row metadata must be "
						  "specified before the "
						  "row data."));
			havemeta=true;
		}

	}

	if (havemeta)
		throw EXCEPTION(_("Row metadata must be specified before "
				  "the row data."));

	// TODO:
	if (rowmeta.size() * textlist_element.columns != newcells.size())
		throw EXCEPTION(_("I suck at logic."));
}

void textlistlayout_style_impl::do_process_list_item_param
(const list_item_param::variant_t &item,
 textlistObj::implObj &textlist_element,
 halign alignment,
 textlist_rowinfo &next_rowinfo,
 const function<void (const textlist_cell &)>&item_callback,
 const function<void ()> &separator_callback) const
{
	std::visit(visitor
		   {
			   [&](const shortcut &sc)
			   {
				   next_rowinfo.listitem_shortcut=&sc;
			   },
			   [&](const list_item_status_change_callback &cb)
			   {
				   next_rowinfo.listitem_callback=&cb;
			   },
			   [&](const menuoption &mo)
			   {
				   next_rowinfo.menu_item=menu_item_option{};
			   },
			   [&](const submenu &sm)
			   {
				   auto ret=menubarlayoutmanagerObj::implObj
					   ::create_popup_menu
					   (elementimpl(&textlist_element),
					    sm.creator,
					    attached_to::submenu_next);

				   next_rowinfo.menu_item=menu_item_submenu{
					   std::get<0>(ret),
					   std::get<1>(ret)
				   };
			   },
			   [&](const text_param &s)
			   {
				   auto rts=textlist_element
					   .create_richtextstring
					   (textlist_element.itemlabel_meta, s);

				   auto t=textlist_celltext
					   ::create(rts, alignment, 0);

				   item_callback(t);
			   },
			   [&](const image_param &s)
			   {
				   auto i=textlist_element
					   .get_window_handler()
					   .create_icon_mm(s.c_str(),
							   render_repeat
							   ::none,
							   0,0);

				   auto t=textlist_cell
					   (textlist_cellimage::create
					    (std::vector<icon>{i},
					     alignment));
				   item_callback(t);
			   },
			   [&](const separator &)
			   {
				   separator_callback();
			   }
		   }, item);
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

	size_t extra_trailing_columns() const override
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

	textlist_cell create_trailing_column(textlistObj::implObj
					     &textlist_element,
					     size_t column_number,
					     const textlist_rowinfo &info)
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

const textlistlayout_style_impl &int_highlighted_list_style=
	textlistlayout_style_instance;

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

	size_t extra_trailing_columns() const override
	{
		return 0;
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

	textlist_cell create_trailing_column(textlistObj::implObj
					     &textlist_element,
					     size_t column_number,
					     const textlist_rowinfo &info)
		const override
	{
		throw EXCEPTION("Should not get here.");
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

//////////////////////////////////////////////////////////////////////////////
//
// Menu list style.
//
// Based on the bulleted style, but with an additional column,
// containing the shortcut or the submenu indicator

class LIBCXX_HIDDEN menu_list_style_impl
	: public bulleted_textlistlayout_style_impl {

	// Two extra columns: bullet and shortcut/submenu indicator

	size_t actual_columns(const new_listlayoutmanager &style)
		const override
	{
		return style.columns+2;
	}

	size_t extra_trailing_columns() const override
	{
		return 1;
	}

	// This creates the rightmost column in a dropdown menu that shows
	// either the submenu icon, or the menu item's shortcut, if it is
	// set.

	textlist_cell create_trailing_column(textlistObj::implObj
					     &textlist_element,
					     size_t column_number,
					     const textlist_rowinfo &info)
		const override
	{
		if (std::holds_alternative<menu_item_submenu>(info.menu_item))
		{
			auto i=textlist_element
				.get_window_handler()
				.create_icon_mm("submenu",
						render_repeat::none,
						0, 0);

			return textlist_cellimage::create
				(std::vector<icon>{i}, halign::left);
		}

		std::u32string s;

		if (info.listitem_shortcut)
			s=*info.listitem_shortcut;

		auto rt=textlist_element
			.create_richtextstring(textlist_element
					       .itemshortcut_meta, s);

		return textlist_celltext::create(rt, halign::left, 0);
	}
};

static const
menu_list_style_impl menu_style_instance;

const textlistlayout_style_impl &int_menu_list_style=
	menu_style_instance;

LIBCXXW_NAMESPACE_END
