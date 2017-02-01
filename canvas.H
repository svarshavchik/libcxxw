/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef canvas_h
#define canvas_h

#include "x/w/canvasobj.H"
#include "x/w/metrics/mmaxis.H"
#include "x/w/screenobj.H"
#include "connection_threadfwd.H"
#include "child_elementobj.H"

LIBCXXW_NAMESPACE_START

//! Base class for a canvas implementation.

class LIBCXX_HIDDEN canvasObj::implObj : public child_elementObj {

	//! Horizontal metrics
	metrics::mmaxis horiz_metrics_thread_only;

	//! Vertical metrics
	metrics::mmaxis vert_metrics_thread_only;
public:

	THREAD_DATA_ONLY(horiz_metrics);
	THREAD_DATA_ONLY(vert_metrics);

	implObj(const ref<containerObj::implObj> &container,
		const metrics::mmaxis &horiz_metrics,
		const metrics::mmaxis &vert_metrics);

 private:
	//! Private constructor

	implObj(const ref<containerObj::implObj> &container,
		const ref<screenObj::implObj> &screen,
		const metrics::mmaxis &horiz_metrics,
		const metrics::mmaxis &vert_metrics);

 public:
	//! Destructor
	~implObj();

	//! Inherit from elementObj::implObj
	void initialize(IN_THREAD_ONLY) override;

	//! Inherit from elementObj::implObj
	void theme_updated(IN_THREAD_ONLY) override;

 private:
	//! Recalculate my metrics
	void recalculate(IN_THREAD_ONLY);
};

LIBCXXW_NAMESPACE_END

#endif
