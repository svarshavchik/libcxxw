/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_main_windowobj_h
#define x_w_main_windowobj_h

#include <x/w/main_windowfwd.H>
#include <x/w/new_layoutmanagerfwd.H>
#include <x/w/generic_windowobj.H>
#include <x/w/menubarlayoutmanagerfwd.H>
#include <x/w/gridlayoutmanagerfwd.H>
#include <x/w/gridfactoryfwd.H>
#include <x/w/dialogfwd.H>
#include <x/w/shortcutfwd.H>
#include <x/w/input_dialogfwd.H>
#include <x/w/new_layoutmanagerfwd.H>
#include <x/w/text_paramfwd.H>
#include <x/w/alert_messagefwd.H>
#include <x/w/stop_messagefwd.H>
#include <x/w/busyfwd.H>
#include <x/w/containerfwd.H>
#include <x/w/types.H>
#include <x/w/input_field_configfwd.H>
#include <x/w/input_fieldfwd.H>
#include <x/w/file_dialogfwd.H>
#include <x/w/file_dialog_configfwd.H>
#include <x/w/print_dialogfwd.H>
#include <x/w/print_dialog_configfwd.H>
#include <x/w/connection_threadfwd.H>
#include <x/obj.H>
#include <x/logger.H>
#include <x/functional.H>
#include <x/functionalrefptrfwd.H>
#include <x/refptr_traits.H>
#include <x/exception.H>

#include <utility>
#include <unordered_map>
#include <unordered_set>
#include <string_view>
#include <string>

LIBCXXW_NAMESPACE_START

//! Main application window.

//! \see main_window

class main_windowObj : public generic_windowObj {

public:
	LOG_CLASS_SCOPE;

	class implObj;
	class handlerObj;

	//! Internal implementation details

	const ref<implObj> impl;

	//! Constructor, phase 1.
	main_windowObj(const ref<implObj> &impl,
		       const ref<layoutmanagerObj::implObj> &lm)
		LIBCXX_HIDDEN;

	//! Constructor, phase 2.
	void constructor(const ref<implObj> &impl,
			 const ref<layoutmanagerObj::implObj> &lm)
		LIBCXX_HIDDEN;

	//! Destructor
	~main_windowObj() LIBCXX_HIDDEN;

	//! Install a window icon.
	void install_window_icon(const std::vector<std::string> &);

	//! Install a window icon.
	void install_window_icon(const std::vector<std::tuple
				 <std::string, dim_t, dim_t>> &);

	//! Install a window icon.
	void install_window_theme_icon(const std::vector<std::string> &);

	//! Install a window icon.
	void install_window_theme_icon(const std::vector<std::tuple
				       <std::string, dim_t, dim_t>> &);

	//! Constructor for a standard theme-specified dialog.

	//! \internal
	std::unordered_map<std::string, container>
	initialize_theme_dialog(const std::string_view &,
				const standard_dialog_elements_t &,
				const std::unordered_map<std::string,
				shortcut> &);

	//! Return the layout manager for the menu bar.

	menubarlayoutmanager get_menubarlayoutmanager();

	//! Return the layout manager for the menu bar.

	const_menubarlayoutmanager get_menubarlayoutmanager() const;

	//! Return the menu bar itself.

	container get_menubar();

	//! Return the menu bar itself.
	const_container get_menubar() const;

	//! Install a callback that gets invoke when the window close button is clicked.

	void on_delete(const functionref<void (THREAD_CALLBACK,
					       const busy &)> &callback);

private:

	//! Inherited from containerObj

	ref<layoutmanagerObj::implObj> get_layout_impl() const override;

public:
	//! Creator for create_mainwindow()

	typedef dialog external_dialog_creator_t(const dialog_args &);

	//! Create an empty dialog.

	template<typename dialog_creator>
	auto create_dialog(const std::string_view &dialog_id,
			   dialog_creator &&f,
			   bool modal=false,
			   const new_layoutmanager &l=new_gridlayoutmanager{})
	{
		return do_create_dialog
			(dialog_id,
			 make_function<void (const dialog &)>
			 (std::forward<dialog_creator>(f)),
			 l, modal);
	}

	template<typename dialog_creator>
	auto create_custom_dialog(const std::string_view &dialog_id,
				  dialog_creator &&f,
				  bool modal=false,
				  const new_layoutmanager &l=new_gridlayoutmanager{})
	{
		typedef decltype(f( std::declval<const dialog_args &>())
				 ) f_ret;

		typename refptr_traits<f_ret>::ptr_t retptr;

		do_create_dialog(dialog_id,
				 make_function<external_dialog_creator_t>
				 ([&]
				  (const dialog_args &args)
				  {
					  auto r=f(args);

					  retptr=r;

					  return r;
				  }),
				 l, modal);

		return f_ret{retptr};
	}

	//! Type-erased create_dialog().

