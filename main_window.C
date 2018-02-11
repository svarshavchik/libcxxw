/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "main_window.H"
#include "main_window_handler.H"
#include "screen.H"
#include "connection_thread.H"
#include "batch_queue.H"
#include "busy.H"
#include "messages.H"
#include "x/w/picture.H"
#include "x/w/screen.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/menubarlayoutmanager.H"
#include "dialog.H"
#include "x/w/image.H"
#include "x/w/button.H"
#include "x/w/text_param.H"
#include "x/w/canvas.H"
#include "x/w/input_field.H"
#include "file_dialog/file_dialog_impl.H"
#include "layoutmanager.H"
#include "peephole/peephole_toplevel.H"
#include "peephole/peepholed_toplevel_element.H"
#include "peepholed_toplevel_main_window.H"
#include "peepholed_toplevel_main_window_impl.H"
#include "menu/menubarlayoutmanager_impl.H"
#include "menu/menubar_container_impl.H"
#include "container_element.H"
#include "x/w/input_dialog.H"
#include "x/w/label.H"
#include "dialog_impl.H"
#include "dialog_handler.H"
#include "always_visible.H"
#include <x/weakcapture.H>
#include <variant>

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::main_windowObj);

LIBCXXW_NAMESPACE_START

main_windowObj::main_windowObj(const ref<implObj> &impl,
			       const ref<layoutmanagerObj::implObj> &lm)
	: generic_windowObj(impl, lm),
	  impl(impl)
{
}

void main_windowObj::constructor(const ref<implObj> &impl,
				 const ref<layoutmanagerObj::implObj> &lm)
{
	impl->handler->public_object=ref(this);
}

main_windowObj::~main_windowObj()=default;

void main_windowObj::on_delete(const std::function<void (const busy &)
			       > &callback)
{
	impl->on_delete(callback);
}

void main_windowObj::install_window_icon(const std::vector<std::string> &a)
{
	std::vector<std::tuple<std::string, dim_t, dim_t>> cpy;

	cpy.reserve(a.size());
	for (const auto &n:a)
		cpy.emplace_back(n, 0, 0);
	install_window_icon(cpy);
}

void main_windowObj::install_window_icon(const std::vector<std::tuple
					 <std::string, dim_t, dim_t>> &a)
{
	impl->handler->install_window_icon(a);
}

void main_windowObj::install_window_theme_icon(const std::vector<std::string>
					       &a)
{
	std::vector<std::tuple<std::string, dim_t, dim_t>> cpy;

	cpy.reserve(a.size());
	for (const auto &n:a)
		cpy.emplace_back(n, 0, 0);
	install_window_theme_icon(cpy);
}

void main_windowObj::install_window_theme_icon(const std::vector<std::tuple
					       <std::string, dim_t, dim_t>> &a)
{
	impl->handler->install_window_theme_icon(a);
}

//////////////////////////////////////////////////////////////////////////////
//
// Create a default main window

main_window main_windowBase::do_create(const function<main_window_creator_t> &f)
{
	return screen::base::create()->do_create_mainwindow(f);
}

main_window main_windowBase::do_create(const function<main_window_creator_t> &f,
				       const new_layoutmanager &factory)
{
	return screen::base::create()->do_create_mainwindow(f, factory);
}

main_window main_windowBase::do_create(const screen_positions_t &pos,
				       const std::string_view &name,
				       const function<main_window_creator_t> &f)
{
	return screen::base::create()->do_create_mainwindow(pos, name, f);
}

main_window main_windowBase::do_create(const screen_positions_t &pos,
				       const std::string_view &name,
				       const function<main_window_creator_t> &f,
				       const new_layoutmanager &factory)
{
	return screen::base::create()->do_create_mainwindow(pos, name, f,
							    factory);
}

main_window screenObj
::do_create_mainwindow(const function<main_window_creator_t> &f)
{
	return do_create_mainwindow(f, new_gridlayoutmanager{});
}

main_window screenObj
::do_create_mainwindow(const screen_positions_t &pos,
		       const std::string_view &name,
		       const function<main_window_creator_t> &f)
{
	return do_create_mainwindow(pos, name, f, new_gridlayoutmanager{});
}

