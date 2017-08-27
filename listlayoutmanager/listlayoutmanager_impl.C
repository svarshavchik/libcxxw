/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/listlayoutmanager.H"
#include "listlayoutmanager/listcontainer_impl.H"
#include "listlayoutmanager/listitemcontainer_impl.H"
#include "listlayoutmanager/firstlistitemcontainer.H"
#include "listlayoutmanager/listlayoutstyle.H"
#include "x/w/factory.H"
#include "themedim.H"
#include "element_screen.H"
#include "grid_map_info.H"
#include "screen.H"
#include "run_as.H"
#include "busy.H"
#include "catch_exceptions.H"
#include "messages.H"
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
	selection_changed_(style.selection_changed)
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

	auto looked_up=lookup_row_col(lock, e);

	if (!looked_up)
		return;

	auto [r, c] = looked_up.value();

	highlighted_keyboard_focus_row(lock)= -1;

	if (!flag)
	{
		if (r == highlighted_row(lock) &&
		    c == highlighted_col(lock))
			unhighlight_current_row(IN_THREAD, lock);
		return;
	}

	if (r != highlighted_row(lock))
	{
		unhighlight_current_row(IN_THREAD, lock);
		highlighted_row(lock)=r;
		highlighted_col(lock)=c;
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
		highlighted_col(lock)=c;
	}
}

void listlayoutmanagerObj::implObj::keyboard_focus(IN_THREAD_ONLY, bool flag)
{
	if (flag)
	{
		grid_map_t::lock lock{grid_map};

		if (highlighted_row(lock) != (size_t)-1)
			 // Could be pointer focus, don't change that.
			return;

		size_t row=highlighted_keyboard_focus_row(lock);

		unhighlight_current_row(IN_THREAD, lock);

		if (row != (size_t)-1 && row < size(lock))
		{
			highlighted_row(lock)=
				highlighted_keyboard_focus_row(lock)=row;
			highlighted_col(lock)=0;
			highlight_current_row(IN_THREAD, lock);
			ensure_current_row_is_visible(IN_THREAD, lock);
		}
	}
	else
	{
		grid_map_t::lock lock{grid_map};

		// Unhighlight, but preserve currently_highlighted_focus_row
		// if we ever get the keyboard focus back.

		size_t row=highlighted_row(lock);
		unhighlight_current_row(IN_THREAD, lock);
		highlighted_keyboard_focus_row(lock)=row;
	}
}

bool listlayoutmanagerObj::implObj::process_key_event(IN_THREAD_ONLY,
						      const key_event &ke)
{
	grid_map_t::lock lock{grid_map};

	size_t s=size(lock);

	if (s == 0)
		return false;

	size_t i=highlighted_row(lock);

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
				highlighted_row(lock)= i-1;
				highlighted_keyboard_focus_row(lock)=i-1;

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
		highlighted_row(lock)=i;
		highlighted_keyboard_focus_row(lock)=i;
		highlight_current_row(IN_THREAD, lock);
		ensure_current_row_is_visible(IN_THREAD, lock);
		return true;
	}
	return false;
}

void listlayoutmanagerObj::implObj::unhighlight_current_row(IN_THREAD_ONLY,
							    grid_map_t::lock &lock)
{
	if (highlighted_row(lock) == (size_t)-1)
		return;

	style.unhighlight(IN_THREAD, *this, lock, highlighted_row(lock));

	highlighted_row(lock)= -1;
	highlighted_col(lock)= -1;
	highlighted_keyboard_focus_row(lock)= -1;
}

void listlayoutmanagerObj::implObj
::ensure_current_row_is_visible(IN_THREAD_ONLY,
				grid_map_t::lock &lock)
{
	if (highlighted_row(lock) == (size_t)-1)
		return;

	const auto &e=(*lock)->elements.at(highlighted_row(lock));

	if (e.empty())
		return; // Shouldn't happen.

	e.at(0)->grid_element->impl->ensure_entire_visibility(IN_THREAD);
}

void listlayoutmanagerObj::implObj::temperature_changed(IN_THREAD_ONLY)
{
	auto new_temperature=container_impl->hotspot_temperature(IN_THREAD);

	grid_map_t::lock lock{grid_map};

	// We ignore warm hotspot temperature. We handle highlighting of
	// individual list item rows ourselves. All we want to know is whether
	// we're hot, or not.

	if (new_temperature != temperature::hot)
		new_temperature=temperature::warm;

	if (new_temperature == current_temperature(lock))
		return;

	current_temperature(lock)=new_temperature;

	highlight_current_row(IN_THREAD, lock);
}

void listlayoutmanagerObj::implObj::highlight_current_row(IN_THREAD_ONLY,
							  grid_map_t::lock &lock)
{
	if (highlighted_row(lock) == (size_t)-1)
		return;

	style.highlight(IN_THREAD, *this, lock, highlighted_row(lock));
}

void listlayoutmanagerObj::implObj::refresh(IN_THREAD_ONLY,
					    grid_map_t::lock &lock,
					    const element &e)
{
	auto looked_up=lookup_row_col(lock, e->impl);

	if (!looked_up)
		return;

	size_t r;

	std::tie(r, std::ignore)=looked_up.value();

	style.refresh(IN_THREAD, *this, lock, r,
		      highlighted_row(lock) == r);
}

std::optional<size_t>
listlayoutmanagerObj::implObj::lookup_item(grid_map_t::lock &lock,
					   const ref<child_elementObj>
					   &item_impl)
{
	auto looked_up=lookup_row_col(lock,
				      ref(&item_impl->child_container
					  ->get_element_impl()));

	if (!looked_up)
		return {};

	size_t r;

	std::tie(r, std::ignore)=looked_up.value();

	return r;
}

