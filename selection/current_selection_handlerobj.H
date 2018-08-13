/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_current_selection_handlerobj_h
#define x_w_current_selection_handlerobj_h

#include "x/w/impl/selection/current_selection_handlerfwd.H"
#include "x/w/generic_windowobj.H"
#include <xcb/xproto.h>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see current_selection_handler

class LIBCXX_HIDDEN current_selection_handlerObj : virtual public obj {

public:
	//! Constructor
	current_selection_handlerObj();

	//! Destructor
	~current_selection_handlerObj();

	//! Start conversion.
	virtual bool begin_converted_data(ONLY IN_THREAD,
					  xcb_atom_t type,
					  xcb_timestamp_t timestamp)=0;

	//! Received converted data.
	virtual void converted_data(ONLY IN_THREAD,
				    void *data,
				    size_t size,
				    generic_windowObj::handlerObj &me)=0;

	//! End of conversion.
	virtual void end_converted_data(ONLY IN_THREAD,
					generic_windowObj::handlerObj &me)=0;

	//! Conversion has failed.
	virtual void conversion_failed(ONLY IN_THREAD,
				       xcb_atom_t type,
				       generic_windowObj::handlerObj &me)=0;
};

LIBCXXW_NAMESPACE_END

#endif
