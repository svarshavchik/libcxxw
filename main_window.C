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
#include "layoutmanager.H"
#include "peephole/peephole_toplevel.H"
#include "peephole/peepholed_toplevel_element.H"
#include "peepholed_toplevel_main_window.H"
#include "peepholed_toplevel_main_window_impl.H"

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

main_window screenObj
::do_create_mainwindow(const function<main_window_creator_t> &f,
		       const new_layoutmanager &layout_factory)
{
	auto queue=connref->impl->thread->get_batch_queue();

	auto handler=ref<main_windowObj::handlerObj>
		::create(connref->impl->thread, screen(this));

	handler->set_window_type("normal");

	peepholed_toplevel_main_windowptr real_container;

	// Create a top level peephole in the main_window.

	auto lm=create_peephole_toplevel
		(handler,
		 nullptr,
		 [&]
		 (const ref<containerObj::implObj> &parent)
		 {
			 // A toplevel_container_implObj is in the peephole,
			 // and that's the container that will use the
			 // requested layout_factory.

			 auto impl=ref<peepholed_toplevel_main_windowObj::implObj>
			 ::create(parent);

			 auto c=peepholed_toplevel_main_window
			 ::create(impl, layout_factory);

			 real_container=c;

			 c->show();
			 return c;
		 });

	auto window_impl=ref<main_windowObj::implObj>
		::create(handler, real_container);

	auto mw=ptrrefBase::objfactory<main_window>
		::create(window_impl, lm->impl);

	f(mw);

	return mw;
}

ref<layoutmanagerObj::implObj> main_windowObj::get_layout_impl() const
{
	return impl->peepholed_container->get_layout_impl();
}

LIBCXXW_NAMESPACE_END
