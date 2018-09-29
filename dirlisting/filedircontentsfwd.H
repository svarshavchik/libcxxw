/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_filedircontentsfwd_h
#define x_w_filedircontentsfwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN filedircontentsObj;

/*! Directory contents monitor.

This object is constructed with the name of the directory to monitor and
a callback function. The contents of the directory are monitored in two
phases.

In the first phase, all existing contents of the directory get passed,
in groups, to the callback function. The second phase starts after the
entire initial contents of the directory get reported to the callback
function.

In the second phase the callback function gets invoked whenever the contents
of the directory change. The callback gets invoked by an internal execution
thread. The constructor starts the execution thread and the destructor
terminates.

The contents of the directory get monitored while this object exists. The
destructor stop the internal execution thread, at which point calls to
the callback functions cease.

\par Race conditions

It is possible that the minor race conditions occur as a result of the
transition between the first and the second phase. It is possible that the
same name gets reported more than once to the callback function (as
added or removed from the directory). The callback function is expected to
use the data it receives to maintain a snapshot of the directory's contents,
and ignore duplicate names, or reports that a nonexist name got removed.

Additionally it is possible that the internal execution thread is in the
middle of reporting an update to the directory's contents, at the time the
destructor gets called, and the callback function still gets invoked after
the last reference to this object goes away and it gets destroyed. This is
a temporary, transient condition. The internal execution thread will wrap
things up as soon as the dust settles.

*/

typedef ref<filedircontentsObj> filedircontents;

//! A constant \ref filedircontents "directory contents monitor".

//! \see filedircontents

typedef const_ref<filedircontentsObj> const_filedircontents;

//! A nullable pointer reference to a \ref filedircontents "directory contents monitor".

//! \see filedircontents

typedef ptr<filedircontentsObj> filedircontentsptr;

//! A nullable pointer reference to a const \ref filedircontents "directory contents monitor".

//! \see filedircontents

typedef const_ptr<filedircontentsObj> const_filedircontentsptr;

LIBCXXW_NAMESPACE_END

#endif
