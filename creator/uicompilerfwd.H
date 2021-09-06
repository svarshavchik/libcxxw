/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef uicompilerfwd_h
#define uicompilerfwd_h

#include <x/ptrfwd.H>

class uicompilerObj;

/*! Parsed uicompiler.xml

Contains a parsed representation of uicompiler.xml

*/

typedef x::ref<uicompilerObj> uicompiler;

//! A constant \ref uicompiler "parsed uicompiler.xml".

//! \see uicompiler

typedef x::const_ref<uicompilerObj> const_uicompiler;

//! A nullable pointer reference to a \ref uicompiler "parsed uicompiler.xml".

//! \see uicompiler

typedef x::ptr<uicompilerObj> uicompilerptr;

//! A nullable pointer reference to a const \ref uicompiler "parsed uicompiler.xml".

//! \see uicompiler

typedef x::const_ptr<uicompilerObj> const_uicompilerptr;

#endif
