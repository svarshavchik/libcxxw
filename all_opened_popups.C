/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "all_opened_popups.H"
#include "popup/popup.H"
#include "generic_window_handler.H"

LIBCXXW_NAMESPACE_START

class all_opened_popupsObj::handler_mcguffinObj : virtual public obj {

public:
	weakptr<ptr<popupObj::handlerObj>> handler;

	handler_mcguffinObj(const ref<popupObj::handlerObj> &handler)
		: handler(handler)
	{
	}

	void hide(IN_THREAD_ONLY)
	{
		auto p=handler.getptr();

		if (!p)
			return;

		p->request_visibility(IN_THREAD, false);
	}
};

all_opened_popupsObj::all_opened_popupsObj()
{
}

all_opened_popupsObj::~all_opened_popupsObj()=default;


ref<obj> all_opened_popupsObj
::opening_combobox_popup(IN_THREAD_ONLY,
			 const ref<popupObj::handlerObj> &popup)
{
	auto mcguffin=ref<handler_mcguffinObj>::create(popup);

	close_combobox_popup(IN_THREAD);

	opened_combobox_popup=mcguffin;

	return mcguffin;
}

void all_opened_popupsObj::close_combobox_popup(IN_THREAD_ONLY)
{
	auto p=opened_combobox_popup.getptr();

	if (p)
		p->hide(IN_THREAD);
}

LIBCXXW_NAMESPACE_END
