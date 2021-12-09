#include "libcxxw_config.h"

#include "creator/app.H"
#include "creator/uicompiler_generators.H"
#include "creator/appgenerator_function.H"
#include "creator/uicompiler.H"
#include "x/w/uigenerators.H"
#include "x/w/uielements.H"
#include "x/w/menubarlayoutmanager.H"
#include "x/w/booklayoutmanager.H"
#include "x/w/dialog_args.H"
#include "x/w/file_dialog_config.H"
#include "x/w/file_dialog.H"
#include "x/w/label.H"
#include "x/w/standard_comboboxlayoutmanager.H"
#include "x/w/editable_comboboxlayoutmanager.H"
#include "x/w/shortcut.H"
#include "x/w/theme_text.H"
#include "x/w/font_picker_config.H"
#include "x/w/font_picker_appearance.H"
#include "x/w/stop_message.H"
#include "catch_exceptions.H"

#include <x/config.H>
#include <x/messages.H>
#include <x/locale.H>
#include <x/to_string.H>
#include <x/imbue.H>
#include <x/xml/escape.H>
#include <x/sentry.H>
#include <x/strtok.H>
#include <x/visitor.H>
#include <x/weakcapture.H>
#include <x/singleton.H>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <iterator>
#include <functional>
#include <algorithm>


static x::xml::doc new_theme_file()
{
	auto d=x::xml::doc::create();

	auto lock=d->writelock();

	lock->create_child()->element({"theme"})->attribute({"version", "1"});

	return d;
}

//! Collection of standard values of various font properties.

standard_font_values_t::standard_font_values_t()
	: standard_weights{x::w::font::standard_weights()},
	  standard_slants{x::w::font::standard_slants()},
	  standard_widths{x::w::font::standard_widths()},
	  standard_spacings{x::w::font::standard_spacings()},
	  standard_point_sizes{x::w::font::standard_point_sizes()}
{
}

namespace {

	struct label_filterObj : virtual public x::obj {
		const x::functionref<x::w::input_field_filter_callback_t>
		label_filter{
			[]
			(ONLY IN_THREAD,
			 const auto &filter_info)
			{
				for (const auto c:filter_info.new_contents)
					if (c <= ' ' || c == '<' || c == '>' ||
					    c == '&' || c > 127)
						return;

				filter_info.update();
			}
		};
	};
}

static x::singleton<label_filterObj> label_filter_singleton;

x::functionref<x::w::input_field_filter_callback_t> get_label_filter()
{
	return label_filter_singleton.get()->label_filter;
}

appObj::init_args::init_args()
	: configfile{x::configdir("cxxwcreator@libcxx.com") + "/windows"},
	  theme{new_theme_file()},
	  uicompiler_info{uicompiler::create()}
{

	auto appearances=x::xml::doc::create(CREATORDIR "/appearances.xml");

	auto appearance_type=appearances->readlock();

	appearance_type->get_root();
	auto xpath=appearance_type->get_xpath("/root/appearance");

	size_t n=xpath->count();

	for (size_t i=1; i<=n; i++)
	{
		xpath->to_node(i);

		// Clone the <appearance> element's lock.

		auto save=appearance_type->clone();

		// Get the <name>.
		appearance_type->get_xpath("name")->to_node();

		// The text of <name> is the key.
		appearance_types.emplace(appearance_type->get_text(),
					 save);
	}

	// All loaded uicompiler_generators, by their IDs.

	std::unordered_map<std::string,
			   std::tuple<x::xml::readlock,
				      uicompiler_generators>> loaded_generators;

	auto generators=x::xml::doc::create(UICOMPILERDIR "/uicompiler.xml");

	auto generator=generators->readlock();

	generator->get_root();

	xpath=generator->get_xpath("/api/parser");

	n=xpath->count();

	// Make an initial pass, constructing an uicompiler_generators object for
	// each parser and storing the uicompiler_generators object and its
	// xml node.

	for (size_t i=1; i<=n; ++i)
	{
		xpath->to_node(i);

		auto name=generator->clone();

		name->get_xpath("name")->to_node();

		if (!loaded_generators
		    .emplace(name->get_text(),
			     std::tuple{generator->clone(),
				     uicompiler_generators::create()
			     }).second)
		{
			throw EXCEPTION("Internal error: duplicate "
					<< name->get_text()
					<< "generator");
		}
	}

	// Now that all objects are created, store all the uicompiler_generators
	// into their map.

	for (const auto &[name, tuple]: loaded_generators)
	{
		const auto &[readlock, gen]=tuple;
		uicompiler_info->uigenerators.emplace(name, gen);
	}

	// Second pass
	//
	// And now we call everyone's initialize()

	for (const auto &[name, tuple]: loaded_generators)
	{
		const auto &[readlock, gen]=tuple;

		gen->initialize(readlock, uicompiler_info->uigenerators);

		// Take the layoutmanager and factory generators and put them
		// into the layouts_and_factories list.
		//
		// Also store them in the uigenerators_lookup, so they
		// can be looked up by layout/factory and its name.

		switch (gen->type_category.type) {
		case appuigenerator_type::new_layoutmanager:
			if (gen->type_category.category.empty())
				break; // A few superclasses

			if (!uicompiler_info->new_layouts.emplace
			    (gen->type_category.category, gen).second)
			{
				throw EXCEPTION("Duplicate entry for new"
						" layout "
						<<
						gen->type_category.category);
			}
			break;

		case appuigenerator_type::layoutmanager:
		case appuigenerator_type::factory:
		case appuigenerator_type::elements:
		case appuigenerator_type::list_items:

			if (gen->type_category.category.empty())
				break; // Generic factory

			uicompiler_info
				->layouts_and_factories.push_back(name);

			if (!uicompiler_info
			    ->uigenerators_lookup.insert(gen).second)
			{
				throw EXCEPTION("Duplicate entry for layout "
						"or factory "
						<< gen->type_category.category);
			}
			break;
		default:
			break;
		}
	}
	std::sort(uicompiler_info->layouts_and_factories.begin(),
		  uicompiler_info->layouts_and_factories.end());

	uicompiler_info->layouts_and_factories.shrink_to_fit();
}

