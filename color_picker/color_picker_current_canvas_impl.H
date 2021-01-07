/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef color_picker_current_canvas_impl_h
#define color_picker_current_canvas_impl_h

#include "color_picker/color_picker_current_canvas.H"
#include "color_picker/color_picker_impl.H"
#include "x/w/impl/canvas.H"
#include "x/w/impl/connection_threadfwd.H"

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

class color_picker_current_canvasObj::implObj : public canvasObj::implObj {

	//! Current color
	rgb current_color_thread_only;
public:
	THREAD_DATA_ONLY(current_color);

	//! Constructor
	implObj(const container_impl &container,
		const std::string name,
		const color_pickerObj::implObj::official_color &initial_color,
		const canvas_init_params &params);

	//! Destructor
	~implObj();

	//! Name of this restore color picker
	const std::string name;

	//! This color picker's current official color.
	const color_pickerObj::implObj::official_color current_official_color;

	//! Override cleared_to_background_color

	void cleared_to_background_color(ONLY IN_THREAD,
					 const picture &,
					 const pixmap &,
					 const gc &,
					 const draw_info &,
					 const rectangle &) override;

	//! Implement save()
	void save(ONLY IN_THREAD, const screen_positions &pos) override;

};

LIBCXXW_NAMESPACE_END

#endif
