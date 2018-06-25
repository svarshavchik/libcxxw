/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_bordercontainer_impl_h
#define x_w_impl_bordercontainer_impl_h

#include <x/w/types.H>
#include <x/w/border_arg.H>
#include <x/w/connection_threadfwd.H>
#include <x/w/containerobj.H>
#include <x/w/generic_windowobj.H>
#include <x/w/pictformatfwd.H>
#include <x/w/scratch_bufferfwd.H>
#include <x/w/impl/current_border_implfwd.H>

LIBCXXW_NAMESPACE_START

//! A mixin for a container that uses the \ref borderlayoutmanager "border layout manager".

//! \see bordercontainer_elementObj

class bordercontainer_implObj : virtual public obj {

 public:

	//! Create the initial container border.

	//! Used by template mixins to create the initial current_border_impl().

	static current_border_impl
	initial_bordercontainer_border(const border_arg &initial_border,
				       const container_impl &parent_container);

	//! Scratch buffer for the corner border

	//! For optimal use of scratch buffers, separate scratch buffer are
	//! used for horizontal, vertical, and corner borders. The corner
	//! border uses the element's default scratch buffer.

	const scratch_buffer corner_scratch_buffer;

	//! Scratch buffer for the horizontal border

	//! For optimal use of scratch buffers, separate scratch buffer are
	//! used for horizontal, vertical, and corner borders. The corner
	//! border uses the element's default scratch buffer.

	const scratch_buffer h_scratch_buffer;

	//! Scratch buffer for the horizontal border

	//! For optimal use of scratch buffers, separate scratch buffer are
	//! used for horizontal, vertical, and corner borders. The corner
	//! border uses the element's default scratch buffer.

	const scratch_buffer v_scratch_buffer;

	//! Masking scratch buffer for the display element.

	//! For optimal use of scratch buffers, separate scratch buffer are
	//! used for horizontal, vertical, and corner borders. The corner
	//! border uses the element's default scratch buffer.
	const scratch_buffer corner_mask_buffer;

	//! Masking scratch buffer for the horizontal border

	//! For optimal use of scratch buffers, separate scratch buffer are
	//! used for horizontal, vertical, and corner borders. The corner
	//! border uses the element's default scratch buffer.
	const scratch_buffer h_mask_buffer;

	//! Masking scratch buffer for the vertical border

	//! For optimal use of scratch buffers, separate scratch buffer are
	//! used for horizontal, vertical, and corner borders. The corner
	//! border uses the element's default scratch buffer.
	const scratch_buffer v_mask_buffer;

	//! Constructor
	bordercontainer_implObj(const container_impl &parent_container);

private:
	//! Internal constructor
	bordercontainer_implObj(const container_impl &parent_container,
				generic_windowObj::handlerObj &h);

	//! Internal constructor
	bordercontainer_implObj(const container_impl &parent_container,
				const const_pictformat &my_pictformat,
				const const_pictformat &mask_pictformat,
				const screen &s);
public:

	//! Destructor
	~bordercontainer_implObj();

	//! Implemented by the mixin template, to return my container.

	virtual containerObj::implObj &get_container_impl()=0;

	//! Implemented by the mixin template, to return my container.

	virtual const containerObj::implObj &get_container_impl() const=0;

	//! Implemented by the mixin template, to return my border.

	virtual current_border_impl get_border(ONLY IN_THREAD) const=0;

	//! Implemented by the mixin template, to set my border.

	virtual void set_border(ONLY IN_THREAD,
				const current_border_impl &)=0;

	//! Additional padding.

	//! Implemented by the mixin template, to return the additional
	//! padding between the border and the element.

	virtual dim_t get_border_hpad(ONLY IN_THREAD) const=0;

	//! Additional padding.

	//! Implemented by the mixin template, to return the additional
	//! padding between the border and the element.

	virtual dim_t get_border_vpad(ONLY IN_THREAD) const=0;
};

LIBCXXW_NAMESPACE_END

#endif
