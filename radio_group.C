/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "radio_group.H"
#include "radio_button.H"
#include "busy.H"
#include <x/weaklist.H>

LIBCXXW_NAMESPACE_START

radio_groupObj::radio_groupObj() : button_list{button_list_t::create()}
{
}

radio_groupObj::~radio_groupObj()=default;

void radio_groupObj::turn_off(ONLY IN_THREAD,
			      const radio_button &turned_on,
			      const container_impl &parent_container,
			      busy_impl &busy,
			      const callback_trigger_t &trigger)
{
	// Invoke all callbacks after updating all radio buttons' state.

	std::vector<radio_button> turned_off_buttons;

	turned_off_buttons.reserve(button_list->size());

	// Turn off all other radio buttons...

	for (const auto &buttonptr : *button_list)
	{
		auto buttonp=buttonptr.getptr();

		if (!buttonp)
			continue;

		radio_button button=buttonp;

		// ... except me.

		if (button != turned_on)
			turned_off_buttons.push_back(button);
	}

	for (const auto &b:turned_off_buttons)
		b->turn_off(IN_THREAD, parent_container, busy, trigger);
}

LIBCXXW_NAMESPACE_END
