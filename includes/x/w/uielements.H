/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_uielements_h
#define x_w_uielements_h

#include <x/w/uielementsfwd.H>
#include <x/w/uigeneratorsfwd.H>
#include <x/w/listitemhandle.H>
#include <x/w/factory.H>
#include <x/w/shortcut.H>
#include <x/w/container.H>
#include <x/w/synchronized_axisfwd.H>
#include <x/w/copy_cut_paste_menu_items.H>
#include <x/w/layoutmanagerfwd.H>
#include <x/w/text_param.H>
#include <x/w/validated_input_field_contents.H>
#include <x/w/validated_input_fieldfwd.H>
#include <x/functionalrefptr.H>
#include <x/vector.H>
#include <x/hash.H>
#include <unordered_map>
#include <map>
#include <string>
#include <optional>

LIBCXXW_NAMESPACE_START

/*! Optional elements factory specified in a theme file.

The internal UI compiler parses an optional \<elements>, and records its
name and the compiled instructions here, if they exist.
*/

struct named_element_factory {
	//! The name of the element factory
	std::string name;

	//! The compiled generator
	const_vector<elements_generator> generator;
};

/*! XML element factory.

Passed to a supported layout manager's generate(), to
\ref uigenerators "generate the contents of the container from an XML template".

\note
This is not a thread-safe object. It's expected to be constructed in
automatic scope, and then used with the generator. Use get_element()
and other methods to retrieve the generated widgets and other objects, then
this element factory object goes out of scope and gets destroyed.

The widgets get created from the template. The first member of this object
is a map for custom widget factories.
The key is the custom \<element>'s \<name> from the
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

The second member of this object is a map for custom layout manager

*/

struct uielements {

	//! Generators for factories

	//! Referenced by name, from the template.

	std::unordered_map<std::string,
			   functionref<void (const factory &)>> factories;

	//! Custom creators for layout managers.

	//! Referenced by name, from the template.

	std::unordered_map<std::string,
			   functionref<void (const layoutmanager &)>
			   > layout_creators;

	//! Pre-defined text hotspot callbacks

	label_hotspots_t label_hotspots;

	//! Input field validators

	std::unordered_map<
		std::string,
		std::tuple<input_field_validation_callback,
			   ref<validated_input_field_contents_baseObj>>,
		std::hash<void>,
		std::equal_to<void>> input_field_validators;

	//! Install an input field validator

	//! Shortcut for calling create_validated_input_field_contents()
	//! and then emplacing the result into input_field_validators.
	//!
	//! Returns the 2nd value returned from emplace().

	template<input_field_validation_function validation_function,
		 typename canonical_format_functor,
		 typename new_value_functor=default_new_value_function<
			 input_field_validation_function_type<validation_function>
			 >
		 >
	bool create_validated_input_field(
		std::string id,
		validation_function &&validator,
		canonical_format_functor &&formatter,
		const std::optional<input_field_validation_function_type<
		validation_function>> &initial_value=std::nullopt,
		new_value_functor &&new_value=new_value_functor{}
	)
	{
		return input_field_validators.emplace(
			std::move(id),
			create_validated_input_field_contents(
				std::forward<validation_function>(validator),
				std::forward<canonical_format_functor>(
					formatter
				),
				initial_value,
				std::forward<new_value_functor>(new_value)
			)
		).second;
	}

	//! Install an input field validator

	//! Shortcut for calling create_string_validated_input_field_contents()
	//! and then emplacing the result into input_field_validators.
	//!
	//! Returns the 2nd value returned from emplace().

	template<typename T,
		 input_field_string_validation_function<T> validation_function,
		 typename canonical_format_functor,
		 typename new_value_functor=default_new_value_function<T>
		 >
	bool create_string_validated_input_field(
		std::string id,
		validation_function &&validator,
		canonical_format_functor &&canonical_format,
		const std::optional<T> &initial_value=std::nullopt,
		new_value_functor &&new_value=new_value_functor{}
	)
	{
		return input_field_validators.emplace(
			std::move(id),
			create_string_validated_input_field_contents<T>(
				std::forward<validation_function>(validator),
				std::forward<canonical_format_functor>(
					canonical_format
				),
				initial_value,
				std::forward<new_value_functor>(new_value)
			)
		).second;
	}

	//! The new elements that get created

	//! generate() puts new elements that get created in here.

