/*
** Copyright 2017-2020 Double Precision, Inc.
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
	  modified{false},
	  queue{impl->layout_container_impl->get_window_handler().thread()
			  ->get_batch_queue()}

{
}

#if 0
void why(const ref<layoutmanagerObj::implObj> &impl)
{
	std::cout << impl->objname() << " WAS NOT CLASSIFIED!"
		  << std::endl;

	if (getenv("DEBUG_MOD"))
		abort();
}

#endif

// When the public object drops off, trigger layout recalculation.

layoutmanagerObj::~layoutmanagerObj()
{
#if 0
	if (!modified && !notmodified_flag)
		why(impl);
#else
	if (modified)
		impl->needs_recalculation(queue);
#endif
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
