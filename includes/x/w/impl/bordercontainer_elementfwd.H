/*
** Copyright 2017-2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_bordercontainer_elementfwd_h
#define x_w_impl_bordercontainer_elementfwd_h

#include <x/w/impl/bordercontainer_impl.H>
#include <x/w/impl/current_border_implfwd.H>
#include <x/w/impl/themedim_elementfwd.H>
#include <x/w/connection_threadfwd.H>

LIBCXXW_NAMESPACE_START

struct borderhpad_tag;
struct bordervpad_tag;

//! A mixin for inheriting from a display element, and a bordercontainer_implObj.

//! The template parameter must be a subclass of containerObj::implObj.
//! This template class multiply-inherits from the template parameter,
//! and two dims that specify additional padding between the border and the
//! display element inside it.
//!
//! The first parameter to the constructor is the initial current_border_impl,
//! followed by two border_args, and the remaining parameters get forwarded
//! to the superclass.
//!
//! \note
//! This template mixin is not responsible for invoking current_border_impl's
//! initialize(). Both the initial current border's, and the new set_border()'s
//! callers are responsible for this.

template<typename element_impl>
class bordercontainer_elementObj
	: public themedim_elementObj<element_impl,
				     borderhpad_tag,
				     bordervpad_tag>,
	  public bordercontainer_implObj {

	//! Alias.
	typedef themedim_elementObj<element_impl,
				    borderhpad_tag,
				    bordervpad_tag> superclass_t;

	//! The current border.
	current_border_impl current_border_thread_only;
 public:
	THREAD_DATA_ONLY(current_border);

	template<typename ...Args>
	bordercontainer_elementObj(const current_border_impl &initial_border,
				   const dim_arg &hpad,
				   const dim_arg &vpad,
				   Args && ...args);

	~bordercontainer_elementObj();

	containerObj::implObj &get_container_impl() override;
	const containerObj::implObj &get_container_impl() const override;

	current_border_impl get_border(ONLY IN_THREAD) const override;
	void set_border(ONLY IN_THREAD,
			const current_border_impl &) override;
	dim_t get_border_hpad(ONLY IN_THREAD) const override;
	dim_t get_border_vpad(ONLY IN_THREAD) const override;
};


LIBCXXW_NAMESPACE_END

#endif
