/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_richtextcursorlocationownerobj_h
#define x_w_richtextcursorlocationownerobj_h

#include "richtext/richtextcursorlocationownerfwd.H"
#include "x/w/impl/richtext/richtextcursorlocationfwd.H"
#include "x/w/impl/richtext/richtextfragmentfwd.H"
#include "x/w/impl/richtext/richtextiteratorfwd.H"
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see richtextcursorlocationowner

class richtextcursorlocationownerObj: virtual public obj {

protected:

	//! My cursor location object.

	//! An internal implementation object, basically. Accessible only
	//! while holding a lock on the underlying implementation object.

	richtextcursorlocation my_location;
public:
	//! Constructor for a new location
	richtextcursorlocationownerObj(const richtextcursorlocation
				       &my_location,
				       richtextfragmentObj *my_fragment,
				       size_t offset,
				       new_location location_option);

	//! Constructor for cloning an existing location0
	richtextcursorlocationownerObj(const richtextcursorlocationownerObj
				       &clone);

	//! Destructor
	~richtextcursorlocationownerObj();
};

LIBCXXW_NAMESPACE_END

#endif