	std::unordered_map<std::string, element,
			   std::hash<void>,
			   std::equal_to<void>> new_elements;

	//! The new validated_input_field objects that get created

	std::unordered_map<std::string, ref<obj>,
			   std::hash<void>,
			   std::equal_to<void>> new_validated_input_fields;

	//! Retrieve the validated input field

	template<typename T>
	validated_input_field<T> get_validated_input_field(
		const std::string_view &name) const
	{
		auto obj=get_validated_input_field_obj(name);

		if (!obj->isa<validated_input_field<T>>())
		{
			validated_input_field_not_found(name);
		}
		return obj;
	}

private:

	ref<obj> get_validated_input_field_obj(const std::string_view &) const;

	static void validated_input_field_not_found(const std::string_view &)
		__attribute__((noreturn));
public:
	//! The new layout managers that get created.

	//! generate() saved layout managers of created containers in here.

	std::unordered_map<std::string, layoutmanager,
			   std::hash<void>,
			   std::equal_to<void>> new_layoutmanagers;

	//! Container for the generated synchronized axis handles.

	//! Implements default constructors and destructors, avoiding the
	//! need to pull in the full definition of synchronized_axis objects.

	struct new_synchronized_axis_t :
		std::unordered_map<std::string, synchronized_axis,
				   std::hash<void>,
				   std::equal_to<void>> {

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

	//! New synchronized axis objects.
	new_synchronized_axis_t new_synchronized_axis;

	//! New list item handles
	std::unordered_map<std::string, listitemhandle> new_list_item_handles;

	//! New copy_cut_paste_menu_item
	copy_cut_paste_menu_itemsptr new_copy_cut_paste_menu_items;

	//! Return a created widget.

	//! An exception gets thrown if the specified widget was not found.

	element get_element(const std::string_view &name) const;

	//! Return a synchronized axis
	synchronized_axis get_synchronized_axis(const std::string_view &name)
		const;

	//! Return a created listitemhandle
	listitemhandle get_listitemhandle(const std::string_view &name)
		const;

	//! Return a created layoutmanager
	layoutmanager get_layoutmanager(const std::string_view &name)
		const;

	//! Process the named element factory after generating some layout.

	//! The layout gets recorded in the element_factories_to_generate,
	//! temporarily, until the entire layout or factory is completely
	//! generated, then they get processed in order.
	void generate_factory(const named_element_factory &);

	//! Temporary container that stores layouts to generate

	//! generate_factory() collects them here, then when all layouts
	//! or factories get generated, these get generated as well, then
	//! this container gets emptied.

	std::map<std::string, const_vector<elements_generator>
		 > element_factories_to_generate;

	//! Generate something, using the generated elements.

	//! Generates using a \<factory> of \@type='elements'

	void generate(const std::string_view &name,
		      const const_uigenerators &generators);

	//! Invoked by generate()
	void autofocus_enable(const std::string &focusable_value)
		const LIBCXX_HIDDEN;

	//! Invoked by generate()
	void autofocus_disable(const std::string &focusable_value)
		const LIBCXX_HIDDEN;

	//! Invoked by generate()
	void request_focus(const std::string &focusable_value,
			   bool now_or_never) const
		LIBCXX_HIDDEN;

	//! Invoked by generate()

	void get_focus_first(const std::string &focusable_value) const
		LIBCXX_HIDDEN;

	//! Invoked by generate()

	void get_focus_before(const std::string &focusable_value,
			      const std::string &before_focusable_value) const
		LIBCXX_HIDDEN;

	//! Invoked by generate()

	void get_focus_after(const std::string &focusable_value,
			     const std::string &after_focusable_value) const
		LIBCXX_HIDDEN;

	//! Invoked by generate()

	void get_focus_before_me(const std::string &focusable_value,
				 const std::vector<std::string>
				 &other_focusables_value) const
		LIBCXX_HIDDEN;

	//! Invoked by generate()

	void get_focus_after_me(const std::string &focusable_value,
				const std::vector<std::string>
				&other_focusables_value) const
		LIBCXX_HIDDEN;


	//! Invoked by generate()

	void show_all(const std::string &element_value) const;

	//! Invoked by generate()

	void hide_all(const std::string &element_value) const;

	//! Invoked by generate()

	void show(const std::string &element_value) const;

	//! Invoked by generate()

	void hide(const std::string &element_value) const;

};

LIBCXXW_NAMESPACE_END

#endif
