/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_attachedto_container_impl_h
#define peepholed_attachedto_container_impl_h

#include "peephole/peepholed_attachedto_container.H"
#include "x/w/impl/theme_font_elementfwd.H"
#include "x/w/impl/always_visiblefwd.H"
#include "x/w/impl/container_elementfwd.H"
#include "x/w/impl/child_elementobj.H"

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

class LIBCXX_HIDDEN peepholed_attachedto_containerObj::implObj
	: public theme_font_elementObj<always_visibleObj<
						   container_elementObj<
							   child_elementObj>>
					   > {

	//! Alias
	typedef theme_font_elementObj<always_visibleObj<
		container_elementObj<
			child_elementObj>>> superclass_t;

 public:
	//! Constructor
	implObj(const container_impl &parent,
		const child_element_init_params &init_params);

	//! Destructor
	~implObj();
};

LIBCXXW_NAMESPACE_END

#endif
