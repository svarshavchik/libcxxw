/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "radio_group.H"
#include "radio_button.H"
#include "image_button_internal_impl.H"
#include "busy.H"

LIBCXXW_NAMESPACE_START

radio_groupObj::implObj::implObj() : button_list(button_list_t::create())
{
}

radio_groupObj::implObj::~implObj()=default;

void radio_groupObj::implObj::turn_off(ONLY IN_THREAD,
				       const radio_button &turned_on,
				       busy_impl &busy,
				       const callback_trigger_t &trigger)
{
	// Invoke all callbacks after updating all radio buttons' state.

	std::vector<radio_button> turned_off_buttons;

	// Turn off all other radio buttons...

	for (const auto &buttonptr : *button_list)
	{
		auto buttonp=buttonptr.getptr();

		if (!buttonp)
			continue;

		radio_button button=buttonp;

		// ... except me.

		if (button != turned_on &&
		    button->turn_off(IN_THREAD))
			turned_off_buttons.push_back(button);
	}

	for (const auto &b:turned_off_buttons)
		b->turned_off(IN_THREAD, busy, trigger);
}

LIBCXXW_NAMESPACE_END