// Helper for installing a main menu action.

static void install_menu_event(x::w::uielements &ui,
			       const char *menu_item,
			       void (appObj::*menu_event)(ONLY IN_THREAD))
{
	ui.get_listitemhandle(menu_item)->on_status_update
		([menu_event]
		 (ONLY IN_THREAD,
		  const auto &i)
		 {
			 if (std::holds_alternative<x::w::initial>(i.trigger))
				 return;

			 appinvoke(menu_event, IN_THREAD);
		 });
}

inline appObj::init_args appObj::create_init_args()
{
	appObj::init_args args;

	auto pos=x::w::screen_positions::create(args.configfile);

	x::w::main_window_config config;

	config.restore(pos, "main");

	auto utf8_locale=x::locale::base::utf8();

	auto catalog=x::messages::create("libcxxw", utf8_locale);

	auto cxxwui_generators=
		x::w::const_uigenerators::create(CREATORDIR "/main.xml", pos,
						 catalog);

	args.cxxwui_generators=cxxwui_generators;

	args.elements.main_window=x::w::main_window::create
		(config,
		 [&]
		 (const auto &mw)
		 {
			 // Generated container for the font preview.
			 x::w::containerptr font_preview_containerptr;

			 // And the actual widget that previews the font
			 x::w::font_picker_previewptr font_previewptr;

			 x::w::uielements ui
				 {
				  {
				   {
				    "font_preview_container",
				    [&]
				    (const auto &factory)
				    {
					    // Cusotm factory for the
					    // font preview widget.

					    x::w::font_picker_config config;

					    config.appearance=cxxwui_generators
						    ->lookup_appearance
						    ("font-preview-appearance");

					    const auto &[font_preview_container,
							 font_preview_widget]=
						    x::w::
						    create_font_picker_preview
						    (factory, config);

					    font_preview_containerptr=
						    font_preview_container;
					    font_previewptr=
						    font_preview_widget;
				    },
				   },
				  },
				 };

			 dimension_elements_create(ui);
			 fonts_elements_create(ui);
			 colors_elements_create(ui);
			 borders_elements_create(ui);

			 mw->on_disconnect([]
					   {
						   exit(1);
					   });

			 mw->on_delete
				 ([]
				  (ONLY IN_THREAD,
				   const auto &ignore)
				  {
					  appinvoke(&appObj::file_quit_event,
						    IN_THREAD);
				  });

			 // Create the main menu.

			 auto mb=mw->get_menubarlayoutmanager();

			 mb->generate("mainmenubar", cxxwui_generators, ui);

			 // Create the file save dialog

			 x::w::standard_dialog_args file_save_dialog_args
				 {
				  "filesave@creator.w.libcxx.com",
				  true
				 };
			 file_save_dialog_args.restore(pos, "filesave");

			 x::w::file_dialog_config file_save_dialog_config
				 {[](ONLY IN_THREAD,
				     const auto &me,
				     const std::string &filename,
				     const x::w::busy &mcguffin)
				  {
					  me->dialog_window->hide();

					  appinvoke(&appObj
						    ::save_dialog_closed,
						    IN_THREAD,
						    filename);
				  },
				  [](ONLY IN_THREAD,
				     const auto &ignore)
				  {
				  },
				  x::w::file_dialog_type::create_file};

			 file_save_dialog_config.filename_filters
				 .emplace_back(
					       _(".xml (XML Theme Files)"),
					       "\\.(xml|XML)$"
					       );
			 file_save_dialog_config.initial_filename_filter=1;
			 auto d=mw->create_file_dialog(file_save_dialog_args,
						       file_save_dialog_config);

			 d->dialog_window->set_window_title
				 (_("Save As..."));
			 d->dialog_window->set_window_class
				 ("filesave", "creator@w.libcxx.com");

			 // Create the file open dialog

			 x::w::standard_dialog_args file_open_dialog_args
				 {
				  "fileopen@creator.w.libcxx.com",
				  true
				 };
			 file_open_dialog_args.restore(pos, "fileopen");

			 x::w::file_dialog_config file_open_dialog_config
				 {[](ONLY IN_THREAD,
				     const auto &me,
				     const std::string &filename,
				     const x::w::busy &mcguffin)
				  {
					  me->dialog_window->hide();

					  appinvoke(&appObj
						    ::open_dialog_closed,
						    IN_THREAD,
						    filename);
				  },
				  [](ONLY IN_THREAD,
				     const auto &ignore)
				  {
				  },
				  x::w::file_dialog_type::existing_file};

			 file_open_dialog_config.filename_filters=
				 file_save_dialog_config.filename_filters;
			 file_open_dialog_config.initial_filename_filter=1;
			 d=mw->create_file_dialog(file_open_dialog_args,
						  file_open_dialog_config);

			 d->dialog_window->set_window_title
				 (_("Open File"));
			 d->dialog_window->set_window_class
				 ("fileopen", "creator@w.libcxx.com");

			 mw->set_window_class("main", "creator@w.libcxx.com");

			 // Install main menu events

			 install_menu_event(ui, "file_new",
					    &appObj::file_new_event);
			 install_menu_event(ui, "file_open",
					    &appObj::file_open_event);
			 install_menu_event(ui, "file_save",
					    &appObj::file_save_event);
			 install_menu_event(ui, "file_save_as",
					    &appObj::file_save_as_event);
			 install_menu_event(ui, "file_quit",
					    &appObj::file_quit_event);
			 install_menu_event(ui, "help_about",
					    &appObj::help_about);

			 args.elements.file_save_menu_handle=
				 ui.get_listitemhandle("file_save");

			 args.elements.file_save_as_menu_handle=
				 ui.get_listitemhandle("file_save_as");

			 // Initial contents.

			 x::w::gridlayoutmanager lm=mw->get_layoutmanager();
			 lm->generate("main", cxxwui_generators, ui);

			 // We created a custom widget, pretend that it
			 // came off the same assembly line sa the rest.
			 ui.new_elements.emplace("font_preview_container",
						 font_preview_containerptr);
			 ui.new_elements.emplace("font_preview",
						 font_previewptr);

			 args.elements.status=ui.get_element("status");
			 args.elements.main_tabs=ui.get_element("main_tabs");

			 appObj::dimension_elements_initialize
				 (args.elements, ui, args);

			 appObj::colors_elements_initialize
				 (args.elements, ui, args);

			 appObj::borders_elements_initialize
				 (args.elements, ui, args);

			 appObj::fonts_elements_initialize
				 (args.elements, ui, args);

			 appObj::appearances_elements_initialize
				 (args.elements, ui, args, mw, catalog, pos);

			 appObj::generators_elements_initialize
				 (args.elements, ui, args.uicompiler_info);

			 appgenerator_functionsObj
				 ::generators_values_elements_initialize
				 (args.generator_elements, ui,
				  args.uicompiler_info);
		 });

	return args;
}

