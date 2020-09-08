/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/draw_info.H"
#include "x/w/impl/background_color.H"

#include "listlayoutmanager/listlayoutstyle_impl.H"
#include "listlayoutmanager/list_element_impl.H"
#include "listlayoutmanager/list_celltext.H"
#include "listlayoutmanager/list_cellimage.H"
#include "listlayoutmanager/list_cellseparator.H"
#include "listlayoutmanager/listlayoutmanager_impl.H"
#include "listlayoutmanager/listcontainer_pseudo_impl.H"
#include "listlayoutmanager/list_cell.H"
#include "listlayoutmanager/extra_list_row_info.H"
#include "listlayoutmanager/listitemhandle_impl.H"
#include "peephole/peephole_impl.H"
#include "popup/popup.H"
#include "popup/popup_handler.H"
#include "menu/menu_popup.H"
#include "x/w/impl/richtext/richtextstring.H"
#include "x/w/impl/icon.H"
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
	 const new_listlayoutmanager &style,
	 const new_listlayoutmanager::list_create_info &create_info,
	 const synchronized_axis &synchronized_columns) const
{
	// A container that uses the listlayoutmanager.

	auto internal_listcontainer_impl=
		ref<listcontainer_pseudo_implObj>::create(peephole_parent);

	// The single element in the container, used to draw the list, and
	// its public object.

	auto t_impl=create_info
		.create_list_element_impl({
					   internal_listcontainer_impl, style,
					   synchronized_columns
			});

	auto t=list_element::create(t_impl);

	// Layout manager implementation object...

	auto internal_listlayoutmanager_impl=create_info
		.create_listlayoutmanager_impl(internal_listcontainer_impl, t);

	/// ... which is registered as the container for the peepholed object.
	auto container=peepholed_listcontainer::create
		(internal_listcontainer_impl,
		 t_impl,
		 internal_listlayoutmanager_impl);

	t->show();

	return {container, container, container, t_impl};
}

namespace {
#if 0
}
#endif

struct create_cells_helper {

	//! We're helping this style
	const listlayoutstyle_impl &style;

	//! The implementation element that's creating cells.
	list_elementObj::implObj &textlist_element;

	//! Where the new cells are being created.
	new_cells_info &info;

	//! Precomputed value
	const size_t extra_leading=style.extra_leading_columns();

	//! Precomputed value
	const size_t extra_trailing=style.extra_trailing_columns();

	//! Precomputed value
	const size_t extra=extra_leading+extra_trailing;

	//! Non-separator rowinfos.

	//! If we are asked to get_new_items, only non-separator rows will
	//! have their rowinfos added here.

	std::vector<extra_list_row_info> nonseparator_rows;

	//! Internal flag
	bool havemeta=false;

	//! Column counter

	//! Gets increment each time a cell gets created.

	size_t column_counter=0;

	//! The rowinfo for the next cell, that we are helping to create.

	textlist_rowinfo next_rowinfo;

	//! Compute a new cell's alignments.

	auto compute_alignments() const
	{
		std::tuple ret{halign::left, valign::bottom};

		auto c=column_counter % textlist_element.columns;

		// Was this column's alignment specifically requested?
		auto col_alignment=textlist_element.col_alignments.find(c);

		if (col_alignment != textlist_element.col_alignments.end())
			std::get<halign>(ret)=col_alignment->second;

		auto row_alignment=textlist_element.row_alignments.find(c);

		if (row_alignment != textlist_element.row_alignments.end())
			std::get<valign>(ret)=row_alignment->second;

		return ret;
	}

private:
	//! Internal - create a new cell.
	void create_cell(const list_cell &);

	//! Internal - cretae a new separator.
	void create_separator();
public:
	//! Whether process_list_item_param() created a new cell

	//! ... as a result of the given list_item_param.

	bool created_cell;

	//! Whether process_list_item_param() created a new separator

	//! ... as a result of the given list_item_param.

