/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "messages.H"
#include "dialog.H"
#include "catch_exceptions.H"
#include "print_dialog/print_dialog_impl.H"
#include "x/w/print_dialog_config.H"
#include "x/w/input_field.H"
#include "x/w/button.H"
#include "x/w/image_button.H"
#include "x/w/radio_group.H"
#include "x/w/gridfactory.H"
#include "x/w/label.H"
#include "x/w/main_windowobj.H"
#include "x/w/standard_comboboxlayoutmanager.H"
#include "x/w/focusable_container.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/booklayoutmanager.H"
#include "x/w/error_message.H"
#include <x/weakptr.H>
#include <x/mpobj.H>
#include <x/threads/run.H>

LIBCXXW_NAMESPACE_START

print_dialogObj::print_dialogObj(const print_dialog_args &args)
	: dialogObj{args.args}, impl{args.print_dialog_impl}
{
}

print_dialogObj::~print_dialogObj()=default;

print_dialog_config::~print_dialog_config()=default;

namespace {
#if 0
};
#endif

/*! Which came first, the chicken or the egg.

A helper object captured by callbacks attached to the print dialog's
display elements.

The callbacks invoke the print dialog's methods, hence they need to
weakly-capture the dialog object. This is simple enough. However these
display elements themselves cannot be created until the dialog gets
created first. So, this object gets created first, the dialog object
gets created, together with its display elements, the once the dialog
object is fully cooked, its weakly captured and stashed in here. Then
the callback have full access to it.

That's really a little white lie. It's possible to create the bare dialog
object first and weakly capture it before creating its display elements.
But this is simply easier, and cleaner.

 */

class LIBCXX_HIDDEN print_dialog_parentObj : virtual public obj {


 public:

	print_dialog_parentObj()=default;

	~print_dialog_parentObj()=default;

	//! The parent print dialog object, weakly captured.

	mpobj<weakptr<print_dialogptr>> parent;

	//! Recover the weakly-captured dialog object.

	//! If we succeed in recovering it, pass it to the closure.
	template<typename f> inline void invoke_closure(f &&F)
	{
		auto p=({
				mpobj<weakptr<print_dialogptr>>::lock l{parent};

				l->getptr();
			});

		if (p)
			F(p);
	}

	//! A new printer was selected.

	void printer_selected(const list_item_status_info_t &i)
	{
		if (!i.selected)
			return;

		invoke_closure([&]
			       (const auto &dialog)
			       {
				       dialog->impl->show_printer
					       (i.item_number,
						i.mcguffin);
			       });
	}
};

//! Helper object for creating the print dialog.

struct LIBCXX_HIDDEN print_dialog_init_helper {

	//! Who is creating the dialog.
	main_window parent_window;

	//! The new fields, created as we go along.
	print_dialog_fieldsptr fields;

	//! Radio group for the "pages" button.

	radio_group pages_radio_group=radio_group::create();

	//! Return the element factories for the new dialog.

	//! The returned value gets passed to initialize_theme_dialog().
	standard_dialog_elements_t
		create_elements(const print_dialog_config &conf,
				const ref<print_dialog_parentObj> &parent);
};

#if 0
{
#endif
};

