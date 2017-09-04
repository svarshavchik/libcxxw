/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/dialog.H"
#include "main_window.H"
#include "dialog_impl.H"
#include "dialog_handler.H"
#include "connection_thread.H"
#include "batch_queue.H"

LIBCXXW_NAMESPACE_START

dialogObj::dialogObj(const main_window &parent,
		     const ref<implObj> &impl,
		     const ref<layoutmanagerObj::implObj> &lm)
	: main_windowObj(impl, lm), impl(impl), parent(parent)
{
}

dialogObj::~dialogObj()=default;

///////////////////////////////////////////////////////////////////////////

dialog main_windowObj::do_create_dialog(const function<dialog_creator_t>
					&creator,
					const new_layoutmanager &layout_factory,
					bool modal)
{
	// Keep a batch queue in scope for the duration of the creation,
	// so everything gets buffered up.

	auto s=get_screen();

	auto queue=s->connref->impl->thread->get_batch_queue();

	auto handler=ref<dialogObj::handlerObj>
		::create(s->connref->impl->thread,
			 impl->handler, "dialog_background", modal);

	handler->set_window_type("dialog,normal");

	ptr<dialogObj::implObj> dialog_impl;

	auto lm=std::get<1>(create_main_window_impl
			    (handler, layout_factory,
			     [&,this](const auto &args)
		{
			auto impl=ref<dialogObj::implObj>::create
			(handler, this->impl->handler, args);

			dialog_impl=impl;

			return impl;
		}));

	auto d=dialog::create(main_window(this), dialog_impl, lm->impl);

	creator(d);

	return d;
}

LIBCXXW_NAMESPACE_END
