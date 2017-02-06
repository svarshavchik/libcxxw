/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "gridlayoutmanager_impl_elements.H"
#include "catch_exceptions.H"

LIBCXXW_NAMESPACE_START

bool gridlayoutmanagerObj::implObj::rebuild_elements(IN_THREAD_ONLY)
{
	grid_map_t::lock lock(grid_map);

	if (!lock->modified)
	{
		// Still need to recalculate everything if child_metrics_updated

		bool flag=lock->child_metrics_updated;

		lock->child_metrics_updated=false;
		return flag;
	}

	std::vector<elementsObj::pos_axis> &all_elements=
		grid_elements(IN_THREAD)->all_elements;

	all_elements.clear();
	all_elements.reserve(lock->elements.size());

	for (const auto &element:lock->elements)
	{
		// We don't care about the keys. The value in the map is:
		//
		//       element - the child element.
		//       pos     - its metrics::grid_pos
		//
		// The all_elements vector contains a class that inherits from
		// metrics::pos_axis which contains:
		//       pos     - the metrics::grid_pos
		//       horizvert - the element's axises
		//
		// plus the element itself.
		//
		// So this is really just shuffling the deck, a little bit.

		all_elements.emplace_back
			(element.second.pos,
			 metrics::horizvert(element.first->impl
					    ->get_horizvert(IN_THREAD)),
			 element.first);
	}
	lock->modified=false;
	lock->child_metrics_updated=false;

	return true;
}

void gridlayoutmanagerObj::implObj::initialize_new_elements(IN_THREAD_ONLY)
{
	bool flag;

	// Before calculating the metrics, check if any of these
	// need to be initialize()d.

	do
	{
		for (const auto &element:grid_elements(IN_THREAD)->all_elements)
		{
			try {
				element.child_element->impl
					->initialize_if_needed(IN_THREAD);
			} CATCH_EXCEPTIONS;
		}

		// initialize_new_elements() was invoked only if
		// rebuild_elements() retursn true.
		//
		// Maybe the initialize() callback add more elements
		// to this grid? If so, let's do this again.

		flag=rebuild_elements(IN_THREAD);
	} while (flag);
}

void gridlayoutmanagerObj::implObj
::do_for_each_child(IN_THREAD_ONLY,
		    const function<void (const element &e)> &callback)
{
	for (const auto &child:grid_elements(IN_THREAD)->all_elements)
		callback(child.child_element);
}

bool gridlayoutmanagerObj::implObj::elementsObj
::recalculate_metrics(IN_THREAD_ONLY,
		      bool flag,
		      const metrics::horizvert &my_metrics)
{
	auto new_horiz_metrics=
		metrics::calculate_grid_horiz_metrics(all_elements);

	auto new_vert_metrics=
		metrics::calculate_grid_vert_metrics(all_elements);

	if (!flag && (horiz_metrics != new_horiz_metrics ||
		      vert_metrics != new_vert_metrics))
		flag=true;

	horiz_metrics=new_horiz_metrics;
	vert_metrics=new_vert_metrics;

	my_metrics->set_element_metrics(IN_THREAD,
					total_metrics(horiz_metrics),
					total_metrics(vert_metrics));

	return flag;
}

metrics::axis gridlayoutmanagerObj::implObj::elementsObj
::total_metrics(const metrics::grid_metrics_t &metrics)
{
	metrics::axis total;

	auto b=metrics.begin();
	auto e=metrics.end();

	if (b != e)
	{
		total=b->second;
		++b;
	}

	while (b != e)
	{
		total=total+b->second;
		++b;
	}

	return total;
}


bool gridlayoutmanagerObj::implObj::elementsObj
::recalculate_sizes(dim_t target_width,
		    dim_t target_height)
{
	bool flag=false;

	if (metrics::calculate_grid_size(horiz_metrics,
					 horiz_sizes,
					 target_width))
		flag=true;

	if (metrics::calculate_grid_size(vert_metrics,
					 vert_sizes,
					 target_height))
		flag=true;

	return flag;
}


void gridlayoutmanagerObj::implObj
::process_updated_position(IN_THREAD_ONLY,
			   const rectangle &position)
{
	auto &elements=*grid_elements(IN_THREAD);

	// Ignore the return value from recalculate_sizes(). We can get
	// here when the child metrics have changed. This may not necessarily
	// result in the sizes of the rows and columns changing. However if
	// the child element's maximum metrics have changed, the child
	// element might need to be reposition within its cells, by the
	// code below.
	elements.recalculate_sizes(position.width, position.height);

	for (const auto &child:elements.all_elements)
	{
		const auto &element=child.child_element;

		auto hv=element->impl->get_horizvert(IN_THREAD);

		// Find the element's first and last row and column.

		const auto &horiz_pos=child.pos->horiz_pos;
		const auto &vert_pos=child.pos->vert_pos;

		auto h_start=elements.horiz_sizes.find(horiz_pos.start);
		auto h_end=elements.horiz_sizes.find(horiz_pos.end);

		auto v_start=elements.horiz_sizes.find(vert_pos.start);
		auto v_end=elements.horiz_sizes.find(vert_pos.end);

		if (h_start == elements.horiz_sizes.end() ||
		    h_end == elements.horiz_sizes.end() ||
		    v_start == elements.vert_sizes.end() ||
		    v_end == elements.vert_sizes.end())
		{
			LOG_FATAL("Internal: cannot find grid rows or columns for an element, horiz_pos=" << horiz_pos.start << "-" << horiz_pos.end
				  << ", vert_pos=" << vert_pos.start << "-" <<vert_pos.end);
			continue;
		}

		coord_t x=std::get<coord_t>(h_start->second);
		coord_t y=std::get<coord_t>(v_start->second);

		// Compute the sum total size of the element's rows and columns

		auto new_width=
			(std::get<coord_t>(h_end->second)
			 + std::get<dim_t>(h_end->second)
			 - x);

		auto new_height=
			(std::get<coord_t>(v_end->second)
			 + std::get<dim_t>(v_end->second)
			 - y);

		if (dim_t::overflows(new_width))
			new_width=std::numeric_limits<dim_t::value_type>::max();

		if (dim_t::overflows(new_height))
			new_height=std::numeric_limits<dim_t::value_type>::max();

		// If the total size of the element's rows and columns
		// exceeds the elements maximum metric, position the
		// element accordingly. Adjust new_width and new_height,
		// so the element does not get resized.

		if (hv->horiz.maximum() != dim_t::infinite())
		{
			auto max_width=hv->horiz.maximum()+dim_t(0);

			if (max_width < new_width)
			{
				auto padding=(new_width - max_width) / 2;

				new_width=max_width;

				x=x.truncate(x+padding);
			}
		}

		if (hv->vert.maximum() != dim_t::infinite())
		{
			auto max_height=hv->vert.maximum()+dim_t(0);

			if (max_height < new_height)
			{
				auto padding=
					(new_height - max_height) / 2;

				new_height=max_height;
				y=y.truncate(y+padding);
			}

		}

		element->impl->update_current_position
			(IN_THREAD,
			 {
				 x, y,
					 dim_t::truncate(new_width),
					 dim_t::truncate(new_height),
			 });
	}
}

LIBCXXW_NAMESPACE_END
