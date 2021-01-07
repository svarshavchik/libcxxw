/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "main_window.H"
#include "main_window_handler.H"
#include "override_redirect.H"
#include "screen.H"
#include "shared_handler_data.H"
#include "connection_thread.H"
#include "batch_queue.H"
#include "busy.H"
#include "messages.H"
#include "screen_positions_impl.H"
#include "x/w/picture.H"
#include "x/w/screen.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/menubarlayoutmanager.H"
#include "x/w/main_window_appearance.H"
#include "dialog.H"
#include "x/w/image.H"
#include "x/w/button.H"
#include "x/w/text_param.H"
#include "x/w/canvas.H"
#include "x/w/input_field.H"
#include "file_dialog/file_dialog_impl.H"
#include "x/w/impl/layoutmanager.H"
#include "peephole/peephole_toplevel.H"
#include "peephole/peepholed_toplevel_element.H"
#include "peepholed_toplevel_main_window.H"
#include "peepholed_toplevel_main_window_impl.H"
#include "menu/menubarlayoutmanager_impl.H"
#include "menu/menubar_container_impl.H"
#include "x/w/impl/container_element.H"
#include "x/w/input_dialog.H"
#include "x/w/label.H"
#include "dialog_impl.H"
#include "dialog_handler.H"
#include "x/w/uielements.H"
#include "x/w/impl/always_visible.H"
#include <x/weakcapture.H>
#include <x/xml/doc.H>
#include <x/visitor.H>
#include <variant>

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::main_windowObj);

LIBCXXW_NAMESPACE_START

main_windowObj::main_windowObj(const ref<implObj> &impl,
			       const layout_impl &lm)
	: generic_windowObj(impl, lm),
	  impl(impl)
{
}

void main_windowObj::constructor(const ref<implObj> &impl,
				 const layout_impl &lm)
{
	impl->handler->public_object=ref(this);
}

main_windowObj::~main_windowObj()=default;

void main_windowObj::on_delete(const functionref<void (THREAD_CALLBACK,
						       const busy &)> &callback)
{
	impl->on_delete(callback);
}

void main_windowObj::install_window_icons(const std::vector<std::string> &a)
{
	impl->handler->install_window_icons(a);
}

void main_windowObj::install_window_icons(const std::string &filename)
{
	impl->handler->install_window_icons(filename);
}

void main_window_config::restore(const const_screen_positions &pos,
				 const std::string_view &name_arg)
{
	if (name_arg.empty())
		throw EXCEPTION(_("Window name cannot be empty"));

	name=name_arg;

	try {
		window_info=pos->impl->find(name);
	} catch (const exception &e)
	{
		auto ee=EXCEPTION( "Error restoring window \""
				   << name << "\": " << e );

		ee->caught();
	}
}

main_window_config::main_window_config()
	: appearance{main_window_appearance::base::theme()}
{
}

main_window_config::main_window_config(const main_window_config &)=default;

main_window_config &main_window_config::operator=(const main_window_config &)
=default;

main_window_config::~main_window_config()=default;

border_arg splash_window_config::default_border()
{
	return "splash_border_square";
}

splash_window_config::~splash_window_config()=default;

border_arg transparent_splash_window_config::default_border()
{
	return "splash_border";
}

transparent_splash_window_config::~transparent_splash_window_config()=default;

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

main_window main_windowBase::do_create(const main_window_config_t &config,
				       const function<main_window_creator_t> &f)
{
	return do_create(config, f, new_gridlayoutmanager{});
}

main_window main_windowBase::do_create(const main_window_config_t &config,
				       const function<main_window_creator_t> &f,
				       const new_layoutmanager &factory)
{
	return std::visit
		([&](const main_window_config &std_config)
		 {
			 auto s=screen::base::create();

			 if (std_config.window_info &&
			     std_config.window_info->screen_number)
			 {
				 auto conn=s->get_connection();

				 auto n=*std_config.window_info->screen_number;
				 if (n < conn->screens())
					 s=screen::create(conn, n);
			 }
			 return s->do_create_mainwindow(config, f, factory);
		 }, config);
}

