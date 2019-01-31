/*
** Copyright 2018-2019 Double Precision, Inc.
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
#include <x/w/impl/richtext/richtextfwd.H>

LIBCXXW_NAMESPACE_START

//! A mixin for a container that uses the \ref borderlayoutmanager "border layout manager".

//! \see bordercontainer_elementObj

class bordercontainer_implObj : virtual public obj {

 public:

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

	virtual current_border_impl get_left_border(ONLY IN_THREAD) const=0;
	virtual current_border_impl get_right_border(ONLY IN_THREAD) const=0;
	virtual current_border_impl get_top_border(ONLY IN_THREAD) const=0;
	virtual current_border_impl get_bottom_border(ONLY IN_THREAD) const=0;

	//! Implemented by the mixin template, to set my border.

	virtual void set_border(ONLY IN_THREAD,
				const border_arg &left_border,
				const border_arg &right_border,
				const border_arg &top_border,
				const border_arg &bottom_border)=0;

	//! Implemented by the mixin container, to get the title.
	virtual richtextptr get_title(ONLY IN_THREAD) const=0;

	//! Implemented by the mixin container, to set the title.
	virtual void set_title(ONLY IN_THREAD, const richtextptr &)=0;

	//! Get title indentation
	virtual dim_t get_title_indent(ONLY IN_THREAD) const=0;

	//! Additional padding.

	//! Implemented by the mixin template, to return the additional
	//! padding between the border and the element.

	virtual dim_t get_border_hpad(ONLY IN_THREAD) const=0;

	//! Additional padding.

	//! Implemented by the mixin template, to return the additional
	//! padding between the border and the element.

	virtual dim_t get_border_vpad(ONLY IN_THREAD) const=0;


	//! Initialize the title in the connection thread.

	//! Helper used by the bordercontainer_element mixin.

	void initialize(ONLY IN_THREAD);
};

LIBCXXW_NAMESPACE_END

#endif
