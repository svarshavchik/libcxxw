/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_uicompiler_h
#define x_w_uicompiler_h

#include "x/w/gridlayoutmanagerfwd.H"
#include "x/w/gridfactoryfwd.H"
#include "x/w/booklayoutmanagerfwd.H"
#include "x/w/bookpagefactoryfwd.H"
#include "x/w/factoryfwd.H"
#include "x/w/uielementsfwd.H"
#include "x/w/uigeneratorsfwd.H"
#include "x/w/shortcutfwd.H"
#include "theme_parser_lock.H"
#include "uicompilerfwd.H"

#include <x/vector.H>
#include <tuple>
#include <unordered_map>

LIBCXXW_NAMESPACE_START

/*! Compiler for XML-specified display elements.

The bulk of the compiler gets robo-generated from uicompiler.xml. This
declares the uicompiler-generated methods, and provides some supporting
functions.

 */

struct uicompiler {

public:

	//! Constructor

	//! Take the XML node represented by lock, and populate the generators
	//! with the compiled code.
	//!
	//! Once constructed, this object is just an empty hulk that can be
	//! discarded.
	//!
	//! "allowthemerefs" gets set to false when parsing the default
	//! theme, and true when parsing a user-specified layout/theme.

	uicompiler(const theme_parser_lock &lock,
		   uigeneratorsObj &generators,
		   bool allowthemerefs);

private:

	//! What's being generated.
	uigeneratorsObj &generators;

	bool allowthemerefs;

	//! Top level generators in the XML code that aren't compiled yet.

	//! The constructor puts all children of the node it gets passed in,
	//! into this lookup map, keyed by their @id attribute.
	//!
	//! The constructor then repeatedly removes an element from this map
	//! and compiles, placing the result into the generators, until the
	//! map is empty.
	//!
	//! A generator may recursively invoke another generator. If it's
	//! already been generated it gets plucked from the generators,
	//! otherwise it's removed from uncompiled_elements and compiled,
	//! recursively.
	std::unordered_map<std::string, theme_parser_lock> uncompiled_elements;

	//! Parser for grid layout manager generator.
	vector<gridlayoutmanager_generator
	       > gridlayout_parseconfig(const theme_parser_lock &lock);

	//! Parser for grid factory generator.
	vector<gridfactory_generator
	       > gridfactory_parseconfig(const theme_parser_lock &lock);

	//! Parser for book layout manager generator.
	vector<booklayoutmanager_generator
	       > booklayout_parseconfig(const theme_parser_lock &lock);

	//! Parser for book page factory generator.
	vector<bookpagefactory_generator
	       > bookpagefactory_parseconfig(const theme_parser_lock &lock);

	//! Implement <element>

	//! Executed by the compiled <element> code.

	static void generate(const factory &,
			     uielements &,
			     const std::string &name);

	//! Generate a single grid layout manager instruction.
	gridlayoutmanager_generator
	gridlayout_parser(const theme_parser_lock &lock);

	//! Generate a single grid factory instruction.
	gridfactory_generator gridfactory_parser(const theme_parser_lock &lock);

	//! Generate a single book layout manager instruction.
	booklayoutmanager_generator
	booklayout_parser(const theme_parser_lock &lock);

	//! Generate a single book page factory instruction.
	bookpagefactory_generator
	bookpagefactory_parser(const theme_parser_lock &lock);

	//! Return compiled grid factory generators.

	//! Extract single_value() from element, and look it up in
	//! gridfactory_generators, returning the generators. If it doesn't
	//! exist, search for them in uncompiled_elements, compile them, put
	//! them into gridfactory_generators, and return them.

	vector<gridfactory_generator
	       > lookup_gridfactory_generators(const theme_parser_lock &lock,
					       const char *element,
					       const char *parent);

	//! Return compiled book page factory generators.

	//! Extract single_value() from element, and look it up in
	//! gridfactory_generators, returning the generators. If it doesn't
	//! exist, search for them in uncompiled_elements, compile them, put
	//! them into bookpagefactory_generators, and return them.

	vector<bookpagefactory_generator>
	lookup_bookpagefactory_generators(const theme_parser_lock &lock,
					  const char *element,
					  const char *parent);

	//! Return compiled grid layout manager generators.

