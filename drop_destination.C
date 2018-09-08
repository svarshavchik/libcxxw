/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "generic_window_handler.H"
#include "connection.H"
#include "connection_thread.H"
#include "x/w/rectangle.H"
#include "x/w/screen.H"
#include "x/w/impl/child_element.H"
#include "selection/current_selection_handler.H"
#include "catch_exceptions.H"

LOG_FUNC_SCOPE_DECL(LIBCXX_NAMESPACE::w::drop_destination, drop_log);

LIBCXXW_NAMESPACE_START

void generic_windowObj::handlerObj
::process_drag_enter(ONLY IN_THREAD,
		     const xcb_client_message_event_t *event)
{
	LOG_FUNC_SCOPE(drop_log);

	auto flags=event->data.data32[1];

	source_dnd_formats(IN_THREAD).clear();

	source_dnd_formats(IN_THREAD).insert(event->data.data32+2,
					     event->data.data32+5);

	source_dnd_version(IN_THREAD)=(flags >> 24);
	source_dnd(IN_THREAD)=event->data.data32[0];

	LOG_DEBUG("XdndEnter from " << source_dnd(IN_THREAD)
		  << " version " << (int)source_dnd_version(IN_THREAD));

	if (flags & 1)
	{
		IN_THREAD->info->collect_property_with
			(source_dnd(IN_THREAD),
			 IN_THREAD->info->atoms_info.XdndTypeList,
			 XCB_ATOM_ATOM,
			 false,
			 [&]
			 (xcb_atom_t atom,
			  uint8_t format,
			  void *data,
			  size_t data_size)
			 {
				 if (atom != XCB_ATOM_ATOM)
					 return;
				 if (data_size == 0 ||
				     data_size % sizeof(xcb_atom_t))
					 return;
				 xcb_atom_t *atomp=
					 reinterpret_cast<xcb_atom_t *>
					 (data);

				 source_dnd_formats(IN_THREAD)
					 .insert(atomp,
						 atomp+data_size
						 /sizeof(xcb_atom_t));
			 });
	}

	source_dnd_formats(IN_THREAD).erase(XCB_NONE); // Tidy up.

	LOG_DEBUG("   Supported formats: " <<
		  ({
			  std::ostringstream o;
			  const char *sep="";

			  for (auto format:source_dnd_formats(IN_THREAD))
			  {
				  o << sep <<
					  IN_THREAD
					  ->info->get_atom_name(format);
				  sep=", ";
			  }

			  o.str();
		  }));
}

