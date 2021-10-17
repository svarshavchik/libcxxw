/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/layoutmanager.H"
#include "x/w/impl/container.H"
#include "generic_window_handler.H"
#include "xid_t.H"
#include "connection_thread.H"
#include "container_impl.H"
#include "run_as.H"
#include "x/w/impl/draw_info.H"
#include "x/w/impl/child_element.H"
#include "catch_exceptions.H"
#include "batch_queue.H"
#include "catch_exceptions.H"
#include <x/functionalrefptr.H>

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::layoutmanagerObj::implObj);

LIBCXXW_NAMESPACE_START

layoutmanagerObj::implObj
::implObj(const container_impl &layout_container_impl)
	: layout_container_impl{layout_container_impl}
{
}

layoutmanagerObj::implObj::~implObj()=default;

void layoutmanagerObj::implObj::uninstalling(ONLY IN_THREAD)
{
}

void layoutmanagerObj::implObj::run_as(const functionref<void (ONLY IN_THREAD)>
				       &f)
{
	auto e=ref(&layout_container_impl->container_element_impl());

	e->get_window_handler().thread()
		->run_as([e, f]
			 (ONLY IN_THREAD)
			 {
				 try {
					 f(IN_THREAD);
				 } REPORT_EXCEPTIONS(e);
			 });
}

void layoutmanagerObj::implObj::needs_recalculation()
{
	needs_recalculation(get_element_impl().THREAD->get_batch_queue());
}

void layoutmanagerObj::implObj::needs_recalculation(const batch_queue &queue)
{
	// This is called from the public object. Existance of a public
	// object blocks all container recalculation and update processing.
	//
	// We must mark that this container needs_recalculation. For the
	// currect sequence of events to occur, the queue object must
	// continue to exist until this container is marked for
	// recalculation, so we capture it by value, and note that
	// this container needs_recalculation IN_THREAD; and only then
	// the queue object goes out of scope and gets destroyed.

	run_as([queue,me=ref{this}]
	       (ONLY IN_THREAD)
	       {
		       me->needs_recalculation(IN_THREAD);
	       });
}

void layoutmanagerObj::implObj::child_metrics_updated(ONLY IN_THREAD)
{
	needs_recalculation(IN_THREAD);
}

void layoutmanagerObj::implObj::needs_recalculation(ONLY IN_THREAD)
{
	layout_container_impl->needs_recalculation(IN_THREAD);
}

void layoutmanagerObj::implObj::process_same_position(ONLY IN_THREAD,
						      const rectangle &position)
{
}

void layoutmanagerObj::implObj
::child_background_color_changed(ONLY IN_THREAD, const element_impl &child)
{
}

void layoutmanagerObj::implObj
::requested_child_visibility_changed(ONLY IN_THREAD,
				     const element_impl &child,
				     bool flag)
{
}

void layoutmanagerObj::implObj
::inherited_child_visibility_changed(ONLY IN_THREAD, const element_impl &child,
				     inherited_visibility_info &info)
{
}

rectangle layoutmanagerObj::implObj::padded_position(ONLY IN_THREAD,
						     const element_impl &e_impl)
{
	return e_impl->data(IN_THREAD).current_position;
}

elementObj::implObj &layoutmanagerObj::implObj::get_element_impl()
{
	return layout_container_impl->container_element_impl();
}

void layoutmanagerObj::implObj::initialize(ONLY IN_THREAD)
{
}

void layoutmanagerObj::implObj::theme_updated(ONLY IN_THREAD,
					      const const_defaulttheme &new_theme)
{
}

void layoutmanagerObj::implObj::ensure_visibility(ONLY IN_THREAD,
						  elementObj::implObj &e,
						  const rectangle &r)
{
}

void layoutmanagerObj::implObj::request_visibility_recursive(ONLY IN_THREAD,
							     bool flag)
{
	for_each_child
		(IN_THREAD,
		 [&]
		 (const element &e)
		 {
			 e->impl->request_visibility_recursive
				 (IN_THREAD, flag);
		 });
}

void layoutmanagerObj::implObj::do_draw(ONLY IN_THREAD,
					const draw_info &di,
					clip_region_set &clip,
					rectarea &drawn_areas)
{
	auto &element_impl=layout_container_impl->container_element_impl();

	drawn_areas.reserve(drawn_areas.size()+num_children(IN_THREAD));

	for_each_child
		(IN_THREAD,
		 [&]
		 (const element &e)
		 {
			 container_clear_padding(IN_THREAD,
						 element_impl,
						 *this,
						 e->impl,
						 di, clip,
						 drawn_areas);
		 });
}

void layoutmanagerObj::implObj::save(ONLY IN_THREAD,
				     const screen_positions &pos)
{
}

void layoutmanagerObj::implObj::process_finalized_position(ONLY IN_THREAD)
{
}

LIBCXXW_NAMESPACE_END
