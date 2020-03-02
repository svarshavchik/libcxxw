/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_layoutmanagerobj_h
#define x_w_layoutmanagerobj_h

#include <x/w/layoutmanagerfwd.H>
#include <x/w/batch_queuefwd.H>
#include <x/w/uielementsfwd.H>
#include <x/w/uigeneratorsfwd.H>
#include <x/obj.H>

#include <string_view>

LIBCXXW_NAMESPACE_START

//! Methods common to all layout managers.

//! \see layoutmanager

class layoutmanagerObj : virtual public obj {

public:
	class LIBCXX_HIDDEN implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Internal implementation object.

	const batch_queue queue;

	//! Constructor
	layoutmanagerObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~layoutmanagerObj() LIBCXX_HIDDEN;

	//! Create the contents from a predefined theme template.

	//! Use the generators from the current display theme.
	void generate(const std::string_view &name,
		      uielements &);

	//! Create the contents from a predefined theme template.

	//! The default implementation does nothing.
	//! Overridden by implementing layout manager.
	virtual void generate(const std::string_view &name,
			      const const_uigenerators &generators,
			      uielements &);
};

//! Alias
typedef ref<layoutmanagerObj::implObj> layout_impl;

//! Alias
typedef const_ref<layoutmanagerObj::implObj> const_layout_impl;

//! Alias
typedef ptr<layoutmanagerObj::implObj> layout_implptr;

//! Alias
typedef const_ptr<layoutmanagerObj::implObj> const_layout_implptr;

LIBCXXW_NAMESPACE_END

#endif