	//! Extract the single_value() name of the generators. Return the
	//! name, and the compiled generators.
	std::tuple<std::string,
		   vector<gridlayoutmanager_generator>>
	lookup_gridlayoutmanager_generators(const theme_parser_lock &lock,
					    const char *element,
					    const char *parent);

	//! Return compiled grid layout manager generators.

	//! If the generators are already compiled, return them. If not,
	//! search for them in uncompiled_elements, compile them, put them
	//! into gridfactory_generators, and return them.

	vector<gridlayoutmanager_generator>
	lookup_gridlayoutmanager_generators(const theme_parser_lock &lock,
					    const std::string &);

	//! Return compiled book layout manager generators.

	//! Extract the single_value() name of the generators. Return the
	//! name, and the compiled generators.
	std::tuple<std::string,
		   vector<booklayoutmanager_generator>>
	lookup_booklayoutmanager_generators(const theme_parser_lock &lock,
					    const char *element,
					    const char *parent);

	//! Return compiled book layout manager generators.

	//! If the generators are already compiled, return them. If not,
	//! search for them in uncompiled_elements, compile them, put them
	//! into gridfactory_generators, and return them.

	vector<booklayoutmanager_generator>
	lookup_booklayoutmanager_generators(const theme_parser_lock &lock,
					    const std::string &);

	//! Execute <append_row>.

	//! Executed by the compiled <append_row>.
	static void gridlayout_append_row(const gridlayoutmanager &,
					  uielements &,
					  const vector<gridfactory_generator> &
					  );

	//! Execute <insert_row>.

	//! Executed by the compiled <insert_row>.
	static void gridlayout_insert_row(const gridlayoutmanager &,
					  size_t,
					  uielements &,
					  const vector<gridfactory_generator> &
					  );

	//! Execute <replace_row>.

	//! Executed by the compiled <insert_row>.
	static void gridlayout_replace_row(const gridlayoutmanager &,
					   size_t,
					   uielements &,
					   const vector<gridfactory_generator> &
					   );

	//! Execute <append_columns>.

	//! Executed by the compiled <append_columns>.
	static void gridlayout_append_columns(const gridlayoutmanager &,
					      size_t,
					      uielements &,
					      const
					      vector<gridfactory_generator> &
					      );

	//! Execute <insert_columns>.

	//! Executed by the compiled <insert_columns>.
	static void gridlayout_insert_columns(const gridlayoutmanager &,
					      size_t,
					      size_t,
					      uielements &,
					      const
					      vector<gridfactory_generator> &);

	//! Execute <replace_cell>

	//! Executed by the compiled <replace_cell>.
	static void gridlayout_replace_cell(const gridlayoutmanager &,
					    size_t,
					    size_t,
					    uielements &,
					    const
					    vector<gridfactory_generator> &);

	//! Execute a grid factory generator.
	static void generate_gridfactory(const gridfactory &,
					 uielements &,
					 const vector<gridfactory_generator>
					 &generators);

	//! Execute <append_pages>.

	//! Executed by the compiled <append_pages>.
	static void booklayout_append_pages(const booklayoutmanager &,
					    uielements &,
					    const vector<
					    bookpagefactory_generator> &);

	//! Execute <insert_pages>.

	//! Executed by the compiled <insert_pages>.
	static void booklayout_insert_pages(const booklayoutmanager &,
					    size_t,
					    uielements &,
					    const vector<
					    bookpagefactory_generator> &);

	//! Execute <book>

	//! Executed by the compiled <book>.

	static void container_booklayoutmanager
	(const factory &,
	 uielements &,
	 const std::tuple<std::string, vector<booklayoutmanager_generator>> &);

	//! Execute <grid>

	//! Executed by the compiled <grid>.
	static void container_gridlayoutmanager
	(const factory &,
	 uielements &,
	 const std::tuple<std::string, vector<gridlayoutmanager_generator>> &);

	//! Execute <add>

	//! Executed by the compiled <add>.
	static void container_addbookpage(const bookpagefactory &f,
					  uielements &factories,
					  const std::string &label,
					  const std::string &sc,
					  const std::tuple<std::string,
					  vector<gridlayoutmanager_generator>>
					  &generators);
};

LIBCXXW_NAMESPACE_END

#endif
