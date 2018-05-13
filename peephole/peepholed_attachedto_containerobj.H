/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_peepholed_attachedto_containerobj_h
#define x_w_peepholed_attachedto_containerobj_h

#include "peephole/peepholed_attachedto_containerfwd.H"
#include "peephole/peepholed_fontelementfwd.H"
#include "peephole/peepholed_toplevel_elementfwd.H"
#include "popup/popup_attachedto_info.H"
#include "x/w/containerobj.H"

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see peepholed_attachedto_container

class LIBCXX_HIDDEN peepholed_attachedto_containerObj :
	public peepholed_fontelementObj<peepholed_toplevel_elementObj
					<containerObj>> {

	//! Alias
	typedef peepholed_fontelementObj<peepholed_toplevel_elementObj<
		containerObj>> superclass_t;

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	peepholed_attachedto_containerObj(const popup_attachedto_info &,
					  const ref<implObj> &,
					  const layout_impl &);


	//! Destructor
	~peepholed_attachedto_containerObj();

	//! Override recalculate_peepholed_metrics
	void recalculate_peepholed_metrics(ONLY IN_THREAD,
					   const screen &s) override;

	//! Override max_width
	dim_t max_width(ONLY IN_THREAD) const override;

	//! Override max_height
	dim_t max_height(ONLY IN_THREAD) const override;

 private:
	//! Attachment calculation info for the popup.
	const popup_attachedto_info attachedto_info;

	//! Computed maximum width value.
	dim_t max_width_value;

	//! Computed maximum height value.
	dim_t max_height_value;
};

//! A reference to an \ref peepholed_attachedto_container "peepholed container in an attahced popup"'s implementation object.

typedef ref<peepholed_attachedto_containerObj
	    ::implObj> peepholed_attachedto_container_impl;

//! A constant \ref peepholed_attachedto_container_impl "attached popup container's implementation object".

//! \see peepholed_attachedto_container

typedef const_ref<peepholed_attachedto_containerObj
	    ::implObj> const_peepholed_attachedto_container_impl;

//! A nullable pointer reference to a \ref peepholed_attachedto_container_impl "attached popup container's implementation object".

//! \see peepholed_attachedto_container

typedef ptr<peepholed_attachedto_containerObj
	    ::implObj> peepholed_attachedto_container_implptr;

//! A nullable pointer reference to a const \ref peepholed_attachedto_container_impl "attached popup container's implementation object".

//! \see peepholed_attachedto_container

typedef const_ptr<peepholed_attachedto_containerObj
	    ::implObj> const_peepholed_attachedto_container_implptr;

LIBCXXW_NAMESPACE_END

#endif
