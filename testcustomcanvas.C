/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/obj.H>

#include "x/w/main_window.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/canvas.H"

#include "x/w/impl/canvas.H"
#include "x/w/impl/background_color_element.H"
#include "x/w/impl/scratch_and_mask_buffer_draw.H"
#include "x/w/impl/container.H"

#include <string>
#include <iostream>

using namespace LIBCXX_NAMESPACE;
using namespace LIBCXX_NAMESPACE::w;

class close_flagObj : public obj {

public:
	mpcobj<bool> flag;

	close_flagObj() : flag{false} {}
	~close_flagObj()=default;

	void close()
	{
		mpcobj<bool>::lock lock{flag};

		*lock=true;
		lock.notify_all();
	}
};

typedef ref<close_flagObj> close_flag_ref;

struct circle_color_tag;

class my_canvas_implObj :
	public scratch_and_mask_buffer_draw<background_color_elementObj<
						    canvasObj::implObj,
						    circle_color_tag>> {

	typedef scratch_and_mask_buffer_draw<background_color_elementObj<
						     canvasObj::implObj,
						     circle_color_tag>
					     > superclass_t;
public:

	my_canvas_implObj(const container_impl &parent_container)
		: my_canvas_implObj(parent_container,
				    canvas_init_params{
					    {50.0, 50.0, 100.0},
					    {50.0, 50.0, 100.0},
					    "my_canvas@examples.w.libcxx.com"})
	{
	}

	my_canvas_implObj(const container_impl &parent_container,
			  const canvas_init_params &init_params)
		: my_canvas_implObj{parent_container,
			init_params,
			create_child_element_params(parent_container,
						    init_params)}
	{
	}

	my_canvas_implObj(const container_impl &parent_container,
			  const canvas_init_params &init_params,
			  const child_element_init_params &child_init_params)
		: superclass_t{
		        "my_canvas@examples.w.libcxx.com",
		        "0%",
			parent_container, init_params, child_init_params}
	{
	}

	~my_canvas_implObj()=default;

	void do_draw(ONLY IN_THREAD,
		     const draw_info &di,
		     const picture &area_picture,
		     const pixmap &area_pixmap,
		     const gc &area_gc,
		     const picture &mask_picture,
		     const pixmap &mask_pixmap,
		     const gc &mask_gc,
		     const clip_region_set &clipped,
		     const rectangle &area_entire_rect) override;
};

typedef ref<my_canvas_implObj> my_canvas_impl;

void my_canvas_implObj::do_draw(ONLY IN_THREAD,
				const draw_info &di,
				const picture &area_picture,
				const pixmap &area_pixmap,
				const gc &area_gc,
				const picture &mask_picture,
				const pixmap &mask_pixmap,
				const gc &mask_gc,
				const clip_region_set &clipped,
				const rectangle &area_entire_rect)
{
	gc::base::properties props;

	props.function(gc::base::function::CLEAR);

	mask_gc->fill_rectangle(area_entire_rect, props);

	props.function(gc::base::function::SET);

	mask_gc->fill_arc(0, 0, area_entire_rect.width, area_entire_rect.height,
			  0, 360*64, props);

	dim_t circle_width=area_entire_rect.width/10;
	dim_t circle_height=area_entire_rect.height/10;

	if (circle_width > 0 && circle_height > 0)
	{
		dim_t inner_width=area_entire_rect.width
			-circle_width-circle_width;

		dim_t inner_height=area_entire_rect.height
			-circle_height-circle_height;

		props.function(gc::base::function::CLEAR);

		mask_gc->fill_arc(coord_t::truncate(circle_width),
				  coord_t::truncate(circle_height),
				  inner_width,
				  inner_height,
				  0, 360*64, props);
	}

	area_picture->composite(background_color_element<circle_color_tag>
				::get(IN_THREAD)->get_current_color(IN_THREAD),
				mask_picture,
				0, 0, // src_x, src_y
				0, 0, // mask_x, mask_y
				0, 0, // dst_x, dst_y
				area_entire_rect.width,
				area_entire_rect.height,
				render_pict_op::op_over);
}

class my_canvasObj : public canvasObj {

public:

	const my_canvas_impl impl; // My implementation object.

	// Constructor

	my_canvasObj(const my_canvas_impl &impl)
		: canvasObj{impl}, impl{impl}
	{
	}

	~my_canvasObj()=default;
};

typedef x::ref<my_canvasObj> my_canvas;

void testcustomcanvas()
{
	destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=main_window::create
		([&]
		 (const auto &main_window)
		 {
			 gridlayoutmanager layout{
				 main_window->get_layoutmanager()
			 };

			 auto factory=layout->append_row();
			 factory->padding(0);

			 auto impl=my_canvas_impl::create(factory->get_container_impl());
			 auto c=my_canvas::create(impl);

			 factory->created_internally(c);

		 },
		 LIBCXX_NAMESPACE::w::new_gridlayoutmanager{});

	main_window->set_window_title("Custom canvas");

	guard(main_window->connection_mcguffin());

	main_window->on_disconnect([]
				   {
					   _exit(1);
				   });

	main_window->on_delete
		([close_flag]
		 (THREAD_CALLBACK,
		  const auto &ignore)
		 {
			 close_flag->close();
		 });

	main_window->show_all();

	mpcobj<bool>::lock lock{close_flag->flag};

	lock.wait([&] { return *lock; });
}

int main(int argc, char **argv)
{
	try {
		LIBCXX_NAMESPACE::property
			::load_property(LIBCXX_NAMESPACE_STR "::themes",
					"themes", true, true);

		testcustomcanvas();
	} catch (const exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
