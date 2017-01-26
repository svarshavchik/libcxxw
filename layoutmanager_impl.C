/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "layoutmanager.H"
#include "container.H"
#include "generic_window_handler.H"
#include "xid_t.H"
#include "connection_thread.H"
#include "draw_info.H"
#include "child_element.H"
#include "catch_exceptions.H"
#include "batch_queue.H"

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::layoutmanagerObj::implObj);

LIBCXXW_NAMESPACE_START

layoutmanagerObj::implObj
::implObj(const ref<containerObj::implObj> &container_impl)
	: container_impl(container_impl)
{
}

layoutmanagerObj::implObj::~implObj()=default;

void layoutmanagerObj::implObj::needs_recalculation(const batch_queue &queue)
{
	queue->run_as(RUN_AS,
		      [me=ref<layoutmanagerObj::implObj>(this)]
		      (IN_THREAD_ONLY)
		      {
			      me->needs_recalculation(IN_THREAD);
		      });
}

void layoutmanagerObj::implObj::child_metrics_updated(IN_THREAD_ONLY)
{
	needs_recalculation(IN_THREAD);
}

void layoutmanagerObj::implObj::needs_recalculation(IN_THREAD_ONLY)
{
	(*IN_THREAD->containers_2_recalculate(IN_THREAD))
		[container_impl->get_element_impl().nesting_level]
		.insert(container_impl);
}

rectangle_set layoutmanagerObj::implObj::draw(IN_THREAD_ONLY,
					      const draw_info &di,
					      const rectangle_set &areas)
{
	rectangle_set drawn;

	for_each_child(IN_THREAD,
		       [&]
		       (const element &c)
		       {
			       // Skip invisible elements. Although the
			       // element's draw() will not do anything,
			       // we don't want to include its area in the
			       // drawn set.

			       if (!c->impl->data(IN_THREAD).inherited_visibility)
				       return;

			       const auto &current_position=
				       c->impl->data(IN_THREAD).current_position;
			       drawn.insert(current_position);

			       // Create an updated draw_info for this
			       // child element.

			       draw_info child_di=di;

			       // Adjust the viewport to the child element.

			       child_di.viewport.x =
				       (coord_squared_t::value_type)
				       (child_di.viewport.x +
					current_position.x);
			       child_di.viewport.y =
				       (coord_squared_t::value_type)
				       (child_di.viewport.y +
					current_position.y);
			       child_di.viewport.width=current_position.width;
			       child_di.viewport.height=current_position.height;

			       // Take which areas we were told to draw.

			       // Intersect it with the child element's
			       // viewport, that's what the child has to draw.
			       rectangle_set adjust_drawn_area;

			       adjust_drawn_area.insert(current_position);

			       adjust_drawn_area=
				       intersect(areas, adjust_drawn_area,
						 // Once the intersection is
						 // computed, we adjust its
						 // (0, 0) coordinates
						 // to be relative to the
						 // child element. That's what
						 // it expects its drawn area
						 // to be.
						 current_position.x * -1,
						 current_position.y * -1
						 );

			       try {
				       c->impl->prepare_draw_info(IN_THREAD,
								  child_di);

				       c->impl->draw(IN_THREAD, child_di,
						     adjust_drawn_area);
			       } CATCH_EXCEPTIONS;
		       });
	return drawn;
}

void layoutmanagerObj::implObj::current_position_updated(IN_THREAD_ONLY)
{
	container_impl->get_element_impl().current_position_updated(IN_THREAD);
}

LIBCXXW_NAMESPACE_END
