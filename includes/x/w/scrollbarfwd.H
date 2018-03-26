/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_scrollbarfwd_h
#define x_w_scrollbarfwd_h

#include <x/w/connection_threadfwd.H>
#include <x/ptrfwd.H>
#include <x/number.H>
#include <x/functionalrefptrfwd.H>
#include <cstdint>

LIBCXXW_NAMESPACE_START

enum class scrollbar_visibility;

class scrollbarObj;

struct scroll_v_tag;

//! Discrete data type for scrollbar values.
typedef number<uint32_t, scroll_v_tag> scroll_v_t;

struct scrollbar_config;

struct LIBCXX_HIDDEN scrollbar_info_t;

//! Callback that reports the scrollbar's updated values.

typedef functionref<void (THREAD_CALLBACK,
			  const scrollbar_info_t &)> scrollbar_cb_t;

/*! A scroll-bar display element.

\code
sb->set(4);

INSERT_LIBX_NAMESPACE::w::scrollbar_config config;

config.range=110;
config.page_size=10;
config.increment=5;

sb->reconfigure(sb);

auto v=sb->get_value();

auto v=sb->get_dragged_value();
\endcode

set() manually changes the position of the handle in the scroll-bar.
reconfigure() replaces the scroll-bar's original configuration.

get_value() polls the scroll-bar and returns its current value.
get_value() returns the value before the scroll-bar was dragged if the
scrollbar is in the process of being dragged, at this time.

get_dragged_value() returns the current value of the scroll-bar, including
when it's being dragged.

Of course, since the scroll-bar is managed by an internal library execution
thread, the value of the scroll-bar can change at any time. set() and
reconfigure() sends a message to the execution thread, and immediately
calling get_value() or get_dragged_value() after set() or reconfigure() may
still return the previous value of the scroll-bar if the internal execution
thread did not yet process the message.

\par Scrollbar configuration

An \ref scrollbar_config "INSERT_LIBX_NAMESPACE::w::scrollbar_config"
specifies the range of the potential scroll-bar values. The visual position
of the scroll-bar's handle is scaled to an unsigned 32 bit value
between 0 and (range-page_size).
The above example configures a scroll-bar whose virtual position is always
between 0 and 100, inclusively. Note that with the default page_size of 1,
a range of 100 means that the scroll-bar's values range from 0 to 99.

The "page_size" component determines how big is the scroll-bar's draggable
handle, in terms of the scroll-bar's range. This example sets the handle's
size equivalent to ten scroll-bar positions, and clicking on the scroll-bar
outside of its handle area using the first pointer button advances the
scroll-bar's position by 10. "increment" sets how far the scroll-bar's value
changes when the arrow buttons at either end of the scroll-bar get clicked
with the first pointer button. Holding "CTRL" while clicking on the arrows
advances the scrollbar's value by 1.

\code
sb->on_update([]
              (ONLY IN_THREAD,
               const INSERT_LIBX_NAMESPACE::w::scrollbar_info_t &info)
              {

              });
\endcode

Installing a scroll-bar callback is better than using get_value() and
get_dragged_value(). The callback gets automatically
executed whenever the scroll-bar's values change.

\ref factory "create_horizontal_scrollbar() and create_vertical_scrollbar()"
take the initial callback as an optional parameter. Alternatively, on_update()
replaces any existing callback with the provided callback. The new callback
always gets executed immediately, giving the scroll-bar's current value, and
then continues to get executed whenever the scroll-bar's value changes.
This happens only with the replacement on_update() callback. The initial
callback, provided when the scroll-bar gets created, does not get
automatically executed.

\note
The initial invocation of a replacement callback gets executed by the
library's internal execution thread, and may happen before or after
on_update() returns.

*/

typedef ref<scrollbarObj> scrollbar;

//! A nullable pointer reference to a \ref scrollbar "scrollbar".

//! \see scrollbar

typedef ptr<scrollbarObj> scrollbarptr;

//! A reference to a constant \ref scrollbar "scrollbar".

//! \see scrollbar

typedef const_ref<scrollbarObj> const_scrollbar;

//! A nullable pointer reference to a constant \ref scrollbar "scrollbar".

//! \see scrollbar

typedef const_ptr<scrollbarObj> const_scrollbarptr;

LIBCXXW_NAMESPACE_END

#endif
