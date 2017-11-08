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
#include "dialog_impl.H"
#include "dialog_handler.H"
#include "always_visible.H"
#include <x/weakcapture.H>

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::main_windowObj);

LIBCXXW_NAMESPACE_START

main_windowObj::main_windowObj(const ref<implObj> &impl,
			       const ref<layoutmanagerObj::implObj> &lm)
	: generic_windowObj(impl, lm),
	  impl(impl)
{
}

main_windowObj::~main_windowObj()=default;

void main_windowObj::on_delete(const std::function<void (const busy &)
			       > &callback)
{
	impl->on_delete(callback);
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

main_window screenObj
::do_create_mainwindow(const function<main_window_creator_t> &f)
{
	return do_create_mainwindow(f, new_gridlayoutmanager{});
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

	menu_and_app->show();

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
	app->show();

	menu_and_app_container=menu_and_app;
	menubar_container=menubar;
	app_container=app;

	return menu_and_app;
}

std::tuple<ref<main_windowObj::implObj>, layoutmanager>
do_create_main_window_impl(const ref<main_windowObj::handlerObj> &handler,
			   const new_layoutmanager &layout_factory,
			   const function<make_window_impl_factory_t> &factory)
{
	containerptr menu_and_app_container,
		menubar_container, app_container;

	// Create a top level peephole in the main_window.

	auto lm=create_peephole_toplevel
		(handler,
		 nullptr,
		 peephole_style(),
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
::do_create_mainwindow(const function<main_window_creator_t> &f,
		       const new_layoutmanager &layout_factory)
{
	// Keep a batch queue in scope for the duration of the creation,
	// so everything gets buffered up.

	auto queue=connref->impl->thread->get_batch_queue();

	auto handler=ref<main_windowObj::handlerObj>
		::create(connref->impl->thread, screen(this),
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

static auto icon_element(const std::string_view &icon)
{
	return std::function<void (const gridfactory &)>
		([=](const gridfactory &f)
		 {
			 f->create_image_mm(icon);
		 });
}

// Return a factory that creates a button in a theme template

std::function<void (const gridfactory &)>
dialog_ok_button(const text_param &label,
		 buttonptr &ret,
		 char32_t key)
{
	return [label, &ret, key](const gridfactory &f)
	{
		ret=f->create_special_button_with_label(label, {key});
	};
}

std::function<void (const gridfactory &)>
dialog_cancel_button(const text_param &label,
		     buttonptr &ret,
		     char32_t key)
{
	return [label, &ret, key](const gridfactory &f)
	{
		ret=f->create_normal_button_with_label(label, {key});
	};
}

std::function<void (const gridfactory &)> dialog_filler()
{
	return [](const gridfactory &f)
	{
		f->create_canvas();
	};
}

// Hide a theme-generated dialog, then invoke a callback action.

void hide_and_invoke(const captured_dialog_t &d,
		     const busy &yes_i_am,
		     const std::function
		     <void (const busy &)>  &action)
{
	d.get([&]
	      (const auto &d)
	      {
		      d->dialog_window->hide();
		      action(yes_i_am);
	      });
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
				    hide_and_invoke(d, yes_i_am, action);
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
					    hide_and_invoke(d, yes_i_am,
							    action);
				    });
}

dialog main_windowObj
::create_ok_dialog(const std::string_view &dialog_id,
		   const std::string_view &icon,
		   const std::function<void (const gridfactory &)>
		   &content_factory,
		   const std::function<void (const busy &)>
		   &ok_action,
		   bool modal)
{
	return create_ok_dialog(dialog_id,
				icon, content_factory, ok_action,
				_("Ok"), modal);
}

dialog main_windowObj
::create_ok_dialog(const std::string_view &dialog_id,
		   const std::string_view &icon,
		   const std::function<void (const gridfactory &)>
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
				 {"ok", dialog_ok_button(ok_label, ok_button, '\n')}
			 });
		 },
		 modal);

	hide_and_invoke_when_activated(d, ok_button, ok_action);
	hide_and_invoke_when_closed(d, ok_action);

	return d;
}

dialog main_windowObj
::create_ok_cancel_dialog(const std::string_view &dialog_id,
			  const std::string_view &icon,
			  const std::function<void (const gridfactory &)>
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
			  const std::string_view &icon,
			  const std::function<void (const gridfactory &)>
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
			 });
		 },
		 modal);

	hide_and_invoke_when_activated(d, ok_button, ok_action);
	hide_and_invoke_when_activated(d, cancel_button, cancel_action);
	hide_and_invoke_when_closed(d, cancel_action);

	return d;
}

input_dialog main_windowObj
::create_input_dialog(const std::string_view &dialog_id,
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
		      const std::string_view &icon,
		      const std::function<void (const gridfactory &)>
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
			 });

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

LIBCXXW_NAMESPACE_END
