/*
** Copyright 2018-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef panecontainer_impl_h
#define panecontainer_impl_h

#include "x/w/impl/container_elementfwd.H"
#include "x/w/impl/child_element.H"
#include "x/w/impl/theme_font_elementfwd.H"
#include "x/w/metrics/axis.H"

LIBCXXW_NAMESPACE_START

//! Internal implementation object for a container with the \ref panelayoutmanager "pane layout manager".

//! Keep a reference font around, for the purposes of setting the default
//! amount of keyboard-based sliding.

class LIBCXX_HIDDEN panecontainer_implObj
	: public theme_font_elementObj<container_elementObj
					   <child_elementObj>>
{

	//! Alias.

	typedef theme_font_elementObj<container_elementObj
					  <child_elementObj>> superclass_t;

 public:
	//! Constructor
	panecontainer_implObj(const container_impl &parent,
			      const child_element_init_params &init_params);

	//! Destructor
	~panecontainer_implObj();
};

LIBCXXW_NAMESPACE_END

#endif
