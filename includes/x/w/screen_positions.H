/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_screen_positions_h
#define x_w_screen_positions_h

#include <x/w/screen_positionsfwd.H>
#include <x/w/main_windowfwd.H>
#include <x/w/rectangle.H>
#include <x/xml/doc.H>
#include <string>
#include <optional>

LIBCXXW_NAMESPACE_START

/*!
Memorized positions of \ref main_window "main windows" on the screen.

\code
#include <INSERT_LIBX_NAMESPACE/config.H>

auto configfile=INSERT_LIBX_NAMESPACE::configdir("app@example.com") + "/windows";

INSERT_LIBX_NAMESPACE::w::screen_positions my_screen_positions{configfile};

auto main_window=INSERT_LIBX_NAMESPACE::w::main_window::create(my_screen_positions, "main",
     []
     (const INSERT_LIBX_NAMESPACE::w::main_window &new_main_window)
     {
          // Initialize the new main window.
     });

// When the application exits.
INSERT_LIBX_NAMESPACE::w::screen_positions my_screen_positions;

main_window->save("main", my_screen_positions);
my_screen_positions.save(configfile);
\endcode

\par Restoring the main window's previous size and position.

\c screen_positions memorizes the screen position and size of a
\ref main_window "main window". This provides the means to save the
main window's position and size, and the next time the application runs
its window opens in the same position.w

Use <a href="/configdir.html">LibCXX's configdir()</a> to prepare the
application's configuration directory. The constructor's first parameter
is the application's configuration file's name. The file gets loaded if
it exists. An empty \c screen_positions gets constructed if the file
does not exist.

A \c screen_positions object and an internal window identifier are optional
parameters when creating a new
\ref main_window "INSERT_LIBX_NAMESPACE::w::main_window".
More than one main window's screen position can be memorized, using a unique
identifier for each window. The identifiers are opaque labels and the library
treats them as arbitrary identifiers, except that the should not include
punctuation, as they're used verbatim in an XPath expression (the configuration
file is XML-formatted).

If the \c screen_positions object is empty (because the configuration file
does not exist, probably), this is quietly ignored and the new main window
opens with its default size and position.

In all cases, the window's ultimate position and size are controlled by
the display's window manager, this serves merely to politely ask the window
manager to open the window with this size and position.

\par Memorizing the main window's size and position.

The application must take explicit steps to memorize its windows' position
and size.
A \ref main_window "main window"'s save() method records its current
position and size, using the specified its identifier. Each main window
must use a unique, opaque, identifier. Any existing recorded position
with the same identifier gets replaced by the new position, so the
the application can use the same \c screen_position object that
it initially constructed to restore their previous size and position, and it
still reflects it, or the application can default-construct a new, empty,
\c screen_position object.

\c screen_position 's save() saves all recorded positions in the specified
configuration file.

*/

class screen_positions {

public:

	//! Internal XML data.
	const xml::doc data;

	//! Default constructor. An empty list of memorized screen positions.

	screen_positions();

	//! Destructor
	~screen_positions();

	//! The copy constructor is deleted.
	screen_positions(const screen_positions &)=delete;

	//! Move constructor
	screen_positions(screen_positions &&);

	//! Save all memorized positions in a file.
	void save(const std::string &filename) const;

	//! Load memorized screen positions from a file.

	//! Construct a new instance and initialize it with the contents
	//! of the specified file.
	screen_positions(const std::string &filename);

	//! Internal structure

	//! \internal
	struct LIBCXX_HIDDEN window_info {
		xml::doc::base::readlock config;
		std::optional<size_t> screen_number;
		rectangle coordinates;
	};

	//! Find a memorized position.

	//! \internal
	std::optional<window_info> find(const std::string &identifier)
		const LIBCXX_HIDDEN;

	//! Create a new node when saving something.

	xml::doc::base::writelock
	create_writelock_for_saving(const std::string &type,
				    const std::string &name);
};

//! Internal Xpath assembler.

//! \internal

std::string saved_element_to_xpath(const std::string &type,
				   const std::string &name) LIBCXX_HIDDEN;

LIBCXXW_NAMESPACE_END

#endif