appObj::appObj() : appObj{create_init_args()}
{
}

// Clean up entered double-values to at most three digits of precision.
//
// Returns a double as a string.

std::string appObj::fmtdblval(double d)
{
	std::stringstream o;

	x::imbue im{x::locale::base::c(), o};

	o << std::fixed << std::setprecision(3) << d;

	o >> d;

	std::string s=o.str();

	// Trim any extra trailing 0s

	auto p=s.find_last_not_of('0');

	if (p != s.npos)
	{
		auto c=s.at(p);

		if (c >= '0'  && c <= '9')
			++p;
	}
	s.erase(p);

	return s;
}

// Create a validator for an input_field for a double value
//
// The value is represented as a std::string.
//
// "allownan" enables supporting "inf" to represent a NAN.
//
// errmsg is an error message to display for bad input.
//
// The callback gets invoked when the field gets validated.

void appObj::create_value_validator(
	x::w::uielements &ui,
	const char *field_name,
	bool allownan,
	const char *errmsg,
	void (appObj::*callback)(ONLY IN_THREAD))
{
	ui.create_validated_input_field(
		field_name,
		[allownan, errmsg]
		(ONLY IN_THREAD,
		 const std::string &value,
		 const auto &lock,
		 const auto &trigger) -> std::optional<std::string>
		{
			std::optional<std::string> ret;

			ret.emplace();

			auto &double_value=*ret;

			if (value.empty())
				return ret;

			if (allownan && value == _("inf"))
			{
				double_value="inf";
				return ret;
			}

			double v;

			std::istringstream i{value};

			i >> v;

			if (!i.fail())
			{
				i.get();
				if (i.eof())
				{
					if (v >= 0 && v < 10000)
					{
						double_value=fmtdblval(v);
						return ret;
					}
				}
			}
			ret.reset();

			lock.stop_message(cxxwlibmsg(errmsg));
			return std::nullopt;
		},
		[]
		(const std::string &v) -> std::string
		{
			if (v == "inf")
				return _("inf");

			return v;
		},
		std::nullopt,
		[callback]
		(ONLY IN_THREAD,
		 const std::optional<std::string> &v)
		{
			appinvoke(callback, IN_THREAD);
		}
	);
}

