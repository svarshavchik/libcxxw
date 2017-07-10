/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/listlayoutmanager.H"
#include "listlayoutmanager/listcontainer_impl.H"
#include "listlayoutmanager/listitemcontainer_impl.H"
#include "listlayoutmanager/listlayoutstyle.H"
#include "themedim.H"
#include "element_screen.H"
#include "grid_map_info.H"
#include "screen.H"
#include "run_as.H"
#include "busy.H"
#include "catch_exceptions.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/key_event.H"
#include <X11/keysym.h>

LIBCXXW_NAMESPACE_START

listlayoutmanagerObj::implObj
::implObj(const ref<listcontainerObj::implObj> &container_impl,
	  const new_listlayoutmanager &style)
	: gridlayoutmanagerObj::implObj(container_impl),
	container_impl(container_impl),
	style(style.layout_style),
	columns(style.columns),
	selection_type_thread_only(style.selection_type),
	selection_changed_thread_only(style.selection_changed)
{
}

listlayoutmanagerObj::implObj::~implObj()=default;

layoutmanager listlayoutmanagerObj::implObj::create_public_object()
{
	return listlayoutmanager::create(ref<implObj>(this));
}

void listlayoutmanagerObj::implObj::pointer_focus(IN_THREAD_ONLY,
						  const ref<elementObj::implObj>
						  &e)
{
	if (!e->enabled(IN_THREAD))
		return;

	auto focus_report=e->most_recent_pointer_focus_change(IN_THREAD);
	bool flag=in_focus(focus_report);

	grid_map_t::lock lock{grid_map};

	auto rc=lookup_row_col(lock, e);

	// TODO: structured bindings

	size_t r=std::get<0>(rc);
	size_t c=std::get<1>(rc);

	if (r == (size_t)-1)
		return;

	previously_highlighted_keyboard_focus_row= -1;

	if (!flag)
	{
		if (r == currently_highlighted_row &&
		    c == currently_highlighted_col)
			unhighlight_current_row(IN_THREAD, lock);
		return;
	}

	if (r != currently_highlighted_row)
	{
		unhighlight_current_row(IN_THREAD, lock);
		currently_highlighted_row=r;
		currently_highlighted_col=c;
		highlight_current_row(IN_THREAD, lock);
	}
	else
	{
		// When the pointer moves from one column to the other we
		// conveniently receive the focus gained event from the
		// new column before the focus lost event from the old column.
		//
		// By meticulously keeping track of the column we avoid
		// needless clearing and redrawing of the highlighted
		// background color.
		currently_highlighted_col=c;
	}
}

void listlayoutmanagerObj::implObj::keyboard_focus(IN_THREAD_ONLY, bool flag)
{
	if (flag)
	{
		grid_map_t::lock lock{grid_map};

		if (currently_highlighted_row != (size_t)-1)
			 // Could be pointer focus, don't change that.
			return;

		size_t row=previously_highlighted_keyboard_focus_row;

		unhighlight_current_row(IN_THREAD, lock);

		if (row != (size_t)-1 && row < size(lock))
		{
			currently_highlighted_row=
				previously_highlighted_keyboard_focus_row=row;
			currently_highlighted_col=0;
			highlight_current_row(IN_THREAD, lock);
			ensure_current_row_is_visible(IN_THREAD, lock);
		}
	}
	else
	{
		grid_map_t::lock lock{grid_map};

		// Unhighlight, but preserve currently_highlighted_focus_row
		// if we ever get the keyboard focus back.

		size_t row=currently_highlighted_row;
		unhighlight_current_row(IN_THREAD, lock);
		previously_highlighted_keyboard_focus_row=row;
	}
}