class LIBCXX_HIDDEN app_container_implObj :
	public always_visibleObj<container_elementObj<child_elementObj>> {

	typedef always_visibleObj<container_elementObj<child_elementObj>
				  > superclass_t;
 public:
	using superclass_t::superclass_t;

	~app_container_implObj()=default;
};

static inline peepholed_toplevel
init_containers(const ref<containerObj::implObj> &parent,
		containerptr &menu_and_app_container,
		containerptr &menubar_container,
		containerptr &app_container,
		const new_layoutmanager &layout_factory)
{
	// This is the element in a peephole. It's a container
	// with two elements.

	auto menu_and_app_impl=ref<peepholed_toplevel_main_windowObj::implObj>
		::create(parent);

	auto menu_and_app=peepholed_toplevel_main_window
		::create(menu_and_app_impl,
			 ref<gridlayoutmanagerObj::implObj>::create
			 (menu_and_app_impl));

	gridlayoutmanager glm=menu_and_app->get_layoutmanager();

	// Create the first element, a container with the menubarlayoutmanager.

	// Fill the menu bar horizontally across the entire window.
	auto f=glm->append_row();
	f->padding(0);
	f->halign(halign::fill);

	auto menubar_impl=ref<menubar_container_implObj>
		::create(menu_and_app_impl);

	menubar_impl->elementObj::implObj
		::set_background_color("menubar_background_color");
	auto menubar=container::create(menubar_impl,
				       ref<menubarlayoutmanagerObj::implObj>
				       ::create(menubar_impl));
	f->remove_when_hidden(true);

	menubarlayoutmanager mblm=menubar->get_layoutmanager();

	{
		menubar_lock lock{mblm};
		mblm->impl->initialize(&*mblm, lock);
	}

	f->created_internally(menubar);

	// The second element is the container with the app-requested
	// layout manager.

	f=glm->append_row();
	f->padding(0);

	// The top level main window should get sized based on the container's
	// metrics, but if there's any extra space, it all goes there.
	glm->requested_row_height(1, 100);

	auto app_impl=ref<app_container_implObj>
		::create(menu_and_app_impl);

	auto layout_impl=layout_factory.create(app_impl);

	auto app=container::create(app_impl, layout_impl);
	f->created_internally(app);

	menu_and_app_container=menu_and_app;
	menubar_container=menubar;
	app_container=app;

	return menu_and_app;
}

// We have the main_windowObj::handlerObj.
//
// This creates main_windowObj::implObj, and the peephole layout manager.
// The main window implementation, the handler object, gets the peephole
// layout manager, for scrolling the contents of the main window if they
// exceed the maximum permissible size of the main window, whose size is
// limited by the desktop's size.

std::tuple<ref<main_windowObj::implObj>, layoutmanager>
do_create_main_window_impl(const ref<main_windowObj::handlerObj> &handler,
			   const new_layoutmanager &layout_factory,
			   const function<make_window_impl_factory_t> &factory)
{
	// menu_and_app_container is the element in the peephole, the
	// container for the window's menu, and the container for the
	// actual contents of the window, with the layoutmanager that
	// the layout_factory creates, the layout manager that the
	// application requested for the main window.
	//
	// menubar_container is the menu bar's container, and app_container
	// is the container that uses the app-specified layout manager.

	containerptr menu_and_app_container,
		menubar_container, app_container;

	// Create a top level peephole in the main_window.

	peephole_style main_window_peephole_style{halign::fill};

	auto lm=create_peephole_toplevel
		(handler,
		 std::nullopt,
		 std::nullopt,
		 main_window_peephole_style,
		 [&]
		 (const ref<containerObj::implObj> &parent)
		 {
			 auto c=init_containers(parent,
						menu_and_app_container,
						menubar_container,
						app_container,
						layout_factory);

			 return c;
		 });

	auto window_impl=factory(main_window_impl_args{
			handler, menu_and_app_container,
				menubar_container, app_container});
	return {window_impl, lm};
}

main_window screenObj
::do_create_mainwindow(const screen_positions_t &pos,
		       const std::string_view &name,
		       const function<main_window_creator_t> &f,
		       const new_layoutmanager &factory)
{
	std::string s{name};
	auto iter=pos.find(s);

	if (iter == pos.end())
		return do_create_mainwindow(f, factory);
	return do_create_mainwindow(iter->second, f, factory);
}