// Double value input field validator.

static auto optional_double_validator_closure()
{
	return []
		(ONLY IN_THREAD,
		 const std::string &value,
		 const x::w::input_lock &lock,
		 const auto &trigger) -> std::optional<std::optional<double>>
	{
		if (value.empty())
			return std::optional<double>{};

		double parsed_value;

		{
			std::istringstream i{value};

			i >> parsed_value;

			if (i.fail() || !(i.get(), i.eof()))
			{
				lock.stop_message(_("Invalid value"));
				return std::nullopt;
			}
		}

		if (parsed_value < 0)
		{
			lock.stop_message(_("Value cannot be"
					    " negative"));
			return std::nullopt;
		}

		std::istringstream i{appObj::fmtdblval(parsed_value)};

		std::optional<std::optional<double>> ret;

		auto &valid_value=ret.emplace();

		valid_value.emplace(0);

		i >> *valid_value;

		return ret;
	};
}

static auto optional_double_formatter_closure()
{
	return []
		(const auto &v) -> std::string
	       {
		       if (v)
		       {
			       return appObj::fmtdblval(*v);
		       }
		       return "";
	       };
}

static auto optional_double_new_value_closure(void
					      (appObj::*validated_cb)(ONLY))
{
	return [validated_cb]
		(ONLY IN_THREAD, const auto &v)
	       {
		       appinvoke(validated_cb, IN_THREAD);
	       };
}

void appObj::create_optional_double_validator(
	x::w::uielements &ui,
	const char *field_name,
	void (appObj::*validated_cb)(ONLY)
)
{
	ui.create_validated_input_field(
		field_name,
		optional_double_validator_closure(),
		optional_double_formatter_closure(),
		std::nullopt,
		optional_double_new_value_closure(validated_cb)
	);
}


std::string appObj::border_format_size(const std::variant<std::string,
				       double> &v)
{
	return std::visit
		(x::visitor{
			[](const std::string &s)
			{
				return s;
			},[](double v)
			  {
				  return appObj::fmtdblval(v);
			  }},
			v);
}

appObj::appObj(init_args &&args)
	: const_app_elements_t{std::move(args.elements)},
	  configfile{args.configfile},
	  theme{args.theme},
	  current_edited_info{args.filename},
	  appearance_types{std::move(args.appearance_types)},
	  /////////////////////////////////////////////////////////////////////

	  standard_font_values{std::move(static_cast<standard_font_values_t &>
					 (args))},
	  current_generators{
		  appgenerator_functions::create(args.generator_elements,
						 args.elements.main_window,
						 args.cxxwui_generators,
						 args.uicompiler_info,
						 x::ref<all_generatorsObj>
						 ::create())
	  }
{
}

void appObj::loaded_file(ONLY IN_THREAD)
{
	update_title();
	dimension_initialize(IN_THREAD);
	colors_initialize(IN_THREAD);
	borders_initialize(IN_THREAD);
	fonts_initialize(IN_THREAD);
	appearances_initialize(IN_THREAD);
	generators_initialize(IN_THREAD);
}

// Update the main window title's after loading or saving a file.

void appObj::update_title()
{
	std::string title=_("LibCXXW UI Creator");

	{
		x::mpobj_lock lock{current_edited_info};

		auto n=lock->themename;

		n=n.substr(n.rfind('/')+1);

		if (!n.empty())
		{
			title += " - ";
			title += n;
		}
	}
	main_window->set_window_title(title);
}

appObj::~appObj()
{
	auto pos=x::w::screen_positions::create();

	main_window->save(pos);
	pos->save(configfile);
}

