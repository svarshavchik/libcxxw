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
#include "x/w/picture.H"
#include "x/w/screen.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/menubarlayoutmanager.H"
#include "layoutmanager.H"
#include "peephole/peephole_toplevel.H"
#include "peephole/peepholed_toplevel_element.H"
#include "peepholed_toplevel_main_window.H"
#include "peepholed_toplevel_main_window_impl.H"
#include "menu/menubarlayoutmanager_impl.H"
#include "menu/menubar_container_impl.H"
#include "container_element.H"
#include "always_visible.H"

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::main_windowObj);

LIBCXXW_NAMESPACE_START

main_windowObj::main_windowObj(const ref<implObj> &impl,
			       const ref<layoutmanagerObj::implObj> &lm)
	: generic_windowObj(impl, lm),
	  impl(impl)
{
}

main_windowObj::~main_windowObj()=default;

void main_windowObj::on_delete(const std::function<void ()> &callback)
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

LIBCXXW_NAMESPACE_END
