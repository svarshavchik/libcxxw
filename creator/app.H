#ifndef creator_app_H
#define creator_app_H

#include <x/obj.H>
#include <x/ref.H>
#include <x/refptr_traits.H>
#include <x/singletonptr.H>
#include <x/functionalrefptr.H>
#include <x/eventqueue.H>
#include <x/mpobj.H>
#include <x/functional.H>
#include <x/xml/doc.H>
#include "x/w/main_window.H"
#include "x/w/uigenerators.H"
#include "x/w/file_dialogfwd.H"
#include "x/w/focusable_container.H"
#include "x/w/input_field.H"
#include "x/w/image_button.H"
#include "x/w/button.H"
#include "x/w/label.H"
#include "x/w/standard_comboboxlayoutmanagerfwd.H"
#include "x/w/listitemhandle.H"
#include "x/w/impl/connection_threadfwd.H"

#include "creator/appelements.inc.H"

class appObj;

typedef x::ref<appObj> app;

//! Singleton application class.

//! Inherits from UI elements, the UI elements in the app window, mostly
//! the main menu.

class appObj : public app_elements_t, virtual public x::obj {

	//! My configuration file

	const std::string configfile;

public:
	//! Constructor
	appObj(int argc, char **argv);

private:

	struct LIBCXX_HIDDEN init_args;

	static init_args create_init_args(int argc, char **argv) LIBCXX_HIDDEN;

	//! Parameters for an internal constructor
	struct init_args {

		std::string configfile;

		std::string filename;
		x::xml::doc theme;
		app_elements_tptr elements;

		x::functionref<x::w::input_field_filter_callback_t
			       > label_filter;

		init_args(int argc, char **argv);
	};

	//! Internal constructor
	appObj(init_args &&);

	//! UI event queue type
	typedef x::eventqueue< x::functionref<void (const app &)>> eventqueue_t;

	//! UI events
	const eventqueue_t eventqueue=eventqueue_t::create();

public:
	//! Destructor
	~appObj();

private:

	//! Update everything, after a new file is loaded or created.
	void loaded_file();

	//! Update the application title

	//! (include the name of the current theme)

	void update_title();

	//! Disable/enable menu items based on what things are now.

	void enable_disable_menus();
public:
	//! Main application loop
	void mainloop();

	//! Currently loaded theme file
	x::mpobj<x::xml::doc> theme;

	//! Signature of a callback used by update_theme.

	typedef x::function<bool (const x::xml::doc &)> update_callback_t;

	//! Update the theme

	//! Invoked by various update buttons.
	//!
	//! Invoke the update's button callback, passing to it two callbacks:
	//!
	//! A) Validate input fields and verify that they're valid.
	//!
	//! B) If the first callbacks returns true, the second callback
	//! gets invoked, and gets an opaque closure, that
	//! receives an x::xml::doc as a parameter. This callback is
	//! is responsible for invoking the closure to
	//! "propose" its update.
	//!
	//! The closure returns true to indicate that the updated was valid,
	//! and the callback is free to then apply the update to its UI.
	//!
	//! The closure must receive an x::xml::doc without any locks, because
	//! it will be parsed.
	void update_theme(ONLY IN_THREAD,
			  const x::w::busy &mcguffin,
			  bool (appObj::*validator)(ONLY IN_THREAD),
			  void (appObj::*callback)(const update_callback_t &));

	//! Filename
	x::mpobj<std::string> themename;

	//! Whether the theme file has been modified
	x::mpobj<bool> edited=false;

	//! Dimensions page: validated new name field
	const x::w::validated_input_field<std::string
					  > dimension_new_name_validated;

	//! Dimensions page: validated value field.
	const x::w::validated_input_field<std::string
					  > dimension_value_validated;

	//! Dimensions page: validated scale field.
	const x::w::validated_input_field<std::string
					  > dimension_scale_value_validated;

	//////////////////////////////////////////////////////////////////////
	//
	// Events from the UI. They invoke appevent to send the event to
	// main thread for execution.

	friend void appevent(const x::functionref<void (const app &)> &);

	//! File save event.
	void file_save_event(ONLY IN_THREAD);

	//! File save as event
	void file_save_as_event(ONLY IN_THREAD);

private:
	//! We know what we want to do after saving the file.
	void do_file_save_event(ONLY IN_THREAD,
				void (appObj::*what_to_do_next)());

	//! We know what we want to do after save-as dialog closes.
	void do_file_save_as_event(ONLY IN_THREAD,
				   void (appObj::*what_to_do_next)());
public:
	//! File new event
	void file_new_event(ONLY IN_THREAD);
	//! File save dialog closed
	void do_check_and_file_save(std::string,
				    void (appObj::*what_to_do_next)());

	//! File save dialog closed, checked for overwriting, ok to save now
	void do_file_save(const std::string &filename,
			  void (appObj::*what_to_do_next)());

	//! What do_file_save calls when only saving a file, and nothing more.
	void only_save();

	//! File open event
	void file_open_event(ONLY IN_THREAD);

	//! Show the file open dialog
	void open_file();

	//! File open dialog was closed
	void open_dialog_closed(ONLY IN_THREAD,
				const std::string &filename);

	//! Load the theme file into x::xml::doc

	//! And do a minimal sanity check.
	static x::xml::doc load_file(const std::string &filename);

	//! File quit event.
	void file_quit_event(ONLY IN_THREAD);

private:
	//! Stashed pointer to pick up after "Save As" closes.

