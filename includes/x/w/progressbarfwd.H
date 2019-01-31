/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_progressbarfwd_h
#define x_w_progressbarfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

struct progressbar_config;

class LIBCXX_PUBLIC progressbarObj;

/*! A progress indication bar

\code

INSERT_LIBX_NAMESPACE::w::progressbar
    pb=factory->create_progressbar(
         []
         (const auto &)
         {
               // ...
         });

pb->update(
      15, 50,
      []
      (ONLY IN_THREAD)
      {
      });

\endcode

*/

typedef ref<progressbarObj> progressbar;

//! A constant \ref progressbar "progress bar".

//! \see progressbar

typedef const_ref<progressbarObj> const_progressbar;

//! A nullable pointer reference to a \ref progressbar "progress bar".

//! \see progressbar

typedef ptr<progressbarObj> progressbarptr;

//! A nullable pointer reference to a const \ref progressbar "progress bar".

//! \see progressbar

typedef const_ptr<progressbarObj> const_progressbarptr;

LIBCXXW_NAMESPACE_END

#endif
