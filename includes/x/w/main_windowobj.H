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
#include <x/w/new_layoutmanagerfwd.H>
#include <x/w/text_paramfwd.H>
#include <x/w/busyfwd.H>
#include <x/w/input_field_configfwd.H>
#include <x/w/input_fieldfwd.H>
#include <x/obj.H>
#include <x/logger.H>
#include <x/functional.H>

#include <utility>
#include <unordered_map>
#include <string_view>
#include <string>
#include <functional>

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

	//! Constructor
	main_windowObj(const ref<implObj> &impl,
		       const ref<layoutmanagerObj::implObj> &lm)
		LIBCXX_HIDDEN;

	//! Destructor
	~main_windowObj() LIBCXX_HIDDEN;

	//! Return the layout manager for the menu bar.

	menubarlayoutmanager get_menubarlayoutmanager();

	//! Return the layout manager for the menu bar.

	const_menubarlayoutmanager get_menubarlayoutmanager() const;

	//! Return the menu bar itself.

	container get_menubar();

	//! Return the menu bar itself.
	const_container get_menubar() const;

	//! Install a callback that gets invoke when the window close button is clicked.

	void on_delete(const std::function<void (const busy &)> &callback);

private:

	//! Inherited from containerObj

	ref<layoutmanagerObj::implObj> get_layout_impl() const override;

public:
	//! Creator for create_mainwindow()

	typedef void dialog_creator_t(const dialog &);

	//! Create a generic dialog.

	template<typename functor>
	dialog create_dialog(functor &&f,
			     const new_layoutmanager &l=new_gridlayoutmanager(),
			     bool modal=false)
	{
		return do_create_dialog(make_function<dialog_creator_t>
					(std::forward<functor>(f)),
					l, modal);
	}

	//! Type-erased create_dialog().

	dialog do_create_dialog(const function<dialog_creator_t> &creator,
				const new_layoutmanager &layout_factory,
				bool modal)
		LIBCXX_PUBLIC;

	//! Create a standard dialog.
	dialog create_standard_dialog(const std::string_view &name,
				      bool modal=false,
				      std::unordered_map<std::string,
				      std::function<void (const gridfactory &)>>
				      &&elements={});

	//! Create a standard dialog.
	dialog create_standard_dialog(const std::string_view &name,
				      bool modal,
				      const std::unordered_map<std::string,
				      std::function<void (const gridfactory &)>>
				      &elements);

	//! Create an OK dialog

	dialog create_ok_dialog(const std::string_view &icon,
				const std::function<void (const gridfactory &)>
				&content_factory,
				const std::function<void (const busy &)>
				&ok_action,
				bool modal=false);

	//! Create an OK dialog

	dialog create_ok_dialog(const std::string_view &icon,
				const std::function<void (const gridfactory &)>
				&content_factory,
				const std::function<void (const busy &)>
				&ok_action,
				const text_param &ok_label,
				bool modal=false);

	//! Create an OK or CANCEL dialog

	dialog
	create_ok_cancel_dialog(const std::string_view &icon,
				const std::function<void (const gridfactory &)>
				&content_factory,
				const std::function<void (const busy &)>
				&ok_action,
				const std::function<void (const busy &)>
				&cancel_action,
				bool modal=false);

	//! Create an OK or CANCEL dialog

	dialog
	create_ok_cancel_dialog(const std::string_view &icon,
				const std::function<void (const gridfactory &)>
				&content_factory,
				const std::function<void (const busy &)>
				&ok_action,
				const std::function<void (const busy &)>
				&cancel_action,
				const text_param &ok_label,
				const text_param &cancel_label,
				bool modal=false);

	//! Create a dialog with an input field.

	dialog
	create_input_dialog(const std::string_view &icon,
			    const std::function<void (const gridfactory &)>
			    &label_factory,
			    const text_param &initial_text,
			    const input_field_config &config,
			    const std::function<void (const input_field &,
						      const busy &)>
			    &ok_action,
			    const std::function<void (const busy &)>
			    &cancel_action,
			    bool modal=false);

	//! Create a dialog with an input field.

	dialog
	create_input_dialog(const std::string_view &icon,
			    const std::function<void (const gridfactory &)>
			    &label_factory,
			    const text_param &initial_text,
			    const input_field_config &config,
			    const std::function<void (const input_field &,
						      const busy &)>
			    &ok_action,
			    const std::function<void (const busy &)>
			    &cancel_action,
			    const text_param &ok_label,
			    const text_param &cancel_label,
			    bool modal=false);
};

LIBCXXW_NAMESPACE_END

#endif
