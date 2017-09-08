/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef generic_window_handler_resources_h
#define generic_window_handler_resources_h

#include "generic_window_handler.H"
#include "icon_images_set_elementfwd.H"
#include "background_color_elementfwd.H"

LIBCXXW_NAMESPACE_START

struct disabled_mask_tag;
struct background_color_tag;
struct shaded_color_tag;

//! Attach resources to generic_windowObj::handlerObj

typedef icon_images_set_elementObj<
	background_color_elementObj<generic_windowObj::handlerObj,
				    background_color_tag,
				    shaded_color_tag>,
				   disabled_mask_tag
				   > generic_window_handler_and_resources_t;

//! Subclass of generic_windowObj::handlerObj + its resources.

class LIBCXX_HIDDEN generic_windowObj::handlerObj::resourcesObj
	: public generic_window_handler_and_resources_t {

 public:
	//! Constructor

	resourcesObj(IN_THREAD_ONLY,
		     const screen &me,
		     const char *background_color,
		     const shared_handler_data &handler_data,
		     size_t nesting_level);

	//! Implemented disabled_mask()

	const icon disabled_mask(IN_THREAD_ONLY) override;

	//! Implemented current_background_color()

	const background_color current_background_color(IN_THREAD_ONLY)
		override;

	//! Use new background color.

	void set_background_color(IN_THREAD_ONLY,
				  const background_color &) override;

	//! Implemented shaded_color()

	const background_color shaded_color(IN_THREAD_ONLY)
		override;
};

LIBCXXW_NAMESPACE_END

#endif