bool listlayoutmanagerObj::implObj::process_key_event(IN_THREAD_ONLY,
						      const key_event &ke)
{
	grid_map_t::lock lock{grid_map};

	size_t s=size(lock);

	if (s == 0)
		return false;

	size_t i=currently_highlighted_row;

	switch (ke.keysym) {
	case XK_Up:
	case XK_KP_Up:
		if (!ke.keypress)
			return true;
		if (i != (size_t)-1)
		{
			if (i > 0)
			{
				unhighlight_current_row(IN_THREAD, lock);
				currently_highlighted_row= i-1;
				previously_highlighted_keyboard_focus_row=i-1;

				highlight_current_row(IN_THREAD, lock);
				ensure_current_row_is_visible(IN_THREAD, lock);
			}
		}
		return true;
	case XK_Down:
	case XK_KP_Down:
		if (!ke.keypress)
			return true;
		if (i == (size_t)-1)
		{
			i=0;
		}
		else
		{
			if (++i >= s)
				return true;
		}
		unhighlight_current_row(IN_THREAD, lock);
		currently_highlighted_row=i;
		previously_highlighted_keyboard_focus_row=i;
		highlight_current_row(IN_THREAD, lock);
		ensure_current_row_is_visible(IN_THREAD, lock);
		return true;
	}
	return false;
}

void listlayoutmanagerObj::implObj::unhighlight_current_row(IN_THREAD_ONLY,
							    grid_map_t::lock &l)
{
	if (currently_highlighted_row == (size_t)-1)
		return;

	style.unhighlight(IN_THREAD, *this, l, currently_highlighted_row);

	currently_highlighted_row= -1;
	currently_highlighted_col= -1;
	previously_highlighted_keyboard_focus_row= -1;
}

void listlayoutmanagerObj::implObj
::ensure_current_row_is_visible(IN_THREAD_ONLY,
				grid_map_t::lock &l)
{
	if (currently_highlighted_row == (size_t)-1)
		return;

	const auto &e=(*l)->elements.at(currently_highlighted_row);

	if (e.empty())
		return; // Shouldn't happen.

	e.at(0)->grid_element->impl->ensure_entire_visibility(IN_THREAD);
}

void listlayoutmanagerObj::implObj::temperature_changed(IN_THREAD_ONLY)
{
	auto new_temperature=container_impl->hotspot_temperature(IN_THREAD);

	// We ignore warm hotspot temperature. We handle highlighting of
	// individual list item rows ourselves. All we want to know is whether
	// we're hot, or not.

	if (new_temperature != temperature::hot)
		new_temperature=temperature::warm;

	if (new_temperature == current_temperature)
		return;

	current_temperature=new_temperature;

	grid_map_t::lock lock{grid_map};
	highlight_current_row(IN_THREAD, lock);
}

void listlayoutmanagerObj::implObj::highlight_current_row(IN_THREAD_ONLY,
							  grid_map_t::lock &l)
{
	if (currently_highlighted_row == (size_t)-1)
		return;

	style.highlight(IN_THREAD, *this, l, currently_highlighted_row);
}

void listlayoutmanagerObj::implObj::refresh(IN_THREAD_ONLY,
					    grid_map_t::lock &lock,
					    const element &e)
{
	auto rc=lookup_row_col(lock, e->impl);

	// TODO: structured bindings

	size_t r=std::get<0>(rc);

	if (r == (size_t)-1)
		return;

	style.refresh(IN_THREAD, *this, lock, r,
		      currently_highlighted_row == r);
}

std::tuple<size_t, size_t>
listlayoutmanagerObj::implObj::lookup_row_col(grid_map_t::lock &lock,
					      const ref<elementObj::implObj>
					      &e)
{
	const auto &lookup_table=(*lock)->get_lookup_table();

	auto iter=lookup_table.find(e);

	if (iter != lookup_table.end())
		return {iter->second->row, iter->second->col};

	return {-1, -1};
}

void listlayoutmanagerObj::implObj
::activated(IN_THREAD_ONLY,
	    const listlayoutmanager &my_public_object)
{
	list_lock lock{my_public_object};

	try {
		selection_type(IN_THREAD)
			(lock, my_public_object, currently_highlighted_row);
	} CATCH_EXCEPTIONS;
}

size_t listlayoutmanagerObj::implObj::size(grid_map_t::lock &lock) const
{
	return (*lock)->elements.size();
}

