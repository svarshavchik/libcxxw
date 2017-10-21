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
#include <x/w/file_dialogfwd.H>
#include <x/w/file_dialog_configfwd.H>
#include <x/obj.H>
#include <x/logger.H>
#include <x/functional.H>
#include <x/refptr_traits.H>

#include <utility>
#include <unordered_map>
#include <unordered_set>
#include <string_view>
#include <string>
#include <functional>

LIBCXXW_NAMESPACE_START

//! What kind of a creator functor gets passed to \ref main_window "create_dialog"().

//! The parameter is one of two different kinds of functors -- a standard
//! functor that takes a created \ref dialog "INSERT_LIBX_NAMESPACE::w::dialog"
//! object, and creates its contents, or a custom creator that takes an
//! opaque \c dialog_args parameter, and uses it to construct a custom subclass
//! of \ref dialogObj "dialogObj", and then create its contents.
//!
//! The member \c type is a \ref dialog_creator_type "dialog_creator_type".

enum class dialog_creator_type { standard_creator, custom_creator };

//! Classify the parameter to create_dialog().

//! The template defines the member type which is a
//! \ref dialog_creator_type "dialog_creator_type".

template<typename functor, typename=void> struct classify_dialog_creator_type;

//! Classify the parameter to create_dialog().

//! Specialization that sets the \c type member to
//! \ref dialog_creator_type "standard_creator".

template<typename functor>
struct classify_dialog_creator_type<
	functor, std::void_t<decltype(std::declval<functor &&>()
				      (std::declval<const dialog &>()))>> {

	static constexpr dialog_creator_type type=
		dialog_creator_type::standard_creator;
};

//! Classify the parameter to create_dialog().

//! Specialization that sets the \c type member to
//! \ref dialog_creator_type "custom_creator".

template<typename functor>
struct classify_dialog_creator_type<
	functor, std::void_t<decltype(std::declval<functor &&>()
				      (std::declval<const dialog_args &>()))>> {

	static constexpr dialog_creator_type type=
		dialog_creator_type::custom_creator;
};

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

	typedef dialog external_dialog_creator_t(const dialog_args &);

	//! Create a dialog.

	template<typename dialog_creator>
	auto create_dialog(const std::string_view &dialog_id,
			   dialog_creator &&f,
			   bool modal=false,
			   const new_layoutmanager &l=new_gridlayoutmanager{})
	{
		if constexpr (classify_dialog_creator_type<
			      std::remove_reference_t<dialog_creator>>::type ==
			      dialog_creator_type::standard_creator)
		{
			return do_create_dialog
				(dialog_id,
				 make_function<void (const dialog &)>
				 (std::forward<dialog_creator>(f)),
				 l, modal);
		}
		else
		{
			typedef decltype(f( std::declval<const dialog_args &>())
					 ) f_ret;

			typename refptr_traits<f_ret>::ptr_t retptr;

			do_create_dialog(dialog_id,
					 make_function<external_dialog_creator_t
					 >
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
	}

	//! Type-erased create_dialog().

	dialog do_create_dialog(const std::string_view &dialog_id,
				const function<void (const dialog &)> &creator,
				const new_layoutmanager &layout_factory,
				bool modal);

	//! Type-erased create_dialog().

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
				const std::string_view &icon,
				const std::function<void (const gridfactory &)>
				&content_factory,
				const std::function<void (const busy &)>
				&ok_action,
				bool modal=false);

	//! Create an OK dialog

	dialog create_ok_dialog(const std::string_view &dialog_id,
				const std::string_view &icon,
				const std::function<void (const gridfactory &)>
				&content_factory,
				const std::function<void (const busy &)>
				&ok_action,
				const text_param &ok_label,
				bool modal=false);

	//! Create an OK or CANCEL dialog

	dialog
	create_ok_cancel_dialog(const std::string_view &dialog_id,
				const std::string_view &icon,
				const std::function<void (const gridfactory &)>
				&content_factory,
				const std::function<void (const busy &)>
				&ok_action,
				const std::function<void (const busy &)>
				&cancel_action,
				bool modal=false);

	//! Create an OK or CANCEL dialog

	dialog
	create_ok_cancel_dialog(const std::string_view &dialog_id,
				const std::string_view &icon,
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
	create_input_dialog(const std::string_view &dialog_id,
			    const std::string_view &icon,
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
	create_input_dialog(const std::string_view &dialog_id,
			    const std::string_view &icon,
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

	//! Create a file dialog
	file_dialog create_file_dialog(const std::string_view &dialog_id,
				       const file_dialog_config &,
				       bool modal=false);

	//! Construct a callback that calls remove_dialog().

	//! This can be specified as an ok or cancel callback, with
	//! create_ok_dialog() or create_ok_cancel_dialog(). The invoked
	//! callback invokes remove_dialog() in order to remove the dialog
	//! automatically after it's closed.

	std::function<void (const busy &)
		      > destroy_when_closed(const std::string_view &);
};

LIBCXXW_NAMESPACE_END

#endif
