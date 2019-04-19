/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_uigeneratorsfwd_h
#define x_w_uigeneratorsfwd_h

#include <x/w/namespace.H>
#include <x/w/gridlayoutmanagerfwd.H>
#include <x/w/booklayoutmanagerfwd.H>
#include <x/w/gridfactoryfwd.H>
#include <x/w/bookpagefactoryfwd.H>
#include <x/w/uielementsfwd.H>
#include <x/functionalrefptrfwd.H>

LIBCXXW_NAMESPACE_START

//! Generate something in the grid layout manager

//! \see uigenerators

typedef functionref<void (const gridlayoutmanager &, uielements &)
		    > gridlayoutmanager_generator;

//! Generate something using a grid factory.

//! \see uigenerators
typedef functionref<void (const gridfactory &, uielements &)
		    > gridfactory_generator;

//! Generate something with the book layout manager

//! \see uigenerators
typedef functionref<void (const booklayoutmanager &, uielements &)
		    > booklayoutmanager_generator;

//! Generate something using a book page factory.

//! \see uigenerators
typedef functionref<void (const bookpagefactory &, uielements &)
		    > bookpagefactory_generator;

class LIBCXX_PUBLIC uigeneratorsObj;
class LIBCXX_PUBLIC uigeneratorsBase;

/*! Loaded and parsed generators

XML-based specification that's compiled into a form that gets used to
generate() the contents of supported layout manager (and factories).

*/

typedef ref<uigeneratorsObj, uigeneratorsBase> uigenerators;

//! Constant \ref uigenerators "compiled display element generators".

//! \see uigenerators

typedef const_ref<uigeneratorsObj, uigeneratorsBase> const_uigenerators;

//! A nullable pointer reference to \ref uigenerators "compiled display element generators".

//! \see uigenerators

typedef ptr<uigeneratorsObj, uigeneratorsBase> uigeneratorsptr;

//! A nullable pointer reference to const \ref uigenerators "compiled display element generators".

//! \see uigenerators

typedef const_ptr<uigeneratorsObj, uigeneratorsBase> const_uigeneratorsptr;

LIBCXXW_NAMESPACE_END

#endif
