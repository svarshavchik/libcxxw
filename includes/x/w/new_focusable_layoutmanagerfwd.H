/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_new_focusable_layoutmanagerfwd_h
#define x_w_new_focusable_layoutmanagerfwd_h

#include <x/w/namespace.H>
#include <x/w/focusable_containerfwd.H>
#include <x/w/focusable_containerobj.H>
#include <x/w/text_paramfwd.H>
#include <x/w/input_fieldfwd.H>

#include <x/functional.H>
#include <tuple>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN new_focusable_layoutmanager;

//! A new focusable layout manager

//! A new focusable container's layout manager gets created before the
//! \ref focusable_container "focusable_container" gets created.
//!
//! A subclass of new_focusable_layoutmanager gets passed to methods that create
//! focusable layout managers.

class new_focusable_layoutmanager {

public:

	//! Constructor
	new_focusable_layoutmanager();

	//! Destructor
	~new_focusable_layoutmanager();

	//! Internal implementation
	virtual focusable_container create(
		const container_impl &parent,
		const function<void (const focusable_container &)> &creator
	) const=0;

	//! Internal implementation
	virtual std::tuple<focusable_container, input_field> create(
		const container_impl &parent,
		const function<void (const focusable_container &)> &creator,
		const text_param &initial_contents,
		const input_field_validation_callback &callback,
		bool validated
	) const;
};

LIBCXXW_NAMESPACE_END

#endif
