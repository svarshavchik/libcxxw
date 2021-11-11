/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_focusable_labelfwd_h
#define x_w_focusable_labelfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC focusable_labelObj;

struct focusable_label_config;

/*! \brief A label that processes input focus.

A label \ref label "label" that has a focus frame and offers
callbacks for processing keyboard input.

It also inherits from a container object, this is for internal use only.

This widget implements the shown current value in combo-boxes.

\code

INSERT_LIBX_NAMESPACE::w::focusable_label label=factory->create_focusable_label(
    {
      "Click ",
      INSERT_LIBX_NAMESPACE::w::start_hotspot{"hotspot1"},
      "here",
      INSERT_LIBX_NAMESPACE::w::end_hotspot{},
    },
    {
         {"hotspot1",
            []
            (ONLY IN_THREAD,
             const INSERT_LIBX_NAMESPACE::w::text_event_t &event)
            {
                // ...
            }
         },
    });
\endcode

Creates a \ref focusable_label "focusable label" widget, a label with clickable
parts. The first parameter is a
\ref text_param "INSERT_LIBX_NAMESPACE::w::text_param" with clickable
parts designated by hotspot markers.

create_focusable_label()'s second parameter is a
\ref label_hotspots_t "INSERT_LIBX_NAMESPACE::w::label_hotspots_t" container
that maps hotspot identifiers to hotspot handler callbacks.

create_focusable_label()'s optional third parameter is a
\ref focusable_label_config "INSERT_LIBX_NAMESPACE::w::focusable_label_config".

\code
#include <INSERT_LIBX_NAMESPACE/w/text_param_literals.H>

    label->update(
       {
          "Click ",
          "hotspot1"_hotspot,
          INSERT_LIBX_NAMESPACE::w::start_hotspot{"hotspot1"},
          "here",
	  end_hotspot{},
          " again",
       },
       {
         {"hotspot1",
            []
            (ONLY IN_THREAD,
             const INSERT_LIBX_NAMESPACE::w::text_event_t &event)
            {
                // ...
            }
         },
       });
\endcode

Focusable labels' update() sets the label's new contents and any hotspot
callbacks. \c text_param_literals.H imports user defined literals:

\code
     "label"_hotspot

     1_hotspot
\endcode

These user-defined literals construct a \ref start_hotspot "start_hotspot"
parameter, from a literal string or an \c int value. Additionally the
\ref end_hotspot "INSERT_LIBX_NAMESPACE::w::end_hotspot" placeholder gets
imported from the \c INSERT_LIBX_NAMESPACE::w namespace.

*/

typedef ref<focusable_labelObj> focusable_label;

//! A constant \ref focusable_label "focusable label".

//! \see focusable_label

typedef const_ref<focusable_labelObj> const_focusable_label;

//! A nullable pointer reference to a \ref focusable_label "focusable label".

//! \see focusable_label

typedef ptr<focusable_labelObj> focusable_labelptr;

//! A nullable pointer reference to a const \ref focusable_label "focusable label".

//! \see focusable_label

typedef const_ptr<focusable_labelObj> const_focusable_labelptr;

LIBCXXW_NAMESPACE_END

#endif
