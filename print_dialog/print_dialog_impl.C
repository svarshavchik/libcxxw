/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "print_dialog/print_dialog_impl.H"
#include "x/w/main_window.H"
#include "x/w/button.H"
#include "x/w/image_button.H"
#include "x/w/input_field.H"
#include "x/w/label.H"
#include "x/w/standard_comboboxlayoutmanager.H"
#include "x/w/text_param_literals.H"
#include "x/w/text_param.H"
#include "x/w/busy.H"
#include "messages.H"
#include "run_as.H"
#include "catch_exceptions.H"
#include <sstream>
#include <iterator>
#include <algorithm>
#include <x/cups/available.H>
#include <x/cups/destination.H>
#include <x/cups/job.H>
#include <x/functional.H>
#include <cups/cups.h>
#include <x/ymdhms.H>
#include <x/visitor.H>
#include <x/locale.H>
#include <x/threads/run.H>
#include <courier-unicode.h>

LIBCXXW_NAMESPACE_START

print_dialogObj::implObj::implObj(const main_window &parent_window,
				  const functionref<void (THREAD_CALLBACK)>
				  &cancel_callback,
				  const print_dialog_fieldsptr &fields)
	: printer_info{ref<printer_infoObj>::create()},
	  parent_window{parent_window},
	  cancel_callback{cancel_callback},
	  fields{fields},
	  number_of_copies_value{
		  fields.number_of_copies->set_string_validator
			  ([printer_info=this->printer_info]
			   (THREAD_CALLBACK,
			    const std::string &value,
			    int *parsed_value,
			    const input_field &f,
			    const auto &ignore) -> std::optional<int>
			   {
				   if (parsed_value)
				   {
					   printer_info_lock lock{printer_info};

					   if (lock->number_of_copies.empty())
					   {
						   // Option does not specify
						   // values, a small sanity
						   // check.
						   if (*parsed_value > 0)
							   return *parsed_value;
					   }

					   for (const auto &r:
							lock->number_of_copies)
					   {
						   auto &[from, to]=r;

						   if (*parsed_value >= from &&
						       *parsed_value <= to)
							   return *parsed_value;
					   }
				   }

				   if (!value.empty())
					   f->stop_message
						   (_("Invalid number of "
						      "copies value"));
				   return std::nullopt;
			   },
			   []
			   (int n)
			   {
				   return std::to_string(n);
			   })},

	  // Use cups::parse_range_string to validate the page ranges field.
	  page_ranges_value{
		  fields.page_range->set_validator
			  ([]
			   (THREAD_CALLBACK,
			    const std::string &value,
			    const input_field &f,
			    const auto &ignore)
			   -> std::optional<std::vector<std::tuple<int, int>>>
			   {
				   auto v=cups::parse_range_string(value);

				   if (!v)
					   f->stop_message
						   (_("Invalid page range "
						      "specified"));

				   return v;
			   },
			   []
			   (const std::optional<std::vector<std::tuple
			    <int, int>>> &v) -> std::string
			   {
				   if (!v || v->empty())
					   return "";

				   return cups::range_to_string(*v);

			   })}
{
	number_of_copies_value->set(1);

	fields.number_of_copies->on_spin
		([number_of_copies_value=this->number_of_copies_value]
		 (ONLY IN_THREAD,
		  const auto &trigger,
		  const auto &mcguffin)
		 {
			 auto n=number_of_copies_value->validated_value
				 .get().value_or(1);

			 if (--n)
			 {
				 number_of_copies_value->set(n);
			 }
		 },
		 [number_of_copies_value=this->number_of_copies_value,
		  printer_info=this->printer_info]
		 (ONLY IN_THREAD,
		  const auto &trigger,
		  const auto &mcguffin)
		 {
			 auto n=number_of_copies_value->validated_value
				 .get().value_or(0);

			 ++n;

			 printer_info_lock lock{printer_info};

			 if (lock->number_of_copies.empty())
				 return;

			 for (const auto &r:lock->number_of_copies)
			 {
				 auto &[from, to]=r;

				 if (n >= from && n <= to)
				 {
					 number_of_copies_value->set(n);
					 return;
				 }
			 }

		 });

	// The page range input field is enabled only when its radio button
	// is enabled.
	fields.page_range_radio_button->on_activate
		([page_range=fields.page_range]
		 (THREAD_CALLBACK,
		  size_t n,
		  const auto &trigger,
		  const auto &busy)
		 {
			 if (n)
			 {
				 page_range->set_enabled(true);
			 }
			 else
			 {
				 page_range->set_enabled(false);
			 }
		 });
}