standard_dialog_elements_t print_dialog_init_helper
::create_elements(const print_dialog_config &conf,
		  const ref<print_dialog_parentObj> &parent)
{
	return {
		{"print-dialog-general-label",
				[&]
				(const auto &factory)
				{
					factory->create_label(_("General"));
				}},
		{"print-dialog-page-label",
				[&]
				(const auto &factory)
				{
					factory->create_label(_("Page Options"))
						;
				}},
		{"print-dialog-image-label",
				[&]
				(const auto &factory)
				{
					factory->create_label(_("Image "
								"Options"));
				}},
		{"select-printer-label",
				[&]
				(const auto &factory)
				{
					factory->create_label
						(_("Printer:"));
				}},
		{"select-printer-field",
				[&, this]
				(const auto &factory)
				{
					new_listlayoutmanager nlm{
						highlighted_list};

					nlm.selection_type=
						single_selection_type;

					nlm.selection_changed=
						[parent](const auto &info)
						{
							parent->printer_selected
								(info);
						};

					nlm.height=4;

					auto f=factory
						->create_focusable_container
						([](const auto &){}, nlm);

					fields.selected_printer=f;
				}},
		{"printer-info",
				[&, this]
				(const auto &factory)
				{
					auto l=factory->create_label("");

					fields.printer_info=l;
				}},
		{"number-of-copies-label",
				[&]
				(const auto &factory)
				{
					factory->create_label
						(_("Number of copies:"));
				}},
		{"number-of-copies-field",
				[&, this]
				(const auto &factory)
				{
					input_field_config conf{4, 1, true};
					auto f=factory->create_input_field
						("", conf);
					f->autofocus(false);
					fields.number_of_copies=f;
				}},
		{"all-pages-radio-button",
				[&, this]
				(const auto &factory)
				{
					auto f=factory->create_radio
						(pages_radio_group,
						 []
						 (const auto &factory)
						 {
							 factory->create_label
							 (_("All pages"));
						 });
					fields.all_pages_radio_button=f;
				}},
		{"page-range-radio-button",
				[&, this]
				(const auto &factory)
				{
					auto f=factory->create_radio
						(pages_radio_group,
						 []
						 (const auto &factory)
						 {
							 factory->create_label
							 (_("Pages: "));
						 });
					fields.page_range_radio_button=f;
				}},
		{"page-range-field",
				[&, this]
				(const auto &factory)
				{
					input_field_config conf{8, 1, true};
					auto f=factory->create_input_field
						("", conf);
					f->autofocus(false);
					fields.page_range=f;
				}},
		{"orientation-label",
				[&]
				(const auto &factory)
				{
					factory->create_label
						(_("Orientation:"));
				}},
		{"orientation-field",
				[&, this]
				(const auto &factory)
				{
					new_standard_comboboxlayoutmanager nlm;

					nlm.selection_required=false;
					auto f=factory
						->create_focusable_container
						([](const auto &){}, nlm);
					fields.orientation_requested=f;
				}},
		{"duplex-label",
				[&]
				(const auto &factory)
				{
					factory->create_label(_("Duplex:"));
				}},
		{"duplex-field",
				[&, this]
				(const auto &factory)
				{
					new_standard_comboboxlayoutmanager nlm;

					nlm.selection_required=false;
					auto f=factory
						->create_focusable_container
						([](const auto &){}, nlm);
					fields.sides=f;
				}},
		{"pages-per-side-label",
				[&]
				(const auto &factory)
				{
					factory->create_label
						(_("Pages Per Side:"));
				}},
		{"pages-per-side-field",
				[&, this]
				(const auto &factory)
				{
					new_standard_comboboxlayoutmanager nlm;

					nlm.selection_required=false;
					auto f=factory
						->create_focusable_container
						([](const auto &){}, nlm);
					fields.number_up=f;
				}},
		{"page-size-label",
				[&]
				(const auto &factory)
				{
					factory->create_label
						(_("Page size:"));
				}},
		{"page-size-field",
				[&, this]
				(const auto &factory)
				{
					new_standard_comboboxlayoutmanager nlm;

					nlm.selection_required=false;

					auto f=factory
						->create_focusable_container
						([](const auto &){}, nlm);
					fields.page_size=f;
				}},
		{"finishings-label",
				[&]
				(const auto &factory)
				{
					factory->create_label
						(_("Finishing process:"));
				}},
		{"finishings-field",
				[&, this]
				(const auto &factory)
				{
					new_standard_comboboxlayoutmanager nlm;

					nlm.selection_required=false;
					auto f=factory
						->create_focusable_container
						([](const auto &){}, nlm);
					fields.finishings=f;
				}},
		{"print-color-mode-label",
				[&]
				(const auto &factory)
				{
					factory->create_label
						(_("Color Mode:"));
				}},
		{"print-color-mode-field",
				[&, this]
				(const auto &factory)
				{
					new_standard_comboboxlayoutmanager nlm;

					nlm.selection_required=false;
					auto f=factory
						->create_focusable_container
						([](const auto &){}, nlm);
					fields.print_color_mode=f;
				}},
		{"print-quality-label",
				[&]
				(const auto &factory)
				{
					factory->create_label
						(_("Print Quality:"));
				}},
		{"print-quality-field",
				[&, this]
				(const auto &factory)
				{
					new_standard_comboboxlayoutmanager nlm;

					nlm.selection_required=false;
					auto f=factory
						->create_focusable_container
						([](const auto &){}, nlm);
					fields.print_quality=f;
				}},
		{"printer-resolution-label",
				[&]
				(const auto &factory)
				{
					factory->create_label
						(_("Resolution:"));
				}},
		{"printer-resolution-field",
				[&, this]
				(const auto &factory)
				{
					new_standard_comboboxlayoutmanager nlm;

					nlm.selection_required=false;
					auto f=factory
						->create_focusable_container
						([](const auto &){}, nlm);
					fields.printer_resolution=f;
				}},
		{"ok", dialog_ok_button(_("Print"), fields.ok_button, '\n')},
		{"filler", dialog_filler()},
		{"cancel", dialog_cancel_button(_("Cancel"),
						fields.cancel_button,
						'\e')}
	};
}

