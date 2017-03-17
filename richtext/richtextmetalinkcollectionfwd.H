/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef richtextmetalinkcollectionfwd_H
#define richtextmetalinkcollectionfwd_H

#include <x/ptrfwd.H>
#include <x/vectorfwd.H>
#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

class richtextmetalinkcollectionObj;
class richtextmetalinkcollectionBase;

//! A helper for marking \ref richtextmetalink "linked parts" of a \ref label "label".

//! Allows assembling a \ref richtextstring "richtextstring", and its
//! associated data first, then marking which parts of the string are linked.
//! The helper class then modifies the metadata by inserting links as indicated.
//!
//! This is a reference-counted vector. Use emplace_back() to construct the
//! entire list of linkable parts of a metadata. apply() then modifies the
//! metadata accordingly.

typedef ref<richtextmetalinkcollectionObj,
	    richtextmetalinkcollectionBase> richtextmetalinkcollection;

//! A nullable pointer reference to a \ref richtextmetalinkcollection "metadata link collection".

typedef ptr<richtextmetalinkcollectionObj,
	    richtextmetalinkcollectionBase> richtextmetalinkcollectionptr;

//! A constant \ref richtextmetalinkcollection "metadata link collection".

typedef const_ref<richtextmetalinkcollectionObj,
		  richtextmetalinkcollectionBase
		  > const_richtextmetalinkcollection;

//! A nullable pointer reference to a constant \ref richtextmetalinkcollection "metadata link collection".

typedef const_ptr<richtextmetalinkcollectionObj,
		  richtextmetalinkcollectionBase
		  > const_richtextmetalinkcollectionptr;

LIBCXXW_NAMESPACE_END

#endif
