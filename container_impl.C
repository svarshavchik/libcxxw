/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "container.H"
#include "x/w/container.H"
#include "layoutmanager.H"
#include "child_element.H"
#include "connection_thread.H"
#include "draw_info.H"

LIBCXXW_NAMESPACE_START

containerObj::implObj::implObj()=default;

containerObj::implObj::~implObj()=default;

void containerObj::implObj
::install_layoutmanager(const ref<layoutmanagerObj::implObj> &impl)
{
	layoutmanager_ptr_t::lock lock(layoutmanager_ptr);

	*lock=impl;
}


void containerObj::implObj::uninstall_layoutmanager()
{
	layoutmanager_ptr_t::lock lock(layoutmanager_ptr);

	*lock=ptr<layoutmanagerObj::implObj>();
}

void containerObj::implObj::do_draw(IN_THREAD_ONLY,
				    const draw_info &di,
				    const rectangle_set &areas)
{
	invoke_layoutmanager([&, this]
			     (const auto &manager)
			     {
				     auto drawn=manager->draw(IN_THREAD,
							      di,
							      areas);

				     this->get_element_impl()
					     .clear_to_color(IN_THREAD,
							     di,
							     subtract
							     (areas, drawn));
			     });
}

void containerObj::implObj::inherited_visibility_updated(IN_THREAD_ONLY,
							 bool flag)
{
	// When the container gets hidden, the child elements are hidden
	// first. When the container gets shown, the child elements are
	// shown after the container.

	if (!flag)
		propagate_inherited_visibility(IN_THREAD, flag);

	get_element_impl().do_inherited_visibility_updated(IN_THREAD, flag);

	if (flag)
		propagate_inherited_visibility(IN_THREAD, flag);
}

// If this container's child elements have actual_visibility set (they
// were explicitly shown, then hiding or showing the container changes
// the child elements' inherited visibility.
//
// Get the child elements from the layout manager, and invoke their
// inherited_visibility_updated().

void containerObj::implObj::propagate_inherited_visibility(IN_THREAD_ONLY,
							   bool flag)
{
	invoke_layoutmanager
		([&]
		 (const auto &manager)
		 {
			 manager->for_each_child
				 (IN_THREAD,
				  [&]
				  (const child_element &e)
				  {
					  if (!e->data(IN_THREAD)
					      .actual_visibility)
						  return;

					  e->inherited_visibility_updated
						  (IN_THREAD,
						   flag);
				  });
		 });
}

LIBCXXW_NAMESPACE_END
