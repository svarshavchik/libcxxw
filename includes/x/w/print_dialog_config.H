/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_print_dialog_config_h
#define x_w_print_dialog_config_h

#include <x/w/print_dialog_configfwd.H>
#include <x/w/print_dialog_appearancefwd.H>
#include <x/w/connection_threadfwd.H>
#include <x/w/busy.H>
#include <x/cups/job.H>
#include <x/functionalrefptr.H>

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

//! This is not a THREAD_CALLBACK, it gets started in a separate thread.

typedef void print_callback_t(const print_callback_info &);

//! Print dialog configuration's basic settings.

//! Inherited by \ref print_dialog_config "print_dialog_config", making
//! it possible to use aggregate initialization with default initialization
//! of the print dialog's appearance object.

struct print_dialog_config_settings {

	//! Callback, "Print" selected, dialog closed

	functionptr<print_callback_t> print_callback;

	//! Callback, dialog closed.
	functionptr<void (THREAD_CALLBACK)> cancel_callback;

	//! Destructor
	~print_dialog_config_settings();
};

//! Wrapper for the print dialog's appearance.

//! Inherited by \ref print_dialog_config "print_dialog_config", with an
//! option of default initialization without pulling in the entire
//! print dialog appearance object.

struct print_dialog_config_appearance {

	//! The print dialog's appearance.

	const_print_dialog_appearance appearance;

	//! Default constructor
	print_dialog_config_appearance();

	//! Constructor
	print_dialog_config_appearance(const const_print_dialog_appearance &);

	//! Copy constructor
	print_dialog_config_appearance(const print_dialog_config_appearance &);

	//! Assignment operator
	print_dialog_config_appearance &operator=
	(const print_dialog_config_appearance &);

	~print_dialog_config_appearance();
};

//! The configuration of a print dialog.

//! \see print_dialog.

struct print_dialog_config : print_dialog_config_settings,
			     print_dialog_config_appearance
{
};


LIBCXXW_NAMESPACE_END

#endif
