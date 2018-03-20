/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "peepholed_toplevel_listcontainer/handler.H"
#include "gridlayoutmanager.H"

LIBCXXW_NAMESPACE_START

peepholed_toplevel_listcontainer_handlerObj
::peepholed_toplevel_listcontainer_handlerObj
(const peepholed_toplevel_listcontainer_handler_args &args)
	: superclass_t{args.popup_args},
	  topleft_color{this->create_background_color(args.topleft_color)},
	  bottomright_color{this->create_background_color
			  (args.bottomright_color)}
{
}

peepholed_toplevel_listcontainer_handlerObj
::~peepholed_toplevel_listcontainer_handlerObj()=default;

popup_position_affinity
peepholed_toplevel_listcontainer_handlerObj
::recalculate_popup_position(IN_THREAD_ONLY,
			     rectangle &r,
			     dim_t screen_width,
			     dim_t screen_height)
{
	auto adjusted_current_affinity=current_affinity;

	current_affinity=superclass_t::recalculate_popup_position
		(IN_THREAD, r,
		 screen_width,
		 screen_height);

	auto adjusted_new_affinity=current_affinity;

	switch (adjusted_current_affinity) {
	case popup_position_affinity::left:
		adjusted_current_affinity=
			popup_position_affinity::above;
		break;

	case popup_position_affinity::right:
		adjusted_current_affinity=
			popup_position_affinity::below;
		break;
	case popup_position_affinity::above:
	case popup_position_affinity::below:
		break;
	}

	switch (adjusted_new_affinity) {
	case popup_position_affinity::left:
		adjusted_new_affinity=popup_position_affinity::above;
		break;

	case popup_position_affinity::right:
		adjusted_new_affinity=
			popup_position_affinity::below;
		break;
	case popup_position_affinity::above:
	case popup_position_affinity::below:
		break;
	}

	if (adjusted_current_affinity == adjusted_new_affinity)
		return current_affinity;

	invoke_layoutmanager
		([&]
		 (const ref<gridlayoutmanagerObj::implObj> &peephole_lm)
		 {
			 // The top level element is a grid with a
			 // peephole being element (0, 0) in the grid.

			 auto i=peephole_lm->get(0, 0)->impl;

			 auto &c=adjusted_new_affinity ==
				 popup_position_affinity::above
				 ? topleft_color
				 : bottomright_color;

			 i->set_background_color(IN_THREAD, c);
		 });

	return current_affinity;
}

LIBCXXW_NAMESPACE_END
