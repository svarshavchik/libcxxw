/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_progressbarobj_h
#define x_w_progressbarobj_h

#include <x/w/progressbarfwd.H>
#include <x/w/rgbfwd.H>
#include <x/w/containerobj.H>
#include <x/functionalrefptrfwd.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see progressbar

class progressbarObj : public containerObj {

public:
	class LIBCXX_HIDDEN implObj;
	class LIBCXX_HIDDEN handlerObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	progressbarObj(const ref<implObj> &impl,
		       const layout_impl &container_layout_impl)
		LIBCXX_HIDDEN;

	//! Destructor
	~progressbarObj() LIBCXX_HIDDEN;

	//! Override get_layout_impl()

	layout_impl get_layout_impl() const override;

	/*! Update the progress bar's value.

	  Set the progress bar to indicate that X out of Y steps,
	  of some process, are complete.

	  The second parameter specifies the total number of steps.
	  The first parameter specifies the number of completed steps,
	  between 0 and the 2nd parameter's value.

	  The progress bar's slider position is completely empty when the
	  first value is zero. When both values are the same the slider flils
	  the entire width of the progress bar.
	*/

	void update(size_t value, size_t maximum_value);

	/*!
	  Invoke the closure and update the progress bar's value.

	  This is an optimization. A progress bar update typically involves
	  updating the slider's position, and updating the descriptive content
	  of the progress bar.

	  Using update() followed by making the appropriate changes to the
	  element in the progressbar container usually
	  results in the progress bar getting redrawn, only to
	  redraw it again as a result of updating the descriptive content
	  in the progress bar.

	  This overload makes it possible to redraw everything just once.
	  The closure gets invoked after the progressbar's value gets updated,
	  but before it gets redrawn. Presumably the closure updates the
	  progressbar's container. This should result in a single redraw.

	  The closure gets invoked by the connection thread,
	  asynchronously, before or after update() returns.
	*/

	void update(size_t value, size_t maximum_value,
		    const functionref <void (THREAD_CALLBACK)> &closure);
};

LIBCXXW_NAMESPACE_END

#endif