main_window screenObj
::do_create_mainwindow(const function<main_window_creator_t> &f)
{
	return do_create_mainwindow(f, new_gridlayoutmanager{});
}

main_window screenObj
::do_create_mainwindow(const main_window_config_t &config,
		       const function<main_window_creator_t> &f)
{
	return do_create_mainwindow(config, f, new_gridlayoutmanager{});
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
init_containers(const container_impl &parent,
		containerptr &menu_and_app_container,
		containerptr &menubar_container,
		const main_window_config &config,
		containerptr &app_container,
		const new_layoutmanager &layout_factory)
{
	// This is the element in a peephole. It's a container
	// with two elements.

	auto menu_and_app_impl=ref<peepholed_toplevel_main_windowObj::implObj>
		::create(parent);

	auto menu_and_app=peepholed_toplevel_main_window
		::create(menu_and_app_impl,
			 new_gridlayoutmanager{}.create(menu_and_app_impl));

	gridlayoutmanager glm=menu_and_app->get_layoutmanager();

	// Create the first element, a container with the menubarlayoutmanager.

	// Fill the menu bar horizontally across the entire window.
	auto f=glm->append_row();
	f->padding(0);
	f->halign(halign::fill);

	auto menubar_impl=ref<menubar_container_implObj>
		::create(menu_and_app_impl, config.appearance
			 ->menubar_background_color);

	auto menubar=container::create(menubar_impl,
				       ref<menubarlayoutmanagerObj::implObj>
				       ::create(menubar_impl,
						config.appearance));
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

create_main_window_impl_ret_t
do_create_main_window_impl(const ref<main_windowObj::handlerObj> &handler,
			   const std::optional<border_arg> &border,
			   const std::optional<color_arg>
			   &peephole_background_color,
			   const std::optional<color_arg>
			   &scrollbars_background_color,
			   const main_window_config &config,
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

	peephole_style main_window_peephole_style{peephole_algorithm::automatic,
						  peephole_algorithm::automatic,
						  halign::fill};

	auto lm=create_peephole_toplevel
		(handler,
		 border,
		 peephole_background_color,
		 scrollbars_background_color,
		 main_window_peephole_style,
		 config.appearance->horizontal_scrollbar,
		 config.appearance->vertical_scrollbar,
		 [&]
		 (const container_impl &parent)
		 {
			 auto c=init_containers(parent,
						menu_and_app_container,
						menubar_container,
						config,
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
::do_create_mainwindow(const function<main_window_creator_t> &f,
		       const new_layoutmanager &layout_factory)
{
	return do_create_mainwindow(main_window_config{}, f, layout_factory);
}

///////////////////////////////////////////////////////////////////////////

namespace {
#if 0
}
#endif

class LIBCXX_HIDDEN splash_window_handlerObj;

//! An override-redirected main window handler.

class splash_window_handlerObj
	: public override_redirect_elementObj<main_windowObj::handlerObj> {

	//! Alias.

	typedef override_redirect_elementObj<main_windowObj::handlerObj
					     > superclass_t;

public:

	using superclass_t::superclass_t;

	void recalculate_window_position(ONLY IN_THREAD,
					 rectangle &r,
					 dim_t screen_width,
					 dim_t screen_height) override
	{
		if (r.width > screen_width)
			r.width=screen_width;

		if (r.height > screen_height)
			r.height=screen_height;

		r.x=coord_t::truncate((screen_width-r.width)/2);
		r.y=coord_t::truncate((screen_height-r.height)/2);
	}
};

#if 0
{
#endif
}

static ref<main_windowObj::handlerObj>
create_splash_window_handler(const std::reference_wrapper<const screen> &me,
			     const splash_window_config &config,
			     const std::reference_wrapper<const color_arg>
			     & background_color,
			     std::optional<border_arg> &main_window_border)
{
	main_window_handler_constructor_params
		main_params{me, "splash,normal", "above",
			    background_color,
			    config.appearance,
			    true,
	};

	main_window_border=config.border;

	return ref<splash_window_handlerObj>::create(main_params,
						     std::nullopt,
						     config.name);
}

static ref<main_windowObj::handlerObj>
create_splash_window_handler(const std::reference_wrapper<const screen> &me,
			     const transparent_splash_window_config &config,
			     std::optional<border_arg> &main_window_border,
			     std::optional<color_arg> &inner_background_color)

{
	if (me.get()->impl->toplevelwindow_pictformat->alpha_depth == 0)
	{
		const splash_window_config &nonalpha_config=config;

		return create_splash_window_handler
			(me, nonalpha_config,
			 nonalpha_config.appearance->background_color,
			 main_window_border);
	}

	color_arg transparent_background_color{transparent};

	auto handler=create_splash_window_handler(me, config,
						  transparent_background_color,
						  main_window_border);

	main_window_border=config.border;
	inner_background_color=config.appearance->background_color;

	return handler;
}

///////////////////////////////////////////////////////////////////////////

main_window screenObj
::do_create_mainwindow(const main_window_config_t &config,
		       const function<main_window_creator_t> &f,
		       const new_layoutmanager &layout_factory)
{
	// Keep a batch queue in scope for the duration of the creation,
	// so everything gets buffered up.

	auto queue=connref->impl->thread->get_batch_queue();

	auto me=screen{this};

	std::optional<border_arg> main_window_border;
	std::optional<color_arg> inner_background_color;

	auto [handler, config_ptr]
		=std::visit(visitor{
			[&](const main_window_config &std_config)
			{
				std::optional<rectangle> suggested_position;

				if (std_config.window_info)
					suggested_position=
						std_config.window_info
						->coordinates;

				main_window_handler_constructor_params
					main_params{me, "normal", "",
						    std_config.appearance
						    ->background_color,
						    std_config.appearance,
						    false,
				};

				return std::tuple
				{ref<main_windowObj::handlerObj>
						::create(main_params,
							 suggested_position,
							 std_config.name),
						&std_config};
			},
			[&](const splash_window_config &splash_config)
			{
				auto ret=create_splash_window_handler
					(me, splash_config,
					 splash_config.appearance
					 ->background_color,
					 main_window_border);

				return std::tuple
				{ret, &static_cast<const main_window_config &>
						(splash_config)};
			},
			[&](const transparent_splash_window_config
			    &splash_config)
			{
				auto ret=create_splash_window_handler
					(me, splash_config,
					 main_window_border,
					 inner_background_color);

				return std::tuple
				{ret, &static_cast<const main_window_config &>
						(splash_config)};
			}},
		config);

	auto [window_impl, lm]=create_main_window_impl
		(handler, main_window_border,
		 inner_background_color,
		 inner_background_color,
		 *config_ptr,
		 layout_factory,
		 [](const auto &args)
		 {
			 return ref<main_windowObj::implObj>::create(args);
		 });

	auto mw=ptrref_base::objfactory<main_window>
		::create(window_impl, lm->impl);

	f(mw);

	return mw;
}

layout_impl main_windowObj::get_layout_impl() const
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
	return functionref<void (const factory &)>
		([=](const factory &f)
		 {
			 f->create_image(icon);
		 });
}

// Return a factory that creates a button in a theme template

functionref<void (const factory &)>
dialog_ok_button(const text_param &label,
		 buttonptr &ret,
		 char32_t key)
{
	return [label, &ret, key](const factory &f)
	       {
		       ret=f->create_button
			       (label,
				{
					default_button(),
						shortcut{key}
				});
	       };
}

static inline functionref<void (const factory &)>
dialog_ok2_button(const text_param &label,
		  buttonptr &ret,
		  const shortcut &sc)
{
	return [label, &ret, &sc](const factory &f)
	       {
		       ret=f->create_button
			       (label,
				{
				 normal_button(),
				 sc
				});
	};
}

functionref<void (const factory &)>
dialog_cancel_button(const text_param &label,
		     buttonptr &ret,
		     char32_t key)
{
	return [label, &ret, key](const factory &f)
	       {
		       ret=f->create_button
			       (label,
				{
				       normal_button(),
					       shortcut{key}
				});
	};
}

// Hide a theme-generated dialog, then invoke a callback action.

static void hide_and_invoke(ONLY IN_THREAD,
			    const main_windowptr &parent_window,
			    const main_windowptr &dialog_window,
			    const ref<generic_windowObj::handlerObj>
			    &parent_handler,
			    const ok_cancel_dialog_callback_t &action)
{
	if (dialog_window)
		dialog_window->hide();

	// Make sure that the callback gets the parent display
	// element's main window.

	busy_impl yes_i_am{parent_handler};

	action(IN_THREAD, ok_cancel_callback_args{parent_window, yes_i_am});
}

// When a theme dialog's button is pressed, hide and invoke the callback.

void hide_and_invoke_when_activated(const main_window &parent_window,
				    const dialog &d,
				    const hotspot &button,
				    const ok_cancel_dialog_callback_t &action)
{
	button->on_activate([parent_window=weakptr<main_windowptr>
			{parent_window},
			     dialog_window=weakptr<main_windowptr>
			{d->dialog_window},
			     parent_handler=parent_window->impl->handler,
			     action]
			    (ONLY IN_THREAD,
			     const auto &ignore, const busy &yes_i_am)
			    {
				    hide_and_invoke(IN_THREAD,
						    parent_window.getptr(),
						    dialog_window.getptr(),
						    parent_handler,
						    action);
			    });
}

// When a theme dialog's close button is pressed, hide and invoke the callback.

void hide_and_invoke_when_closed(const main_window &parent_window,
				 const dialog &d,
				 const ok_cancel_dialog_callback_t &action)
{
	d->dialog_window->on_delete([parent_window=weakptr<main_windowptr>
			{parent_window},
				     dialog_window=weakptr<main_windowptr>
			{d->dialog_window},
				     parent_handler=
				     parent_window->impl->handler,
				     action]
				    (ONLY IN_THREAD,
				     const busy &yes_i_am)
				    {
					    hide_and_invoke
						    (IN_THREAD,
						     parent_window.getptr(),
						     dialog_window.getptr(),
						     parent_handler,
						     action);
				    });
}

dialog main_windowObj
::create_ok_dialog(const standard_dialog_args &args,
		   const std::string &icon,
		   const functionref<void (const factory &)>
		   &content_factory,
		   const ok_cancel_dialog_callback_t &ok_action)
{
	return create_ok_dialog(args,
				icon, content_factory, ok_action,
				stop_message_config::default_ok_label());
}

dialog main_windowObj
::create_ok_dialog(const standard_dialog_args &args,
		   const std::string &icon,
		   const functionref<void (const factory &)>
		   &content_factory,
		   const ok_cancel_dialog_callback_t &ok_action,
		   const text_param &ok_label)
{
	buttonptr ok_button;

	auto d=create_dialog
		(create_dialog_args{args},
		 [&]
		 (const dialog &d)
		 {
			 uielements tmpl
				 {
				  {
				   {"icon", icon_element(icon)},
				   {"message", content_factory},

				   {"ok", dialog_ok_button(ok_label,
							   ok_button, '\e')}
				  }};

			 d->dialog_window->generate("ok-dialog", tmpl);
		 });

	// Ok button's shortcut is ESC. autofocus() th einput on the Ok
	// button. "Enter" will activate the "Ok" button, closing the popup.
	// Esc will activate the button via the shortcut, also closing
	// the popup.

	ok_button->autofocus(true);

	auto me=ref{this};
	hide_and_invoke_when_activated(me, d, ok_button, ok_action);
	hide_and_invoke_when_closed(me, d, ok_action);

	return d;
}

void main_windowObj::exception_message(const exception &e)
{
	stop_message_config conf;

	// Exception messages can be very wide, wrap them.
	conf.widthmm=100;
	exception_message(e, conf);
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
	auto autodestroy=destroy_when_closed(config.dialog_id);

	auto d=create_ok_dialog(config,
				"stop",
				[&]
				(const auto &f)
				{
					label_config lconfig;

					lconfig.widthmm=config.widthmm;
					f->create_label(msg, lconfig);
				},
				[autodestroy, cb=config.acknowledged_callback]
				(ONLY IN_THREAD, const auto &args)
				{
					autodestroy(IN_THREAD, args);
					if (cb)
						cb(IN_THREAD);
				},
				config.ok_label);

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

stop_message_config::stop_message_config()
	: standard_dialog_args{"stop_message@libcxx.com"}
{
	modal=true;
	urgent=true;
}

stop_message_config::~stop_message_config()=default;

void main_windowObj::alert_message(const text_param &msg)
{
	alert_message(msg, {});
}

void main_windowObj::alert_message(const text_param &msg,
				   const alert_message_config &config)
{
	auto autodestroy=destroy_when_closed(config.dialog_id);

	auto d=create_ok_dialog(config,
				"alert",
				[&]
				(const auto &f)
				{
					f->create_label(msg);
				},
				[autodestroy, cb=config.acknowledged_callback]
				(ONLY IN_THREAD, const auto &args)
				{
					autodestroy(IN_THREAD, args);
					if (cb)
						cb(IN_THREAD);
				},
				config.ok_label);

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

alert_message_config::alert_message_config()
	: standard_dialog_args{"alert_message@libcxx.com"}
{
	modal=true;
	urgent=true;
}

alert_message_config::~alert_message_config()=default;

dialog main_windowObj
::create_ok_cancel_dialog(const standard_dialog_args &args,
			  const std::string &icon,
			  const functionref<void (const factory &)>
			  &content_factory,
			  const ok_cancel_dialog_callback_t &ok_action,
			  const ok_cancel_dialog_callback_t &cancel_action)
{
	return create_ok_cancel_dialog(args,
				       icon, content_factory, ok_action,
				       cancel_action,
				       _("Ok"),
				       _("Cancel"));
}

dialog main_windowObj
::create_ok_cancel_dialog(const standard_dialog_args &args,
			  const std::string &icon,
			  const functionref<void (const factory &)>
			  &content_factory,
			  const ok_cancel_dialog_callback_t &ok_action,
			  const ok_cancel_dialog_callback_t &cancel_action,
			  const text_param &ok_label,
			  const text_param &cancel_label)
{
	buttonptr ok_button;
	buttonptr cancel_button;

	auto d=create_dialog
		(create_dialog_args{args},
		 [&]
		 (const dialog &d)
		 {
			 uielements tmpl
				 {
				  {
				   {"icon", icon_element(icon)},
				   {"message", content_factory},
				   {"ok", dialog_ok_button(ok_label,
							   ok_button,
							   '\n')},
				   {"cancel", dialog_cancel_button
				    (cancel_label,
				     cancel_button, '\e')}
				  }
				 };

			 d->dialog_window->generate("ok-cancel-dialog", tmpl);
		 });

	auto me=ref{this};

	hide_and_invoke_when_activated(me, d, ok_button, ok_action);
	hide_and_invoke_when_activated(me, d, cancel_button, cancel_action);
	hide_and_invoke_when_closed(me, d, cancel_action);

	return d;
}

dialog main_windowObj
::create_ok2_cancel_dialog(const standard_dialog_args &args,
			   const std::string &icon,
			   const functionref<void (const factory &)>
			   &content_factory,
			   const ok_cancel_dialog_callback_t &ok_action,
			   const ok_cancel_dialog_callback_t &ok2_action,
			   const ok_cancel_dialog_callback_t &cancel_action,
			   const text_param &ok_label,
			   const text_param &ok2_label,
			   const text_param &cancel_label)
{
	return create_ok2_cancel_dialog(args, icon, content_factory,
					ok_action, ok2_action, cancel_action,
					ok_label, ok2_label, cancel_label,
					{});
}

dialog main_windowObj
::create_ok2_cancel_dialog(const standard_dialog_args &args,
			   const std::string &icon,
			   const functionref<void (const factory &)>
			   &content_factory,
			   const ok_cancel_dialog_callback_t &ok_action,
			   const ok_cancel_dialog_callback_t &ok2_action,
			   const ok_cancel_dialog_callback_t &cancel_action,
			   const text_param &ok_label,
			   const text_param &ok2_label,
			   const text_param &cancel_label,
			   const shortcut &ok2_shortcut)
{
	buttonptr ok_button;
	buttonptr ok2_button;
	buttonptr cancel_button;

	auto d=create_dialog
		(create_dialog_args{args},
		 [&]
		 (const dialog &d)
		 {
			 uielements tmpl
				 {
				  {
				   {"icon", icon_element(icon)},
				   {"message", content_factory},
				   {"ok", dialog_ok_button(ok_label,
							   ok_button,
							   '\n')},
				   {"ok2", dialog_ok2_button(ok2_label,
							     ok2_button,
							     ok2_shortcut)},
				   {"cancel", dialog_cancel_button
				    (cancel_label,
				     cancel_button, '\e')}
				  }
				 };

			 d->dialog_window->generate("ok2-cancel-dialog", tmpl);
		 });

	auto me=ref{this};

	hide_and_invoke_when_activated(me, d, ok_button, ok_action);
	hide_and_invoke_when_activated(me, d, ok2_button, ok2_action);
	hide_and_invoke_when_activated(me, d, cancel_button, cancel_action);
	hide_and_invoke_when_closed(me, d, cancel_action);

	return d;
}



input_dialog main_windowObj
::create_input_dialog(const standard_dialog_args &args,
		      const std::string &icon,
		      const functionref<void (const factory &)>
		      &label_factory,
		      const text_param &initial_text,
		      const input_field_config &config,
		      const functionref<void (THREAD_CALLBACK,
					      const input_dialog_ok_args &)>
		      &ok_action,
		      const ok_cancel_dialog_callback_t &cancel_action)
{
	return create_input_dialog(args,
				   icon, label_factory,
				   initial_text, config,
				   ok_action,
				   cancel_action,
				   _("Ok"),
				   _("Cancel"));
}

input_dialog main_windowObj
::create_input_dialog(const standard_dialog_args &args,
		      const std::string &icon,
		      const functionref<void (const factory &)>
		      &label_factory,
		      const text_param &initial_text,
		      const input_field_config &config,
		      const functionref<void (THREAD_CALLBACK,
					      const input_dialog_ok_args &)>
		      &ok_action,
		      const ok_cancel_dialog_callback_t &cancel_action,
		      const text_param &ok_label,
		      const text_param &cancel_label)
{
	buttonptr ok_button;
	buttonptr cancel_button;
	input_fieldptr field;

	input_dialogptr new_input_dialog;

	auto me=ref{this};

	auto d=create_custom_dialog
		(create_dialog_args{args},
		 [&]
		 (const auto &args)
		 {
			 uielements tmpl
				 {
				  {
				   {"icon", icon_element(icon)},
				   {"label", label_factory},
				   {"input",
				    [&](const auto &factory)
				    {
					    field=factory->create_input_field
						    (initial_text,
						     config);
				    }},
				   {"ok", dialog_ok_button(ok_label,
							   ok_button,
							   '\n')},
				   {"cancel", dialog_cancel_button
				    (cancel_label,
				     cancel_button, '\e')}
				  }
				 };

			 args.dialog_window->generate("input-dialog", tmpl);

			 auto id=input_dialog::create(args, field, ok_button);

			 new_input_dialog=id;

			 return id;

		 });

	hide_and_invoke_when_activated
		(me, d, ok_button,

		 // Although the input field can be officially captured
		 // strongly here, this creates an internal strong reference
		 // that might interfere with any additional references
		 // our user may want to create.
		 //
		 // The file dialog, for example, creates an on_change()
		 // callback on the input field that captures the "ok"
		 // button, and enables or disables it accordingly. That
		 // would create a circular reference with this one.
		 //
		 // So, we'll capture the input field weakly, here.

		 [field=weakptr<input_fieldptr>{field}, ok_action,
		  cancel_action]
		 (ONLY IN_THREAD,
		  const auto &args)
		 {
			 // We expect the weakly-captured input field to
			 // still exist. But, if not, we'll just interpret
			 // this as a "cancel" action, instead.

			 auto f=field.getptr();

			 if (!f)
				 cancel_action(IN_THREAD, args);
			 else
				 ok_action(IN_THREAD, input_dialog_ok_args{
						 args, f});
		 });
	hide_and_invoke_when_activated(me, d, cancel_button, cancel_action);
	hide_and_invoke_when_closed(me, d, cancel_action);

	return new_input_dialog;
}

LIBCXXW_NAMESPACE_END
