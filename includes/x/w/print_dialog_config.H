/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_print_dialog_config_h
#define x_w_print_dialog_config_h

#include <x/w/print_dialog_configfwd.H>
#include <x/w/busy.H>
#include <x/cups/job.H>
#include <functional>

LIBCXXW_NAMESPACE_START

//! Parameter passed to the print dialog's callback.

//! A discrete object as a parameter, for future expandability.

struct print_callback_info {

	//! The print job created by the print dialog.

	cups::job job;

	//! The print dialog conveniently created a busy-wait mcguffin.

	//! Because it takes a while to print something.

	ref<obj> wait_mcguffin;
};

//! Callback used by the "Print" button in the \ref print_dialog "print dialog".

typedef void print_callback_t(const print_callback_info &);

//! The configuration of a print dialog.

//! \see print_dialog.

struct print_dialog_config {

	//! Callback, "Print" selected, dialog closed

	std::function<print_callback_t> print_callback;

	//! Callback, dialog closed.
	std::function<void ()> cancel_callback;

	//! Destructor
	~print_dialog_config();
};

LIBCXXW_NAMESPACE_END

#endif