print_dialog main_windowObj
::create_print_dialog(const std::string_view &dialog_id,
		      const print_dialog_config &conf,
		      bool modal)
{
	auto future_parent=ref<print_dialog_parentObj>::create();

	auto cancel_callback_impl=
		functionref<void ()>::create
		([cb=conf.cancel_callback,
		  me=make_weak_capture(ref(this))]
		 ()
		 {
			 if (!cb)
				 return;

			 auto got=me.get();

			 if (!got)
				 return;

			 auto &[mw]=*got;


			 try {
				 cb();
			 } REPORT_EXCEPTIONS(mw);
		 });

	print_dialog_init_helper helper{ref(this)};

	auto d=create_custom_dialog
		(dialog_id,
		 [&]
		 (const dialog_args &args)
		 {
			 auto containers=args.dialog_window
				->initialize_theme_dialog
				("print-dialog",
				  helper.create_elements(conf,
							 future_parent), {});

			 auto iter=containers.find("print-dialog-options");

			 if (iter == containers.end())
				 throw EXCEPTION("Internal error: dialog "
						 "container was not created.");

			 helper.fields.options_book=iter->second;

			 auto impl=ref<print_dialogObj::implObj>
				 ::create(ref(this),
					  cancel_callback_impl,
					  helper.fields);

			 impl->fields.ok_button->autofocus(true);
			 return print_dialog::create(print_dialog_args{
					 args, impl});
		 },
		 modal);

	future_parent->parent=d;

	// Finish initializing the print and cancel button, by constructing
	// and installing their callbacks.

	auto print_callback_impl=
		functionref<print_callback_t>::create
		([cb=conf.print_callback]
		 (const cups::job &j, const ref<obj> &mcguffin)
		 {
			 if (cb)
				 cb(j, mcguffin);
		 });

	hide_and_invoke_when_activated(d, d->impl->fields.ok_button,
				       [what=make_weak_capture(d, ref(this)),
					callback=print_callback_impl]
				       (const busy &mcguffin)
				       {
					       auto got=what.get();

					       if (!got)
						       return;

					       auto &[d, mw]=*got;

					       d->impl->print(mw, callback);
				       });

	hide_and_invoke_when_activated(d, d->impl->fields.cancel_button,
				       [cancel_callback_impl]
				       (const busy &mcguffin)
				       {
					       cancel_callback_impl->invoke();
				       });

	hide_and_invoke_when_closed(d,
				    [cancel_callback_impl]
				    (const busy &mcguffin)
				    {
					    cancel_callback_impl->invoke();
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

	// TODO -- invoke cancel_callback_impl if an exception gets thrown
	// in the lambda.

	run_lambda([]
		   (const auto &my_main_window,
		    const auto &mcguffin, const auto &me)
		   {
			   error_message_config config;

			   config.acknowledged_callback=
				   [cb=me->impl->cancel_callback]
				   {
					   (*cb)();
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