	bool created_separator;

	//! Helper function for converting a list_item_param.

	//! Processes the next list_item_param. Invokes item_callback if
	//! it specifies new cell content. Invokes separator_callback for a
	//! separator.
	//!
	//! Otherwise updates next_rowinfo.

	void process_list_item_param(const list_item_param_base &item);
};

inline void create_cells_helper::create_cell(const list_cell &new_cell)
{
	if (std::holds_alternative<menu_item_submenu>(next_rowinfo.menu_item))
	{
		if (next_rowinfo.listitem_shortcut ||
		    next_rowinfo.listitem_callback)
			throw EXCEPTION
				(_("Cannot specify shortcuts or "
				   "callbacks for sub-menus"));
	}

	bool first_column=
		(column_counter % textlist_element.columns) == 0;

	if (first_column)
	{
		// First column on each row.
		// Add leading cells.

		for (size_t i=0; i<extra_leading; ++i)
		{
			info.newcells.push_back(style.create_leading_column
						(textlist_element, i));
						 ++column_counter;
		}
	}

	info.newcells.push_back(new_cell);
	created_cell=true;
}

inline void create_cells_helper::create_separator()
{
	bool first_column=(column_counter % textlist_element.columns) == 0;

	// The preliminary pass that reserved all
	// elements should've checked this already.
	if (!first_column)
		throw EXCEPTION(_("Internal error: separator element "
				  "is out of place"));

	if (havemeta)
		throw EXCEPTION(_("Cannot specify any other "
				  "attributes for a separator element")
				);

	// We must push a separator object for each
	// column. We cannot just link the same object
	// into all columns. The logic dealing with
	// column_iterators expects each cell in each
	// column to be a discrete object, for
	// recording the column_iterator.

	for (size_t i=0; i<textlist_element.columns; ++i)
		info.newcells.push_back(list_cellseparator::create());

	// next_rowinfo should be clean
	info.rowmeta.emplace_back(extra_list_row_info::create(),
				  next_rowinfo);

	column_counter += textlist_element.columns;
	created_separator=true;
}

