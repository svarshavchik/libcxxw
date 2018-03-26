/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef print_dialog_impl_h
#define print_dialog_impl_h

#include "x/w/print_dialog.H"
#include "x/w/print_dialog_config.H"
#include "x/w/dialog.H"
#include "x/w/input_fieldfwd.H"
#include "x/w/main_windowfwd.H"
#include "x/w/buttonfwd.H"
#include "x/w/image_buttonfwd.H"
#include "x/w/focusable_containerfwd.H"
#include "x/w/labelfwd.H"
#include "x/w/busyfwd.H"
#include <x/ptrfwd.H>
#include <x/weakptr.H>
#include <x/mpobj.H>
#include <x/functionalrefptr.H>
#include <x/cups/available.H>
#include <x/cups/jobfwd.H>
#include <x/functionalrefptr.H>
#include <functional>

LIBCXXW_NAMESPACE_START

#pragma GCC visibility push(hidden)
#include "print_dialog/print_dialog.inc.H"
#pragma GCC visibility pop

//! Internal implementation object.

class LIBCXX_HIDDEN print_dialogObj::implObj : virtual public obj {

	//! Helper object.

	//! Stores the list of available printers, and the currently selected
	//! printer's possible options.

	struct printer_info_s {

		//! The printers in the available printer list.
		std::vector<cups::available> available_printers;

		//! Which one is currently selected. Updated by the callback.
		cups::availableptr currently_selected_printer;

		// The dropdown combo-boxes contain descriptions of each
		// possible option, and we store the internal CUPS value for
		// each option in these vectors.

		std::vector<std::tuple<int, int>> number_of_copies;
		std::vector<std::string> page_size;
		std::vector<std::string> orientation;
		std::vector<std::string> sides;
		std::vector<std::string> number_up;
		std::vector<std::string> finishings;
		std::vector<std::string> print_color_mode;
		std::vector<std::string> print_quality;
		std::vector<std::string> printer_resolution;
	};

	//! Mutex-protected helper object.

	typedef mpobj<printer_info_s, std::recursive_mutex
		      > printer_info_mpobj_t;

	//! Reference-counted mutex-protected helper object.

	//! We put the mutex-protected printer_info into a discrete object,
	//! so that its ref can be captured by various callbacks that cannot
	//! conveniently capture this implementaiton object itself.

	struct printer_infoObj
		: public printer_info_mpobj_t, virtual public obj {};

	typedef ref<printer_infoObj> printer_info_t;

	//! Printer metadata

	printer_info_t printer_info;

	//! Wrapper for a printer_info_mpobj_t

	//! Provides a lock object with a helpful -> operator.

	struct printer_info_lock : public printer_info_mpobj_t::lock {

		inline printer_info_lock(const printer_info_t &info)
			: printer_info_mpobj_t::lock{*info}
		{
		}

		using printer_info_mpobj_t::lock::operator->;
	};

 public:

	//! The dialog's parent.
	const weakptr<main_windowptr> parent_window;

	//! The dialog's cancel callback.

	const functionref<void (THREAD_CALLBACK)> cancel_callback;

	//! The display elements for the various options.

	const print_dialog_fields fields;

	//! Validator attached to the "number of copies" field.
	const validated_input_field<int> number_of_copies_value;

	//! Validator attached to the "page ranges" field.
	const validated_input_field<std::vector<std::tuple<int, int>>
				    > page_ranges_value;

	//! Constructor
	implObj(const main_window &parent_window,
		const functionref<void (THREAD_CALLBACK)> &cancel_callback,
		const print_dialog_fieldsptr &fields);

	//! Destructor
	~implObj();

	//! Before opening the dialog, initialize all the fields.

	void enumerate_printers();

	//! Show information about the given printer.

	//! Callback executed when a printer get picked in the
	//! selected_printer combo-box.

	void show_printer(size_t i,
			  const busy &mcguffin);

	//! Print

	//! Callback attached to the "Print" button invokes this.

	void print(const main_window &from_window,
		   const functionref<print_callback_t> &cb);

 private:

	//! Locked the printer_info, and initialize all fields for this printer.
	void show_printer(printer_info_lock &lock,
			  const cups::available &printer);

	//! Construct the cups::job object.
	cups::jobptr create_print_job();
};

struct LIBCXX_HIDDEN print_dialog_args {

	const dialog_args &args;

	ref<print_dialogObj::implObj> print_dialog_impl;
};

LIBCXXW_NAMESPACE_END

#endif
