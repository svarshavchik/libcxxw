/*
** Copyright 2017-2021 Double Precision, Inc.
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
#include "x/w/impl/connection_threadfwd.H"

#include "dirlisting/filedirlist_managerfwd.H"

#include <x/pcrefwd.H>
#include <x/weakptr.H>
#include <x/functionalrefptr.H>
#include <string>

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

class LIBCXX_HIDDEN file_dialogObj::implObj : virtual public obj {

	class popup_menu_status {

		std::optional<size_t> current_list_item;

	public:

		inline void new_focus(bool flag)
		{
			if (!flag)
				current_list_item.reset();
		}

		// New item is reported.
		inline void new_list_item(const std::optional<size_t> &item)
		{
			current_list_item=item;
		}

		// Must have keyboard focus to make current_list_item official.

		inline std::optional<size_t> item() const
		{
			return current_list_item;
		}
	};

	class update_popup_menu_status;

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

 private:
	//! Current status of the directory list right context popup
	popup_menu_status dir_status_thread_only;

	//! Current status of the file list right context popup
	popup_menu_status file_status_thread_only;
 public:
	THREAD_DATA_ONLY(dir_status);
	THREAD_DATA_ONLY(file_status);

	//! Message shown when access to the file is denied.
	const std::string access_denied_message;

	//! Title of the access denied dialog.
	const std::string access_denied_title;

	//! Appearance settings.
	const const_file_dialog_appearance appearance;

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
		const std::string &access_denied_title,
		const const_file_dialog_appearance &appearance);

	//! Destructor
	~implObj();

	//! My public object. Referenced by callbacks.
	weakptr<file_dialogptr> the_file_dialog;

	// A directory entry was clicked on.

	void clicked(ONLY IN_THREAD,
		     const filedirlist_entry_id &id,
		     const callback_trigger_t &trigger,
		     const busy &mcguffin);

	// A list item was selected, for the purposes of updating the
	// right context popup menu items' status.

	void file_or_dir_selection(ONLY IN_THREAD,
				   int section,
				   const std::optional<size_t> &n);

	// We take action whenever button 1 gets clicked.

	bool button_clicked(const button_event &be);

	// Show this in the directory field.

	std::tuple<text_param,
		   label_hotspots_t> create_dirlabel(const std::string &);

	// Enter pressed in the filename field.

	// Obtain the contents of the filename field, then call
	// process_filename().

	void enter_key(ONLY IN_THREAD, const busy &mcguffin);

	// Process a filename

	// It was either entered in the filename field, or dragged and dropped
	// into the dialog.

	void process_filename(ONLY IN_THREAD,
			      const std::string &filename,
			      const busy &mcguffin);

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
			    const std::string &path,
			    label_hotspots_t &hotspots,
			    int &hotspot_counter);

	//! Callback that gets invoked when a directory component gets clicked.

	//! If the activation event is a focus change, returns highlighted
	//! or unhighlighted replacement text, for the hotspot. Invokes
	//! chdir() for an actual click.

	text_param hotspot_activated(ONLY IN_THREAD,
				     const text_event_t &event,
				     const std::string &name,
				     const std::string &path);
 public:
	//! Switch to another directory.

	//! Notifies the file directory manager. Updates the
	//! directory_field.

	void chdir(ONLY IN_THREAD, const std::string &path);
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