void generic_windowObj::handlerObj
::process_drag_position(ONLY IN_THREAD,
			const xcb_client_message_event_t *event)
{
	LOG_FUNC_SCOPE(drop_log);

	if (event->data.data32[0] != source_dnd(IN_THREAD))
		return;

	coord_t x{(uint16_t)(event->data.data32[2] >> 16)};
	coord_t y{(uint16_t)event->data.data32[2]};

	xcb_timestamp_t timestamp=XCB_CURRENT_TIME;

	if (source_dnd_version(IN_THREAD) >= 1)
		timestamp=event->data.data32[3];

	xcb_atom_t action=XCB_NONE;
	if (source_dnd_version(IN_THREAD) >= 2)
		action=event->data.data32[4];

	LOG_DEBUG("Drag and drop position: ("
		  << x << ", " << y << "), timestamp "
		  << timestamp << ", action "
		  << IN_THREAD->info->get_atom_name(action));

	// Be a pessimistic, assume we won't accept a drop. If we find
	// a suitable display element under the pointer we will change our
	// minds.

	xcb_client_message_event_t reply_message{};

	reply_message.response_type=XCB_CLIENT_MESSAGE;
	reply_message.window=source_dnd(IN_THREAD);
	reply_message.format=32;
	reply_message.type=IN_THREAD->info->atoms_info.XdndStatus;
	reply_message.data.data32[0]=id();

	rectangle rect=data(IN_THREAD).current_position;

	get_absolute_location_on_screen(IN_THREAD, rect);

	if (x >= rect.x && y >= rect.y)
	{
		// Pointer is inside our window, that's a good start.
		// (Well it should be, how else would we get this?)

		coord_t elem_x=coord_t::truncate(x-rect.x);
		coord_t elem_y=coord_t::truncate(y-rect.y);

		auto e=find_element_under(IN_THREAD, elem_x, elem_y);

		bool find_acceptable_drop=false;

		try {
			find_acceptable_drop=
				e->find_acceptable_drop
				(IN_THREAD, e,
				 source_dnd_formats(IN_THREAD),
				 timestamp);
		} REPORT_EXCEPTIONS(this);

		LOG_TRACE("Element at ("
			  << x << ", " << y
			  << ") will accept dropped content: "
			  << find_acceptable_drop);

		if (find_acceptable_drop)
		{
			auto e_rect=e->get_absolute_location_on_screen
				(IN_THREAD);

			LOG_TRACE("Element's rectangle: " << e_rect);

			elem_x=coord_t::truncate(x-e_rect.x);
			elem_y=coord_t::truncate(y-e_rect.y);

			if (elem_x < 0)
				elem_x=0;

			if (e->data(IN_THREAD).current_position.width
			    <= dim_t::truncate(elem_x))
			{
				elem_x=coord_t::truncate
					(e->data(IN_THREAD).
					 current_position.width)-1;
			}

			if (elem_y < 0)
				elem_y=0;

			if (e->data(IN_THREAD).current_position.height
			    <= dim_t::truncate(elem_y))
			{
				elem_y=coord_t::truncate
					(e->data(IN_THREAD).
					 current_position.height)-1;
			}

			try {
				e->dragging_location(IN_THREAD, elem_x,
						     elem_y, timestamp);
			} REPORT_EXCEPTIONS(this);

			reply_message.data.data32[1]=1;

			if (source_dnd_version(IN_THREAD) >= 2)
				reply_message.data.data32[4]=
					IN_THREAD->info->atoms_info
					.XdndActionPrivate;

			dnd_drop_target(IN_THREAD)=e;
			// We want to continue getting position updates
			rect={};
		}
		else
		{
			// Don't bother to give me position updates
			// until the mouse pointer leaves this
			// non-accepting element.
			//
			// If this element is a container, we need
			// to exclude the childrens' areas, because
			// some child element may accept the drop.

			rect=e->data(IN_THREAD).current_position;

			rect.x=0;
			rect.y=0;

			rectarea child_rects;

			e->for_each_child
				(IN_THREAD,
				 [&]
				 (const auto &child_element)
				 {
					 auto child=child_element->impl;

					 if (!child
					     ->can_be_under_pointer
					     (IN_THREAD))
						 return;

					 child_rects.insert
						 (child->data(IN_THREAD)
						  .current_position);
				 });

			auto abspos=e->get_absolute_location_on_screen
				(IN_THREAD);

			// Visible parts of the container.

			rectarea visible_areas=
				subtract( {rect}, child_rects,
					  abspos.x, abspos.y);

			LOG_TRACE("Visible areas:" <<
				  ({
					  std::ostringstream o;
					  const char *sep="";

					  for (const auto &r
						       :visible_areas)
					  {
						  o << sep << r;
						  sep=", ";
					  }

					  o.str();
				  }) << ", coordinate: ("
				  << x << ", " << y << ")");

			rect={};

			// Which one of these is under the pointer?

			for (const auto &c:visible_areas)
			{
				if (!c.overlaps(x, y))
					continue;

				rect=c;
				break;
			}

			LOG_TRACE("Don't bother checking until pointer "
				  " is outside of "
				  << rect);

		}
		if (rect.width > 0 && rect.height > 0)
			reply_message.data.data32[1] |= 2;

		reply_message.data.data32[2]=
			((uint32_t)(uint16_t)(coord_t::value_type)
			 (rect.x)
			 << 16) | (uint16_t)(coord_t::value_type)
			(rect.y);
		reply_message.data.data32[3]=
			((uint32_t)(uint16_t)(dim_t::value_type)
			 (rect.width)
			 << 16) | (uint16_t)(dim_t::value_type)
			(rect.height);
	}
	xcb_send_event(IN_THREAD->info->conn, 0,
		       source_dnd(IN_THREAD),
		       0,
		       reinterpret_cast<char *>(&reply_message));
}

void generic_windowObj::handlerObj
::process_drag_leave(ONLY IN_THREAD,
		     const xcb_client_message_event_t *event)
{
	LOG_FUNC_SCOPE(drop_log);

	if (event->data.data32[0] == source_dnd(IN_THREAD))
	{
		LOG_DEBUG("Drop aborted.");
		source_dnd(IN_THREAD)=XCB_NONE;
	}
}

namespace {
#if 0
}
#endif

