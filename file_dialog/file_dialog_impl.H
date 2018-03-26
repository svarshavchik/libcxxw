/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef file_dialog_impl_h
#define file_dialog_impl_h

#include "x/w/file_dialog.H"
#include "x/w/focusable_labelfwd.H"
#include "x/w/focusable_containerfwd.H"
#include "x/w/input_fieldfwd.H"
#include "x/w/buttonfwd.H"
#include "x/w/busyfwd.H"
#include "x/w/file_dialog_config.H"
#include "x/w/callback_triggerfwd.H"
#include "x/w/button_eventfwd.H"
#include "x/w/text_paramfwd.H"

#include "dirlisting/filedirlist_managerfwd.H"

#include <x/pcrefwd.H>
#include <x/weakptr.H>
#include <x/functionalrefptr.H>
#include <string>

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

class LIBCXX_HIDDEN file_dialogObj::implObj : virtual public obj {


 public:

	//! The label with the current directory's path components.
	const focusable_label directory_field;

	//! Filename input field

	const input_field filename_field;

	//! File pattern field filter

	const focusable_container filter_field;

	//! Manager for the directory contents.
	const filedirlist_manager directory_contents_list;

	//! "Ok" button
	const button ok_button;

	//! "Cancel" button
	const button cancel_button;

	//! "Ok" callback.
	const functionref<void (THREAD_CALLBACK, const file_dialog &,
				const std::string &, const busy &)
			    > ok_action;

	//! The semantics of this file dialog.
	const file_dialog_type type;

	//! Message shown when access to the file is denied.
	const std::string access_denied_message;

	//! Title of the access denied dialog.
	const std::string access_denied_title;

	//! Constructor
	implObj(const focusable_label &directory_field,
		const input_field &filename_field,
		const focusable_container &filter_field,
		const filedirlist_manager &directory_contents_list,
		const button &ok_button,
		const button &cancel_button,
		const functionref<void (THREAD_CALLBACK,
					const file_dialog &,
					const std::string &, const busy &)
		> &ok_action,
		file_dialog_type type,
		const std::string &access_denied_message,
		const std::string &access_denied_title);

	//! Destructor
	~implObj();

	//! My public object. Referenced by callbacks.
	weakptr<file_dialogptr> the_file_dialog;

	// A directory entry was clicked on.

	void clicked(ONLY IN_THREAD,
		     const filedirlist_entry_id &id,
		     const callback_trigger_t &trigger,
		     const busy &mcguffin);

	// We take action whenever button 1 gets clicked.

	bool button_clicked(const button_event &be);

	// Show this in the directory field.

	text_param create_dirlabel(const std::string &);

	// Enter pressed in the filename field.

	void enter_key(ONLY IN_THREAD, const busy &mcguffin);

	//! Existing contents of the directory was selected, or typed in.

	//! Perform the appropriate action, depending on the file dialog type.
	void selected(ONLY IN_THREAD,
		      const std::string &filename,
		      const busy &mcguffin);

 private:

	//! Used by create-dirlabel().

	//! Initialized the passed-in text_param with a clickable hotspot
	//! labeled by the given directory name. When clicked,
	//! hotspot_activated() gets called.

	void create_hotspot(text_param &t,
			    const std::string &name,
			    const std::string &path);

	//! Callback that gets invoked when a directory component gets clicked.

	//! If the activation event is a focus change, returns highlighted
	//! or unhighlighted replacement text, for the hotspot. Invokes
	//! chdir() for an actual click.

	text_param hotspot_activated(const text_event_t &event,
				     const std::string &name,
				     const std::string &path);
 public:
	//! Switch to another directory.

	//! Notifies the file directory manager. Updates the
	//! directory_field.

	void chdir(const std::string &path);
 private:
	//! Switch to another filter.
	void chfilter(const pcre &filter);

	//! Invoked to pop up an error dialog.

	//! If the file is not readable, or writable, depending on the
	//! file dialog's semantics.

	void access_denied(const file_dialog &the_file_dialog,
			   const std::string &error_message,
			   const std::string &filename);

	static void error_dialog(const file_dialog &the_file_dialog,
				 const std::string &message,
				 const std::string &title);
};

LIBCXXW_NAMESPACE_END

#endif
