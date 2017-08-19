/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef all_opened_popupsobj_h
#define all_opened_popupsobj_h

#include "all_opened_popupsfwd.H"
#include "popup/popup_handler.H"
#include "connection_threadfwd.H"
#include "x/w/generic_windowobj.H"

#include <x/weaklist.H>
#include <x/weakptr.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see all_opened_popups

class LIBCXX_HIDDEN all_opened_popupsObj : virtual public obj {

	class handler_mcguffinObj;

	//! Currently opened combobox popup.

	weakptr<ptr<handler_mcguffinObj>> opened_combobox_popup;

public:
	//! Constructor
	all_opened_popupsObj() LIBCXX_HIDDEN;

	//! Destructor
	~all_opened_popupsObj() LIBCXX_HIDDEN;

	//! A new combo-box popup is being opened.

	//! Invokes close_combobox_popup(). Returns a mcguffin for the
	//! now-visible combo-box popup.

	ref<obj> opening_combobox_popup(IN_THREAD_ONLY,
					const ref<popupObj::handlerObj> &);

	//! Close the currently-opened combo-box popup, if there is one.
	void close_combobox_popup(IN_THREAD_ONLY);

};

LIBCXXW_NAMESPACE_END

#endif