void appObj::mainloop()
{
	app me{this};

	using x::exception;

	while (running)
	{
		try {
			eventqueue->pop()();
		} REPORT_EXCEPTIONS(main_window);
	}
}

void appObj::update_theme(ONLY IN_THREAD,
			  const x::w::busy &mcguffin,
			  get_updatecallbackptr
			  (appObj::*callback)(ONLY IN_THREAD))
{
	auto callbackptr=(this->*callback)(IN_THREAD);

	if (!callbackptr)
		return;

	eventqueue->event
		([callback=get_updatecallback{callbackptr},
		  iambusy=mcguffin.get_wait_busy_mcguffin()]
		 {
			 appinvoke(&appObj::update_theme2, callback,
				   iambusy);
		 });
}

void appObj::update_theme2(const x::functionref<update_callback_t (appObj *)
			   > &callback,
			   const x::ref<x::obj> &mcguffin)
{
	auto ret=callback(this);

	if (!ret)
		return;

	auto &[lock, callback2]=*ret;

	auto new_theme=lock->clone_document();

#if 0
	std::string s;

	lock->save_to(std::back_insert_iterator{s}, true);

	std::cout << s << std::flush;
#endif
	try {
		// Try to reparse the proposed theme file.

		(void)x::w::uigenerators::create(new_theme);

		theme=new_theme;

		update([]
		       (auto &info)
		{
			info.updated_theme();
		});

		main_window->in_thread(
			[callback2, mcguffin]
			(ONLY IN_THREAD)
			{
				appinvoke(&appObj::update_theme3,
					  IN_THREAD, callback2, mcguffin);
			}
		);
	} catch (const x::exception &e)
	{
		std::ostringstream o;

		o << _("This change cannot be made for the"
		       " following reason:\n\n")
		  << e;

		// Some error messages are quite verbose. Word-wrap them to
		// 200 mm in width.

		x::w::stop_message_config config;

		config.widthmm=200;

		main_window->stop_message(o.str(), config);

		// Hold a reference on the mcguffin until the UI is fully
		// updated.
		main_window->in_thread_idle([mcguffin]
					    (ONLY IN_THREAD)
					    {
					    });
	}
}

void appObj::update_theme3(
	ONLY IN_THREAD,
	const x::functionref<void (appObj *, ONLY,
				   const x::ref<x::obj> &)> &callback,
	const x::ref<x::obj> &mcguffin
)
{
	{
		border_info_t::lock lock{border_info};

		border_selected_locked(IN_THREAD, lock);
	}

	// Log this message
	// before calling callback2
	//
	// Callback2 may post its own
	// message, so this is the
	// default otherwise.
	status->update(_("Update saved."));
	callback(this, IN_THREAD, mcguffin);
}

void appObj::file_save_event(ONLY IN_THREAD)
{
	do_file_save_event(IN_THREAD, &appObj::only_save);
}

void appObj::do_file_save_event(ONLY IN_THREAD,
				void (appObj::*what_to_do_next)(ONLY IN_THREAD))
{
	auto filename=({
			x::mpobj_lock lock{current_edited_info};

			lock->themename;
		});

	if (!filename.empty())
	{
		do_file_save(IN_THREAD, filename, what_to_do_next);
		return;
	}

	do_file_save_as_event(IN_THREAD, what_to_do_next);
}

void appObj::file_save_as_event(ONLY IN_THREAD)
{
	do_file_save_as_event(IN_THREAD, &appObj::only_save);
}

void appObj::do_file_save_as_event(ONLY IN_THREAD,
				   void (appObj::*what_to_do_next)
				   (ONLY IN_THREAD))
{
	what_to_do_after_save_as(IN_THREAD)=what_to_do_next;
	main_window->get_dialog("filesave@creator.w.libcxx.com")
		->dialog_window->show_all();
}

void appObj::save_dialog_closed(ONLY IN_THREAD,
				const std::string &filename)
{
	do_check_and_file_save(IN_THREAD,
			       filename, what_to_do_after_save_as(IN_THREAD));
}

void appObj::do_check_and_file_save(ONLY IN_THREAD,
				    std::string filename,
				    void (appObj::*what_to_do_next)
				    (ONLY IN_THREAD))
{
	bool exists=true;

	// Warn if the file exists.

	if (access(filename.c_str(), 0))
	{
		if (filename.find('.', filename.rfind('/')+1) == filename.npos)
			filename += ".xml";

		if (access(filename.c_str(), 0))
			exists=false;
	}

	if (exists)
	{
		main_window->create_ok_cancel_dialog
			({"alert@creator.w.libcxx.com", true},
			 "alert",
			 []
			 (const auto &f)
			 {
				 f->create_label(_("Overwrite existing"
						   " file?"));
			 },
			 [filename, what_to_do_next]
			 (ONLY IN_THREAD,
			  const auto &ignore)
			 {
				 unlink(filename.c_str());

				 appinvoke(&appObj::do_file_save,
					   IN_THREAD,
					   filename,
					   what_to_do_next);
			 },
			 []
			 (ONLY IN_THREAD,
			  const auto &ignore)
			 {
			 },
			 _("Overwrite"),
			 _("Cancel"))->dialog_window->show_all();
		return;
	}


	do_file_save(IN_THREAD, filename, what_to_do_next);
}

