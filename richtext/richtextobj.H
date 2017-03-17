/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef richtextobj_H
#define richtextobj_H

#include "connection_threadfwd.H"
#include "richtext/richtextfwd.H"
#include "richtext/richtextstring.H"
#include "x/w/types.H"
#include <x/obj.H>
#include <x/mpobj.H>

LIBCXXW_NAMESPACE_START

//! A \ref richtext "rich text" object.

class LIBCXX_HIDDEN richtextObj : virtual public obj {


 public:

	class implObj;

 private:
	//! The implementation object is mutex-protected.

	typedef	mpobj<ref<implObj>> impl_t;

	//! The implementation object.

	impl_t impl;

	//! Whether this rich text object is word-wrapped.

	//! If word_wrap_width is 0, this richtext object is not wrapped
	//! (each paragraph consists of a single line); otherwise this is the
	//! targeted word wrap width, in pixels.

	dim_t word_wrap_width_thread_only;

 public:

	THREAD_DATA_ONLY(word_wrap_width);

	//! Constructor
	richtextObj(const ref<implObj> &impl,
		    dim_t word_wrap_width);

	//! Destructor
	~richtextObj();

	//! Set the entire contents of the richtext.

	void set(IN_THREAD_ONLY, richtextstring &string);

	//! For testing purpose.

	ref<implObj> debug_get_impl();
};


LIBCXXW_NAMESPACE_END;
#endif