print_dialogObj::implObj::~implObj()=default;

void print_dialogObj::implObj::enumerate_printers()
{
	printer_info_lock lock{printer_info};

	lock->available_printers=cups::available_destinations();
	lock->currently_selected_printer=nullptr;

	std::vector<list_item_param> printer_list;

	printer_list.reserve(lock->available_printers.size());

	auto l=locale::base::global();

	// Grab each printer's description. If there is none, use its name.

	for (const auto &printer:lock->available_printers)
	{
		auto options=printer->options();

		auto name=options.find("printer-info");

		auto n=name == options.end()
			? printer->name():name->second;

		auto ustr=unicode::iconvert::tou::convert(n, l->charset())
			.first;

		printer_list.emplace_back(text_param{
				theme_font{ printer->is_discovered()
						? "printer_remote_font"
						: "printer_local_font" },
				ustr
					});
	}

	listlayoutmanager selected_printer_list=
		fields.selected_printer->get_layoutmanager();

	size_t i=0;

	for (const auto &printer:lock->available_printers)
	{
		if (printer->is_default())
		{
			lock->currently_selected_printer=printer;
			break;
		}
		++i;
	}

	selected_printer_list->replace_all_items(printer_list);
	if (lock->currently_selected_printer)
	{
		selected_printer_list->autoselect(i);
		show_printer(lock, lock->currently_selected_printer);
	}
}

void print_dialogObj::implObj::show_printer(size_t i,
					    const busy &mcguffin)
{
	printer_info_lock lock{printer_info};

	auto printer=lock->available_printers.at(i);

	// enumerate_printers() calls show_printer() followed by autoselect().
	// It explicitly calls show_printer() so that all initialization occurs
	// in enumerate_printers().
	//
	// However autoselect() results in show_printer() getting called again,
	// by the printer combo-box's selection callback. Therefore, we
	// check if the printer is already selected, here, and do nothing.

	if (lock->currently_selected_printer==printer)
		return;

	lock->currently_selected_printer=printer;

	// This can be time-consuming. Do this in a separate execution thread.

	run_lambda([]
		   (const auto &me,
		    const auto &printer,
		    const auto &busy_mcguffin)
		   {
			   auto mw=me->parent_window.getptr();

			   if (!mw)
				   return;

			   try {
				   printer_info_lock lock{me->printer_info};

				   me->show_printer(lock, printer);
			   } REPORT_EXCEPTIONS(mw);

		   }, ref(this),
		   printer, mcguffin.get_wait_busy_mcguffin());
}

// Prepare combo-box value list for enumerated values.

// Initialize the "options" for the combo-box, setting the default_option
// to be shown, initially. Set "values" to be the list of the enumerated
// value, as string, that corresponds to each option, with "default_value"
// being the user-specified default value for the option.
//
// The enumerated option values are supplied as "v".

// 1) v is an enumeration.

static inline
void set_combobox_values(std::vector<list_item_param> &options,
			 const char *option_name,
			 std::optional<size_t> &default_option,
			 std::vector<std::string> &option_values,
			 const std::optional<std::string> &default_value,
			 const std::unordered_map<int, std::u32string> &v)
{
	// Copy all the keys.
	std::vector<int> keys;

	keys.reserve(v.size());

	for (const auto &s:v)
		keys.push_back(s.first);

	// Translate all enumerated text to lowercase, before sorting it.
	{
		std::unordered_map<int, std::u32string> uv;

		for (const auto &vm:v)
		{
			auto s=vm.second;

			std::transform(s.begin(), s.end(), s.begin(),
				       unicode_lc);

			uv.emplace(vm.first, s);
		}

		// Sort the keys according to their values.
		std::sort(keys.begin(),
			  keys.end(),
			  [&]
			  (int a, int b)
			  {
				  return uv.at(a) < uv.at(b);
			  });
	}

	options.reserve(keys.size());
	option_values.reserve(keys.size());

	for (auto k:keys)
	{
		options.push_back(v.at(k));

		std::ostringstream o;

		o << k;

		auto s=o.str();

		if (default_value && s==*default_value)
		{
			default_option=option_values.size();
		}

		option_values.push_back(s);
	}
}

// 2) v are strings

