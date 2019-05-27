/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_input_fieldfwd_h
#define x_w_input_fieldfwd_h

#include <x/w/input_field_configfwd.H>
#include <x/w/busyfwd.H>
#include <x/w/callback_triggerfwd.H>
#include <x/w/connection_threadfwd.H>
#include <x/w/factoryfwd.H>
#include <x/ptrfwd.H>
#include <string>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC input_fieldObj;
template<typename type> class validated_input_fieldObj;

//! Callback for validating the contents of the input field.

typedef bool input_field_validation_callback_t(THREAD_CALLBACK,
					       const callback_trigger_t &);

//! Parameter to the input field filter callback.

struct LIBCXX_HIDDEN input_field_filter_info;

//! Callback for filtering changes to the input field.

typedef void input_field_filter_callback_t(THREAD_CALLBACK,
					   const input_field_filter_info &);

/*! A text input field.

\code
#include <INSERT_LIBX_NAMESPACE/w/input_field.H>

INSERT_LIBX_NAMESPACE::w::input_field f=factory->create_input_field({""}, {20});
\endcode

This is a text input field, with a blinking cursor.

\code
f->set("Hello world");

f->set_unicode(U"Hello world");
\endcode

set() specifies the new contents of the input field.
set_unicode() specifies the new contents as a unicode string.

\code
INSERT_LIBX_NAMESPACE::w::input_lock lock{f};
\endcode

Constructing an \ref input_lock "INSERT_LIBX_NAMESPACE::w::input_lock"
blocks the internal library execution thread (and all other execution
threads from accessing the
\ref input_field "input_field", providing access to its contents).

Once locked, \ref input_lock "input_lock" implements methods to
retrieve the contents of the input field.

An \c input_lock is not required to set() new contents of the text
input field. The internal library execution thread actually updates
the contents, and set() only sends a message to the execution thread.
set() returns immediately after doing so.

\note
For that reason,
getting the contents of the input field immediately after setting it may
still retrieve the original contents. The input field, like all other
widgets, gets updated by an internal execution thread, which
might be busy with other things, before focusing its attention on
updating the input field's contents.

\code
f->on_change([]
             (ONLY IN_THREAD,
              const INSERT_LIBX_NAMESPACE::w::input_change_info_t &)
             {
             });

f->on_autocomplete([]
              (ONLY IN_THREAD,
               const INSERT_LIBX_NAMESPACE::w::input_autocomplete_info_t &)
              {
                      return false;
              });
\endcode

on_change() installs a callback that gets invoked whenever the
contents of the input field change. The callback receives an
\ref input_change_info_t "input_change_info_t" parameter describing the
change.


on_autocomplete() installs an additional callback that gets invoked
whenever the contents of the input field change as a result of typing
with the cursor at the end of the field. The callback receives a
\ref input_autocomplete_info_t "input_autocomplete_info_t" parameter.
Returning \c true from this callback results in the contents of the
input field getting updated.

\see factory

\par Validation

\code
f->on_validate([f=INSERT_LIBX_NAMESPACE::make_weak_capture(f)]
              (ONLY IN_THREAD,
               const INSERT_LIBX_NAMESPACE::w::callback_trigger_t &triggering_event)
              {
                  auto got=me.get();

                  if (!got)
                      return true;

                  auto &[me]=*got;

                  if (INSERT_LIBX_NAMESPACE::w::input_lock{me}.get() == "4")
                      return true;

                  return false;
              });
\endcode

on_validate() installs a callback that gets executed when:

- attempting to tab out of the input field, or

- a button press outside of the input field

A more detailed reason for the callback getting executed gets specified
by the \ref callback_trigger_t "triggering event" parameter.

The callback does not get executed if the input field's contents do not
change while the input field has focus, and the input field's contents
already passed validation. There are some rare edge cases where the input
field loses input focus in some other way (such as the not-very edge case
of the application explicitly invoke request_focus() for some other
widget). When the input field regains focus, the input field
becomes subject to validation once again.

Returning \c true marks the field as validated, and the input field
gets normally tabbed out of, or loses input focus as a result of a pointer
click else. Returning \c false does not move the input focus, and the
field's contents remain unvalidated.

\code
INSERT_LIBX_NAMESPACE::w::validated_input_field<int> validated_field=field->set_validator
       ([]
         (ONLY IN_THREAD,
          const std::string &value,
          const INSERT_LIBX_NAMESPACE::w::input_field &field,
          const INSERT_LIBX_NAMESPACE::w::callback_trigger_t &trigger)-> std::optional<int>
         {
             std::istringstream i{value};

             int n;

             if (i >> n)
                  return n;

             field->stop_message("Numeric input required");

             return std::nullopt;
         },
         []
         (int v) -> std::string
         {
             return std::to_string(v);
         },
         []
         (ONLY IN_THREAD, const std::optional<int> &v)
         {
             if (v)
	         std::cout << "New value: " << *v << std::endl;
             else
                 std::cout << "No new value" << std::endl;
         });

// ...

std::optional<int> value=validated_vield->validated_value.get();

\endcode

set_validator() uses on_validate() to provide a consistent input validation
framework for parsing and edit-checking the text input and converting it
to a specific data type.

set_validator() returns a \ref validated_input_field "INSERT_LIBX_NAMESPACE::w::validated_input_field<T>",
an object that represents the most recently validated contents of the
text input field. set_validator() takes two closures as parameters.
The first closure returns a \c std::optional<T>, where \c T is some
data type that the entered text should get converted to. This constrols
set_validator()'s return value as
\c INSERT_LIBX_NAMESPACE::w::validated_input_field<T>.

set_validator()'s first closure receives the following parameters:

- either a \c std::string or \c std::u32string, the closure receives
the current contents of the input field as either a UTF-8 string, or a
unicode string.

- an \c INSERT_LIBX_NAMESPACE::w::input_field reference, to its own input
field, for convenience.

- the triggering event.

The closure returns a \c T value to indicate the value of the converted
string, or a \c std::nullopt if the string is not valid.

The closure is responsible for reporting invalid input, and the usual
way to do that is to use the \c field parameter and call the input field
element's stop_message(), that displays an error dialog in the input field's
window.

In all cases the value returned by the closure gets placed into the
\ref validated_input_field "validated_input_field<T>" object that gets
returned from set_validator().

The second closure passed to set_validator() takes a
T parameter and returns either a \c std::string in the current locale's
encoding or a unicode string. The returned string represents the
entered \c T value in canonical form. It may or may not be identical
to what was entered into the text input field. It's expected that if the
returned value gets passed to the first closure, the first closure returns
the same value of \c T. The validator updates the text input field to the
canonical format.

The \ref date_input_field "date input field"'s parser provides a default
year, if none was typed in, and its formatter converts the full date to
canonical format. In this manner, typing in just the month and the day
then tabbing out of the date input field updates it to show the full
date.

The third closure is an optional parameter. If provided, the third closure
gets invoked after the first closure returns, and after the returned
\ref validated_input_field "INSERT_LIBX_NAMESPACE::w::validated_input_field<T>"
gets updated with the validated value. This provides a hook that gets
executed in the context where the \c validated_input_field reflects the
new value (it doesn't until the first closure returns). But, for convenience's
sake, the parsed value also gets passed to it as its first parameter.

\code
INSERT_LIBX_NAMESPACE::w::validated_input_field<int> validated_int=field->set_string_validator
        ([]
         (ONLY IN_THREAD,
          const std::string &value,
          int *parsed_value,
          const INSERT_LIBX_NAMESPACE::w::input_field &field,
          const INSERT_LIBX_NAMESPACE::w::callback_trigger_t &trigger) -> std::optional<int>
         {
             if (parsed_value)
             {
                 if (*parsed_value >= 0 && *parsed_value <= 99)
                     return *parsed_value;
             }
             else
             {
                 if (value.empty())
                 {
                     field->stop_message("Input required");
                     return std::nullopt;
                 }
             }

             field->stop_message("Must enter a number 0-99");
             return std::nullopt;
         },
         []
         (int v) -> std::string
         {
             return std::to_string(v);
         },
         []
         (ONLY IN_THREAD, const std::optional<int> &v)
         {
             if (v)
	         std::cout << "New value: " << *v << std::endl;
             else
                 std::cout << "No new value" << std::endl;
         });
\endcode

set_string_validator() is a shortcut for using set_validator() with
\c std::istream's formatted input extraction operation, \c >>. This
is convenient when \c T is an integral type, but any default-constructible
T type can be used.

Before calling the validator closure, set_string_validator() uses
`>>` to attempt to extract `T` from the input string.
The validator closure gets invoked whether the extraction attempt
succeeds or fails. The validator's first parameter is a \c std::string
that gives the original contents of the input field, and the second
parameter is either a pointer to the extracted value, or a \c nullptr
if the formatted extraction operator fails.

set_string_validator()'s second closure is the same as \c set_validator()'s.
The third, optional, closure is also the same as  \c set_validator()'s.

\note
set_validator() and set_string_validator() automatically trim off any
leading and trailing whitespace in the typed in text before attempting
to validate it.

\par Input filters

\code
INSERT_LIBX_NAMESPACE::w::input_field_config config{15};

config.maximum_size=14;
config.autoselect=true;
config.autodeselect=true;
auto field=factory->create_input_field("(###) ###-####", config);

field->on_default_filter([]
                         (char32_t c)
                         {
                             return c >= '0' && c <= '9';
                         },
                         {0, 4, 5, 9},
                         '#');
\endcode

on_validate() callbacks get executed only after finishing the editing of the
input field (while tabbing out, or clicking out of the input field).
An input field filter adds more control over the input field's editing process.
An input field filter checks each individual modification to the input field,
and has the ability to ignore it, or do a different modification.

In all cases, the input field edits a single text string (a Unicode text
string). All the input filter does is modify the text that gets added
or removed from the input field, during editing.

on_default_filter() installs a basic input filter, with the following
requirements:

- the input field's initial contents is the same size as the input field's
configured maximum size.

- optionally, the input field's width should be one extra column, to avoid
scrolling (extra room for the trailing cursor), and the input field gets
configured to \c autoselect and \c autodeselect its contents, upon gaining
or losing keyboard focus.

The first parameter to on_default_filter() is a callback that determines
whether a given Unicode character is valid input. Invalid characters
(typed in, or pasted), get ignored.

The second parameter is a \c std::vector<size_t> that specifies which
character in the input field are fixed and cannot be typed over.

The third parameter specifies a placeholder Unicode character where text
input actually goes. A input field with a default filter is, essentially,
a fixed size and the placeholder character designates where text input goes.
As text gets typed in, these placeholder characters get replaced by the
typed text.

Existing text always gets removed from the end of the entered text, and
gets replaced by the placeholder character. Although the cursor is movable
anywhere in the input field, any changes get quietly ignored except
the ones that add or remove text at the end.

\note
The entire contents of the input field inclues the placeholders, and
immutable characters. An input field validation callback gets typically used
together with the input filter, to convert the contents into a usable form,
typically stripping off the immutable characters and placeholders.

\code
field->on_filter([]
                 (ONLY IN_THREAD,
                  const INSERT_LIBX_NAMESPACE::w::input_field_filter_info &info)
                 {
                      size_t starting_pos=info.starting_pos;
                      size_t n_delete=info.n_delete;
                      std::u32string_view new_contents=info.new_contents;

                      INSERT_LIBX_NAMESPACE::w::input_filter_type type=info.type;
                      size_t size=info.size;

                      info.update();
                      info.update(0, 0, new_contents);

                      info.move(0);
		      info.select_all();
                 });
\endcode

on_field() installs a single callback that implements more granular control
over changes to the input field. The callback gets invoked for every
potential change to the contents of the input field, deletion and or
insertion.

on_field() receives an \ref input_field_filter_info "input_field_filter_info"
object. It's \c starting_pos specifies the tentative deletion/insertion
point. \c n_delete indicates the number of characters for a tentative deletion,
and \c new_contents gives the new contents tentatively inserted into the
input field.

A non-0 \c n_delete and a non-empty \c new_contents specifies a change that
simultaneously removes existing contents, and inserts new contents in its place.
\c type provides some context for the reason for the change, and
\c size gives the current number of characters in the input field, for
informational purpose.

Returning from the callback without taking any action results in no changes
to the input field's contents. The tentative change gets ignored.

update() with no parameters applies the tentative change. Alternatively,
invoking update() with explicit parameter applies a specific change which
may or may not have anything to do with the tentative change.

move() moves the cursor pointer to the indicated position in the input field.
select_all() selects the entire contents of the input field.

\par Spin buttons

\code
#include <INSERT_LIBX_NAMESPACE/w/input_field.H>
#include <INSERT_LIBX_NAMESPACE/w/input_field_config.H>

INSERT_LIBX_NAMESPACE::w::input_field_config conf{3};

conf.set_default_spin_control_factories();

INSERT_LIBX_NAMESPACE::w::input_field f=factory->create_input_field({""}, conf);

auto validated_int=field->set_string_validator
        ([]
         (ONLY IN_THREAD,
          const std::string &value,
          int *parsed_value,
          const INSERT_LIBX_NAMESPACE::w::text_param &field,
          const INSERT_LIBX_NAMESPACE::w::callback_trigger_t &trigger)
         -> std::optional<int>
         {
              // ...
         },
         []
         (int n)
         {
             return std::to_string(n);
    	 });

field->on_spin([validated_int]
               (ONLY IN_THREAD,
                const INSERT_LIBX_NAMESPACE::w::callback_trigger_t &trigger,
                const INSERT_LIBX_NAMESPACE::w::busy &mcguffin)
               {
                      // ...
               },
               [validated_int]
               (ONLY IN_THREAD,
                const INSERT_LIBX_NAMESPACE::w::callback_trigger_t &trigger,
                const INSERT_LIBX_NAMESPACE::w::busy &mcguffin)
               {
                      // ...
               });
\endcode

Spin buttons are enabled by
\ref input_field_config "INSERT_LIBX_NAMESPACE::w::input_field_config"'s
\c set_default_spin_control_factories() or
\c set_spin_control_factories() method.
\c on_spin() installs two callbacks that get executed in response to clicking
on the appropriate button. A typical implementation uses an input field
validator to validate the contents of the input field, with the callbacks
taking the validated input value, incrementing or decrementing it,
and then updating the input field with the new value.

\par Hint

\code
#include <INSERT_LIBX_NAMESPACE/w/input_field.H>
#include <INSERT_LIBX_NAMESPACE/w/input_field_config.H>

INSERT_LIBX_NAMESPACE::w::input_field_config conf{20};

conf.hint("Enter something here");

INSERT_LIBX_NAMESPACE::w::input_field f=factory->create_input_field({""}, conf);
\endcode

A hint is a brief message that's shown in the input field when it's empty
and does not have input focus. Its purpose is to provide a brief description
of what's expected to be entered into this field. The hint disappears when
the input field gains keyboard focus and its cursor starts blinking.

\note
There are no provisions for the hint's size exceeding the width of the
input field, it simply gets cut off. It's your responsibility to make sure
that it's shorter. Additionally, the hint's text appears starting on the
first line of a multi-line input field, but it's possible to manually center
the hint vertical in the field by prepending explicit \c \\n characters in
the hint.

\note
\ref input_field_config "input_field_config"'s \c hint is an
\ref text_param "INSERT_LIBX_NAMESPACE::w::text_param".

*/