// mcguffin responsible for sending the XdndFinished ack after drag and drop
// is finished.

struct LIBCXX_HIDDEN finishedDropMcguffinObj : virtual public obj {

	const connection c;

public:

	xcb_client_message_event_t reply_message;

	bool forget_it=false;

	finishedDropMcguffinObj(const connection &c)
		: c{c}, reply_message{}
	{
	}

	~finishedDropMcguffinObj()
	{
		LOG_FUNC_SCOPE(drop_log);
		LOG_DEBUG("Drop finished");

		if (forget_it)
			return; // Old DnD source.

		c->in_thread([reply_message=this->reply_message]
			     (ONLY IN_THREAD)
			     mutable // Only because of xcb_send_event's param.
			     {
				     xcb_send_event(IN_THREAD->info->conn, 0,
						    reply_message.window,
						    0,
						    reinterpret_cast<char *>
						    (&reply_message));
			     });
	}
};

#if 0
{
#endif
}

void generic_windowObj::handlerObj
::process_drag_drop(ONLY IN_THREAD,
		    const xcb_client_message_event_t *event)
{
	LOG_FUNC_SCOPE(drop_log);

	if (event->data.data32[0] != source_dnd(IN_THREAD))
		return;

	xcb_timestamp_t timestamp=XCB_CURRENT_TIME;

	if (source_dnd_version(IN_THREAD) >= 1)
		timestamp=event->data.data32[2];

	LOG_DEBUG("Beginning drop");
	auto mcguffin=ref<finishedDropMcguffinObj>
		::create(screenref->get_connection());

	if (source_dnd_version(IN_THREAD) < 2)
		mcguffin->forget_it=true;

	mcguffin->reply_message.response_type=XCB_CLIENT_MESSAGE;
	mcguffin->reply_message.window=source_dnd(IN_THREAD);
	mcguffin->reply_message.format=32;
	mcguffin->reply_message.type=
		IN_THREAD->info->atoms_info.XdndFinished;
	mcguffin->reply_message.data.data32[0]=id();

	if (source_dnd_version(IN_THREAD) >= 5)
	{
		mcguffin->reply_message.data.data32[1]=1;
		mcguffin->reply_message.data.data32[2]=
			IN_THREAD->info->atoms_info.XdndActionPrivate;

	}
	source_dnd(IN_THREAD)=XCB_NONE;

	// Sanity check, we should've captured the dnd_drop_target,
	// and it is sane.

	auto dropping_elementptr=dnd_drop_target(IN_THREAD).getptr();

	if (!(dropping_elementptr &&
	      dropping_elementptr->can_be_under_pointer(IN_THREAD) &&
	      dropping_elementptr->enabled(IN_THREAD)))
		return;

	try
	{
		xcb_atom_t type=XCB_NONE;

		auto handler=dropping_elementptr->drop(IN_THREAD,
						       type, mcguffin);

		if (handler && type != XCB_NONE &&
		    convert_selection(IN_THREAD,
				      IN_THREAD->info->atoms_info.XdndSelection,
				      IN_THREAD->info->atoms_info.cxxwpaste,
				      type,
				      timestamp))
		{
			clipboard_being_pasted(IN_THREAD)=
				IN_THREAD->info->atoms_info.cxxwpaste;
			clipboard_paste_timestamp(IN_THREAD)=timestamp;
			conversion_handler(IN_THREAD)=handler;
		}
	} REPORT_EXCEPTIONS(this);
}

void elementObj::implObj::dragging_location(ONLY IN_THREAD,
					    coord_t x, coord_t y,
					    xcb_timestamp_t timestamp)
{
}

bool elementObj::implObj
::find_acceptable_drop(ONLY IN_THREAD,
		       element_impl &accepting_element,
		       const source_dnd_formats_t &source_formats,
		       xcb_timestamp_t timestamp)
{
	return false;
}

bool child_elementObj::find_acceptable_drop(ONLY IN_THREAD,
					    element_impl &accepting_element,
					    const source_dnd_formats_t
					    &source_formats,
					    xcb_timestamp_t timestamp)
{
	return child_container->container_element_impl()
		.find_acceptable_drop(IN_THREAD,
				      accepting_element, source_formats,
				      timestamp);
}

current_selection_handlerptr elementObj::implObj
::drop(ONLY IN_THREAD,
       xcb_atom_t &type,
       const ref<obj> &finish_mcguffin)
{
	return {};
}

LIBCXXW_NAMESPACE_END
