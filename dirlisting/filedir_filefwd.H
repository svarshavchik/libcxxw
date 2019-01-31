/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_filedir_filefwd_h
#define x_w_filedir_filefwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>
#include <x/functionalrefptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN filedir_fileObj;

/*! What's changed a directory reported by a \ref filedircontents "directory contents monitor".

The callback passed to the monitor object gets repeatedly invoke with this
object passed as a parameter, reporting both the initial contents of the
monitored directory and any subsequent changes to it.

This object contains a vector of filenames in the directory indicating that
the file was added or removed. It is expected that the vector gets processed
in order, in order to maintain an accurate snapshot of the directory's
contents.

*/

typedef ref<filedir_fileObj> filedir_file;

//! A constant \ref filedir_file "changes to files in a directory".

//! \see filedir_file

typedef const_ref<filedir_fileObj> const_filedir_file;

//! A nullable pointer reference to a \ref filedir_file "changes to files in a directory".

//! \see filedir_file

typedef ptr<filedir_fileObj> filedir_fileptr;

//! A nullable pointer reference to a const \ref filedir_file "changes to files in a directory".

//! \see filedir_file

typedef const_ptr<filedir_fileObj> const_filedir_fileptr;


//! A callback function that receives the next chunk of files read.

typedef functionref<void (const const_filedir_file &)> filedir_callback_t;


LIBCXXW_NAMESPACE_END

#endif