static inline
void set_combobox_values(std::vector<list_item_param> &options,
			 std::optional<size_t> &default_option,
			 std::vector<std::string> &option_values,
			 const std::optional<std::string> &default_value,
			 const std::unordered_map<std::string,
			 std::u32string> &v)
{
	// Copy all the keys.
	std::vector<std::string> keys;

	keys.reserve(v.size());

	for (const auto &s:v)
		keys.push_back(s.first);

	// Translate all enumerated text to lowercase, before sorting it.
	{
		std::unordered_map<std::string, std::u32string> uv;

		for (const auto &vm:v)
		{
			auto s=vm.second;

			std::transform(s.begin(), s.end(), s.begin(),
				       unicode_lc);

			uv.emplace(vm.first, s);
		}

		// Sort the keys according to their values.
		std::sort(keys.begin(),
			  keys.end(),
			  [&]
			  (const std::string &a, const std::string &b)
			  {
				  return uv.at(a) < uv.at(b);
			  });
	}

	options.reserve(keys.size());
	option_values.reserve(keys.size());

	for (auto k:keys)
	{
		options.push_back(v.at(k));

		if (default_value && k==*default_value)
		{
			default_option=option_values.size();
		}

		option_values.push_back(k);
	}
}

// 3) v are print resolutions

static inline
void set_combobox_values(std::vector<list_item_param> &options,
			 std::optional<size_t> &default_option,
			 std::vector<std::string> &option_values,
			 const std::optional<std::string> &default_value,
			 const std::vector<cups::resolution> &v)
{
	options.reserve(v.size());
	option_values.reserve(v.size());

	for (std::string res:v)
	{
		options.push_back(res);

		if (default_value && res==*default_value)
		{
			default_option=option_values.size();
		}

		option_values.push_back(res);
	}
}

// 4) v are integers

static inline
void set_combobox_values(std::vector<list_item_param> &options,
			 std::optional<size_t> &default_option,
			 std::vector<std::string> &option_values,
			 const std::optional<std::string> &default_value,
			 const std::unordered_set<int> &v)
{
	std::vector<int> keys{v.begin(), v.end()};

	std::sort(keys.begin(), keys.end());

	options.reserve(keys.size());
	option_values.reserve(keys.size());

	for (auto k:keys)
	{
		auto s=std::to_string(k);

		options.push_back(s);

		if (default_value && s==*default_value)
		{
			default_option=option_values.size();
		}

		option_values.push_back(s);
	}
}

// Parse the printer default value setting.

// set_combobox_values() expects a simple string representation of the default
// value. Parse it.

static inline std::optional<std::string>
get_printer_default(const cups::option_values_t &v)
{
	return std::visit(visitor {
			[](const std::unordered_map<int,
			    std::u32string> &v) -> std::optional<std::string>
			{
				if (!v.empty())
				{
					// Expect one value.

					std::ostringstream o;

					o << v.begin()->first;

					return o.str();
				}

				return std::nullopt;
			},
			[](const std::unordered_map<std::string,
			   std::u32string> &v) -> std::optional<std::string>
			{
				if (!v.empty())
					return v.begin()->first;

				return std::nullopt;
			},
			[](const std::vector<cups::resolution> &v)
				-> std::optional<std::string>
			{
				if (!v.empty())
					return std::string{*v.begin()};

				return std::nullopt;
			},
			[](const auto &v) -> std::optional<std::string>
			{
				return std::nullopt;
			}
		}, v);
}

// Initialize a combo-box for a printing option.
//
// Check if the option is supported by the printer, if not disable the combo-
// box. Otherwise:
//
// Take care of figuring out the option's default value, and pass it to the
// supplied callback, together with the list of list_item_params for the
// combo-box.

static void set_combobox_values(const focusable_container &c,
				const char *name,
				const std::unordered_map<std::string,
				std::string> &user_defaults,
				const cups::destination &printer,
				std::vector<std::string> &option_values)
{
	standard_comboboxlayoutmanager lm=c->get_layoutmanager();

	std::vector<list_item_param> options;

	std::optional<size_t> default_option;

	option_values.clear();

	if (printer->supported(name))
	{
		// If this is a supported option, we retrieve its value,
		// in unicode, and enable the combo-box.

		auto unicode_name=std::string{"{unicode}"}+name;

		// Get the user-specified default value for the option.

		std::optional<std::string> default_value;

		{
			auto user_default_iter=user_defaults.find(name);

			if (user_default_iter != user_defaults.end())
				default_value=user_default_iter->second;
			else
			{
				auto printer_defaults=printer
					->default_option_values(unicode_name);

				default_value=
					get_printer_default(printer_defaults);
			}
		}

		std::visit(visitor {
				[&](const std::unordered_map<int,std::u32string>
				    &v)
				{
					set_combobox_values
						(options,
						 name,
						 default_option,
						 option_values,
						 default_value,
						 v);
				},
				[&](const std::unordered_map<std::string,
				    std::u32string> &v)
				{
					set_combobox_values
						(options,
						 default_option,
						 option_values,
						 default_value,
						 v);
				},
				[&](const std::vector<cups::resolution> &v)
				{
					set_combobox_values
						(options,
						 default_option,
						 option_values,
						 default_value,
						 v);
				},
				[&](const std::unordered_set<int> &v)
				{
					set_combobox_values
						(options,
						 default_option,
						 option_values,
						 default_value,
						 v);
				},
				[](const auto &) {}},
			printer->option_values(unicode_name));

		c->set_enabled(true);
	}
	else
	{
		// Not supported, disable the combo-box.

		c->set_enabled(false);

		// But put one value in there, so the combo-box's height
		// remains the same.
		options.push_back(" ");
	}

	lm->replace_all_items(options);

	if (default_option)
		lm->autoselect(*default_option);
}