main_window screenObj
::do_create_mainwindow(const function<main_window_creator_t> &f,
		       const new_layoutmanager &layout_factory)
{
	return do_create_mainwindow(std::nullopt, f, layout_factory);
}

main_window screenObj
::do_create_mainwindow(const std::optional<screen_position> &pos,
		       const function<main_window_creator_t> &f,
		       const new_layoutmanager &layout_factory)
{
	auto new_screen=screen(this);

	if (pos && pos->screen_number)
	{
		auto conn=get_connection();

		if (*pos->screen_number < conn->screens())
			new_screen=screen::create(*pos->screen_number);
	}

	// Keep a batch queue in scope for the duration of the creation,
	// so everything gets buffered up.

	auto queue=connref->impl->thread->get_batch_queue();

	auto handler=ref<main_windowObj::handlerObj>
		::create(connref->impl->thread, new_screen,
			 pos ? std::optional<rectangle>{pos}
			 : std::optional<rectangle>{},
			 "mainwindow_background");

	handler->set_window_type("normal");

	auto [window_impl, lm]=create_main_window_impl
		(handler, layout_factory,
		 [](const auto &args)
		 {
			 return ref<main_windowObj::implObj>::create(args);
		 });

	auto mw=ptrref_base::objfactory<main_window>
		::create(window_impl, lm->impl);

	f(mw);

	return mw;
}

ref<layoutmanagerObj::implObj> main_windowObj::get_layout_impl() const
{
	return impl->app_container->get_layout_impl();
}


container main_windowObj::get_menubar()
{
	return impl->menubar_container;
}

const_container main_windowObj::get_menubar() const
{
	return impl->menubar_container;
}

menubarlayoutmanager main_windowObj::get_menubarlayoutmanager()
{
	return get_menubar()->get_layoutmanager();
}

const_menubarlayoutmanager main_windowObj::get_menubarlayoutmanager() const
{
	return get_menubar()->get_layoutmanager();
}

/////////////////////////////////////////////////////////////////////////////

// Return a factory that inserts an icon into a theme template.

static auto icon_element(const std::string &icon)
{
	return std::function<void (const factory &)>
		([=](const factory &f)
		 {
			 f->create_image(icon);
		 });
}

// Return a factory that creates a button in a theme template

std::function<void (const factory &)>
dialog_ok_button(const text_param &label,
		 buttonptr &ret,
		 char32_t key)
{
	return [label, &ret, key](const factory &f)
	{
		ret=f->create_special_button_with_label(label, {key});
	};
}

std::function<void (const factory &)>
dialog_cancel_button(const text_param &label,
		     buttonptr &ret,
		     char32_t key)
{
	return [label, &ret, key](const factory &f)
	{
		ret=f->create_normal_button_with_label(label, {key});
	};
}

std::function<void (const factory &)> dialog_filler()
{
	return [](const factory &f)
	{
		f->create_canvas();
	};
}

// Hide a theme-generated dialog, then invoke a callback action.

static void hide_and_invoke(const captured_dialog_t &d,
			    const std::function
			    <void (const busy &)>  &action)
{
	auto got=d.get();

	if (got)
	{
		auto &[d]=*got;

		d->dialog_window->hide();

		// Make sure that the callback gets the parent display
		// element's main window.

		busy_impl yes_i_am{d->impl->handler->parent_handler};

		action(yes_i_am);
	}
}

// When a theme dialog's button is pressed, hide and invoke the callback.

void hide_and_invoke_when_activated(const dialog &d,
				    const hotspot &button,
				    const std::function
				    <void (const busy &)> &action)
{
	button->on_activate([d=make_weak_capture(d), action]
			    (const auto &ignore, const busy &yes_i_am)
			    {
				    hide_and_invoke(d, action);
			    });
}

// When a theme dialog's close button is pressed, hide and invoke the callback.

void hide_and_invoke_when_closed(const dialog &d,
				 const std::function
				 <void (const busy &)> &action)
{
	d->dialog_window->on_delete([d=make_weak_capture(d), action]
				    (const busy &yes_i_am)
				    {
					    hide_and_invoke(d, action);
				    });
}

