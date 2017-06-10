/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef listlayoutmanager_h
#define listlayoutmanager_h

#include "x/w/listlayoutmanager.H"
#include "gridlayoutmanager.H"
#include "listlayoutmanager/listcontainerobj.H"
#include <x/functional.H>

LIBCXXW_NAMESPACE_START

//! The list layout manager implementation object.

//! The list layout manager is really the grid layout manager in disguise.
//!
//! \see listlayoutmanager

class LIBCXX_HIDDEN listlayoutmanagerObj::implObj
	: public gridlayoutmanagerObj::implObj {

 public:
	const ref<listcontainerObj::implObj> container_impl;

	//! This list layout manager's style.
	const listlayoutstyle &style;

	//! Number of columns
	const size_t columns;

	//! Constructor
	implObj(const ref<listcontainerObj::implObj> &container_impl,
		const listlayoutstyle &style,
		size_t columns);

	//! Destructor
	~implObj();

	//! Override create_public_object to create the listlayoutmanager.

	layoutmanager create_public_object() override;
};

template<typename builder_t>
container create_listlayoutmanager(const factory &f,
				   size_t columns,
				   const listlayoutstyle &style,
				   const list_padding &padding,
				   builder_t &&builder)
	LIBCXX_HIDDEN;

//! Create a new list layout manager.

//! The first parameter is the parent container's factory. The number of
//! columns, the style, and the padding gets specified.
//!
//! The lambda is the builder for the new list layout manager.

template<typename builder_t>
inline container create_listlayoutmanager(const factory &f,
					  size_t columns,
					  const listlayoutstyle &style,
					  const list_padding &padding,
					  builder_t &&builder)
{
	return create_listlayoutmanager_impl
		(f, columns, style, padding,
		 make_function<void (const container &)>
		 (std::forward<builder_t>(builder)));
}

//! Type-erased create_listlayoutmanager().

container create_listlayoutmanager_impl(const factory &f,
					size_t columns,
					const listlayoutstyle &style,
					const list_padding &padding,
					const function<void (const container &)>
					&builder)
	LIBCXX_HIDDEN;

LIBCXXW_NAMESPACE_END

#endif
