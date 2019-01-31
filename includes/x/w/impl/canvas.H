/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_canvas_h
#define x_w_impl_canvas_h

#include <x/w/canvasobj.H>
#include <x/w/screenobj.H>
#include <x/w/dim_axis_arg.H>
#include <x/w/connection_threadfwd.H>
#include <x/w/rgbfwd.H>
#include <x/w/impl/child_elementobj.H>
#include <x/w/impl/themedim_axis_elementfwd.H>

LIBCXXW_NAMESPACE_START

//! Additional parameters to the \ref canvasObj::implObj "canvas implementation object".

struct canvas_init_params {

	//! Canvas width
	dim_axis_arg width;

	//! Canvas height
	dim_axis_arg height;

	//! Identifier for the canvas's scratch buffer, for drawing.
	std::string scratch_buffer_id;

	//! Optional canvas background color
	std::optional<color_arg> background_color;
};


//! \ref canvas "Canvas" implementation object.

class canvasObj::implObj
	: public themedim_axis_widthObj<
	themedim_axis_heightObj<child_elementObj>> {

	//! Alias.

	typedef themedim_axis_widthObj<
		themedim_axis_heightObj<child_elementObj>> superclass_t;

public:
	//! Constructor
	implObj(const container_impl &container,
		const canvas_init_params &params) LIBCXX_PUBLIC;

	//! Delegating constructor

	//! create_child_element_params() constructs the additional parameter.

	implObj(const container_impl &container,
		const canvas_init_params &params,
		const child_element_init_params &child_element_params)
	LIBCXX_PUBLIC;


	//! Compute child_element_init_params().

	//! The primary constructor calls this, then invokes the delgating
	//! constructor.

	static child_element_init_params
	create_child_element_params(const container_impl &container,
				    const canvas_init_params &params);

	//! Destructor
	~implObj();

	//! Inherited from elementObj::implObj
	void initialize(ONLY IN_THREAD) override;

	//! Inherited from elementObj::implObj
	void theme_updated(ONLY IN_THREAD, const defaulttheme &new_theme) override;

	//! Update the canvas's metrics.
	void update(ONLY IN_THREAD,
		    const dim_axis_arg &new_width,
		    const dim_axis_arg &new_height);
 private:
	//! Recalculate my metrics
	void recalculate(ONLY IN_THREAD) LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
