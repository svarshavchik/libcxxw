/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_color_pickerfwd_h
#define x_w_color_pickerfwd_h

#include <x/w/namespace.H>
#include <x/w/connection_threadfwd.H>
#include <x/w/rgbfwd.H>
#include <x/w/callback_triggerfwd.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class color_pickerObj;

/*! Color picker display element.

\code
INSERT_LIBX_NAMESPACE::w::color_picker cp=factory->create_color_picker();
\endcode

A small display element for initializing colors. Displays a small rectangle
in the current color, an
\ref rgb "INSERT_LIBX_NAMESPACE::w::rgb" value, and a button next to it.
The button opens a popup for individually adjusting the color's red, green,
and blue components.

\note
The color picker ignores \c INSERT_LIBX_NAMESPACE::w::rgb 's alpha component.

The color popup's main area has a large square filled with gradient color.
Initially, the gradient square's horizontal axis represents the red color
component, and the vertical axis represents the green component. A horizontal
strip above the gradient square and a vertical one to the left of it, show
the red and the green gradients represented by the square's axi.

Clicking anywhere in the gradient square adjusts the color picker's current
color's components to correspond to the click's location.

A third horizontal strip appears below the gradient square, and initially
represents the color picker's blue component. Clicking on it adjusts the
color picker's component to correspond to the click's location on the bottom
strip.

Clicking on the top and the left strips swaps that strip's component with the
one on the bottom strip, and updates the gradient square to represent the
new combination of the horizontal and vertical gradient components. At all
times the gradient square represents the color combination of two of the
three red, green, and blue components, and the bottom strip represents the
color combinations of the third one.

\par Selecting the color with the keyboard

Input fields on the right half of the popup provide the means of specifying
color values with the keyboard. The first three set of input fields are for
entering explicit red, green, and blue components. The next three set of
input fields are for entering explicit hue, saturation, and value components.
The "hexadecimal" button below them enables specifying the values in hexadecimal
instead of decimal.

The "Full Precision" option enables specifying each color component as a 16 bit
value. Most display servers use 8-bit values for red, green, and blue color
components. \ref rgb "INSERT_LIBX_NAMESPACE::w::rgb" always carries the
red, green, and blue color component values with full levels of color precision.
Entering an 8 bit value into the input field quietly scales the value to the
entire full color precision.

\par The "Ok" and "Cancel" buttons

The "Cancel" button closes the color picker popup and restores the original
color if it was changed. "Ok" closes the popup and saves the current
color if it was changed. Closing the color picker popup in any other way
(the "Esc" key, or clicking outside of it with the pointer) is equivalent to
a "Cancel".

\code
INSERT_LIBX_NAMESPACE::w::rgb c=cp->current_color();

cp->current_color(INSERT_LIBX_NAMESPACE::w::rgb{});
cp->current_color(IN_THREAD, INSERT_LIBX_NAMESPACE::w::rgb{});

cp->on_color_update([]
                    (ONLY IN_THREAD,
                     const INSERT_LIBX_NAMESPACE::w::rgb &new_color,
                     const INSERT_LIBX_NAMESPACE::w::callback_trigger_t &trigger,
                     const INSERT_LIBX_NAMESPACE::w::busy &mcguffin)
                    {
                    });

\endcode

current_color() returns or sets the the color picker's most recently saved
value. This is the color value saved by the popup's "Ok" button.

on_color_update() installs a callback that gets executed by the color picker
popup's "Ok" button. A newly-installed callback gets initially executed
by the library's internal connection thread passing a
\c INSERT_LIBX_NAMESPACE::w::initial{} value for
the \ref callback_trigger_t "trigger parameter". This specifies the picker's
current color value at the time the callback gets installed.
The callback then gets invoked by the "Ok" button, or by current_color()
setting a new color.

Only one color picker callback is active at any time.
Installing another color picker callback replaces the previous one.

\note
The color picker callback gets installed by an internal execution thread.
It is possible that the old callback may still be invoked if the "Ok" button
was in the process of being clicked at the time of on_color_update(). In all
cases the old callback is no longer in effect at the time the new callback's
\c INSERT_LIBX_NAMESPACE::w::initial{} call. The
\c INSERT_LIBX_NAMESPACE::w::initial{} call indicates the removal of the
previous callback.

Passing a
\ref color_picker_config "INSERT_LIBX_NAMESPACE::w::color_picker_config"
to a \ref factory "factory's" create_color_picker() method allows setting
the callback when the color picker gets created. The
\c INSERT_LIBX_NAMESPACE::w::initial{} callback invocation does not happen in
that case.

*/

typedef ref<color_pickerObj> color_picker;

//! A constant \ref color_picker "color picker".

//! \see color_picker

typedef const_ref<color_pickerObj> const_color_picker;

//! A nullable pointer reference to a \ref color_picker "color picker".

//! \see color_picker

typedef ptr<color_pickerObj> color_pickerptr;

//! A nullable pointer reference to a const \ref color_picker "color picker".

//! \see color_picker

typedef const_ptr<color_pickerObj> const_color_pickerptr;

//! Signature for the \ref color_picker "color picker"'s callback.

typedef void color_picker_callback_t(THREAD_CALLBACK,
				     const rgb &new_color,
				     const callback_trigger_t &trigger,
				     const busy &mcguffin);

LIBCXXW_NAMESPACE_END

#endif
