/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_factoryfwd_h
#define x_w_factoryfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC factoryObj;

/*! An element factory

A factory object is created by a \ref layoutmanager "layout manager".

Depending on the layout manager, the same factory object can be used
to repetitively create multiple display elements; or a factory object
can be used for creating no more than one element, with unspecified
results (a thrown exception is typical) if the factory object gets reused.

\par Initializer lambdas

The factory provides various methods to create different kinds of
display elements. Each method returns the newly-created display element,
and some methods' first argument is a "initializer lambda".

The initializer lambda gets invoked just before the factory method
finishes
creating the new element, and the parameter to the lambda is the newly-
created element. The lambda's job is to finalize any needed
initialization of the new display element. This results in optimal
performance. The new display element's container integrates the new
element after the creator returns and before the factory method itself
returns. The container generally ignores the new display element until
its creator returns.

If the lambda does not initialize the contents of the new display
element (where appropriate), and that occurs after the factory method
returns, the new display element's container will often need to make
expensive recalculations if the new display element's size changes
(likely). By having the lambda handle the initialization, these calculation
occur, for the first time, after the creator returns with a
fully-initialized display element,

\par Canvases

\code
#include <INSERT_LIBX_NAMESPACE/w/canvas.H>

INSERT_LIBX_NAMESPACE::w::canvas new_canvas=
   factory->create_canvas([]
                          (const auto &new_canvas)
                          {
                                // Initializer lambda
                          }, {4}, {4});

INSERT_LIBX_NAMESPACE::w::canvas new_canvas=factory->create_canvas();

\endcode

create_canvas() creates an empty display element that does nothing but
take up the alloted space. This is typically used to fill in the unused
elements in a \ref gridlayoutmanager "grid".

Besides its initializer lambda (which doesn't really have much to
initializer), create_canvas()'s remaining two parameters specify the
\ref dim_axis_arg "size" of the canvas, its width and height.

An overloaded create_canvas() with no parameters
returns a canvas that's adjustable to any
size. It is equivalent to specify a minimum width and height of 0, and
no bounds to the maximum width and height.

\par Nested containers

\code
#include <INSERT_LIBX_NAMESPACE/w/container.H>
#include <INSERT_LIBX_NAMESPACE/w/gridlayoutmanager.H>

INSERT_LIBX_NAMESPACE::w::container new_container=
   factory->create_container([]
                          (const auto &new_container)
                          {
                                // Initializer lambda
                          }, INSERT_LIBX_NAMESPACE::w::new_gridlayoutmanager());
\endcode

create_container() creates a new element that is, itself, another container.

A grid layout manager's sophisticated layout capabilities are simply
not enough sometimes. For those situations, one of its elements can simply
be another layout manager, such as the grid layout manager.

Also, many specialized display elements, such as combo-boxes, are actually
containers with a specialized layout manager, as described below.

The first parameter is a lambda that initializes the new container before
it's attached to its parent display element. The lambda receives a single
parameter, the new \ref container "INSERT_LIBX_NAMESPACE::w::container" and
 the lambda gets invoked just before create_container() returns.

The second parameter specifies the layout manager for the new container.
It can be one of:

- \ref new_gridlayoutmanager "INSERT_LIBX_NAMESPACE::w::new_gridlayoutmanager":
the new container uses the \ref gridlayoutmanager "grid layout manager".

- \ref new_pagelayoutmanager "INSERT_LIBX_NAMESPACE::w::new_pagelayoutmanager":
the new container uses the \ref pagelayoutmanager "page layout manager".

\code
#include <INSERT_LIBX_NAMESPACE/w/focusable_container.H>
#include <INSERT_LIBX_NAMESPACE/w/listlayoutmanager.H>

INSERT_LIBX_NAMESPACE::w::new_listlayoutmanager list_style{
    INSERT_LIBX_NAMESPACE::w::highlighted_list
};

INSERT_LIBX_NAMESPACE::w::focusable_container new_container=
   factory->create_focusable_container([]
                          (const auto &new_container)
                          {
                                // Initializer lambda
                          }, list_style);
\endcode

create_focusable_container() is similar to create_container(), but
creates a container that handles input focus. Its second parameter can be one
of:

- \ref new_listlayoutmanager "INSERT_LIBX_NAMESPACE::w::new_listlayoutmanager":
the new container uses the \ref listlayoutmanager "list layout manager".
This creates a selection list.

- \ref new_custom_comboboxlayoutmanager "INSERT_LIBX_NAMESPACE::w::new_custom_comboboxlayoutmanager": the new container uses the
\ref custom_comboboxlayoutmanager "custom combo-box layout manager".

- \ref new_standard_comboboxlayoutmanager "INSERT_LIBX_NAMESPACE::w::new_standard_comboboxlayoutmanager": the new container uses the
\ref standard_comboboxlayoutmanager "standard combo-box layout manager".

- \ref new_editable_comboboxlayoutmanager "INSERT_LIBX_NAMESPACE::w::new_editable_comboboxlayoutmanager": the new container uses the
\ref editable_comboboxlayoutmanager "editable combo-box layout manager".

- \ref new_booklayoutmanager "INSERT_LIBX_NAMESPACE::w::new_booklayoutmanager":
the new container uses the \ref booklayoutmanager "book layout manager".

\par Labels

\code
#include <INSERT_LIBX_NAMESPACE/w/label.H>

INSERT_LIBX_NAMESPACE::w::label new_label=
   factory->create_label({"Hello world!"});

INSERT_LIBX_NAMESPACE::w::label new_label=
   factory->create_label({"Hello world!"}, 40.0);
\endcode

create_label() creates a \ref label "text label element". The first
parameter is a \ref text_param "INSERT_LIBX_NAMESPACE::w::text_param"
that sets the label's text.

Newline characters in the label create a multi-line label. An optional
second parameter sets the label's width, specified in millimeters.
The label's text gets word-wrapped to fit within the given width; otherwise
only newlines introduce linebreaks.

A word-wrapped label's width is flexible. The optional second parameter
only sets the suggested width. When placed in a container with other
display elements, the label adjusts its width to fit within its alloted
space, if additional space is available. If the label's width is not
spcified, the label's size remains fixed.

An optional third parameter sets the label's \ref halign "alignment".

\par Bordered elements

\code
INSERT_LIBX_NAMESPACE::w::container border_container=
   factory->create_bordered_element
     ([]
      (const INSERT_LIBX_NAMESPACE::w::factory &f)
      {
         auto l=f->create_label("There's a border around me");

         l->show();
      },
      "inputfocuson_border",
      1, 1);
\endcode

created_bordered_element() creates a container with a single element that
has a border drawn around it. The container uses a special-purpose
\ref borderlayoutmanager "border layout manager" that's optimized for this
specific purpose. Using the \ref gridlayoutmanager "grid layout manager"
with a bordered cell results in the same appearance, but with more overhead.

\par Buttons

\code
#include <INSERT_LIBX_NAMESPACE/w/button.H>

INSERT_LIBX_NAMESPACE::w::button button=
   factory->create_normal_button(
          []
          (const INSERT_LIBX_NAMESPACE::w::factory &f)
          {
              f->create_label({"Cancel"});
          });

INSERT_LIBX_NAMESPACE::w::button button=
   factory->create_special_button(
          []
          (const INSERT_LIBX_NAMESPACE::w::factory &f)
          {
              f->create_label({"Ok"});
          });

INSERT_LIBX_NAMESPACE::w::button button=
   factory->create_normal_button_with_label({"Cancel"});

INSERT_LIBX_NAMESPACE::w::button button=
   factory->create_special_button_with_label({"Cancel"});
\endcode

These functions create buttons. Buttons have either normal borders, or
"special" borders, that emphasize that the button is the default choice,
in some fashion.

\c create_normal_button()'s and \c create_special_button()'s
parameter
is an initializer lambda. The initializer lambda receives its own
factory as a parameter. The initializer lambda must use the factory to
create a single display element that servers as the contents of the
button.

create_normal_button_with_label() and
create_special_button_with_label() are short-cuts that use
\c create_label() to create the contents of the button, a plain
text label. They take the same parameters as create_label().

\code
INSERT_LIBX_NAMESPACE::w::button button=
   factory->create_normal_button_with_label({"Cancel"}, {'\n'});
\endcode

When creating a button, a \ref shortcut "INSERT_LIBX_NAMESPACE::w::shortcut"
is an optional second parameter that defines a keyboard shortcut
for activating the button even when it does not have keyboard input focus.

\par Input fields

\code
#include <INSERT_LIBX_NAMESPACE/w/input_field.H>

INSERT_LIBX_NAMESPACE::w::input_field field=
   factory->create_input_field({""},{40});
\endcode

create_input_field() creates an \ref input_field "input_field". The
first parameter to \c create_input_field is a
\ref text_param "text_param" that sets the input field's initial contents.

The \c text_param may not contain any special formatting, just plain text,
with one exception. The \c text_param may specify an initial font and/or
color:

\code
#include <INSERT_LIBX_NAMESPACE/w/input_field.H>
#include <INSERT_LIBX_NAMESPACE/w/text_param_literals.H>

INSERT_LIBX_NAMESPACE::w::input_field field=
   factory->create_input_field({"sans serif"_font,
                                INSERT_LIBX_NAMESPACE::w::rgb{0, 0,
				     INSERT_LIBX_NAMESPACE::w::rgb::maximum},
				"Hello world!"},{40});
\endcode

This creates a text input field that uses a non-default font and/or text
color.

The second parameter to \c create_input_field() is optional. It is an
\ref input_field_config "input_field_config" that sets the width of the
text input field, and various other properties.

\code
#include <INSERT_LIBX_NAMESPACE/w/date_input_field.H>

INSERT_LIBX_NAMESPACE::w::date_input_field field=
   factory->create_date_input_field();
\endcode

This creates a new date input field.

\par Images

\code
#include <INSERT_LIBX_NAMESPACE/w/image.H>

INSERT_LIBX_NAMESPACE::w::image image=factory->create_image("./image1.gif");
\endcode

This creates an
\ref image "INSERT_LIBX_NAMESPACE::w::image" that shows an image loaded
from a file.

\note
Filenames without a path component get searched for in the
current display theme's directory, and NOT the current
directory. Use "./filename" to load an image file from the current directory.

\par Checkboxes and radio buttons.

\code
#include <INSERT_LIBX_NAMESPACE/w/image_button.H>

INSERT_LIBX_NAMESPACE::w::image_button
    checkbox=factory->create_checkbox([]
                                      (const x::w::factory &f)
                                      {
                                           f->create_label("Check me!")->show();
                                      });
\endcode

create_checkbox() creates a simple checkbox display element.

\code
#include <INSERT_LIBX_NAMESPACE/w/radio_group.H>
#include <INSERT_LIBX_NAMESPACE/w/image_button.H>

auto group=INSERT_LIBX_NAMESPACE::w::radio_group::create();

INSERT_LIBX_NAMESPACE::w::image_button
     button=factory->create_radio(group,
                                  []
                                  (const x::w::factory &f)
                                  {
                                       f->create_label("Good choice!")->show();
                                  });
\endcode

A \ref radio_group "radio_group" object gets created in advance.
create_radio() creates a new radio button.

\par Progress bars

\code
#include <INSERT_LIBX_NAMESPACE/w/progressbar.H>

INSERT_LIBX_NAMESPACE::w::progressbar_config config;

INSERT_LIBX_NAMESPACE::w::progressbar pb=factory->create_progressbar(
    []
    (const INSERT_LIBX_NAMESPACE::w::progressbar &pb)
    {
        INSERT_LIBX_NAMESPACE::w::gridlayoutmanager glm=pb->get_layoutmanager();

        // ...
    },
    config,
    INSERT_LIBX_NAMESPACE::w::new_gridlayoutmanager{});
\endcode

create_progressbar() creates an
\ref progressbar "INSERT_LIBX_NAMESPACE::w::progressbar"
which is a horizontal swath that's meant to visually indicate the
progress of an ongoing process. The horizontal swath has a colored
striped that moves from its left margin to its right margin, to indicate
the progress of an operation.

The progress bar is a container with a layout manager, that's meant to
position an additional descriptive label inside the progress bar.
The first parameter is an initializer lambda that creates the contents
of the progress bar. The remaining parameters are optional:

- an \ref progressbar_config "INSERT_LIBX_NAMESPACE::w::progressbar_config"
for customizing the progress bar's appearance.

- the third parameter sets the progress bar's layout manager, and defaults
to \ref new_gridlayoutmanager "INSERT_LIBX_NAMESPACE::w::new_gridlayoutmanager".

\par Scroll bars

The factory automatically supplies standard scroll-bars to display elements
that use them. create_horizontal_scrollbar() and create_vertical_scrollbar()
create an independent scroll-bar, a convenient user interface for
specifying an integer value in a consecutive range.

\code
#include <INSERT_LIBX_NAMESPACE/w/scrollbar.H>

INSERT_LIBX_NAMESPACE::w::scrollbar_config config;

config.range=110;
config.page_size=10;
config.increment=5;

INSERT_LIBX_NAMESPACE::w::scrollbar sb=factory->create_horizontal_scrollbar(
    config,
    []
    (const INSERT_LIBX_NAMESPACE::w::scrollbar_info_t &info)
    {

    },
    50);
\endcode

The first parameter is the new scrollbar's
\ref scrollbar_config "configuration".
The remaining parameters are optional. The second parameter is the scroll-bar's
initial callback that gets executed whenever the scrollbar's value changes.
The third parameter sets the minimum size of the scroll-bar,
in millimeters. The scroll-bar's actual size may be also adjusted by its
layout manager. Irrespective of the scroll-bar's ultimate size, the reported
scroll-bar values get scaled to its configured range and page size.

\par Color pickers

\code
#include <INSERT_LIBX_NAMESPACE/w/color_picker.H>
#include <INSERT_LIBX_NAMESPACE/w/color_picker_config.H>

INSERT_LIBX_NAMESPACE::w::color_picker_config config;

INSERT_LIBX_NAMESPACE::w::color_picker cp=factory->create_color_picker(config);

\endcode

create_color_picker() returns
\ref color_picker "INSERT_LIBX_NAMESPACE::w::color_picker".
This is a display element that provides an interactive approach for
specifying an \ref rgb "INSERT_LIBX_NAMESPACE::w::rgb" value.
create_color_picker()'s optional
\ref color_picker_config "INSERT_LIBX_NAMESPACE::w::color_picker_config"
parameter sets the initial appearance of the color-picker.

\par Font pickers

\code
#include <INSERT_LIBX_NAMESPACE/w/font_picker.H>
#include <INSERT_LIBX_NAMESPACE/w/font_picker_config.H>

INSERT_LIBX_NAMESPACE::w::color_picker_config config;

INSERT_LIBX_NAMESPACE::w::color_picker cp=factory->create_color_picker(config);

\endcode

create_font_picker() returns
\ref font_picker "INSERT_LIBX_NAMESPACE::w::font_picker".
This is a display element that provides an interactive approach for
setting a \ref font "INSERT_LIBX_NAMESPACE::w::font" specification.
create_font_picker()'s optional
\ref font_picker_config "INSERT_LIBX_NAMESPACE::w::font_picker_config"
parameter sets the initial appearance of the font-picker.

*/

typedef ref<factoryObj> factory;

//! A nullable pointer refereance to a factory.

//! \see factory

typedef ptr<factoryObj> factoryptr;

//! A constant factory

//! \see factory

typedef const_ref<factoryObj> const_factory;

//! A nullable pointer refereance to a constant factory.

//! \see factory

typedef const_ptr<factoryObj> const_factoryptr;

LIBCXXW_NAMESPACE_END

#endif
