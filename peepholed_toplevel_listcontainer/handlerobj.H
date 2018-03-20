/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_toplevel_listcontainer_handlerobj_h
#define peepholed_toplevel_listcontainer_handlerobj_h

#include "peepholed_toplevel_listcontainer/handlerfwd.H"
#include "popup/popup_attachedto_handlerobj.H"
#include "background_color.H"

LIBCXXW_NAMESPACE_START

//! Parameters to \ref peepholed_toplevel_listcontainer_handler "peepholed list container popup handler"'s constructor.

struct LIBCXX_HIDDEN peepholed_toplevel_listcontainer_handler_args {
	const color_arg &topleft_color;
	const color_arg &bottomright_color;
	const popup_attachedto_handler_args &popup_args;
};

//! Implementation object.

//! \see peepholed_toplevel_listcontainer_handler

class LIBCXX_HIDDEN peepholed_toplevel_listcontainer_handlerObj
	: public popup_attachedto_handlerObj {

	typedef popup_attachedto_handlerObj superclass_t;

	//! Which affinity is used to set the current list popup background

	//! do_create_peepholed_toplevel_listcontainer_popup initializes the
	//! list popup's background to the topright background color, so that's
	//! the opening bid.

	popup_position_affinity current_affinity=popup_position_affinity::above;

	//! Background color when attached to left or above

	const background_color topleft_color;

	//! Background color when attached to right or below.

	const background_color bottomright_color;

 public:

	//! Constructor
	peepholed_toplevel_listcontainer_handlerObj
		(const peepholed_toplevel_listcontainer_handler_args &);

	//! Destructor
	~peepholed_toplevel_listcontainer_handlerObj();

	//! Override recalculate_popup_position()
	popup_position_affinity recalculate_popup_position(IN_THREAD_ONLY,
							   rectangle &r,
							   dim_t screen_width,
							   dim_t screen_height)
		override;
};

LIBCXXW_NAMESPACE_END

#endif