typedef ref<input_fieldObj> input_field;

//! A nullable pointer reference to an input_field.

//! \see input_field

typedef ptr<input_fieldObj> input_fieldptr;

//! A reference to a constant input_field object.

//! \see input_field

typedef const_ref<input_fieldObj> const_input_field;

//! A nullable pointer reference to a constant input_field object.

//! \see input_field

typedef const_ptr<input_fieldObj> const_input_fieldptr;

//! Most recently validated value in the \ref input_field.

//! \code
//! INSERT_LIBX_NAMESPACE::w::validated_input_field<int> value=
//!         input_field->set_string_validator(
//!              []
//!		 (ONLY IN_THREAD,
//!               const std::string &value,
//!		  int *parsed_value,
//!		  const INSERT_LIBX_NAMESPACE::w::input_field &field,
//!		  const INSERT_LIBX_NAMESPACE::w::callback_trigger_t &trigger) -> std::optional<int>
//!              {
//!                  // ...
//!              },
//!              []
//!              (int n) { return std::to_string(n) });
//!
//! // ...
//! std::optional<int> last_value=value->validated_value.get()
//!
//! \endcode
//!
//! \ref validated_input_field "INSERT_LIBX_NAMESPACE::w::validated_input_field<T>"
//! gets returned by \ref input_field "input field"'s
//! set_validator() and set_string_validator(). This is an object that
//! holds the most recently entered value into the input field.
//! The callback installed by
//! set_validator() and set_string_validator() owns a reference to this object.
//! Its sole \c validated_value member is the most recently entered value.
//! Generally, no value gets returned before the first value gets entered into
//! the input field, or if the entered
//! contents could not be validated.

