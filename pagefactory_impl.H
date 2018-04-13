/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef pagefactory_impl_h
#define pagefactory_impl_h

#include "x/w/pagefactory.H"
#include "x/w/containerfwd.H"
#include "x/w/alignment.H"
#include "pagelayoutmanager_impl.H"
#include <x/mpobj.H>
#include <optional>

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

class LIBCXX_HIDDEN pagefactoryObj::implObj : virtual public obj {

 public:

	class appendObj;
	class insertObj;

	//! My layout manager

	const pagelayoutmanager lm;

	//! Constructor
	implObj(const pagelayoutmanager &);

	//! Destructor
	~implObj();

	//! All the metadata for new elements.

	struct info_s {

		//! Most recent intermediate container for the new element.
		container_implptr prev_container_impl;

		//! Alignment for the next element to be created.
		LIBCXXW_NAMESPACE::halign horizontal_alignment=
			LIBCXXW_NAMESPACE::halign::center;

		//! Alignment for the next element to be created.
		LIBCXXW_NAMESPACE::valign vertical_alignment=
			LIBCXXW_NAMESPACE::valign::middle;
	};

	//! Container for the new element metadata

	typedef mpobj<info_s> info_t;

	//! New element metadata.

	info_t info;

	//! Created new element, while holding a lock on the info.

	virtual void created_under_lock(const switch_element_info &)=0;
};

LIBCXXW_NAMESPACE_END

#endif