	void (appObj::*what_to_do_after_save_as_thread_only)();

public:
	THREAD_DATA_ONLY(what_to_do_after_save_as);

	//! Save dialog has closed
	void save_dialog_closed(ONLY IN_THREAD, const std::string &filename);

private:

	//! Check if the theme file has been edited before doing something.

	//! If it's edited, show a dialog with the given buttons. If not
	//! then invoke whattodo.
	//!
	//! The buttons save the file first, then do whattodo, go ahead and
	//! do whattodo without saving, and cancel.

	void ifnotedited(void (appObj::*whattodo)(),
			 const char *ok_label,
			 const char *ok2_label,
			 const char *cancel_label);

	//! The end of the road for a file_quit_event
	void stoprunning();

	//! Create a new file
	void new_file();

	//////////////////////////////////////////////////////////////////
	//
	// appdimension.C

	//! Install elements on the dimension page

	//! Called after generating the elements on the dimension page.
	//! Extracts the individual fields from the uielements, and places
	//! them into app_elements.
	//!
	//! Installs callbacks.

	static void dimension_elements_initialize(app_elements_tptr &,
						  x::w::uielements &,
						  init_args &);

	//! Initialize the dimension page after loading/creating a new theme.

	void dimension_initialize();

private:
	/////////////////////////////////////////////////////////////////////
	// metadata about the dimensions page.

	//! New values entered for the new or updated dimension.

	struct dimension_save_params {

		std::string dimension_new_name; //!< When creating a new one.

		std::optional<size_t> scale_from; //!< If not empty

		std::string value; //!< Dimension value
	};

	//! Original dimension parameters

	struct dimension_orig_params {
		size_t index; //!< Which original dimension
		std::optional<size_t> scale_from; //!< If not empty
		std::string value; //!< Original value

		//! Whether the saved parameters are the same as the orig ones

		bool operator==(const dimension_save_params &orig)
		{
			return scale_from == orig.scale_from &&
				value == orig.value;
		}
	};

	//! Dimension metadata
	struct dimension_info_s {

		//! Cached: selected index of an existing dimension.
		std::optional<dimension_orig_params> current_selection;

		//! Cached: selected index of an existing "from" dimension.
		std::optional<size_t> from_index;

		//! The existing dimensions.
		std::vector<std::string> ids;

		//! Updated dimension save params.

		//! This is updated every time something changes on the
		//! dimension. The "Save" button simply takes this to the bank.
		std::optional<dimension_save_params> save_params;
	};

	//! Thread-safe container for dimension metadata.

	typedef x::mpobj<dimension_info_s,
			 std::recursive_mutex> dimension_info_t;

	//! Whether something was edited but not saved
	bool dimension_unsaved_values(dimension_info_t::lock &lock);

	//! Enable or disable the reset button.
	void dimension_enable_disable_buttons(dimension_info_t::lock &lock);

	//! Reset button pressed
	void dimension_reset();

	//! Reset all fields to their original values, for the current dim
	void dimension_reset_values(dimension_info_t::lock &lock);
public:

	dimension_info_t dimension_info;

	//! Something was selected or unselected in the dimension combo-box

	void dimension_selected(ONLY IN_THREAD,
				const
				x::w::standard_combobox_selection_changed_info_t
				&);

	//! Selected an existing from dimension (maybe)
	void dimension_from_selected
	(ONLY IN_THREAD,
	 const
	 x::w::standard_combobox_selection_changed_info_t &);

	//! Selected the dimension value option
	void dimension_value_option_selected(ONLY IN_THREAD);

	//! Selected the dimension scale option
	void dimension_scale_option_selected(ONLY IN_THREAD);

	//! A dimension value was entered.
	void dimension_value_entered(ONLY IN_THREAD);

	//! A scaled dimension value was entered.
	void dimension_scale_value_entered(ONLY IN_THREAD);

	//! One of the dimension fields was changed.
	void dimension_field_updated(ONLY IN_THREAD);

private:

	//! Enable or disable the reset button.
	void dimension_update_enable_disable(dimension_info_t::lock &lock);

	//! Invoked by dimension_field_updated() to process fields' contents.

	bool dimension_update_save_params(ONLY IN_THREAD,
					  dimension_info_t::lock &lock);

public:
	//! Validate parameters for save/update
	bool dimension_validate(ONLY IN_THREAD);

	//! Save/update dimension
	void dimension_update(const update_callback_t &callback);

	//! Verify that it's ok to delete the current shown dimension
	bool dimension_ok_to_delete(ONLY IN_THREAD);

	//! Delete dimension
	void dimension_delete(const update_callback_t &callback);

private:

	//! The app is running.

	//! Cleared by file_quit_event(), to stop the main execution thread.
	bool running=true;

};

// The singleton instance of the app object.
typedef x::singletonptr<appObj> appsingleton;

//! app events

//! Constructs the app singleton
//! then (attempt to) instantiate the app singleton,
//! then put an event closure into the event queue, to be executed
//! by the main application thread that loops and plucks events
//! off the event queue.

void appevent(const x::functionref<void (const app &)> &);

// Helper for invoking something in the app object.

template<typename T, typename ...Args>
static inline void appinvoke(T t, Args && ...args)
{
	appsingleton singleton;

	if (!singleton)
		return;

	auto &me=*singleton;

	std::invoke(t, &me, std::forward<Args>(args)...);
}

#endif
