/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "messages.H"
#include "dialog.H"
#include "catch_exceptions.H"
#include "print_dialog/print_dialog_impl.H"
#include "x/w/uielements.H"
#include "x/w/print_dialog_config.H"
#include "x/w/print_dialog_appearance.H"
#include "x/w/list_appearance.H"
#include "x/w/image_button_appearance.H"
#include "x/w/input_field_appearance.H"
#include "x/w/combobox_appearance.H"
#include "x/w/input_field.H"
#include "x/w/button.H"
#include "x/w/image_button.H"
#include "x/w/gridfactory.H"
#include "x/w/label.H"
#include "x/w/main_windowobj.H"
#include "x/w/standard_comboboxlayoutmanager.H"
#include "x/w/focusable_container.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/booklayoutmanager.H"
#include "x/w/stop_message.H"
#include "x/w/validated_input_field.H"
#include <x/mpweakptr.H>
#include <x/threads/run.H>

LIBCXXW_NAMESPACE_START

print_dialogObj::print_dialogObj(const print_dialog_args &args)
	: dialogObj{args.args}, impl{args.print_dialog_impl}
{
}

print_dialogObj::~print_dialogObj()=default;

print_dialog_config_settings::~print_dialog_config_settings()=default;

print_dialog_config_appearance::print_dialog_config_appearance()
	: appearance{print_dialog_appearance::base::theme()}
{
}

print_dialog_config_appearance
::print_dialog_config_appearance(const const_print_dialog_appearance
				 &appearance)
	: appearance{appearance}
{
}

print_dialog_config_appearance
::print_dialog_config_appearance(const print_dialog_config_appearance &)
=default;

print_dialog_config_appearance &print_dialog_config_appearance::operator=
(const print_dialog_config_appearance &)=default;

print_dialog_config_appearance::~print_dialog_config_appearance()=default;

namespace {
#if 0
};
#endif

//! Helper object for creating the print dialog.

//! The create_elements() method returns the standard_dialog_elements_t
//! parameter that gets passed to initialize_theme_dialog(), to populate
//! the contents of the font dialog.

struct LIBCXX_HIDDEN print_dialog_init_helper {

	//! Who is creating the dialog.
	main_window parent_window;

	//! The new fields, created as we go along.
	print_dialog_fieldsptr fields;

	//! Return the element factories for the new dialog.

	//! The returned value gets passed to initialize_theme_dialog().
	standard_dialog_elements_t
		create_elements(const print_dialog_config &conf);
};

#if 0
{
#endif
};

standard_dialog_elements_t print_dialog_init_helper
::create_elements(const print_dialog_config &conf)
{
	return {
		{"ok", dialog_ok_button(_("Print"), fields.ok_button, '\n')},
		{"cancel", dialog_cancel_button(_("Cancel"),
						fields.cancel_button,
						'\e')}
	};
}

// Dialog creator factored out of create_print_dialog() for readability.

