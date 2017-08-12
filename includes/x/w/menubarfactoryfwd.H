/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_menubarfactoryfwd_h
#define x_w_menubarfactoryfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class menubarfactoryObj;

/*! Menu bar factory.

A factory for adding menus to a \ref menubarlayoutmanager "menu bar".

*/

typedef ref<menubarfactoryObj> menubarfactory;

//! A constant \ref menubarfactory "menu bar factory".

//! \see menubarfactory

typedef const_ref<menubarfactoryObj> const_menubarfactory;

//! A nullable pointer reference to a \ref menubarfactory "menu bar factory".

//! \see menubarfactory

typedef ptr<menubarfactoryObj> menubarfactoryptr;

//! A nullable pointer reference to a const \ref menubarfactory "menu bar factory".

//! \see menubarfactory

typedef const_ptr<menubarfactoryObj> const_menubarfactoryptr;

LIBCXXW_NAMESPACE_END

#endif