void create_cells_helper::process_list_item_param(const list_item_param_base &item)
{
	created_cell=false;
	created_separator=false;

	std::visit(visitor
		   {
			   [&](const shortcut &sc)
			   {
				   if (next_rowinfo.listitem_shortcut)
					   throw EXCEPTION
						   (_("Cannot specify multiple "
						      "shorcuts for list items")
						    );

				   next_rowinfo.listitem_shortcut=sc;
			   },
			   [&](const inactive_shortcut &sc)
			   {
				   if (next_rowinfo.listitem_shortcut)
					   throw EXCEPTION
						   (_("Cannot specify multiple "
						      "shorcuts for list items")
						    );

				   next_rowinfo.listitem_shortcut=sc;
				   next_rowinfo.inactive_shortcut=true;
			   },
			   [&](const list_item_status_change_callback &cb)
			   {
				   if (next_rowinfo.listitem_callback)
					   throw EXCEPTION
						   (_("Cannot specify multiple "
						      "callbacks for list "
						      "items"));
				   next_rowinfo.listitem_callback=cb;
			   },
			   [&](const hierindent &i)
			   {
				   next_rowinfo.indent_level=i.n;
			   },
			   [&](const menuoption &mo)
			   {
				   next_rowinfo.setting_menu_item();
				   next_rowinfo.menu_item=menu_item_option{};
			   },
			   [&](const submenu &sm)
			   {
				   next_rowinfo.setting_menu_item();
				   const auto &[popup, popup_handler]
					   =create_menu_popup
					   (element_impl(&textlist_element),
					    sm.creator,
					    sm.appearance,
					    submenu_popup);

				   next_rowinfo.menu_item=menu_item_submenu{
					   popup,
					   popup_handler
				   };
			   },
			   [&](const text_param &s)
			   {
				   const auto & [halignment, valignment]=
					   compute_alignments();
				   auto rts=textlist_element
					   .create_richtextstring
					   (textlist_element.itemlabel_meta,
					    (s.string.empty() ?
					     text_param{U"\n"}:s)
					    );

				   richtext_options options;

				   options.alignment=halignment;

				   auto t=list_celltext
					   ::create(textlist_element,
						    std::move(rts), options,
						    valignment);

				   create_cell(t);
			   },
			   [&](const image_param &s)
			   {
				   const auto & [halignment, valignment]=
					   compute_alignments();

				   auto i=textlist_element
					   .get_window_handler()
					   .create_icon({s});

				   auto t=list_cell
					   (list_cellimage::create
					    (std::vector<icon>{i},
					     halignment, valignment));
				   create_cell(t);
			   },
			   [&](const separator &)
			   {
				   create_separator();
			   },
			   [&](const get_new_items &)
			   {
			   }
		   }, item);

	if (created_separator)
		return;

	if (created_cell)
	{
		++column_counter;

		// Processed a column value. If this is the end of
		// the row, save the metadata.

		if ((column_counter % textlist_element.columns) ==
		    (textlist_element.columns - extra_trailing))
		{
			// Last column. Add trailing cells.

			for (size_t i=0; i<extra_trailing; ++i)
			{
				info.newcells.push_back
					(style.create_trailing_column
					 (textlist_element, i,
					  next_rowinfo));
				++column_counter;
			}
		}

		if ((column_counter % textlist_element.columns) == 0)
		{
			auto new_extra_list=extra_list_row_info::create();

			nonseparator_rows.push_back(new_extra_list);
			info.rowmeta.emplace_back(new_extra_list,
						  next_rowinfo);
			next_rowinfo={};
			havemeta=false;
		}
	}
	else
	{
		// Metadata must be specified only at the beginning
		// of the row.
		if ((column_counter % textlist_element.columns)
		    != 0)
			throw EXCEPTION(_("Row metadata must be "
					  "specified before the "
					  "row data."));
		havemeta=true;
	}
}

#if 0
{
#endif
}

