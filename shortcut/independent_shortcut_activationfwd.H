/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef independent_shortcut_activationfwd_h
#define independent_shortcut_activationfwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN independent_shortcut_activationObj;

/*! An alternative to a \ref shortcut_activation_elementObj "shortcut_activation_elementObj" mixin.

Used when a shortcut needs to be used without a direct association to
a display element. Ensures appropriate initialization and deinitialization.

*/

typedef ref<independent_shortcut_activationObj> independent_shortcut_activation;

//! A constant \ref independent_shortcut_activation "independent shortcut".

//! \see independent_shortcut_activation

typedef const_ref<independent_shortcut_activationObj> const_independent_shortcut_activation;

//! A nullable pointer reference to a \ref independent_shortcut_activation "independent shortcut".

//! \see independent_shortcut_activation

typedef ptr<independent_shortcut_activationObj> independent_shortcut_activationptr;

//! A nullable pointer reference to a const \ref independent_shortcut_activation "independent shortcut".

//! \see independent_shortcut_activation

typedef const_ptr<independent_shortcut_activationObj> const_independent_shortcut_activationptr;

LIBCXXW_NAMESPACE_END

#endif
