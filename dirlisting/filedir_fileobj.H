/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_filedir_fileobj_h
#define x_w_filedir_fileobj_h

#include "dirlisting/filedir_filefwd.H"
#include <x/obj.H>
#include <vector>
#include <string>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see filedir_file

class LIBCXX_HIDDEN filedir_fileObj : virtual public obj {

public:

	//! files that are added or removed in the directory.

	struct file {

		std::string name; //!< Filename

		bool removed; //!< false: file in a directory, true: file removed
		//! Constructor

		file(const std::string &name, bool removed)
			: name(name), removed(removed)
		{
		}
	};

	//! List of files that were added or removed.

	//! Key is the filename; false - added, true - removed

	std::vector<file> files;

	//! Constructor
	filedir_fileObj();

	//! Destructor
	~filedir_fileObj();
};

LIBCXXW_NAMESPACE_END

#endif
