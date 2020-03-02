/*
** Copyright 2018-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "toolboxlayoutmanager/toolboxlayoutmanager_impl.H"
#include "x/w/element.H"
#include "x/w/impl/container.H"
#include "generic_window_handler.H"
#include "messages.H"

LIBCXXW_NAMESPACE_START

toolboxlayoutmanagerObj::implObj
::implObj(const container_impl &layout_container_impl,
	  const new_toolboxlayoutmanager &ntlm)
	: layoutmanagerObj::implObj{layout_container_impl},
	  default_width{ntlm.default_width}
{
	if (default_width == 0)
		throw EXCEPTION(_("Default number of columns cannot be 0"));
}

layout_impl new_toolboxlayoutmanager::create(const container_impl &c) const
{
	auto impl=ref<toolboxlayoutmanagerObj::implObj>::create(c, *this);

	// Register the new hint increments.

	c->container_element_impl().get_window_handler()
		.install_size_hints(impl);

	return impl;
}

toolboxlayoutmanagerObj::implObj::~implObj()
{
	auto wh=ref{&layout_container_impl->container_element_impl()
		    .get_window_handler()};

	run_as([wh]
	       (ONLY IN_THREAD)
	       {
		       wh->size_hints_updated(IN_THREAD);
	       });
}

void toolboxlayoutmanagerObj::implObj
::do_for_each_child(ONLY IN_THREAD,
		    const function<void (const element &e)> &callback)
{
	toolbox_info_t::lock lock{info};

	for (const auto &c:lock->elements)
		callback(c);
}

size_t toolboxlayoutmanagerObj::implObj::num_children(ONLY IN_THREAD)
{
	toolbox_info_t::lock lock{info};

	return lock->elements.size();
}

layoutmanager toolboxlayoutmanagerObj::implObj::create_public_object()
{
	return toolboxlayoutmanager::create(ref{this});
}

void toolboxlayoutmanagerObj::implObj
::theme_updated(ONLY IN_THREAD, const const_defaulttheme &new_theme)
{
	layoutmanagerObj::implObj::theme_updated(IN_THREAD, new_theme);
	theme_update_in_progress=true;
}

void toolboxlayoutmanagerObj::implObj::recalculate(ONLY IN_THREAD)
{
	toolbox_info_t::lock lock{info};

	// Compute largest toolbox button size.
	lock->w=0;
	lock->h=0;

	for (const auto &c:lock->elements)
	{
		c->impl->initialize_if_needed(IN_THREAD);

		auto hv=c->impl->get_horizvert(IN_THREAD);

		auto w=hv->horiz.minimum();
		auto h=hv->vert.minimum();

		if (w > lock->w)
			lock->w=w;

		if (h > lock->h)
			lock->h=h;
	}

	process_updated_position(IN_THREAD,
				 layout_container_impl->container_element_impl()
				 .data(IN_THREAD).current_position,
				 lock);
}

void toolboxlayoutmanagerObj::implObj
::process_updated_position(ONLY IN_THREAD,
			   const rectangle &position)
{
#ifdef TESTTOOLBOX_DEBUG
	TESTTOOLBOX_DEBUG();
#endif
	toolbox_info_t::lock lock{info};

	process_updated_position(IN_THREAD, position, lock);
}

void toolboxlayoutmanagerObj::implObj
::process_updated_position(ONLY IN_THREAD,
			   const rectangle &position,
			   toolbox_info_t::lock &lock)
{
	dim_t preferred_width=dim_t::truncate(lock->w * default_width);

	if (preferred_width == dim_t::infinite())
		--preferred_width;

	// Given the current container width, this is how many elements
	// will fit on one row.
	size_t elements_per_row=dim_t::truncate(position.width / lock->w);

	// After a theme was updated, continue to use the same # of elements
	// per row, instead of changing it.

	if (theme_update_in_progress)
		elements_per_row=current_elements_per_row;

	if (elements_per_row == 0)
		elements_per_row=1; // Has to be at least one.

	// Until we're resized for the first time, don't both updating
	// the elements' positions.
	if (position.width == 0)
	{
		elements_per_row=default_width;
	}
	else
	{
		// We've been resized, so place all elements where they
		// should be
		coord_t y=0;
		coord_t x=0;
		size_t counter=0;

		for (const auto &c:lock->elements)
		{
			c->impl->initialize_if_needed(IN_THREAD);

			c->impl->update_current_position(IN_THREAD,
							 {x, y, lock->w,
							  lock->h});

			x=coord_t::truncate(x + lock->w);

			if (++counter == elements_per_row) // Next row.
			{
				counter=0;
				x=0;
				y=coord_t::truncate(y+lock->h);
			}
		}
	}

	current_elements_per_row=elements_per_row;

	// Based on the current width, compute number of rows we have.

	size_t nrows=(lock->elements.size() + (elements_per_row-1))
		/ elements_per_row;

	// If we only had one row, this is how wide we would be.
	dim_t maximum_width=dim_t::truncate(lock->w * lock->elements.size());

	// The vertical metrics are fixed, based on the number of elements
	// per row.
	dim_t total_height=dim_t::truncate(dim_t::truncate(nrows) * lock->h);

	width_increment(IN_THREAD)=lock->w;
	height_increment(IN_THREAD)=lock->h;
	base_width(IN_THREAD)=1;
	base_height(IN_THREAD)=nrows;

	 // Sanity check
	if (maximum_width < lock->w)
		maximum_width=lock->w;
	if (preferred_width < lock->w)
		preferred_width=lock->w;
	if (preferred_width > maximum_width)
		preferred_width=maximum_width;

	if (theme_update_in_progress &&
	    dim_t::truncate(position.width / lock->w) == elements_per_row)
	{
		// We were finally resized.
		theme_update_in_progress=false;
	}

	metrics::axis horiz_metrics{lock->w, preferred_width, maximum_width};

	if (theme_update_in_progress)
	{
		dim_t fixed_width{dim_t::truncate(lock->w * elements_per_row)};

		if (fixed_width == dim_t::infinite())
			--fixed_width;

		horiz_metrics={fixed_width, fixed_width, fixed_width};
	}
	layout_container_impl->container_element_impl()
		.get_horizvert(IN_THREAD)->set_element_metrics
		(IN_THREAD, horiz_metrics,
		 {total_height, total_height, total_height});
}

LIBCXXW_NAMESPACE_END
