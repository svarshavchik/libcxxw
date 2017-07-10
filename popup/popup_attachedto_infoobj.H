/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef popup_attachedto_infoobj_h
#define popup_attachedto_infoobj_h

#include "x/w/rectangle.H"
#include "x/w/screenfwd.H"
#include "popup/popup_attachedto_infofwd.H"
#include "connection_threadfwd.H"
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

	dim_t max_peephole_width(IN_THREAD_ONLY, const screen &screenref)
		const;

	dim_t max_peephole_height(IN_THREAD_ONLY, const screen &screenref)
		const;
};

LIBCXXW_NAMESPACE_END

#endif
