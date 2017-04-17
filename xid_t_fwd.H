/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef xid_t_fwd_h
#define xid_t_fwd_h

#include <x/ptrfwd.H>
#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START
class xidObj;

//! An allocated X protocol identifier.

typedef ref<xidObj> xid;

//! A nullable pointer refernece to an \ref xid "allocated X protocol identifier".

typedef ptr<xidObj> xidptr;

LIBCXXW_NAMESPACE_END

#endif
