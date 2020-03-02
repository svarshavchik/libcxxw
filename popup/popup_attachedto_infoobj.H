/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef popup_attachedto_infoobj_h
#define popup_attachedto_infoobj_h

#include "x/w/rectangle.H"
#include "x/w/screenfwd.H"
#include "popup/popup_attachedto_infofwd.H"
#include "x/w/connection_threadfwd.H"
#include "x/w/tooltip.H"
#include "x/w/impl/connection_threadfwd.H"
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Information about the display element a popup is attached to.

class LIBCXX_HIDDEN popup_attachedto_infoObj : virtual public obj {

	//! Most recently reported element position

	rectangle attachedto_element_position_thread_only;

 public:
	//! How this popup is attached

	const attached_to how;

	THREAD_DATA_ONLY(attachedto_element_position);

	//! Constructor
	popup_attachedto_infoObj
		(const rectangle &attachedto_element_position,
		 const attached_to how);

	//! Destructor
	~popup_attachedto_infoObj();

	dim_t max_peephole_width(ONLY IN_THREAD, const screen &screenref)
		const;

	dim_t max_peephole_height(ONLY IN_THREAD, const screen &screenref)
		const;
};

LIBCXXW_NAMESPACE_END

#endif