// A new printer job is being prepared. Read the value of the combo-box,
// and use it to set the job option.
//
// The option is representable as a string.

static void set_combobox_option(const focusable_container &c,
				const char *name,
				const cups::destination &printer,
				std::vector<std::string> &option_values,
				const cups::job &job)
{
	if (!printer->supported(name))
		return;

	standard_comboboxlayoutmanager lm=c->get_layoutmanager();

	auto selected=lm->selected();

	if (!selected)
		return;

	job->set_option(name, option_values.at(*selected));
}

void print_dialogObj::implObj::show_printer(printer_info_lock &lock,
					    const cups::available &printer)
{
	auto info=printer->info();

	auto user_defaults=info->user_defaults();

	auto options=printer->options();

	text_param printer_info;

	printer_info("printer_make_and_model"_theme_font);

	auto iter=options.find("printer-make-and-model");

	if (iter != options.end() && iter->second.size())
	{
		printer_info(iter->second);
	}
	else
	{
		printer_info(" ");
	}
	printer_info("\n");

	iter=options.find("printer-location");

	printer_info("printer_location"_theme_font);

	if (iter != options.end() && iter->second.size())
	{
		printer_info(iter->second);
	}
	else
	{
		printer_info(" ");
	}
	printer_info("\n");

	printer_info("printer_status"_theme_font);

	iter=options.find("printer-is-accepting-jobs");

	if (iter == options.end() || iter->second != "false")
	{
		printer_info(_("Available"));
	}
	else
	{
		printer_info(_("Not available"));
	}

	iter=options.find("printer-state");

	std::string state;

	if (iter != options.end())
	{
		if (iter->second == "3")
		{
			state=_("idle since ");
		}
		else if (iter->second == "4")
		{
			state=_("printing since ");
		}
		else if (iter->second == "5")
		{
			state=_("stopped since ");
		}
	}

	if (!state.empty())
	{
		auto l=locale::base::global();
		bool valid=false;

		iter=options.find("printer-state-change-time");

		if (iter != options.end())
		{
			std::istringstream i{iter->second};

			time_t t;

			if (i >> t)
			{
				struct tm result;

				ymdhms right_now{
					*localtime_r(&t, &result)
						};

				auto fmt=right_now.short_format();

				fmt.toString(std::back_insert_iterator{state},
					      l);
				valid=true;
			}
		}
		if (valid)
		{
			printer_info(", ");
			printer_info(unicode::iconvert::tou
				     ::convert(state,l->charset()).first);
		}
	}

	fields.printer_info->update(printer_info);

	lock->number_of_copies.clear();

	if (info->supported(CUPS_COPIES))
	{
		fields.number_of_copies->set_enabled(true);

		auto iter=user_defaults.find(CUPS_COPIES);

		auto values=info->option_values(CUPS_COPIES);

		if (std::holds_alternative<std::vector<std::tuple<int, int>>>
		    (values))
			lock->number_of_copies=std::get<
				std::vector<std::tuple<int, int>>>(values);

		if (iter != user_defaults.end())
		{
			std::istringstream i{iter->second};

			unsigned n;

			if (i >> n)
				number_of_copies_value->set(n);
		}
	}
	else
	{
		fields.number_of_copies->set_enabled(false);
	}

	fields.all_pages_radio_button->set_value(1);

	if (info->supported("page-ranges"))
	{
		fields.all_pages_radio_button->set_enabled(true);
		fields.page_range_radio_button->set_enabled(true);
	}
	else
	{
		fields.all_pages_radio_button->set_enabled(false);
		fields.page_range_radio_button->set_enabled(false);
	}
	fields.page_range->set("");

	set_combobox_values(fields.page_size,
			    CUPS_MEDIA,
			    user_defaults,
			    info,
			    lock->page_size);

	set_combobox_values(fields.sides,
			    CUPS_SIDES,
			    user_defaults,
			    info,
			    lock->sides);

	set_combobox_values(fields.number_up,
			    CUPS_NUMBER_UP,
			    user_defaults,
			    info,
			    lock->number_up);

	set_combobox_values(fields.orientation_requested,
			    CUPS_ORIENTATION,
			    user_defaults,
			    info,
			    lock->orientation);

	set_combobox_values(fields.finishings,
			    CUPS_FINISHINGS,
			    user_defaults,
			    info,
			    lock->finishings);

	set_combobox_values(fields.print_color_mode,
			    CUPS_PRINT_COLOR_MODE,
			    user_defaults,
			    info,
			    lock->print_color_mode);

	set_combobox_values(fields.print_quality,
			    CUPS_PRINT_QUALITY,
			    user_defaults,
			    info,
			    lock->print_quality);

	set_combobox_values(fields.printer_resolution,
			    "printer-resolution",
			    user_defaults,
			    info,
			    lock->printer_resolution);
}