std::optional<std::tuple<size_t, size_t>>
listlayoutmanagerObj::implObj::lookup_row_col(grid_map_t::lock &lock,
					      const ref<elementObj::implObj>
					      &e)
{
	const auto &lookup_table=(*lock)->get_lookup_table();

	auto iter=lookup_table.find(e);

	if (iter != lookup_table.end())
		return std::tuple{iter->second->row, iter->second->col};

	return {};
}

void listlayoutmanagerObj::implObj
::activated(IN_THREAD_ONLY,
	    const listlayoutmanager &my_public_object)
{
	list_lock lock{my_public_object};

	if (highlighted_row(lock) != (size_t)-1)
		autoselect(IN_THREAD, my_public_object, lock,
			   highlighted_row(lock));
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

	firstlistitemcontainer c=item_row.at(0)->grid_element;

	if (c->impl->selected() == selected_flag)
		return; // Nothing to do

	c->impl->selected(selected_flag);

	auto t=container_impl->get_element_impl().THREAD;

	t->run_as([me, c]
		  (IN_THREAD_ONLY)
		  {
			  list_lock real_lock{me};

			  me->impl->refresh(IN_THREAD, real_lock, c);
		  });


	busy_impl yes_i_am{container_impl->get_element_impl(), t};
	list_lock real_lock{me};

	try {
		if (c->status_change_callback)
			c->status_change_callback(real_lock, i,selected_flag);

	} CATCH_EXCEPTIONS;

	try {
		selection_changed(real_lock)
			(real_lock, me, i, selected_flag, yes_i_am);
	} CATCH_EXCEPTIONS;
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
		busy_impl yes_i_am{container_impl->get_element_impl(),
				IN_THREAD};

		selection_type(IN_THREAD)(lock, me, i, yes_i_am);
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
				 auto looked_up=my_public_object->impl
					 ->lookup_row_col(lock, c->impl);

				 if (!looked_up)
					 return;

				 size_t r;

				 std::tie(r, std::ignore)=looked_up.value();

				 callback(&*my_public_object->impl, IN_THREAD,
					  my_public_object, lock, r);
			 });
}

// Need to hold a lock while an item is being removed.

void listlayoutmanagerObj::implObj::remove_item(const listlayoutmanager &me,
						grid_map_t::lock &lock,
						size_t i)
{
	me->impl->remove_row(i);

	// Make sure that highlighted_row(lock) stays updated.

	if (me->impl->highlighted_row(lock) != (size_t)-1 &&
	    i <= me->impl->highlighted_row(lock))
	{
		if (i == me->impl->highlighted_row(lock))
		{
			me->impl->highlighted_row(lock)= -1;
		}
		else
		{
			--me->impl->highlighted_row(lock);
		}
	}

	if (me->impl->highlighted_keyboard_focus_row(lock) != (size_t)-1 &&
	    i <= me->impl->highlighted_keyboard_focus_row(lock))
	{
		if (i == me->impl->highlighted_keyboard_focus_row(lock))
		{
			me->impl->highlighted_keyboard_focus_row(lock)= -1;
		}
		else
		{
			--me->impl->highlighted_keyboard_focus_row(lock);
		}
	}
}

void listlayoutmanagerObj::implObj
::append_item(const listlayoutmanager &me,
	      const listlayoutstyle::new_list_items_t &new_item)
{
	grid_map_t::lock lock{me->impl->grid_map};

	auto f=me->impl->append_row(&*me);

	me->impl->style.create_item(me->impl, f,
				    me->queue, new_item);
}

void listlayoutmanagerObj::implObj
::insert_item(const listlayoutmanager &me,
	      grid_map_t::lock &lock,
	      const listlayoutstyle::new_list_items_t &new_item,
	      size_t i)
{
	auto f=me->impl->insert_row(&*me, i);

	style.create_item(me->impl,
			  f,
			  me->queue, new_item);

	if (me->impl->highlighted_keyboard_focus_row(lock) != (size_t)-1)
	{
		if (me->impl->highlighted_keyboard_focus_row(lock) >= i)
			++me->impl->highlighted_keyboard_focus_row(lock);
	}

	if (me->impl->highlighted_row(lock) != (size_t)-1)
	{
		if (me->impl->highlighted_row(lock) >= i)
			++me->impl->highlighted_row(lock);
	}
}

void listlayoutmanagerObj::implObj
::replace_item(const listlayoutmanager &me,
	       grid_map_t::lock &lock,
	       const listlayoutstyle::new_list_items_t &new_item,
	       size_t i)
{
	if (i >= (*lock)->elements.size())
	{
		append_item(me, new_item);
		return;
	}

	auto f=replace_row(&*me, i);

	style.create_item(me->impl,
			  f,
			  me->queue, new_item);

	if (me->impl->highlighted_keyboard_focus_row(lock) == i)
		me->impl->highlighted_keyboard_focus_row(lock)= -1;

	if (me->impl->highlighted_row(lock) == i)
		me->impl->highlighted_row(lock)= -1;
}

void listlayoutmanagerObj::implObj
::remove_all_items(const listlayoutmanager &me)
{
	grid_map_t::lock lock{me->impl->grid_map};

	me->impl->remove_all_rows(lock);

	me->impl->highlighted_row(lock)=-1;
	me->impl->highlighted_col(lock)=-1;
	me->impl->highlighted_keyboard_focus_row(lock)= -1;
}

element listlayoutmanagerObj::implObj::item(size_t item_number, size_t column)
{
	auto e=get(item_number, style.physical_column(column));

	if (!e)
		throw EXCEPTION(_("List item does not exist"));

	return e;
}

LIBCXXW_NAMESPACE_END
