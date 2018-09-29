/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_textlabelobj_h
#define x_w_textlabelobj_h

#include <x/w/textlabelfwd.H>
#include <x/w/text_paramfwd.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! A text label interface.

//! \see textlabel

class textlabelObj : virtual public obj {

public:
	class LIBCXX_HIDDEN implObj;

	const ref<implObj> label_impl;

	//! Constructor
	textlabelObj(const ref<implObj> &label_impl) LIBCXX_HIDDEN;

	//! Destructor
	~textlabelObj() LIBCXX_HIDDEN;

	//! Replace the label text.

	void update(const text_param &string);
};

LIBCXXW_NAMESPACE_END

#endif