void print_dialogObj::implObj::print(const main_window &from_window,
				     const functionref<print_callback_t> &cb)
{
	// Run this in a separate execution thread.

	// Obtain a wait mcguffin from the parent window.

	run_lambda([]
		   (const ref<implObj> &me,
		    const functionref<print_callback_t> &cb,
		    const main_window &from_window,
		    const ref<obj> &mcguffin)
		   {
			   cups::jobptr new_job;

			   try {
				   new_job=me->create_print_job();
			   } REPORT_EXCEPTIONS(from_window);

			   if (new_job)
			   {
				   try {
					   cb(print_callback_info{
							   new_job, mcguffin
								   });
				   } REPORT_EXCEPTIONS(from_window);
				   return;
			   }

			   from_window->elementObj::impl->get_window_handler()
				   .thread()->run_as
				   ([me, from_window]
				    (ONLY IN_THREAD)
				    {
					    try {
						    me->cancel_callback
							    (IN_THREAD);
					    } REPORT_EXCEPTIONS(from_window);
				    });
		   }, ref(this),
		   cb,
		   from_window,
		   from_window->get_wait_busy_mcguffin());
}

// Now, parse all the options, and create a new print job.

cups::jobptr print_dialogObj::implObj::create_print_job()
{
	printer_info_lock lock{printer_info};

	listlayoutmanager selected_printer_list=
		fields.selected_printer->get_layoutmanager();

	auto n=selected_printer_list->selected();

	if (!n)
		return {};

	auto info=lock->available_printers.at(*n)->info();

	auto job=info->create_job();

	if (info->supported(CUPS_COPIES))
	{
		auto number_of_copies=number_of_copies_value
			->validated_value.get();

		if (number_of_copies)
			job->set_option(CUPS_COPIES, *number_of_copies);
	}

	set_combobox_option(fields.page_size,
			    CUPS_MEDIA,
			    info,
			    lock->page_size,
			    job);

	set_combobox_option(fields.orientation_requested,
			    CUPS_SIDES,
			    info,
			    lock->sides,
			    job);

	set_combobox_option(fields.orientation_requested,
			    CUPS_NUMBER_UP,
			    info,
			    lock->number_up,
			    job);

	set_combobox_option(fields.sides,
			    CUPS_SIDES,
			    info,
			    lock->sides,
			    job);

	set_combobox_option(fields.number_up,
			    CUPS_NUMBER_UP,
			    info,
			    lock->number_up,
			    job);

	set_combobox_option(fields.orientation_requested,
			    CUPS_ORIENTATION,
			    info,
			    lock->orientation,
			    job);

	set_combobox_option(fields.finishings,
			    CUPS_FINISHINGS,
			    info,
			    lock->finishings,
			    job);

	if (info->supported("page-ranges") &&
	    fields.page_range_radio_button->get_value())
	{
		auto v=page_ranges_value->validated_value.get();

		if (v && !v->empty())
			job->set_option("page-ranges", *v);
	}

	set_combobox_option(fields.print_color_mode,
			    CUPS_PRINT_COLOR_MODE,
			    info,
			    lock->print_color_mode,
			    job);

	set_combobox_option(fields.print_quality,
			    CUPS_PRINT_QUALITY,
			    info,
			    lock->print_quality,
			    job);

	set_combobox_option(fields.printer_resolution,
			    "printer-resolution",
			    info,
			    lock->printer_resolution,
			    job);
	return job;
}

LIBCXXW_NAMESPACE_END