void
listlayoutstyle_impl::create_cells(const std::vector<list_item_param> &t,
				   const ref<listlayoutmanagerObj::implObj>
				   &lilm,
				   new_cells_info &info)
	const
{
	list_elementObj::implObj &textlist_element=
		*lilm->list_element_singleton->impl;

	create_cells_helper helper{*this, textlist_element, info};

	size_t n_real_elements=0;

	size_t n_separators=0;

	if (textlist_element.columns <= helper.extra)
		throw EXCEPTION("Internal error: attempting to initialize a list with too few columns");

	const size_t real_columns=textlist_element.columns-helper.extra;

	const get_new_items *seen_new_items=nullptr;

	for (const auto &item:t)
	{
		if (seen_new_items)
			throw EXCEPTION("new_items to be returned must be specified last");

		std::visit(visitor{
				[&](const shortcut &sc)
				{
				},
				[&](const inactive_shortcut &sc)
				{
				},
				[&](const list_item_status_change_callback &cb)
				{
				},
				[this](const hierindent &)
				{
					nonmenu_attribute_requested();
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
					++n_separators;
				},
				[&](const text_param &)
				{
					++n_real_elements;
				},
				[&](const image_param &)
				{
					++n_real_elements;
				},
				[&](const get_new_items &arg)
				{
					seen_new_items=&arg;
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
	info.newcells.reserve(n_real_elements / real_columns
			      * textlist_element.columns);
	info.rowmeta.reserve(n_real_elements / real_columns);


	helper.nonseparator_rows.reserve(n_real_elements / real_columns
					 - n_separators);

	for (const auto &s:t)
	{
		if (std::holds_alternative<get_new_items>(s))
			continue; // Checks that this is kosher here, above.


		helper.process_list_item_param(s);

	}

	if (helper.havemeta)
		throw EXCEPTION(_("Row metadata must be specified before "
				  "the row data."));

	if (info.rowmeta.size() * textlist_element.columns
	    != info.newcells.size())
		throw EXCEPTION(_("I suck at logic."));

	if (seen_new_items)
	{
		auto &handles=info.ret.handles;


		handles.reserve(helper.nonseparator_rows.size());

		for (const auto &meta:helper.nonseparator_rows)
		{
			handles.push_back(ref<listitemhandleObj::implObj>
					  ::create(lilm, meta));
		}
	}
}

void listlayoutstyle_impl::menu_attribute_requested() const
{
	throw EXCEPTION(_("menuoption and submenu attributes are allowed "
			  "only in menus"));
}

void listlayoutstyle_impl::nonmenu_attribute_requested() const
{
}

////////////////////////////////////////////////////////////////////////////
//
// Highlighted list style.

namespace {
#if 0
}
#endif

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

	std::unordered_map<size_t, valign>
		actual_row_alignments(const new_listlayoutmanager &style)
		const override
	{
		return style.row_alignments;
	}

	void set_selected_background(ONLY IN_THREAD,
				     draw_info &di,
				     const background_color &bgcolor)
		const override
	{
		di.window_background_color=
			bgcolor->get_current_color(IN_THREAD);
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
highlighted_listlayoutstyle_impl highlighted_style_instance;

#if 0
{
#endif
}

const listlayoutstyle_impl &highlighted_list=highlighted_style_instance;

////////////////////////////////////////////////////////////////////////////
//
// Bulleted list style.

namespace {
#if 0
}
#endif

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

	// Adjust row_alignments accordingly.
	std::unordered_map<size_t, valign>
		actual_row_alignments(const new_listlayoutmanager &style)
		const override
	{
		std::unordered_map<size_t, valign> actual;

		for (const auto &cw:style.row_alignments)
		{
			actual.insert({cw.first+1, cw.second});
		}

		return actual;
	}

	void set_selected_background(ONLY IN_THREAD,
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
			halign::center,
			valign::middle);
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

#if 0
{
#endif
}

const listlayoutstyle_impl &bulleted_list=bulleted_style_instance;

const listlayoutstyle_impl &list_style_by_name(const std::string_view &n)
{
	if (n == "highlight")
		return highlighted_list;

	if (n == "bullet")
		return bulleted_list;

	throw EXCEPTION(gettextmsg(_("\"%1%\" is not a known list style"),
				   n));
}

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
			return list_cellimage::create
				(std::vector<icon>{textlist_element.submenu},
				 halign::left,
				 valign::middle);
		}

		std::u32string s;

		if (info.listitem_shortcut)
			s=info.listitem_shortcut->label();

		auto rt=textlist_element
			.create_richtextstring(textlist_element
					       .itemshortcut_meta, s);

		return list_celltext::create(textlist_element,
					     std::move(rt),
					     richtext_options{},
					     valign::bottom);
	}

	void menu_attribute_requested() const override
	{
	}

	void nonmenu_attribute_requested() const override
	{
		throw EXCEPTION(_("hierindent attribute is not allowed "
			  "in menus"));
	}
};

static const
menu_list_style_impl menu_list_instance;

const listlayoutstyle_impl &menu_list=menu_list_instance;

/////////////////////////////////////////////////////////////////////////

// List style for combo-boxes: highlighted list, but not hierindent allowed.

class LIBCXX_HIDDEN combobox_layout_style_impl
	: public highlighted_listlayoutstyle_impl {

 public:
	void nonmenu_attribute_requested() const override
	{
		throw EXCEPTION(_("hierindent attribute is not allowed "
			  "in combo-boxes"));
	}
};

static const combobox_layout_style_impl combobox_list_instance;

const listlayoutstyle_impl &combobox_list=combobox_list_instance;


LIBCXXW_NAMESPACE_END
