/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef progressbar_slider_h
#define progressbar_slider_h

#include "x/w/progressbar.H"
#include "x/w/rgb.H"
#include "background_color_elementfwd.H"
#include "background_color_element_recalculatedfwd.H"
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

typedef background_color_element_recalculatedObj<
	background_color_elementObj<
		always_visibleObj<container_elementObj<child_elementObj>>,
		progressbar_bgcolor_tag, progressbar_gradient_tag>
	>progressbar_slider_base_t;

//! The slider in the progress bar.

//! A container, with a custom background color element that's used to draw the
//! progress bar slider.

class LIBCXX_HIDDEN progressbar_sliderObj : public progressbar_slider_base_t {

	typedef progressbar_slider_base_t superclass_t;

	//! Current logical value.

	size_t value_thread_only=0;

	//! Maximum slider logical value.

	size_t maximum_value_thread_only=1;

 public:

	THREAD_DATA_ONLY(value);
	THREAD_DATA_ONLY(maximum_value);

	//! Constructor
	progressbar_sliderObj(const ref<containerObj::implObj> &parent,
			      const progressbar_config &config);

	//! Destructor
	~progressbar_sliderObj();

	//! Update our background color, which draws the slider, if needed.

	void update(IN_THREAD_ONLY);

	//! Implement all_background_colors_were_recalculated().

	void all_background_colors_were_recalculated(IN_THREAD_ONLY) override;
 private:
	//! \internal

	void update(IN_THREAD_ONLY, const defaulttheme &);
};

LIBCXXW_NAMESPACE_END

#endif
