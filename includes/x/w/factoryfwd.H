/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_factoryfwd_h
#define x_w_factoryfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class factoryObj;

/*! An element factory

A factory object is created by a \ref layoutmanager "layout manager".

Depending on the layout manager, the same factory object can be used
to repetitively create multiple display elements; or a factory object
can be used for creating no more than one element, with unspecified
results if the factory object gets reused.

\par Initializer lambdas

The factory provides various methods to create different kinds of
display elements. Each method returns the newly-created display element,
and most methods' first argument is a "initializer lambda".

The initializer lambda gets invoked just before the factory method
finishes
creating a new element, and the parameter to the lambda is the newly-
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
#include <INSERT_LIBX_NAMESPACE/w/metrics/mmaxis.H>

INSERT_LIBX_NAMESPACE::w::metrics::mmaxis width{4}, height{4};

INSERT_LIBX_NAMESPACE::w::canvas new_canvas=
   factory->create_canvas([]
                          (const auto &new_canvas)
                          {
                                // Initializer lambda
                          }, width, height);
\endcode

create_canvas() creates an empty display element that does nothing but
take up the alloted space. This is typically used to fill in the unused
elements in a \ref gridlayoutmanager "grid".

Besides its initializer lambda (which doesn't really have much to
initializer), create_canvas()'s remaining two parameters specify the
\ref metrics::mmaxis "size" of the new display element.

\par Nested containers

\code
#include <INSERT_LIBX_NAMESPACE/w/container.H>
#include <INSERT_LIBX_NAMESPACE/w/new_gridlayoutmanager.H>

INSERT_LIBX_NAMESPACE::w::container new_container=
   factory->create_container([]
                          (const auto &new_container)
                          {
                                // Initializer lambda
                          }, new_gridlayoutmanager());
\endcode

create_container() creates a new element that is, itself, another container.

A grid layout manager's sophisticated layout capabilities are simply
not enough sometimes. For those situations, one of its elements can simply
be another grid layout manager.

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

\par Images

\code
#include <INSERT_LIBX_NAMESPACE/w/image.H>

INSERT_LIBX_NAMESPACE::w::image image=factory->create_image_mm("image1.png");
\endcode

This creates a display element that shows an image.

*/

typedef ref<factoryObj> factory;

//! A constant generic window handler

//! \see factory

typedef const_ref<factoryObj> const_factory;

LIBCXXW_NAMESPACE_END

#endif