void appObj::do_file_save(ONLY IN_THREAD,
			  const std::string &filename,
			  void (appObj::*what_to_do_next)(ONLY IN_THREAD))
{
	theme.get()->readlock()->save_file(filename, true);

	update([&]
	       (auto &info)
	{
		info.saved(filename);
	});
	update_title();
	(this->*what_to_do_next)(IN_THREAD);
}

void appObj::only_save(ONLY IN_THREAD)
{
	status->update(_("File saved"));
}

void appObj::do_update(const x::function<void (edited_info_t &)> &cb)
{
	x::mpobj_lock lock{current_edited_info};

	auto orig=*lock;

	cb(*lock);

	if (orig != *lock)
		enable_disable_menus(*lock);
}

void appObj::enable_disable_menus(const edited_info_t &info)
{
	file_save_menu_handle->enabled(info.themename.size() > 0 &&
				       info.need_saving() && info.can_save());

	// Cannot save or switch tabs.
	file_save_as_menu_handle->enabled(info.can_save());
	main_tabs->set_enabled(info.can_save());
}

void appObj::help_about(ONLY IN_THREAD)
{
	auto help_about=main_window->create_ok_dialog
		({"help_about@cxxwcreator.w.libcxx.com", true},
		 "alert",
		 []
		 (const auto &f)
		 {
			 f->create_label("LibCXXW creator version " VERSION);
		 },
		 []
		 (THREAD_CALLBACK, const auto &)
		 {
		 });

	help_about->dialog_window->show_all();
}

void appObj::file_quit_event(ONLY IN_THREAD)
{
	ifnotedited(IN_THREAD,
		    &appObj::stoprunning,
		    _("Save And Quit"),
		    _("${decoration:underline}Q${decoration:none}uit Without Saving"),
		    _("Quit Without Saving"),
		    _("Cancel"),
		    _("Alt-Q"));
}

void appObj::file_new_event(ONLY IN_THREAD)
{
	ifnotedited(IN_THREAD,
		    &appObj::new_file,
		    _("Save Changes"),
		    _("${decoration:underline}D${decoration:none}iscard Changes"),
		    _("Discard Changes"),
		    _("Cancel"),
		    _("Alt-D"));
}

void appObj::file_open_event(ONLY IN_THREAD)
{
	ifnotedited(IN_THREAD,
		    &appObj::open_file,
		    _("Save Changes"),
		    _("${decoration:underline}D${decoration:none}iscard Changes"),
		    _("Discard Changes"),
		    _("Cancel"),
		    _("Alt-D"));
}

void appObj::open_file(ONLY IN_THREAD)
{
	main_window->get_dialog("fileopen@creator.w.libcxx.com")
		->dialog_window->show_all();
}

void appObj::open_initial_file(ONLY IN_THREAD,
			       const std::string &filename)
{
	using x::exception;

	main_window->get_menubar()->show(IN_THREAD);
	main_window->show_all(IN_THREAD);

	{
		x::mpobj_lock lock{current_edited_info};
		enable_disable_menus(*lock);
	}

	if (filename.empty())
	{
		loaded_file(IN_THREAD);
	}
	else
	{
		bool thrown_exception=true;

		try {
			open_dialog_closed(IN_THREAD, filename);
			thrown_exception=false;
		} REPORT_EXCEPTIONS(main_window);

		if (thrown_exception)
		{
			new_file(IN_THREAD);
		}
	}
}

void appObj::open_dialog_closed(ONLY IN_THREAD,
				const std::string &filename)
{
	theme=load_file(filename);

	update([&]
	       (auto &info)
	{
		info.saved(filename);
	});
	loaded_file(IN_THREAD);

	auto n=filename;

	n=n.substr(n.rfind('/')+1);

	status->update((std::string)x::gettextmsg(_("Opened %1%"), n));
}

x::xml::doc appObj::load_file(const std::string &filename)
{
	// Attempt to load this thing with xinclude, first, and parse it.
	//
	// If ok, load it without xinclude.

	auto theme=x::xml::doc::create(filename, "noblanks xinclude");

	auto lock=theme->readlock();
	if (!lock->get_root() ||
	    lock->name() != "theme")
		throw EXCEPTION(x::gettextmsg
                               (_("%1% does not appear to be"
                                  " a theme file"), filename));

	(void)x::w::uigenerators::create(theme);

	return x::xml::doc::create(filename, "noblanks");
}