bool listlayoutmanagerObj::implObj::selected(grid_map_t::lock &lock, size_t i)
	const
{
	if (i >= (*lock)->elements.size())
		return false;

	auto &item_row=(*lock)->elements.at(i);

	if (item_row.size() == 0) // Shouldn't happen
		return false;

	listitemcontainer c=item_row.at(0)->grid_element;

	return c->impl->selected();
}

void listlayoutmanagerObj::implObj::selected(const listlayoutmanager &me,
					     grid_map_t::lock &lock, size_t i,
					     bool selected_flag)
{
	if (i >= (*lock)->elements.size())
		return;

	auto &item_row=(*lock)->elements.at(i);

	if (item_row.size() == 0) // Shouldn't happen
		return;

	listitemcontainer c=item_row.at(0)->grid_element;

	// Punt to the connection thread in order to update this element's
	// appearance.

	container_impl->get_element_impl().THREAD
		->run_as([c, me, selected_flag]
			 (IN_THREAD_ONLY)
			 {
				 list_lock lock{me};

				 auto rc=me->impl->lookup_row_col(lock,
								  c->impl);

				 // TODO: structured bindings

				 size_t r=std::get<0>(rc);

				 if (r == (size_t)-1)
					 return;

				 if (c->impl->selected() == selected_flag)
					 return; // Nothing to do

				 c->impl->selected(selected_flag);

				 me->impl->refresh(IN_THREAD, lock, c);

				 busy_impl yes_i_am{me->impl->container_impl
						 ->get_element_impl(),
						 IN_THREAD};

				 try {
					 if (c->status_change_callback)
						 c->status_change_callback
							 (lock,
							  r,
							  selected_flag);
				 } CATCH_EXCEPTIONS;

				 try {
					 me->impl->selection_changed(IN_THREAD)
						 (lock, me, r, selected_flag,
						  yes_i_am);
				 } CATCH_EXCEPTIONS;
			 });
}

void listlayoutmanagerObj::implObj::autoselect(const listlayoutmanager &me,
					       grid_map_t::lock &lock, size_t i)
{
	connection_thread_method_t callback=&implObj::autoselect;

	connection_thread_op(me, lock, i, callback);
}

void listlayoutmanagerObj::implObj::autoselect(IN_THREAD_ONLY,
					       const listlayoutmanager &me,
					       list_lock &lock, size_t i)
{
	try {
		selection_type(IN_THREAD)(lock, me, i);
	} CATCH_EXCEPTIONS;
}

void listlayoutmanagerObj::implObj
::connection_thread_op(const listlayoutmanager &my_public_object,
		       grid_map_t::lock &lock, size_t i,
		       const std::function<connection_thread_op_t> &callback)
{
	if (i >= (*lock)->elements.size())
		return;

	auto &item_row=(*lock)->elements.at(i);

	if (item_row.size() == 0) // Shouldn't happen
		return;

	// The item number can change, when the connection thread runs.
	//
	// So what we do is capture the list item, then in the connection
	// thread look up the item number again, and roll with it.

	auto c=item_row.at(0)->grid_element;

	container_impl->get_element_impl().THREAD
		->run_as([c, my_public_object, callback]
			 (IN_THREAD_ONLY)
			 {
				 list_lock lock{my_public_object};

				 auto rc=my_public_object->impl
					 ->lookup_row_col(lock, c->impl);

				 // TODO: structured bindings

				 size_t r=std::get<0>(rc);

				 if (r == (size_t)-1)
					 return;

				 callback(&*my_public_object->impl, IN_THREAD,
					  my_public_object, lock, r);
			 });
}

void listlayoutmanagerObj::implObj::remove_item(const listlayoutmanager &me,
						grid_map_t::lock &lock,
						size_t i)
{
	connection_thread_method_t callback=&implObj::remove_item;

	connection_thread_op(me, lock, i, callback);
}

void listlayoutmanagerObj::implObj::remove_item(IN_THREAD_ONLY,
						const listlayoutmanager &me,
						list_lock &lock,
						size_t i)
{
	remove_row(i);

	// Make sure that currently_highlighted_row stays updated.

	if (currently_highlighted_row != (size_t)-1 &&
	    i >= currently_highlighted_row)
	{
		if (i == currently_highlighted_row)
		{
			currently_highlighted_row= -1;
		}
		else
		{
			--currently_highlighted_row;
		}
	}

	if (previously_highlighted_keyboard_focus_row != (size_t)-1 &&
	    i >= previously_highlighted_keyboard_focus_row)
	{
		if (i == previously_highlighted_keyboard_focus_row)
		{
			previously_highlighted_keyboard_focus_row= -1;
		}
		else
		{
			--previously_highlighted_keyboard_focus_row;
		}
	}
}