static print_dialog create_new_print_dialog(
	const dialog_args &args,
	print_dialog_init_helper &helper,
	const print_dialog_config &conf,
	const mpweakptr<print_dialogptr> &future_parent,
	const functionref<void (THREAD_CALLBACK)> &cancel_callback_impl,
	const main_window &me)
{
	// Prepare to generate the dialog from the theme file.

	uielements tmpl{helper.create_elements(conf)};

	// We will attach a selection changed callback to the printer list

	tmpl.layout_creators.emplace(
		"select-printer-field",
		[parent=future_parent]
		(const listlayoutmanager &lm)
		{
			lm->on_selection_changed(
				[=]
				(ONLY IN_THREAD,
				 const auto &info)
				{
					auto p=parent->getptr();

					if (!p)
						return;

					auto dialog_impl=p->impl;

					// Enable the OK button when a printer
					// is selected, disable it when a
					// printer gets deselected, for some
					// reason.
					dialog_impl->fields.ok_button
						->set_enabled(IN_THREAD,
							      info.selected);

					// Once a printer is selected, show its
					// particulars
					if (!info.selected)
						return;

					dialog_impl->show_printer(
						info.item_number,
						info.mcguffin
					);
				});
		});

	typedef print_dialogObj::implObj implObj;
	auto printer_info=implObj::printer_info_t::create();

	// Install a validator for the page range field that uses
	// cups::parse_range_string().

	tmpl.create_validated_input_field
		("page-range-field",
		 []
		 (THREAD_CALLBACK,
		  const std::string &value,
		  const auto &lock,
		  const auto &ignore)
		 -> std::optional<std::vector<std::tuple<int, int>>>
		 {
			 auto v=cups::parse_range_string(value);

			 if (!v)
				 lock.stop_message
					 (_("Invalid page range specified"));

			 return v;
		 },
		 []
		 (const std::optional<std::vector<std::tuple<int, int>>> &v)
		 -> std::string
		 {
			 if (!v || v->empty())
				 return "";

			 return cups::range_to_string(*v);

		 }
		);

	// Install a validator for the number of copies field.
	tmpl.create_string_validated_input_field<int>(
		"number-of-copies-field",
		[printer_info]
		(THREAD_CALLBACK,
		 const std::string &value,
		 std::optional<int> &parsed_value,
		 const auto &lock,
		 const auto &ignore)
		{
			if (parsed_value)
			{
				implObj::printer_info_lock lock{printer_info};

				if (lock->number_of_copies.empty())
				{
					// Option does not specify
					// values, a small sanity
					// check.
					if (*parsed_value > 0)
						return;
				}

				for (const auto &r: lock->number_of_copies)
				{
					auto &[from, to]=r;

					if (*parsed_value >= from &&
					    *parsed_value <= to)
						return;
				}
			}

			if (!value.empty())
				lock.stop_message
					(_("Invalid \"number of copies\""
					   " value"));
			parsed_value.reset();
		},
		[]
		(int n)
		{
			return std::to_string(n);
		},

		// Initialize the field to 1 by default
		1
	);

	// Run the generator, fetch out all the generated fields, and collect
	// them into the print_dialog_fieldsptr helper, which will be used
	// to construct the implementation object that will manage them.

	args.dialog_window->generate("print-dialog", tmpl);

	helper.fields.selected_printer=
		tmpl.get_element("select-printer-field");

	helper.fields.orientation_requested=
		tmpl.get_element("orientation-field");

	helper.fields.sides=
		tmpl.get_element("duplex-field");

	helper.fields.number_up=
		tmpl.get_element("pages-per-side-field");

	helper.fields.page_size=
		tmpl.get_element("page-size-field");

	helper.fields.finishings=
		tmpl.get_element("finishings-field");

	helper.fields.print_color_mode=
		tmpl.get_element("print-color-mode-field");

	helper.fields.print_quality=
		tmpl.get_element("print-quality-field");

	helper.fields.printer_resolution=
		tmpl.get_element("printer-resolution-field");

	helper.fields.printer_info=
		tmpl.get_element("printer-info");

	helper.fields.options_book=
		tmpl.get_element("print-dialog-options");

	helper.fields.number_of_copies=
		tmpl.get_element("number-of-copies-field");

	helper.fields.all_pages_radio_button=
		tmpl.get_element("all-pages-radio-button");
	helper.fields.page_range_radio_button=
		tmpl.get_element("page-range-radio-button");
	helper.fields.page_range=
		tmpl.get_element("page-range-field");

	// And in addition to that grab the validated input field objects and
	// save them too.

	helper.fields.number_of_copies_value=
		tmpl.get_validated_input_field<int>(
			"number-of-copies-field"
		);
	helper.fields.page_ranges_value=
		tmpl.get_validated_input_field<
			std::vector<std::tuple<int, int>>>(
				"page-range-field"
			);

	auto impl=ref<print_dialogObj::implObj>
		::create(me,
			 printer_info,
			 conf.appearance,
			 cancel_callback_impl,
			 helper.fields);

	return print_dialog::create(print_dialog_args{args, impl});
}


