/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef switchfactory_impl_h
#define switchfactory_impl_h

#include "x/w/switchfactory.H"
#include "x/w/containerfwd.H"
#include "x/w/alignment.H"
#include "switchlayoutmanager_impl.H"
#include <x/mpobj.H>
#include <optional>

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

class LIBCXX_HIDDEN switchfactoryObj::implObj : virtual public obj {

 public:

	class appendObj;
	class insertObj;

	//! My layout manager

	const switchlayoutmanager lm;

	//! Constructor
	implObj(const switchlayoutmanager &);

	//! Destructor
	~implObj();

	//! All the metadata for new elements.

	struct info_s {

		//! Most recent intermediate container for the new element.
		ptr<containerObj::implObj> prev_container_impl;

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
