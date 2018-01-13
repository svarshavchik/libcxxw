/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "draw_info.H"
#include "background_color.H"

#include "listlayoutmanager/listlayoutstyle_impl.H"
#include "listlayoutmanager/list_element_impl.H"
#include "listlayoutmanager/list_celltext.H"
#include "listlayoutmanager/list_cellimage.H"
#include "listlayoutmanager/list_cellseparator.H"
#include "listlayoutmanager/listlayoutmanager_impl.H"
#include "listlayoutmanager/list_container_impl.H"
#include "listlayoutmanager/list_cell.H"
#include "peepholed_listcontainer_impl_element.H"
#include "peephole/peephole_impl.H"
#include "popup/popup.H"
#include "popup/popup_attachedto_handler.H"
#include "menu/menubarlayoutmanager_impl.H"
#include "richtext/richtextstring.H"
#include "icon.H"
#include "messages.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/text_paramfwd.H"
#include "x/w/shortcut.H"
#include <x/visitor.H>

LIBCXXW_NAMESPACE_START

void textlist_rowinfo::setting_menu_item()
{
	if (std::holds_alternative<std::monostate>(menu_item))
		return;

	throw EXCEPTION(_("Cannot specify duplicate or conflicting list item "
			  "attributes"));
}

std::tuple<container, peepholed, focusable, focusable_impl>
listlayoutstyle_impl
::create(const ref<peepholeObj::implObj> &peephole_parent,
	 const new_listlayoutmanager &style) const
{
	// A container that uses the listlayoutmanager.

	auto internal_listcontainer_impl=
		ref<peepholed_listcontainer_impl_elementObj
		    <list_container_implObj>>
		::create(style, peephole_parent);

	// The single element in the container, used to draw the list, and
	// its public object.

	auto t_impl=ref<list_elementObj::implObj>
		::create(internal_listcontainer_impl, style);

	auto t=list_element::create(t_impl);

	// Layout manager implementation object...

	auto internal_listlayoutmanager_impl=
		ref<listlayoutmanagerObj::implObj>::create
		(internal_listcontainer_impl, t);

	/// ... which is registered as the container for the peepholed object.
	auto container=peepholed_listcontainer::create
		(internal_listcontainer_impl,
		 t_impl,
		 internal_listcontainer_impl,
		 internal_listlayoutmanager_impl);

	t->show();

	return {container, container, container, t_impl};
}

