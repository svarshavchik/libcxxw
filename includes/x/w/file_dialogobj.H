/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_file_dialogobj_h
#define x_w_file_dialogobj_h

#include <x/w/file_dialogfwd.H>
#include <x/w/dialogfwd.H>
#include <x/w/dialogobj.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see file_dialog

class file_dialogObj : public dialogObj {

public:
	struct init_args;

	class LIBCXX_HIDDEN implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor, phase 1.

	file_dialogObj(const dialog_args &d_args,
		       const file_dialog_config &conf,
		       const init_args &args)
		LIBCXX_HIDDEN;

	//! Constructor, phase 2.
	void constructor(const dialog_args &d_args,
			 const file_dialog_config &conf,
			 const init_args &args)
		LIBCXX_HIDDEN;

	//! Destructor
	~file_dialogObj() LIBCXX_HIDDEN;

	//! Switch directories.

	//! Typically used before show()ing the file dialog.
	void chdir(const std::string &path);
};

LIBCXXW_NAMESPACE_END

#endif