dialog main_windowObj
::create_ok_dialog(const std::string_view &dialog_id,
		   const std::string &icon,
		   const std::function<void (const factory &)>
		   &content_factory,
		   const std::function<void (const busy &)>
		   &ok_action,
		   bool modal)
{
	return create_ok_dialog(dialog_id,
				icon, content_factory, ok_action,
				stop_message_config::default_ok_label(),
				modal);
}

dialog main_windowObj
::create_ok_dialog(const std::string_view &dialog_id,
		   const std::string &icon,
		   const std::function<void (const factory &)>
		   &content_factory,
		   const std::function<void (const busy &)>
		   &ok_action,
		   const text_param &ok_label,
		   bool modal)
{
	buttonptr ok_button;

	auto d=create_dialog
		(dialog_id,
		 [&]
		 (const dialog &d)
		 {
			 d->dialog_window->initialize_theme_dialog
			 ("ok-dialog",
			  standard_dialog_elements_t{
				 {"icon", icon_element(icon)},
				 {"message", content_factory},

					 // Still need to add a filler
					 // element to the container
					 // row, so that the container's
					 // horizontal metrics are
					 // open-ended.
					 //
					 // Otherwise the fixed
					 // container row metrics
					 // will constrain the width
					 // of wrappable labels used
					 // for the message.

				 {"filler", dialog_filler()},
				 {"ok", dialog_ok_button(ok_label, ok_button, '\e')}
			 }, {});
		 },
		 modal);

	// Ok button's shortcut is ESC. autofocus() th einput on the Ok
	// button. "Enter" will activate the "Ok" button, closing the popup.
	// Esc will activate the button via the shortcut, also closing
	// the popup.

	ok_button->autofocus(true);
	hide_and_invoke_when_activated(d, ok_button, ok_action);
	hide_and_invoke_when_closed(d, ok_action);

	return d;
}

void main_windowObj::exception_message(const exception &e)
{
	exception_message(e, {});
}


void main_windowObj::exception_message(const exception &e,
				       const stop_message_config &conf)
{
	std::ostringstream o;

	o << e;

	stop_message(o.str(), conf);
}

void main_windowObj::stop_message(const text_param &msg)
{
	stop_message(msg, {});
}

void main_windowObj::stop_message(const text_param &msg,
				   const stop_message_config &config)
{
	auto autodestroy=destroy_when_closed("stop_message@libcxx.com");

	auto d=create_ok_dialog("stop_message@libcxx.com",
				"stop",
				[&]
				(const auto &f)
				{
					f->create_label(msg);
				},
				[autodestroy, cb=config.acknowledged_callback]
				(const auto &ignore)
				{
					autodestroy(ignore);
					if (cb)
						cb();
				},
				config.ok_label,
				config.modal);

	std::visit( [&](const auto &title)
		    {
			    d->dialog_window->set_window_title(title);
		    }, config.title);

	d->dialog_window->show_all();
}

std::string stop_message_config::default_title() noexcept
{
	return _("Error");
}

text_param stop_message_config::default_ok_label() noexcept
{
	return _("Ok");
}

stop_message_config::~stop_message_config()=default;

void main_windowObj::alert_message(const text_param &msg)
{
	alert_message(msg, {});
}

void main_windowObj::alert_message(const text_param &msg,
				   const alert_message_config &config)
{
	auto autodestroy=destroy_when_closed("alert_message@libcxx.com");

	auto d=create_ok_dialog("alert_message@libcxx.com",
				"alert",
				[&]
				(const auto &f)
				{
					f->create_label(msg);
				},
				[autodestroy, cb=config.acknowledged_callback]
				(const auto &ignore)
				{
					autodestroy(ignore);
					if (cb)
						cb();
				},
				config.ok_label,
				config.modal);

	std::visit( [&](const auto &title)
		    {
			    d->dialog_window->set_window_title(title);
		    }, config.title);

	d->dialog_window->show_all();
}

std::string alert_message_config::default_title() noexcept
{
	return _("Attention");
}

text_param alert_message_config::default_ok_label() noexcept
{
	return _("Ok");
}

alert_message_config::~alert_message_config()=default;

