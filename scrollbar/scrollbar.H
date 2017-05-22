/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef scrollbar_h
#define scrollbar_h

#include "scrollbar/scrollbarfwd.H"
#include "scrollbar/scrollbarobj.H"
#include "scrollbar/scrollbar_metrics.H"
#include <x/ref.H>
#include <x/functional.H>

#include "x/w/factoryfwd.H"

LIBCXXW_NAMESPACE_START

//! Scrollbar's initial configuration.

//! This is specified when creating a new scrollbar, or when updating it.

struct scrollbar_config {

	//! The range of the scrollbar's value.

	scroll_v_t range;

	//! Scrollbar page size.

	//! This translates to the size of the scrollbar's handle. Must be
	//! at least 1.
	scroll_v_t page_size;

	//! The current value of the scrollbar.

	//! When the scrollbar gets created, this is the initial value.
	//! The current value of the scrollbar will always be
	//! between 0 and range-page_size.
	scroll_v_t value;

	//! Increment size.

	//! How much the scrollbar's up and down buttons adjust its value
	scroll_v_t increment;
};

struct scrollbar_impl_init_params;

//! The signature for the factory for the scrollbar implementation object.

//! The callback gets passed to create_horizontal_scrollbar() or
//! create_vertical_scrollbar. The callback's job is to create() a subclass of
//! scrollbarObj::implObj and return it. The callback gets the
//! initialization parameters for scrollbarObj::implObj that simply need to
//! be forwarded to its consructor.

typedef ref<scrollbarObj::implObj>
scrollbar_impl_constructor(const scrollbar_impl_init_params &);

//! Implement create_horizontal_scrollbar().

scrollbar do_create_h_scrollbar(const factory &f,
				const scrollbar_config &conf,
				const function<scrollbar_impl_constructor>
				&create_impl) LIBCXX_HIDDEN;

//! Implement create_vertical_scrollbar().

scrollbar do_create_v_scrollbar(const factory &f,
				const scrollbar_config &conf,
				const function<scrollbar_impl_constructor>
				&create_impl) LIBCXX_HIDDEN;

//! Create a horizontal scrollbar.

//! To do that, we need a factory, that expects this scrollbar to be created
//! as its element, a \ref scrollbar_config "scrollbar_config", and a
//! \ref scrollbar_impl_constructor "factory for the scrollbar
//!implementation object".

template<typename impl_factory_t>
inline scrollbar create_horizontal_scrollbar(const factory &f,
					     const scrollbar_config &conf,
					     impl_factory_t &&impl_factory)
{
	return do_create_h_scrollbar(f, conf,
				     make_function<scrollbar_impl_constructor>
				     (std::forward<impl_factory_t>(impl_factory)
				      ));
}

//! Create a vertical scrollbar.

//! Like create_horizontal_scrollbar(), but makes a vertical one.

template<typename impl_factory_t>
inline scrollbar create_vertical_scrollbar(const factory &f,
					   const scrollbar_config &conf,
					   impl_factory_t &&impl_factory)
{
	return do_create_v_scrollbar(f, conf,
				     make_function<scrollbar_impl_constructor>
				     (std::forward<impl_factory_t>(impl_factory)
				      ));
}

LIBCXXW_NAMESPACE_END

#endif