void appObj::ifnotedited(ONLY IN_THREAD,
			 void (appObj::*whattodo)(ONLY IN_THREAD),
			 const char *save_label,
			 const char *nosave_label,
			 const char *nosave_label_without_shortcut,
			 const char *cancel_label,
			 const char *nosave_shortcut)
{
	bool need_saving;
	bool can_save;

	{
		x::mpobj_lock lock{current_edited_info};

		need_saving=lock->need_saving();
		can_save=lock->can_save();
	}

	if (!need_saving)
	{
		(this->*whattodo)(IN_THREAD);
		return;
	}

	auto make_label=
		[]
		(const auto &f)
		{
			f->create_label(_("Some changes have not been saved"));
		};

	auto do_file_save=
		[whattodo]
		(ONLY IN_THREAD,
		 const auto &ignore)
		{
			appinvoke(&appObj::do_file_save_event,
				  IN_THREAD, whattodo);
		};

	auto do_anyway=
		 [whattodo]
		 (ONLY IN_THREAD,
		  const auto &ignore)
		 {
			 appinvoke(whattodo, IN_THREAD);
		 };
	auto do_nothing=
		[]
		 (ONLY IN_THREAD,
		  const auto &ignore)
		 {
		 };

	if (can_save)
	{
		main_window->create_ok2_cancel_dialog
			({"alert@creator.w.libcxx.com", true},
			 "alert",
			 std::move(make_label),
			 std::move(do_file_save),
			 std::move(do_anyway),
			 std::move(do_nothing),
			 x::w::theme_text{save_label},
			 x::w::theme_text{nosave_label},
			 x::w::theme_text{cancel_label},
			 {nosave_shortcut})->dialog_window->show_all();
	}
	else
	{
		main_window->create_ok_cancel_dialog
			({"alert@creator.w.libcxx.com", true},
			 "alert",
			 std::move(make_label),
			 std::move(do_anyway),
			 std::move(do_nothing),
			 x::w::theme_text{nosave_label_without_shortcut},
			 x::w::theme_text{cancel_label})
			->dialog_window->show_all();
	}

	return;
}

void appObj::new_file(ONLY IN_THREAD)
{
	theme=new_theme_file();
	update([&]
	       (auto &info)
	{
		info=edited_info_t{""};
	});
	status->update(_("New theme file created"));
	loaded_file(IN_THREAD);
}

void appObj::stoprunning(ONLY IN_THREAD)
{
	eventqueue->event([]
			  {
				  appinvoke([]
					    (auto *a)
					    {
						    a->running=false;
					    });
			  });
}

std::string appObj::xpath_for(const std::string_view &type,
			      const std::string &id)
{
	std::string s;

	std::string_view::const_iterator b=type.begin(), e=type.end();

	do
	{
		auto p=std::find(b, e, '|');

		s += "/theme/";

		s += std::string_view{b, p};

		if (!id.empty())
		{
			s += "[@id='";
			s += x::xml::escapestr(id, true);
			s += "']";
		}

		if ((b=p) != e)
		{
			s += "|";
			++b;
		}
	} while (b != e);

	return s;
}

x::xml::xpath
appObj::get_xpath_for(const x::xml::readlock &lock,
		      const char *type,
		      const std::string &id)
{
	return lock->get_xpath(xpath_for(type, id));
}

// Helper for creating a new <element>
//
// If there are existing ones, create the new one just before the first one.
//
// Otherwise create one as the first element, lock is positioned at /theme.

static inline auto new_element(const x::xml::writelock &lock,
			       const x::xml::xpath &existing)
{
	if (existing->count() > 0)
	{
		existing->to_node(1);
		return lock->create_previous_sibling();
	}

	lock->get_xpath("/theme")->to_node();

	return lock->create_child();
}

appObj::create_update_t appObj::create_update(const char *type,
					      const std::string &id,
					      bool is_new)
{
	return create_update(type, type, id,
			     theme.get()->readlock(),
			     is_new);
}

appObj::create_update_t appObj::create_update(const char *type,
					      const char *new_type,
					      const std::string &id,
					      const x::xml::readlock &lock,
					      bool is_new)
{
	auto new_doc=lock->clone_document();

	auto doc_lock=new_doc->writelock();

	return create_update_with_new_document(type, new_type, id,
					       doc_lock, is_new);
}