void
listlayoutstyle_impl::create_cells(const std::vector<list_item_param> &t,
				   list_elementObj::implObj &textlist_element,
				   std::vector<list_cell> &newcells,
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
				[this](const menuoption &mo)
				{
					menu_attribute_requested();
				},
				[this](const submenu &sm)
				{
					menu_attribute_requested();
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
				 if (std::holds_alternative<menu_item_submenu>
				     (next_rowinfo.menu_item))
				 {
					 if (next_rowinfo.listitem_shortcut ||
					     next_rowinfo.listitem_callback)
						 throw EXCEPTION
							 (_("Cannot specify "
							    "shortcuts or "
							    "callbacks for "
							    "sub-menus"));
				 }

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

				 // We must push a separator object for each
				 // column. We cannot just link the same object
				 // into all columns. The logic dealing with
				 // column_iterators expects each cell in each
				 // column to be a discrete object, for
				 // recording the column_iterator.

				 for (size_t i=0; i<textlist_element.columns;
				      ++i)
					 newcells.push_back
						 (list_cellseparator::create());

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

	if (rowmeta.size() * textlist_element.columns != newcells.size())
		throw EXCEPTION(_("I suck at logic."));
}

void listlayoutstyle_impl::do_process_list_item_param
(const list_item_param::variant_t &item,
 list_elementObj::implObj &textlist_element,
 halign alignment,
 textlist_rowinfo &next_rowinfo,
 const function<void (const list_cell &)>&item_callback,
 const function<void ()> &separator_callback) const
{
	std::visit(visitor
		   {
			   [&](const shortcut &sc)
			   {
				   if (next_rowinfo.listitem_shortcut)
					   throw EXCEPTION
						   (_("Cannot specify multiple "
						      "shorcuts for list items")
						    );

				   next_rowinfo.listitem_shortcut=&sc;
			   },
			   [&](const list_item_status_change_callback &cb)
			   {
				   if (next_rowinfo.listitem_callback)
					   throw EXCEPTION
						   (_("Cannot specify multiple "
						      "callbacks for list "
						      "items"));
				   next_rowinfo.listitem_callback=&cb;
			   },
			   [&](const menuoption &mo)
			   {
				   next_rowinfo.setting_menu_item();
				   next_rowinfo.menu_item=menu_item_option{};
			   },
			   [&](const submenu &sm)
			   {
				   next_rowinfo.setting_menu_item();
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

				   auto t=list_celltext
					   ::create(rts, alignment, 0);

				   item_callback(t);
			   },
			   [&](const image_param &s)
			   {
				   auto i=textlist_element
					   .get_window_handler()
					   .create_icon({s});

				   auto t=list_cell
					   (list_cellimage::create
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

void listlayoutstyle_impl::menu_attribute_requested() const
{
	throw EXCEPTION(_("menuoption and submenu attributes are allowed "
			  "only in menus"));
}

////////////////////////////////////////////////////////////////////////////
//
// Highlighted list style.

class LIBCXX_HIDDEN highlighted_listlayoutstyle_impl
	: public listlayoutstyle_impl {

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

	list_cell create_leading_column(list_elementObj::implObj
					    &textlist_element,
					    size_t column_number)
		const override
	{
		throw EXCEPTION("Should not get here.");
	}

	list_cell create_trailing_column(list_elementObj::implObj
					     &textlist_element,
					     size_t column_number,
					     const textlist_rowinfo &info)
		const override
	{
		throw EXCEPTION("Should not get here.");
	}

	void selected_changed(list_cell *row,
			      bool selected_flag) const override
	{
	}
};

static const
highlighted_listlayoutstyle_impl textlistlayout_style_instance;

const listlayoutstyle_impl &highlighted_list=textlistlayout_style_instance;

////////////////////////////////////////////////////////////////////////////
//
// Bulleted list style.

class LIBCXX_HIDDEN bulleted_listlayoutstyle_impl
	: public listlayoutstyle_impl {

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

	list_cell create_leading_column(list_elementObj::implObj
					    &textlist_element,
					    size_t column_number)
		const override
	{
		return list_cellimage::create(std::vector<icon>{
				textlist_element.bullet1,
					textlist_element.bullet2},
			halign::center);
	}

	list_cell create_trailing_column(list_elementObj::implObj
					     &textlist_element,
					     size_t column_number,
					     const textlist_rowinfo &info)
		const override
	{
		throw EXCEPTION("Should not get here.");
	}

	// Update the bullet element.
	void selected_changed(list_cell *row,
			      bool selected_flag) const override
	{
		list_cellimage i=*row;

		*mpobj<size_t>::lock{i->n}=selected_flag ? 1:0;
	}
};

static const
bulleted_listlayoutstyle_impl bulleted_style_instance;

const listlayoutstyle_impl &bulleted_list=bulleted_style_instance;

//////////////////////////////////////////////////////////////////////////////
//
// Menu list style.
//
// Based on the bulleted style, but with an additional column,
// containing the shortcut or the submenu indicator

class LIBCXX_HIDDEN menu_list_style_impl
	: public bulleted_listlayoutstyle_impl {

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

	list_cell create_trailing_column(list_elementObj::implObj
					 &textlist_element,
					 size_t column_number,
					 const textlist_rowinfo &info)
		const override
	{
		if (std::holds_alternative<menu_item_submenu>(info.menu_item))
		{
			auto i=textlist_element
				.get_window_handler()
				.create_icon({"submenu"});

			return list_cellimage::create
				(std::vector<icon>{i}, halign::left);
		}

		std::u32string s;

		if (info.listitem_shortcut)
			s=*info.listitem_shortcut;

		auto rt=textlist_element
			.create_richtextstring(textlist_element
					       .itemshortcut_meta, s);

		return list_celltext::create(rt, halign::left, 0);
	}

	void menu_attribute_requested() const override
	{
	}
};

static const
menu_list_style_impl menu_list_instance;

const listlayoutstyle_impl &menu_list=menu_list_instance;

LIBCXXW_NAMESPACE_END