void listlayoutmanagerObj::implObj
::append_item(const listlayoutmanager &me,
	      const listlayoutstyle::new_list_items_t &new_item)
{
	container_impl->get_element_impl().THREAD
		->run_as([=]
			 (IN_THREAD_ONLY)
			 {
				 grid_map_t::lock grid_lock{me->impl->grid_map};

				 me->impl->style.create_item
					 (IN_THREAD,
					  me->impl,
					  append_row(&*me),
					  me->queue,
					  new_item);
			 });
}

void listlayoutmanagerObj::implObj
::insert_item(const listlayoutmanager &me,
	      grid_map_t::lock &lock,
	      const listlayoutstyle::new_list_items_t &new_item,
	      size_t item_number)
{
	connection_thread_op
		(me, lock, item_number,
		 [new_item]
		 (implObj *impl,
		  IN_THREAD_ONLY,
		  const listlayoutmanager &me,
		  list_lock &lock,
		  size_t item_number)
		 {
			 impl->insert_item(IN_THREAD, me, lock, item_number,
					   new_item);
		 });
}

void listlayoutmanagerObj::implObj
::insert_item(IN_THREAD_ONLY,
	      const listlayoutmanager &me,
	      list_lock &lock,
	      size_t i,
	      const listlayoutstyle::new_list_items_t &new_item)
{
	style.create_item(IN_THREAD,
			  me->impl,
			  insert_row(&*me, i),
			  me->queue, new_item);

	if (previously_highlighted_keyboard_focus_row != (size_t)-1)
	{
		if (previously_highlighted_keyboard_focus_row >= i)
			++previously_highlighted_keyboard_focus_row;
	}

	if (currently_highlighted_row != (size_t)-1)
	{
		if (currently_highlighted_row >= i)
			++currently_highlighted_row;
	}
}

void listlayoutmanagerObj::implObj
::replace_item(const listlayoutmanager &me,
	       grid_map_t::lock &lock,
	       const listlayoutstyle::new_list_items_t &new_item,
	       size_t item_number)
{
	if (item_number >= (*lock)->elements.size())
		append_item(me, new_item);

	connection_thread_op
		(me, lock, item_number,
		 [new_item]
		 (implObj *impl,
		  IN_THREAD_ONLY,
		  const listlayoutmanager &me,
		  list_lock &lock,
		  size_t item_number)
		 {
			 impl->replace_item(IN_THREAD, me, lock, item_number,
					   new_item);
		 });
}

void listlayoutmanagerObj::implObj
::replace_item(IN_THREAD_ONLY,
	       const listlayoutmanager &me,
	       list_lock &lock,
	       size_t i,
	       const listlayoutstyle::new_list_items_t &new_item)
{
	style.create_item(IN_THREAD,
			  me->impl,
			  replace_row(&*me, i),
			  me->queue, new_item);

	if (previously_highlighted_keyboard_focus_row == i)
		previously_highlighted_keyboard_focus_row= -1;

	if (currently_highlighted_row == i)
		currently_highlighted_row= -1;
}

void listlayoutmanagerObj::implObj
::remove_all_items(const listlayoutmanager &me)
{
	container_impl->get_element_impl().THREAD
		->run_as([=]
			 (IN_THREAD_ONLY)
			 {
				 me->impl->remove_all_items(IN_THREAD);
			 });
}

void listlayoutmanagerObj::implObj::remove_all_items(IN_THREAD_ONLY)
{
	grid_map_t::lock lock{grid_map};

	remove_all_rows(lock);

	currently_highlighted_row=-1;
	currently_highlighted_col=-1;
	previously_highlighted_keyboard_focus_row= -1;
}

LIBCXXW_NAMESPACE_END