appObj::create_update_t
appObj::create_update_with_new_document(const char *type,
					const char *new_type,
					const std::string &id,
					const x::xml::writelock &doc_lock,
					bool is_new)
{
	doc_lock->get_root();

	auto xpath=get_xpath_for(doc_lock, type, id);

	// This one already exists?

	if (xpath->count() > 0)
	{
		if (is_new) // It shouldn't
		{
			std::string type_str=type;

			std::replace(type_str.begin(), type_str.end(),
				     '|', '/');

			std::string error=
				x::gettextmsg(_("%1% %2% "
						"already exists"),
					      type_str, id);
			main_window->stop_message(error);

			return std::nullopt;
		}
	}
	else
	{
		xpath=get_xpath_for(doc_lock, type, "");
	}

	new_element(doc_lock, xpath)->element({new_type})->create_child()
		->attribute({"id", id});

	// The new element will be the previous sibling of any existing
	// element, so remove the existing element now.

	xpath=get_xpath_for(doc_lock, type, id);

	if (xpath->count() > 1)
	{
		xpath->to_node(2);
		doc_lock->remove();

		xpath=get_xpath_for(doc_lock, type, id);
	}

	xpath->to_node(1); // We're back at the new element


	return std::tuple{doc_lock};
}

size_t
appObj::update_new_element(ONLY IN_THREAD,
			   const new_element_t *new_elements,
			   size_t n_new_elements,
			   std::vector<std::string> &existing_ids,
			   const x::w::focusable_container &id_combo)
{
	return update_new_element(
		IN_THREAD,
		new_elements, n_new_elements, existing_ids, id_combo,
		[]
		(auto ignore)
		{
		}
	);
}

size_t
appObj::update_new_element(ONLY IN_THREAD,
			   const new_element_t &new_element,
			   std::vector<std::string> &existing_ids,
			   const x::w::focusable_container &id_combo)
{
	return update_new_element(IN_THREAD, &new_element, 1,
				  existing_ids,
				  id_combo);
}

size_t
appObj::do_update_new_element(ONLY IN_THREAD,
			      const new_element_t *new_elements,
			      size_t n_new_elements,
			      std::vector<std::string> &existing_ids,
			      const x::w::focusable_container &id_combo,
			      const x::function<void (size_t)> &callback)
{
	if (n_new_elements == 0)
		return 0;

	// Move the focus here first.
	id_combo->request_focus(IN_THREAD);

	x::w::standard_comboboxlayoutmanager id_lm=
		id_combo->get_layoutmanager();

	size_t last;

	size_t j=0;

	do
	{
		auto &new_element=new_elements[j];

		auto insert_pos=std::lower_bound(existing_ids.begin(),
						 existing_ids.end(),
						 new_element.id);

		size_t p=insert_pos-existing_ids.begin();

		auto i=p+1;
		// Pos 0 is new dimension

		existing_ids.insert(insert_pos, new_element.id);

		std::vector<std::string> default_description{new_element.id};

		auto &new_item_description=new_element.description.empty()
			? default_description:new_element.description;

		id_lm->insert_items(IN_THREAD, i, {new_item_description.begin(),
						   new_item_description.end()});
		callback(i);

		last=i;
	} while (++j < n_new_elements);

	id_lm->autoselect(IN_THREAD, last, {});
	return last;
}

void appObj::busy()
{
	auto mcguffin=main_window->get_wait_busy_mcguffin();

	main_window->in_thread_idle([mcguffin]
				    (ONLY IN_THREAD)
				    {
				    });
}

void appObj::enable_disable_urd(bool is_updating,
				bool has_valid_value,
				bool unchanged_value,
				const x::w::button &u_button,
				const x::w::button &d_button,
				const x::w::button &r_button)
{
	// Step 1: determine if there's something to save.

	bool unsaved;

	if (!is_updating)
	{
		// New object. If there's something valid, that's good enough.

		unsaved=has_valid_value;
	}
	else
	{
		// Updating an existing object. There must be a valid value,
		// and it's different.

		unsaved=has_valid_value && !unchanged_value ? true:false;
	}

	bool unchanged=false;

	// Step 2: if there's something to save, both update and reset
	// buttons are enabled.
	if (unsaved)
	{
		u_button->set_enabled(true);
		r_button->set_enabled(true);
	}
	else
	{
		// There's nothing to save. If an existing object is being
		// updated, turn off the update button. Turn off the reset
		// button unless there was a validation error.
		//
		// If this is a new object the update button stays disabled
		// and the reset button is enabled.

		if (is_updating)
		{
			u_button->set_enabled(false);

			if (has_valid_value)
			{
				r_button->set_enabled(false);
				unchanged=true;
			}
			else
			{
				r_button->set_enabled(true);
			}
		}
		else
		{
			u_button->set_enabled(false);
			r_button->set_enabled(true);
		}
	}

	d_button->set_enabled(unchanged);
}
