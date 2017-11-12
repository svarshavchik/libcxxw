/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef progressbar_slider_h
#define progressbar_slider_h

#include "x/w/progressbar.H"
#include "background_color_elementfwd.H"
#include "progressbar_sliderfwd.H"
#include "always_visiblefwd.H"
#include "container_elementfwd.H"
#include "container.H"
#include "child_element.H"

LIBCXXW_NAMESPACE_START

//! Implementation object

//! \see progressbar_slider

struct progressbar_bgcolor_tag;
struct progressbar_gradient_tag;

//! Superclass for the \ref progressbar_sliderObj "progress bar slider implementation object."

typedef background_color_elementObj<
	always_visibleObj<container_elementObj<child_elementObj>>,
	progressbar_bgcolor_tag, progressbar_gradient_tag
	> progressbar_slider_base_t;

//! The slider in the progress bar.

//! A container, with a custom background color element that's used to draw the
//! progress bar slider.

class LIBCXX_HIDDEN progressbar_sliderObj : public progressbar_slider_base_t {

	typedef progressbar_slider_base_t superclass_t;

	//! Gradient for the slider.

	const rgb_gradient_arg slider_color;

	//! Current logical value.

	size_t value_thread_only=0;

	//! Maximum slider logical value.

	size_t maximum_value_thread_only=1;

	//! Logical value last used to draw the slider.
	size_t updated_value=0;

	//! Logical maximum value last used to draw the slider.
	size_t updated_maximum_value=1;

	//! The width of the slider, in pixels, last used to draw the slider.
	dim_t updated_width=0;

	//! Which theme was used to draw the slider.

	defaulttheme updated_theme;

 public:

	THREAD_DATA_ONLY(value);
	THREAD_DATA_ONLY(maximum_value);

	//! Constructor
	progressbar_sliderObj(const ref<containerObj::implObj> &parent,
			      const progressbar_config &config);

 private:

	//! Internal constructor
	progressbar_sliderObj(const ref<containerObj::implObj> &parent,
			      const progressbar_config &config,
			      const background_color &color);
 public:

	~progressbar_sliderObj();

	//! Override initialize()

	//! Invoke superclass's initialize(), then call update().

	void initialize(IN_THREAD_ONLY) override;

	//! Override theme_updated()

	//! Invoke the superclass's method, and call update().

	void theme_updated(IN_THREAD_ONLY,
			   const defaulttheme &new_theme) override;

	//! Override process_updated_position().

	//! Invoke the superclass's method, and call update().

	void process_updated_position(IN_THREAD_ONLY) override;

	//! Update our background color, which draws the slider, if needed.

	void update(IN_THREAD_ONLY);

 private:
	//! \internal

	void update(IN_THREAD_ONLY, const defaulttheme &);
};

LIBCXXW_NAMESPACE_END

#endif
