/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_progressbarobj_h
#define x_w_progressbarobj_h

#include <x/w/progressbarfwd.H>
#include <x/w/rgbfwd.H>
#include <x/w/containerobj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see progressbar

class progressbarObj : public containerObj {

public:
	class implObj;
	class handlerObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	progressbarObj(const ref<implObj> &impl,
		       const ref<layoutmanagerObj::implObj> &layout_impl)
		LIBCXX_HIDDEN;

	//! Destructor
	~progressbarObj() LIBCXX_HIDDEN;

	//! Override get_layout_impl()

	ref<layoutmanagerObj::implObj> get_layout_impl() const;

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

	  update() results in the progress bar getting redrawn, only to
	  redraw it again as a result of updating the descriptive content
	  in the progress bar.

	  This overload schedule the progress bar to get redrawn after
	  invoking the closure, which presumably updates the progress bar's
	  contents. The closure gets invoked by the connection thread,
	  asynchronously, before or after update() returns.
	*/

	void update(size_t value, size_t maximum_value,
		    const std::function <void ()> &closure);
};

LIBCXXW_NAMESPACE_END

#endif