dialog main_windowObj
::create_ok_cancel_dialog(const std::string_view &dialog_id,
			  const std::string &icon,
			  const std::function<void (const factory &)>
			  &content_factory,
			  const std::function<void (const busy &)>
			  &ok_action,
			  const std::function<void (const busy &)>
			  &cancel_action,
			  bool modal)
{
	return create_ok_cancel_dialog(dialog_id,
				       icon, content_factory, ok_action,
				       cancel_action,
				       _("Ok"),
				       _("Cancel"), modal);
}

dialog main_windowObj
::create_ok_cancel_dialog(const std::string_view &dialog_id,
			  const std::string &icon,
			  const std::function<void (const factory &)>
			  &content_factory,
			  const std::function<void (const busy &)>
			  &ok_action,
			  const std::function<void (const busy &)>
			  &cancel_action,
			  const text_param &ok_label,
			  const text_param &cancel_label,
			  bool modal)
{
	buttonptr ok_button;
	buttonptr cancel_button;

	auto d=create_dialog
		(dialog_id,
		 [&]
		 (const dialog &d)
		 {
			 d->dialog_window->initialize_theme_dialog
			 ("ok-cancel-dialog", standard_dialog_elements_t{
				 {"icon", icon_element(icon)},
				 {"message", content_factory},
				 {"ok", dialog_ok_button(ok_label,
							 ok_button,
							 '\n')},
				 {"filler", dialog_filler()},
				 {"cancel", dialog_cancel_button
						 (cancel_label,
						  cancel_button, '\e')}
			 }, {});
		 },
		 modal);

	hide_and_invoke_when_activated(d, ok_button, ok_action);
	hide_and_invoke_when_activated(d, cancel_button, cancel_action);
	hide_and_invoke_when_closed(d, cancel_action);

	return d;
}

input_dialog main_windowObj
::create_input_dialog(const std::string_view &dialog_id,
		      const std::string &icon,
		      const std::function<void (const factory &)>
		      &label_factory,
		      const text_param &initial_text,
		      const input_field_config &config,
		      const std::function<void (const input_field &,
						const busy &)>
		      &ok_action,
		      const std::function<void (const busy &)>
		      &cancel_action,
		      bool modal)
{
	return create_input_dialog(dialog_id,
				   icon, label_factory,
				   initial_text, config,
				   ok_action,
				   cancel_action,
				   _("Ok"),
				   _("Cancel"), modal);
}

input_dialog main_windowObj
::create_input_dialog(const std::string_view &dialog_id,
		      const std::string &icon,
		      const std::function<void (const factory &)>
		      &label_factory,
		      const text_param &initial_text,
		      const input_field_config &config,
		      const std::function<void (const input_field &,
						const busy &)> &ok_action,
		      const std::function<void (const busy &)> &cancel_action,
		      const text_param &ok_label,
		      const text_param &cancel_label,
		      bool modal)
{
	buttonptr ok_button;
	buttonptr cancel_button;
	input_fieldptr field;

	input_dialogptr new_input_dialog;

	auto d=create_custom_dialog
		(dialog_id,
		 [&]
		 (const auto &args)
		 {
			 args.dialog_window->initialize_theme_dialog
			 ("input-dialog", standard_dialog_elements_t{
				 {"icon", icon_element(icon)},
				 {"label", label_factory},
				 {"input", [&](const auto &factory)
					 {
						 field=factory
							 ->create_input_field
							 (initial_text,
							  config);
					 }},
				 {"ok", dialog_ok_button(ok_label,
							 ok_button,
							 '\n')},
				 {"filler", dialog_filler()},
				 {"cancel", dialog_cancel_button
						 (cancel_label,
						  cancel_button, '\e')}
			 }, {});

			 auto id=input_dialog::create(args, field);

			 new_input_dialog=id;

			 return id;

		 }, modal);

	hide_and_invoke_when_activated
		(d, ok_button,
		 [field=input_field{field}, ok_action]
		 (const auto &busy)
		 {
			 ok_action(field, busy);
		 });
	hide_and_invoke_when_activated(d, cancel_button, cancel_action);
	hide_and_invoke_when_closed(d, cancel_action);

	return new_input_dialog;
}

screen_position main_windowObj::get_screen_position() const
{
	auto handler=impl->handler;

	auto [x, y] = handler->root_xy.get();

	auto r=handler->current_position.get();

	return {{x, y, r.width, r.height}, get_screen()->impl->screen_number};
}

LIBCXXW_NAMESPACE_END