template<typename type>
using validated_input_field=ref<validated_input_fieldObj<type>>;

//! A nullable pointer reference to a \ref validated_input_field "validated input field value".

template<typename type>
using validated_input_fieldptr=ptr<validated_input_fieldObj<type>>;

//! A reference to a constant \ref validated_input_field "validated input field value".

template<typename type>
using const_validated_input_field=const_ref<validated_input_fieldObj<type>>;

//! A nullable pointer reference to a constant \ref validated_input_field "validated input field value".

template<typename type>
using const_validated_input_fieldptr=const_ptr<validated_input_fieldObj<type>>;

class LIBCXX_HIDDEN input_lock;

//! Identifies what kind of a change the input field change callback gets invoked for.

//! - deleted: characters were deleted.
//!
//! - inserted: characters were inserted.
//!
//! - set: set() was called.

enum class input_change_type {
	deleted,
	inserted,
	set};

//! Information passed to the input_field's on_changed() callback.

struct input_change_info_t {

	//! What kind of a change

	input_change_type type;

	//! How many unicode characters were inserted

	size_t inserted;

	//! How many unicode characters were deleted

	//! A non-0 value may be passed for \c inserted changes, indicating
	//! that the inserted text replaced a selection portion of the
	//! original text.
	//!
	//! A non-0 value gets passed for \c set changes, indicating the
	//! size of the replaced text.

	size_t deleted;

	//! How many unicode characters there are now.
	size_t size;
};

//! Information passed to the input_field's on_autocomplete() callback.

//! The usual autocomplete behavior is: the callback returns false if
//! no autocomplete information is available; or the callback sets
//! \c selection_start to \c string.size() then replaces \c string
//! with the autocompleted text, whose initial \c string.size() characters
//! are, presumably, unchanged. What happens is, basically, the returned
//! \c string's contents replace the contents of the input field and everything
//! starting from \c selection_start becomes the currently selected portion;
//! so the next keypress removes it, adds the typed-in character, and then
//! invokes the callback again.

struct input_autocomplete_info_t {

	//! String to autocomplete

	//! The autocomplete() callback may update this \c string and
	//! return \c true. The contents of the string completely replace
	//! the existing contents of the input field.
	std::u32string string;

	//! Start of the autocompleted portion.
	size_t selection_start;

	//! Available \ref busy "busy" mcguffin, for callback's needs.
	busy &mcguffin;
};

LIBCXXW_NAMESPACE_END

#endif
