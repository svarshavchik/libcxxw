/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/layoutmanager.H"
#include "x/w/impl/container.H"
#include "x/w/uigenerators.H"
#include "xid_t.H"
#include "generic_window_handler.H"
#include "connection_thread.H"
#include "batch_queue.H"
#include "screen.H"
#include "defaulttheme.H"
#include "messages.H"

LIBCXXW_NAMESPACE_START

layoutmanagerObj::layoutmanagerObj(const ref<implObj> &impl)
	: impl{impl},
	  queue{impl->layout_container_impl->get_window_handler().thread()
			  ->get_batch_queue()}

{
}

// When the public object drops off, trigger layout recalculation.

layoutmanagerObj::~layoutmanagerObj()
{
	impl->needs_recalculation(queue);
}

void layoutmanagerObj::generate(const std::string_view &name,
				uielements &elements)
{
	generate(name,
		 impl->layout_container_impl
		 ->container_element_impl().get_screen()
		 ->impl->current_theme.get(),
		 elements);
}

void layoutmanagerObj::generate(const std::string_view &name,
				const const_uigenerators &generators,
				uielements &elements)
{
	throw EXCEPTION(_("This layout manager does not implement generate()"));
}

LIBCXXW_NAMESPACE_END
