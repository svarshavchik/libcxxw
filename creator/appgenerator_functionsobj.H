/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef appgenerator_functionsobj_h
#define appgenerator_functionsobj_h

#include <x/refptr_traits.H>
#include <x/exception.H>
#include <x/mpobj.H>
#include <x/mpthreadlockobj.H>
#include <x/obj.H>
#include <x/vector.H>

#include "x/w/containerfwd.H"
#include "x/w/focusable_containerfwd.H"
#include "x/w/listitemhandlefwd.H"
#include "x/w/connection_threadfwd.H"
#include "x/w/uigeneratorsobj.H"
#include "x/w/main_window.H"
#include "creator/uicompilerfwd.H"
#include "creator/appgenerator_functionsfwd.H"
#include "creator/appgenerator_functionfwd.H"
#include "creator/appgeneratorelements.inc.H"
#include "creator/uicompiler_generatorsfwd.H"
#include "creator/uicompiler_generatorsobj.H"

#include <vector>
#include <string>
#include <optional>

//! UI widgets that represent a list of generator functions.
typedef const app_generator_elements_t const_app_generator_elements_t;

//! Currently-selected generator (a layout or a factory)

struct current_selection_info_s {

	size_t index; //!< Which original layout/factory is selected.

	//! Its generator functions.
	x::vector<appgenerator_function> main_functions;

	//! Constructor
	current_selection_info_s(size_t index,
				 const x::vector<appgenerator_function> &);

	//! Destructor
	~current_selection_info_s();

	bool modified=false; //!< Whether the generator has changed
};

//! All existing layouts and factories in the theme file.
struct all_generators_info_s {

	//! Their IDs.
	std::vector<std::string> ids;

	//! Which one is currently selected for display/editing.
	std::optional<current_selection_info_s> current_selection;
};

//! Thread-safe container for generators metadata.

typedef x::mptobj<all_generators_info_s,
		  std::recursive_mutex> all_generators_info_t;

//! All existing layouts and factories in the theme file.

//! This is used to establish a consistent lock on the generators.
//! In addition to the list the currently selected layout's or factory's
//! generator functions, some functions use a nested list of generator
//! functions, and additional instances of appgenerator_functions.
//!
//! A generator lock is obtained by first locking the appgenerator_functions'
//! generators and the all_generators. All appgenerator_functions carry
//! a reference to the same all_generators, so locking any
//! appgenerator_functions also locks the global all_generators.

struct all_generatorsObj : virtual public x::obj {
	all_generators_info_t all_generators;
};

//! Information about this layout or a factory's generators.

//! This is protected by a \ref generator_info_lock "lock".

struct generator_info_s {

	//! The generators.

	//! The list of the layout or factory's generators is also
	//! placed in the \ref current_selection_info_s "current selection's"
	//! main_functions.

	x::vector<appgenerator_function> functions;

	//! The compiler for this layout or factory.

	//! The appgenerator_functions object gets constructed before
	//! the compiler is determined, and type and functions gets
	//! set by generator_values_initialize().

	const_uicompiler_generatorsptr compiler;

	//! All generators

	const x::ref<all_generatorsObj> all_generators;

	//! Constructor
	generator_info_s(const x::ref<all_generatorsObj> &all_generators);

	//! Destructor
	~generator_info_s();
};

//! Thread-safe container for generator information.

typedef x::mptobj<generator_info_s,
		  std::recursive_mutex> generator_info_t;

//! A locked generator information.

//! This acquires a lock on the \ref generator_info_s "generator information"
//! and \ref all_generator_info_s "all existing layout and factory generators"
//! in the theme file.

struct generator_info_lock : generator_info_t::lock {

	//! Locked layout and factory generators
	all_generators_info_t::lock all;

	//! Constructor

	//! The constructor uses a consistent locking order.
	generator_info_lock(generator_info_t &info)
		: generator_info_t::lock{info},
		  all{ operator->()->all_generators->all_generators}
	{
	}

	//! Return value from threadlock().

	typedef std::tuple<generator_info_t::threadlock,
			   all_generators_info_t::threadlock> threadlock_t;

	//! Mimic what threadlock() returns for a single lock.

	//! A thread lock is returned for both underlying locks here.

	threadlock_t threadlock(const x::ref<x::obj> &mcguffin)
	{
		return {
			generator_info_t::lock::threadlock(mcguffin),
			all.threadlock(mcguffin)
		};
	}

	//! Construct from a previously-created thread lock.

	generator_info_lock(const threadlock_t &tl)
		: generator_info_t::lock{std::get<0>(tl)},
		  all{ std::get<1>(tl)}
	{
	}

	using generator_info_t::lock::operator->;
};

//! Implementation object.

//! \see appgenerator_functions

class appgenerator_functionsObj:
	public const_app_generator_elements_t,
	virtual public x::obj {

public:
	struct generator_value_info;
	struct generator_value_info_lm;

	//! Parent main window

	const x::w::main_window parent_window;

	//! Main generator for our UI.
	const x::w::const_uigenerators cxxwui_generators;

	//! Shared UI information

	//! Parsed from the XML description, contains information about
	//! all generators.

	const const_uicompiler uicompiler_info;

	//! The generators displayed here.

	generator_info_t generator_info;

	//! Constructor
	appgenerator_functionsObj
	(app_generator_elements_tptr &generator_elements,
	 const x::w::main_window &parent_window,
	 const x::w::const_uigenerators &cxxwui_generators,
	 const const_uicompiler &uicompiler_info,
	 const x::ref<all_generatorsObj> &all_generators);

	//! Destructor
	~appgenerator_functionsObj();

	//! Install callbacks, before constructing this object.

	static void
	generators_values_elements_initialize
	(app_generator_elements_tptr &elements,
	 x::w::uielements &ui,
	 const uicompiler &uiptr);

	//! Parse and initialize the values.

	//! This is called after constructing this object, to load its contents
	//! from XML.
	void generator_values_initialize(ONLY IN_THREAD,
					 generator_info_lock &lock,
					 const const_uicompiler_generators
					 &compiler,
					 const x::xml::readlock &xml);


	//! Currently selected value has changed.

	//! Updates the enabled/disabled status for the context popup menu's
	//! items based on the currently selected value.

	void generator_contents_values_changed(ONLY IN_THREAD,
					       generator_value_info_lm &lock);

	//! "Move Up" generator value
	bool generator_value_move_up_clicked(ONLY IN_THREAD);

	//! "Move Down" generator value
	bool generator_value_move_down_clicked(ONLY IN_THREAD);

	//! "Update" generator value
	void generator_value_update_clicked(ONLY IN_THREAD);

	//! Edit a generator function.

	//! Opens a dialog. Closing the dialog invokes the save function
	//! which either adds a new generator function, or does nothing
	//! (the dialog updated the existing generator function directly).

	void generator_value_edit
	(ONLY IN_THREAD,
	 const appgenerator_function &,
	 void (appgenerator_functionsObj::*save)
	 (ONLY IN_THREAD,
	  const appgenerator_function &function));

	//! "Save" clicked on the function's popup.

	void generator_value_created(ONLY IN_THREAD,
				     const appgenerator_function &function);

	//! "Update" clicked on the function's popup.

	void generator_value_updated(ONLY IN_THREAD,
				     const appgenerator_function &function);

	//! "Delete" generator value
	bool generator_value_delete_clicked(ONLY IN_THREAD);

private:

	//! Generator values have been modified

	//! They have been modified by a modification popup.
	//!
	//! Call generator_contents_values_changed(), to mark the generator
	//! as modified.

	void generator_values_modified(ONLY IN_THREAD);
};

#endif
