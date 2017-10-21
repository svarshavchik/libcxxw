/*
** Copyright 2017 Double Precision, Inc.
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

	struct init_args;
public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor, phase 1.

	file_dialogObj(const dialog_args &args,
		       const file_dialog_config &conf)
		LIBCXX_HIDDEN;

	//! Constructor, phase 2.
	void constructor(const dialog_args &args,
			 const file_dialog_config &conf)
		LIBCXX_HIDDEN;

private:
	//! Internal constructor
	file_dialogObj(const dialog_args &d_args,
		       const file_dialog_config &conf,
		       init_args &&)
		LIBCXX_HIDDEN;

public:
	//! Destructor
	~file_dialogObj() LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
