/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_filedircontentsobj_h
#define x_w_filedircontentsobj_h

#include "dirlisting/filedircontentsfwd.H"
#include "dirlisting/filedir_filefwd.H"
#include <x/obj.H>

#include <string>
LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see filedircontents

class LIBCXX_HIDDEN filedircontentsObj : virtual public obj {

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	filedircontentsObj(const std::string &directory,
			   const filedir_callback_t &callback);

	//! Destructor
	~filedircontentsObj();

	//! Return the directory name that was passed to the constructor

	std::string directory() const;
};

LIBCXXW_NAMESPACE_END

#endif
