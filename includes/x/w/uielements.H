/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_uielements_h
#define x_w_uielements_h

#include <x/w/uielementsfwd.H>
#include <x/w/factory.H>
#include <x/w/shortcut.H>
#include <x/w/container.H>
#include <x/functionalrefptr.H>
#include <unordered_map>
#include <string>

LIBCXXW_NAMESPACE_START

/*! XML element factory.

Passed to a supported layout manager's generate(), to
\ref uigenerators "generate the contents of the container from an XML template".

The first element is a map. The key is the \<element>'s \<name> from the
XML template. The value is a lambda, or a callable object. \<element>
invokes the lambda, passing to it a \ref factory "factory". The lambda
is expected to use the factory to create a display element:

\code
INSERT_LIBX_NAMESPACE::w::uielements factories{
    {
          {"label",
           [&]
           (const INSERT_LIBX_NAMESPACE::w::factory &f)
           {
                  f->create_label("Lorem ipsum");
           }
          },
          {"button",
           [&]
           (const INSERT_LIBX_NAMESPACE::w::factory &f)
           {
                  f->create_button("Lorem ipsum");
           }
          },
    }
};
\endcode

In the XML template, an

\code{.unparsed}
<element>
    <name>label</name>
<element>
\endcode

results in the corresponding lambda getting invoked.

*/

struct uielements {

	//! Generators for new display elements.

	//! Referenced by name, from the template.

	std::unordered_map<std::string,
			   functionref<void (const factory &)>> factories;

	//! Standalone shortcuts, currently used for book pages.

	std::unordered_map<std::string, shortcut> shortcuts;

	//! The internal containers that get created.

	//! Generate puts new containers that get created in here.

	std::unordered_map<std::string, container> new_layouts;
};

LIBCXXW_NAMESPACE_END

#endif