print_dialog main_windowObj
::create_print_dialog(const standard_dialog_args &args,
		      const print_dialog_config &conf)
{

	/*
	  Which came first, the chicken or the egg.

	  Use an mpweakptr for a reference to the main dialog.

	  The callbacks invoke the print dialog's methods, hence they need to
	  weakly-capture the dialog object. This is simple enough. However these
	  display elements themselves cannot be created until the dialog gets
	  created first. So, this object gets created first, the dialog object
	  gets created, together with its display elements, the once the dialog
	  object is fully cooked, its weakly captured and stashed in here. Then
	  the callback have full access to it.

	  That's really a little white lie. It's possible to create the bare
	  dialog object first and weakly capture it before creating its display
	  elements. But this is simply easier, and cleaner.
	*/

	auto future_parent=mpweakptr<print_dialogptr>::create();

	functionref<void (THREAD_CALLBACK)> cancel_callback_impl=
		([cb=conf.cancel_callback,
		  me=make_weak_capture(ref(this))]
		 (ONLY IN_THREAD)
		 {
			 if (!cb)
				 return;

			 auto got=me.get();

			 if (!got)
				 return;

			 auto &[mw]=*got;


			 try {
				 cb(IN_THREAD);
			 } REPORT_EXCEPTIONS(mw);
		 });

	auto me=ref{this};
	print_dialog_init_helper helper{me};

	auto d=create_custom_dialog(
		create_dialog_args{args},
		[&]
		(const dialog_args &args)
		{
			return create_new_print_dialog(
				args,
				helper,
				conf,
				future_parent,
				cancel_callback_impl,
				me
			);
		}
	);

	future_parent->setptr(d);

	// Finish initializing the print and cancel button, by constructing
	// and installing their callbacks.

	functionref<print_callback_t> print_callback_impl=
		conf.print_callback
		? functionref<print_callback_t>{conf.print_callback}
		: functionref<print_callback_t>{
		[]
		(const print_callback_info &info)
		{
		}};

	hide_and_invoke_when_activated(me,
				       d, d->impl->fields.ok_button,
				       [what=make_weak_capture(d, ref(this)),
					callback=print_callback_impl]
				       (ONLY IN_THREAD,
					const auto &ignore)
				       {
					       auto got=what.get();

					       if (!got)
						       return;

					       auto &[d, mw]=*got;

					       d->impl->print(mw, callback);
				       });

	// The ok button is initially disabled because no printer is
	// currently selected. The status callback on the printer list
	// list will enable it when the printer is selected
	d->impl->fields.ok_button->set_enabled(false);
	hide_and_invoke_when_activated(me, d, d->impl->fields.cancel_button,
				       [cancel_callback_impl]
				       (ONLY IN_THREAD,
					const auto &ignore)
				       {
					       cancel_callback_impl(IN_THREAD);
				       });

	hide_and_invoke_when_closed(me, d,
				    [cancel_callback_impl]
				    (ONLY IN_THREAD,
				     const auto &ignore)
				    {
					    cancel_callback_impl(IN_THREAD);
				    });

	return d;
}

void print_dialogObj::initial_show()
{
	auto my_main_window=impl->parent_window.getptr();

	if (!my_main_window)
		return;

	// Create a wait mcguffin, and start a new execution thread.

	auto mcguffin=my_main_window->get_wait_busy_mcguffin();

	run_lambda([]
		   (const auto &my_main_window,
		    const auto &mcguffin, const auto &me)
		   {
			   stop_message_config config;

			   config.acknowledged_callback=
				   [cb=me->impl->cancel_callback]
				   (ONLY IN_THREAD)
				   {
					   (*cb)(IN_THREAD);
				   };

			   try {
				   me->impl->enumerate_printers();

				   booklayoutmanager blm=
					   me->impl->fields.options_book
					   ->get_layoutmanager();

				   blm->open(0);

				   me->dialog_window->show_all();
			   } REPORT_EXCEPTIONS_WITH_CONFIG(my_main_window,
							   config);
		   },
		   my_main_window, mcguffin, ref(this));

}

LIBCXXW_NAMESPACE_END
