/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef richtextmetalinkcollection_H
#define richtextmetalinkcollection_H

#include <x/ref.H>
#include "richtext/richtextmetalink.H"
#include "richtext/richtextmetalinkcollectionfwd.H"
#include "richtext/richtextmetalinkcollectionobj.H"

LIBCXXW_NAMESPACE_START

//! A stub \ref richtextmetalinkcollection "metadata link collection" base class.

class richtextmetalinkcollectionBase
	: public vectorBase<metalink_tuple_t,
			    std::allocator<metalink_tuple_t>> {
};

LIBCXXW_NAMESPACE_END

#endif
