/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_toolboxfactoryfwd_h
#define x_w_toolboxfactoryfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN toolboxfactoryObj;

/*! A toolbox factory.

A factory that adds new display elements to a container with a
\ref toolboxlayoutmanager "toolbox layout manager".

*/

typedef ref<toolboxfactoryObj> toolboxfactory;

//! A constant \ref toolboxfactory "toolbox factory".

//! \see toolboxfactory

typedef const_ref<toolboxfactoryObj> const_toolboxfactory;

//! A nullable pointer reference to a \ref toolboxfactory "toolbox factory".

//! \see toolboxfactory

typedef ptr<toolboxfactoryObj> toolboxfactoryptr;

//! A nullable pointer reference to a const \ref toolboxfactory "toolbox factory".

//! \see toolboxfactory

typedef const_ptr<toolboxfactoryObj> const_toolboxfactoryptr;

LIBCXXW_NAMESPACE_END

#endif
