/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_factoryfwd_h
#define x_w_factoryfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC factoryObj;

/*! A widget factory

A factory object gets created by \ref layoutmanager "layout managers".

Depending on the layout manager, the same factory object can be used
to repetitively create multiple widgets; or a factory object
can be used for creating no more than one widget, with unspecified
results (a thrown exception is typical) if the factory object gets reused.

\par Creator lambdas

The factory provides various methods to create different kinds of
widgets. Each method returns the newly-created widget,
and some methods' first argument is a "creator lambda".

The creator lambda gets invoked just before the factory method
finishes
creating the new widget, and the parameter to the lambda is the newly-
created widget. The lambda's job is to finalize any needed
initialization of the new widget. This results in optimal
performance. The new widget's container integrates the new
widget after the creator returns and before the factory method itself
returns. The container generally ignores the new widget until
its creator returns.

If the lambda does not initialize the contents of the new widget (where appropriate), and that occurs after the factory method
returns, the new widget's container will often need to make
expensive recalculations if the new widget's size changes
(likely). By having the lambda handle the initialization, these calculation
occur, for the first time, after the creator returns with a
fully-initialized widget,

\par Canvases

\code
#include <INSERT_LIBX_NAMESPACE/w/canvas.H>

INSERT_LIBX_NAMESPACE::w::canvas_config config;

config.width=4.0;
config.height=4.0;

INSERT_LIBX_NAMESPACE::w::canvas new_canvas=
   factory->create_canvas(config);

INSERT_LIBX_NAMESPACE::w::canvas new_canvas=factory->create_canvas();

\endcode

create_canvas() creates an empty widget that does nothing but
take up its alloted space. This is typically used to fill in the unused
widgets in a \ref gridlayoutmanager "grid".

create_canvas()'s optional parameter is a \ref canvas_config "canvas_config",
which provides the
\ref dim_axis_arg "size" of the canvas, its width and height, as well
as an optional background color.

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
                                // Creator lambda
                          }, INSERT_LIBX_NAMESPACE::w::new_gridlayoutmanager());
\endcode

create_container() creates a new widget that is, itself, another container.

A grid layout manager's sophisticated layout capabilities are simply
not enough sometimes. For those situations, one of its widgets can simply
be another layout manager, such as the grid layout manager.

Also, many specialized widgets, such as combo-boxes, are actually
containers with a specialized layout manager, as described below.

The first parameter is a lambda that initializes the new container before
it's attached to its parent widget. The lambda receives a single
parameter, the new \ref container "INSERT_LIBX_NAMESPACE::w::container" and
 the lambda gets invoked just before create_container() returns.

The second parameter specifies the layout manager for the new container.
It can be one of:

- \ref new_gridlayoutmanager "INSERT_LIBX_NAMESPACE::w::new_gridlayoutmanager":
the new container uses the \ref gridlayoutmanager "grid layout manager".

- \ref new_pagelayoutmanager "INSERT_LIBX_NAMESPACE::w::new_pagelayoutmanager":
the new container uses the \ref pagelayoutmanager "page layout manager".

- \ref new_toolboxlayoutmanager "INSERT_LIBX_NAMESPACE::w::new_toolboxlayoutmanager":
the new container uses the \ref toolboxlayoutmanager "toolbox layout manager".

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
                                // Creator lambda
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

- \ref new_tablelayoutmanager "INSERT_LIBX_NAMESPACE::w::new_tablelayoutmanager":
the new container uses the \ref tablelayoutmanager "table layout manager".

- \ref new_itemlayoutmanager "INSERT_LIBX_NAMESPACE::w::new_itemlayoutmanager":
the new container uses the \ref itemlayoutmanager "item layout manager".

- \ref new_panelayoutmanager "INSERT_LIBX_NAMESPACE::w::new_panelayoutmanager":
the new container uses the \ref panelayoutmanager "pane layout manager".

\par Labels

\code
#include <INSERT_LIBX_NAMESPACE/w/label.H>

INSERT_LIBX_NAMESPACE::w::label new_label=
   factory->create_label({"Hello world!"});

INSERT_LIBX_NAMESPACE::w::label new_label=
   factory->create_label({"Hello world!"}, 40.0);
\endcode

create_label() creates a \ref label "text label widget". The first
parameter is a \ref text_param "INSERT_LIBX_NAMESPACE::w::text_param"
that sets the label's text.

Newline characters in the label create a multi-line label. An optional
second parameter sets the label's width, specified in millimeters.
The label's text gets word-wrapped to fit within the given width; otherwise
only newlines introduce linebreaks.

A word-wrapped label's width is flexible. The optional second parameter
only sets the suggested width. When placed in a container with other
widgets, the label adjusts its width to fit within its alloted
space, if additional space is available. If the label's width is not
spcified, the label's size remains fixed.

An optional third parameter sets the label's \ref halign "alignment".

\par Bordered widget elements

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

created_bordered_element() creates a container with a single widget that
has a border drawn around it. The container uses a special-purpose
\ref borderlayoutmanager "border layout manager" that's optimized for this
specific purpose. Using the \ref gridlayoutmanager "grid layout manager"
with a bordered cell results in the same appearance, but with more overhead.

\par Buttons

\code
#include <INSERT_LIBX_NAMESPACE/w/button.H>

INSERT_LIBX_NAMESPACE::w::button button=
   factory->create_button(
          []
          (const INSERT_LIBX_NAMESPACE::w::factory &f)
          {
              f->create_label({"Cancel"});
          });

INSERT_LIBX_NAMESPACE::w::button button=
   factory->create_button(
          []
          (const INSERT_LIBX_NAMESPACE::w::factory &f)
          {
              f->create_label({"Ok"});
          },
          {
               INSERT_LIBX_NAMESPACE::w::default_button()
          });

INSERT_LIBX_NAMESPACE::w::button button=factory->create_button(
     "Cancel"
);

#include <INSERT_LIBX_NAMESPACE/w/text_param_literals.H>

INSERT_LIBX_NAMESPACE::w::button button=factory->create_button(
     {
       "underline"_decoration,
       "C",
       "no"_decoration,
       "ancel"
     },
     {
         INSERT_LIBX_NAMESPACE::w::shortcut{"Alt", 'C'}
     });
\endcode

A clickable button. This is a container with one widget, with a border
drawn around it.
The first parameter to create_button() is a creator lambda that
gets invoked to create the contents of the button. The callable object must
use the passed-in factory object to create exactly one widget. This
is what the button looks like.

Alternatively, specifying a
\ref text_param "INSERT_LIBX_NAMESPACE::w::text_param" for the first
parameter creates a typical button containing a text label. This is equivalent
to using a creator lambda that does a create_label().

As with all other widgets, it is necessary to show() the widget, at some point. The created widget can be a container with
other, multiple, widgets.

create_button() has an optional second parameter that sets the new button's
options:

\code
#include <INSERT_LIBX_NAMESPACE/w/button.H>

INSERT_LIBX_NAMESPACE::w::button button=create_button(
    "Ok",
    {
       INSERT_LIBX_NAMESPACE::w::label_config{},
       INSERT_LIBX_NAMESPACE::w::default_button(),
       INSERT_LIBX_NAMESPACE::w::shortcut{'\n'}
    });
\endcode

The second optional parameter is a
\ref create_button_with_label_args_t "INSERT_LIBX_NAMESPACE::w::create_button_with_label_args_t"
when the first parameter is a \ref text_param "text_param", or a
\ref create_button_args_t "INSERT_LIBX_NAMESPACE::w::create_button_args_t"
if the first parameter is a callable object.

These are templates that implement the following
<a href="/optionalargs.html">optional function arguments</a>:

\code
#include <INSERT_LIBX_NAMESPACE/w/button.H>

INSERT_LIBX_NAMESPACE::w::button button=create_button(
    "Ok",
    {
       INSERT_LIBX_NAMESPACE::w::default_button(),
    });

INSERT_LIBX_NAMESPACE::w::button button=create_button(
    "Ok",
    {
       INSERT_LIBX_NAMESPACE::w::shortcut{'\n'}
    });
\endcode

Each one of the following arguments is optional, but the arguments,
if specified, must appear in the following order:

- the \ref label_config "INSERT_LIBX_NAMESPACE::w::label_config" parameter
exists only in the \c create_button_with_label_args_t that's used when the
first parameter is a \c text_param, and gets forwarded to create_label().

- the \ref button_config "INSERT_LIBX_NAMESPACE::w::button_config" parameter
controls the visual appearance of the new button, and defaults to the value
returned by \ref normal_button "INSERT_LIBX_NAMESPACE::w::normal_button"(), the
default button border and background color.
\ref default_button "INSERT_LIBX_NAMESPACE::w::default_button"() returns the
settings for a button with a thicker border, visually indicating the default
button on a form.

- the \ref shortcut "INSERT_LIBX_NAMESPACE::w::shortcut" parameter specifies
an optional keyboard shortcut that activates the button.

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

create_checkbox() creates a simple checkbox widget.

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

        INSERT_LIBX_NAMESPACE::w::gridfactory f=glm->append_row();
        f->halign(INSERT_LIBX_NAMESPACE::w::halign::center);

        glm->create_label("0%");
    },
    config,
    INSERT_LIBX_NAMESPACE::w::new_gridlayoutmanager{});

pb->show_all();

\endcode

create_progressbar() creates an
\ref progressbar "INSERT_LIBX_NAMESPACE::w::progressbar"
which is a horizontal swath that  visually indicates the
progress of an ongoing process. The horizontal swath has a colored
stripe that moves from its left margin to its right margin, to indicate
the progress of an operation.

The progress bar is a container with a layout manager, a
\ref gridlayoutmanager "grid layout manager" by default.
create_progressbar()'s
first parameter is a creator lambda that creates the contents
of the progress bar. Each one of the remaining parameters are optional.
If both are specified they must appear in the following order:

- an \ref progressbar_config "INSERT_LIBX_NAMESPACE::w::progressbar_config"
for customizing the progress bar's appearance.

- the new progress bar's layout manager, this defaults
to \ref new_gridlayoutmanager "INSERT_LIBX_NAMESPACE::w::new_gridlayoutmanager".

The description inside the progress bar is typically just a
centered label, but can
be any arbitrary widget. The creator lambda  constructs
a single label for the initial status of the progress bar.

\par Scroll bars

The factory automatically supplies standard scroll-bars to widgets
that use them. create_horizontal_scrollbar() and create_vertical_scrollbar()
create an independent scroll-bar, a convenient user interface for
specifying an integer value in a consecutive range.

\code
#include <INSERT_LIBX_NAMESPACE/w/scrollbar.H>

INSERT_LIBX_NAMESPACE::w::scrollbar_config config;

config.range=110;
config.page_size=10;
config.increment=5;
config.minimum_size=50;

INSERT_LIBX_NAMESPACE::w::scrollbar sb=factory->create_horizontal_scrollbar(
    config,
    []
    (const INSERT_LIBX_NAMESPACE::w::scrollbar_info_t &info)
    {

    });
\endcode

The first parameter is the new scrollbar's
\ref scrollbar_config "configuration".
The remaining two parameters are optional. Either of the following
parameters is optional, but they must be specified in the given order:

- The scroll-bar's initial callback that gets executed whenever the
scrollbar's value changes. Alternatively, an existing scroll-bar's on_update()
installs the callback.

- A \ref scrollbar_appearance "scrollbar appearance object" parameter.

\ref scrollbar_config "scrollbar_config"'s \c minimum_size sets the
scroll-bar's minimum size
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
This is a widget that provides an interactive approach for
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
This is a widget that provides an interactive approach for
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
