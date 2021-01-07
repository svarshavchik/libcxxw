/*
** Copyright 2019-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_richtextcursorlocationownerfwd_h
#define x_w_richtextcursorlocationownerfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN richtextcursorlocationownerObj;

/*! The object that owns a richtextcursorlocation.

Inherited by richtextiterator. This object is responsible for initializing
and deinitalizing the richtextcursorlocation


*/

typedef ref<richtextcursorlocationownerObj> richtextcursorlocationowner;

//! A constant \ref richtextcursorlocationowner "richtextcursorlocationowner".

//! \see richtextcursorlocationowner

typedef const_ref<richtextcursorlocationownerObj> const_richtextcursorlocationowner;

//! A nullable pointer reference to a \ref richtextcursorlocationowner "richtextcursorlocationowner".

//! \see richtextcursorlocationowner

typedef ptr<richtextcursorlocationownerObj> richtextcursorlocationownerptr;

//! A nullable pointer reference to a const \ref richtextcursorlocationowner "richtextcursorlocationowner".

//! \see richtextcursorlocationowner

typedef const_ptr<richtextcursorlocationownerObj> const_richtextcursorlocationownerptr;

LIBCXXW_NAMESPACE_END

#endif