	dialog do_create_dialog(const std::string_view &dialog_id,
				const function<void (const dialog &)> &creator,
				const new_layoutmanager &layout_factory,
				bool modal);

	//! Type-erased create_custom_dialog().

	void do_create_dialog(const std::string_view &dialog_id,
			      const function<external_dialog_creator_t>
			      &creator,
			      const new_layoutmanager &layout_factory,
			      bool modal)
		LIBCXX_PUBLIC;

	//! Remove an existing dialog, if it exists.
	void remove_dialog(const std::string_view &dialog_id);

	//! Find an existing dialog
	dialogptr get_dialog(const std::string_view &dialog_id) const;

	//! Return all dialogs that currently exist.
	std::unordered_set<std::string> dialogs() const;

	//! Create an OK dialog

	dialog create_ok_dialog(const std::string_view &dialog_id,
				const std::string &icon,
				const functionref<void (const factory &)>
				&content_factory,
				const functionref<void (THREAD_CALLBACK,
							const busy &)>
				&ok_action,
				bool modal=false);

	//! Create an OK dialog

	dialog create_ok_dialog(const std::string_view &dialog_id,
				const std::string &icon,
				const functionref<void (const factory &)>
				&content_factory,
				const functionref<void (THREAD_CALLBACK,
							const busy &)>
				&ok_action,
				const text_param &ok_label,
				bool modal=false);

	//! Use create_ok_dialog() to create a simple informational message dialog.

	void alert_message(const text_param &);

	//! Use create_ok_dialog() to create a simple informational message dialog.

	void alert_message(const text_param &,
			   const alert_message_config &);

	//! Use create_ok_dialog() to create a simple error message dialog.

	void stop_message(const text_param &);

	//! Use create_ok_dialog() to create a simple error message dialog.

	void stop_message(const text_param &,
			   const stop_message_config &);

	//! Format an exception, then use error_message().

	void exception_message(const exception &);

	//! Format an exception, then use error_message().

	void exception_message(const exception &,
			       const stop_message_config &);

	//! Create an OK or CANCEL dialog

	dialog
	create_ok_cancel_dialog(const std::string_view &dialog_id,
				const std::string &icon,
				const functionref<void (const factory &)>
				&content_factory,
				const functionref<void (THREAD_CALLBACK,
							const busy &)>
				&ok_action,
				const functionref<void (THREAD_CALLBACK,
							const busy &)>
				&cancel_action,
				bool modal=false);

	//! Create an OK or CANCEL dialog

	dialog
	create_ok_cancel_dialog(const std::string_view &dialog_id,
				const std::string &icon,
				const functionref<void (const factory &)>
				&content_factory,
				const functionref<void (THREAD_CALLBACK,
							const busy &)>
				&ok_action,
				const functionref<void (THREAD_CALLBACK,
							const busy &)>
				&cancel_action,
				const text_param &ok_label,
				const text_param &cancel_label,
				bool modal=false);

	//! Create a dialog with an input field.

	input_dialog
	create_input_dialog(const std::string_view &dialog_id,
			    const std::string &icon,
			    const functionref<void (const factory &)>
			    &label_factory,
			    const text_param &initial_text,
			    const input_field_config &config,
			    const functionref<void (THREAD_CALLBACK,
						    const input_field &,
						    const busy &)>
			    &ok_action,
			    const functionref<void (THREAD_CALLBACK,
						    const busy &)>
			    &cancel_action,
			    bool modal=false);

	//! Create a dialog with an input field.

	input_dialog
	create_input_dialog(const std::string_view &dialog_id,
			    const std::string &icon,
			    const functionref<void (const factory &)>
			    &label_factory,
			    const text_param &initial_text,
			    const input_field_config &config,
			    const functionref<void (THREAD_CALLBACK,
						    const input_field &,
						    const busy &)>
			    &ok_action,
			    const functionref<void (THREAD_CALLBACK,
						    const busy &)>
			    &cancel_action,
			    const text_param &ok_label,
			    const text_param &cancel_label,
			    bool modal=false);

	//! Create a file dialog
	file_dialog create_file_dialog(const std::string_view &dialog_id,
				       const file_dialog_config &,
				       bool modal=false);

	//! Create a print dialog
	print_dialog create_print_dialog(const std::string_view &dialog_id,
					 const print_dialog_config &,
					 bool modal=false);

	//! Construct a callback that calls remove_dialog().

	//! This method is called on the parent window/dialog object.
	//! The returned lambda
	//! can be specified as an ok or cancel callback, with
	//! create_ok_dialog() or create_ok_cancel_dialog(). The invoked
	//! callback invokes parent window/dialog's remove_dialog()
	//! in order to remove the dialog
	//! automatically after it's closed.

	functionref<void (THREAD_CALLBACK, const busy &)
		    > destroy_when_closed(const std::string_view &);

	//! Get current screen position
	screen_position get_screen_position() const;
};

LIBCXXW_NAMESPACE_END

#endif
