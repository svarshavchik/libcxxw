/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_uielements_h
#define x_w_uielements_h

#include <x/w/uielementsfwd.H>
#include <x/w/listitemhandle.H>
#include <x/w/factory.H>
#include <x/w/shortcut.H>
#include <x/w/container.H>
#include <x/w/radio_groupfwd.H>
#include <x/w/synchronized_axisfwd.H>
#include <x/w/copy_cut_paste_menu_items.H>
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
is expected to use the factory to create a widget:

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

	//! Generators for new widgets.

	//! Referenced by name, from the template.

	std::unordered_map<std::string,
			   functionref<void (const factory &)>> factories;

	//! The new elements that get created

	//! generate() puts new elements that get created in here.

	std::unordered_map<std::string, element> new_elements;

	//! Container for the generated radio groups

	//! Implements default constructors and destructors, avoiding the
	//! need to pull in the full definition of radio_group objects.

	struct new_radio_groups_t :
		std::unordered_map<std::string, radio_group> {

		//! Constructor
		new_radio_groups_t();

		//! Destructor
		~new_radio_groups_t();

		//! Copy constructor
		new_radio_groups_t(const new_radio_groups_t &);

		//! Move constructor
		new_radio_groups_t(new_radio_groups_t &&);

		//! Assignment operator
		new_radio_groups_t &operator=(const new_radio_groups_t &);

		//! Move operator
		new_radio_groups_t &operator=(new_radio_groups_t &&);
	};

	//! Container for the generated synchronized axis handles.

	//! Implements default constructors and destructors, avoiding the
	//! need to pull in the full definition of synchronized_axis objects.

	struct new_synchronized_axis_t :
		std::unordered_map<std::string, synchronized_axis> {

		//! Constructor
		new_synchronized_axis_t();

		//! Destructor
		~new_synchronized_axis_t();

		//! Copy constructor
		new_synchronized_axis_t(const new_synchronized_axis_t &);

		//! Move constructor
		new_synchronized_axis_t(new_synchronized_axis_t &&);

		//! Assignment operator
		new_synchronized_axis_t
		&operator=(const new_synchronized_axis_t &);

		//! Move operator
		new_synchronized_axis_t &operator=(new_synchronized_axis_t &&);
	};

	//! Destructor
	~uielements();

	//! New radio button groups
	new_radio_groups_t new_radio_groups;

	//! New synchronized axis objects.
	new_synchronized_axis_t new_synchronized_axis;

	//! New list item handles
	std::unordered_map<std::string, listitemhandle> new_list_item_handles;

	//! New copy_cut_paste_menu_item
	copy_cut_paste_menu_itemsptr new_copy_cut_paste_menu_items;

	//! Return a created widget.

	//! An exception gets thrown if the specified widget was not found.

	element get_element(const std::string_view &name) const;

	//! Return a radio group
	radio_group get_radio_group(const std::string_view &name) const;

	//! Return a synchronized axis
	synchronized_axis get_synchronized_axis(const std::string_view &name)
		const;

	//! Return a created listitemhandle
	listitemhandle get_listitemhandle(const std::string_view &name)
		const;
};

LIBCXXW_NAMESPACE_END

#endif
