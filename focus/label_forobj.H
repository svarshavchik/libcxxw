/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef label_forobj_h
#define label_forobj_h

#include "focus/label_forfwd.H"
#include "focus/focusablefwd.H"
#include "connection_threadfwd.H"
#include "x/w/elementobj.H"
#include <x/weakptr.H>
#include <x/functional.H>

LIBCXXW_NAMESPACE_START

//! Label to focusable link.

//! \see label_for

class LIBCXX_HIDDEN label_forObj : virtual public obj {

	//! The label element.
	weakptr<ptr<elementObj::implObj>> label_thread_only;

	//! The focusable element.
	weakptr<focusable_implptr> focusable_thread_only;

 public:

	THREAD_DATA_ONLY(label);
	THREAD_DATA_ONLY(focusable);

	//! Constructor

	label_forObj(const ref<elementObj::implObj> &label,
		     const focusable_impl &focusable);

	//! Destructor

	~label_forObj();

	//! Signature for the with_link() callback.

	typedef void with_link_t(const ref<elementObj::implObj> &label,
				 const focusable_impl &focusable);

	//! Invoke a callback, passing it the linked objects.

	//! All references are weak. The callback gets invoked only if
	//! both references can be converted to strong references (the link
	//! still exists).

	template<typename functor_type>
		inline void with_link(IN_THREAD_ONLY, functor_type &&f)
	{
		do_with_link(IN_THREAD, make_function<with_link_t>
			     (std::forward<functor_type>(f)));
	}

	//! Invoke a callback, passing it the linked objects.
	void do_with_link(IN_THREAD_ONLY,
			  const function<with_link_t> &callback);
};

LIBCXXW_NAMESPACE_END

#endif
