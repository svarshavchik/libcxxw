/*
** Copyright 2017-2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_bordercontainer_elementfwd_h
#define x_w_impl_bordercontainer_elementfwd_h

#include <x/w/impl/bordercontainer_impl.H>
#include <x/w/impl/current_border_implfwd.H>
#include <x/w/impl/themedim_elementfwd.H>
#include <x/w/impl/themeborder_elementfwd.H>
#include <x/w/connection_threadfwd.H>

LIBCXXW_NAMESPACE_START

struct borderhpad_tag;
struct bordervpad_tag;
struct titleindent_tag;

struct left_border_tag;
struct right_border_tag;
struct top_border_tag;
struct bottom_border_tag;
template<typename element_impl>
class LIBCXX_PUBLIC bordercontainer_elementObj;

//! A mixin for inheriting from a display element, and a bordercontainer_implObj.

//! The template parameter must be a subclass of containerObj::implObj.
//! This template class multiply-inherits from the template parameter,
//! and two dims that specify additional padding between the border and the
//! display element inside it.
//!
//! The first four parameters to the constructor are the four border_args
//! that set the initial borders, the remaining two arguments are any
//! additional padding, and the remaining parameters get forwarded
//! to the superclass.

template<typename element_impl>
class bordercontainer_elementObj
	: public themeborder_elementObj<themedim_elementObj<element_impl,
							    titleindent_tag,
							    borderhpad_tag,
							    bordervpad_tag>,
					left_border_tag,
					right_border_tag,
					top_border_tag,
					bottom_border_tag>,

// bordercontainer_implObj's constructor takes a ref to the container, which
// we can do because we constructed the superclass first.

	  public bordercontainer_implObj {

	//! Alias.
	typedef themeborder_elementObj<themedim_elementObj<element_impl,
							   titleindent_tag,
							   borderhpad_tag,
							   bordervpad_tag>,
				       left_border_tag,
				       right_border_tag,
				       top_border_tag,
				       bottom_border_tag> superclass_t;

	//! Current title

	richtextptr title_thread_only;

public:
	THREAD_DATA_ONLY(title);

	template<typename ...Args>
	bordercontainer_elementObj(const border_arg &left_border,
				   const border_arg &right_border,
				   const border_arg &top_border,
				   const border_arg &bottom_border,
				   const richtextptr &title,
				   const dim_arg &title_indent,
				   const dim_arg &hpad,
				   const dim_arg &vpad,
				   Args && ...args);

	~bordercontainer_elementObj();

	containerObj::implObj &get_container_impl() override;
	const containerObj::implObj &get_container_impl() const override;

	current_border_impl get_left_border(ONLY IN_THREAD) const override;
	current_border_impl get_right_border(ONLY IN_THREAD) const override;
	current_border_impl get_top_border(ONLY IN_THREAD) const override;
	current_border_impl get_bottom_border(ONLY IN_THREAD) const override;

	void set_border(ONLY IN_THREAD,
			const border_arg &left_border,
			const border_arg &right_border,
			const border_arg &top_border,
			const border_arg &bottom_border) override;
	richtextptr get_title(ONLY IN_THREAD) const override;
	dim_t get_title_indent(ONLY IN_THREAD) const override;
	dim_t get_border_hpad(ONLY IN_THREAD) const override;
	dim_t get_border_vpad(ONLY IN_THREAD) const override;
	void initialize(ONLY IN_THREAD) override;
	void theme_updated(ONLY IN_THREAD,
			   const defaulttheme &new_theme) override;
};


LIBCXXW_NAMESPACE_END

#endif
