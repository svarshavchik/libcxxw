/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_override_redirectfwd_h
#define x_w_override_redirectfwd_h

#include "x/w/namespace.H"
#include "x/w/generic_windowobj.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN override_redirectObj;

template<typename element_impl>
class LIBCXX_HIDDEN override_redirect_elementObj;

typedef ref<override_redirectObj> override_redirect;

//! A constant \ref override_redirect "override-redirected element".

//! \see override_redirect

typedef const_ref<override_redirectObj> const_override_redirect;

//! A nullable pointer reference to a \ref override_redirect "override-redirected element".

//! \see override_redirect

typedef ptr<override_redirectObj> override_redirectptr;

//! A nullable pointer reference to a const \ref override_redirect "override-redirected element".

//! \see override_redirect

typedef const_ptr<override_redirectObj> const_override_redirectptr;

LIBCXXW_NAMESPACE_END

#endif
