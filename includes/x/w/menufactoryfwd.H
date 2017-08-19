/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_menufactoryfwd_h
#define x_w_menufactoryfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class menufactoryObj;

/*! A factory for adding new items to a \ref menulayoutmanager "menu".


*/

typedef ref<menufactoryObj> menufactory;

//! A constant \ref menufactory "menu factory".

//! \see menufactory

typedef const_ref<menufactoryObj> const_menufactory;

//! A nullable pointer reference to a \ref menufactory "menu factory".

//! \see menufactory

typedef ptr<menufactoryObj> menufactoryptr;

//! A nullable pointer reference to a const \ref menufactory "menu factory".

//! \see menufactory

typedef const_ptr<menufactoryObj> const_menufactoryptr;

LIBCXXW_NAMESPACE_END

#endif
