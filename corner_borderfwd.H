/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef corner_borderfwd_h
#define corner_borderfwd_h

#include <x/ptrfwd.H>
#include "x/w/namespace.H"
#include "x/w/elementfwd.H"
#include "x/w/elementobj.H"
#include "x/w/impl/border_implobj.H"

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN corner_borderObj;

//! A corner border element in a grid

//! This is an internal element object that's automatically created by
//! \ref gridlayoutmanager "grid layout manager" for drawing corner borders.
//! and vertical grid borders.

typedef ref<corner_borderObj> corner_border;

//! A nullable pointer reference to a \ref corner_border "corner border".

typedef ptr<corner_borderObj> corner_borderptr;

//! A reference to a constant \ref corner_border "corner border".

typedef const_ref<corner_borderObj> const_corner_border;

//! A nullable pointer reference to a constant \ref corner_border "corner border".

typedef const_ptr<corner_borderObj> const_corner_borderptr;

//! What surrounds the corner element.

struct surrounding_elements_info {

	//! Constructor
	surrounding_elements_info();

	//! Destructor
	~surrounding_elements_info();

	//! Copy constructor
	surrounding_elements_info(const surrounding_elements_info &);

	//! The top/left element, maybe null if this is the grid edge.
	element_implptr topleft;

	//! The bottom/right element, maybe null if this is the grid edge.
	element_implptr topright;

	//! The bottom/left element, maybe null if this is the grid edge.
	element_implptr bottomleft;

	//! The bottom/right element, maybe null if this is the grid edge.
	element_implptr bottomright;
};


LIBCXXW_NAMESPACE_END

#endif
