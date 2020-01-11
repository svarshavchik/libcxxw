#include "libcxxw_config.h"

#include "creator/app.H"
#include "x/w/screen_positions.H"
#include "x/w/uigenerators.H"
#include "x/w/uielements.H"
#include "x/w/menubarlayoutmanager.H"
#include "x/w/booklayoutmanager.H"
#include "x/w/dialog_args.H"
#include "x/w/file_dialog_config.H"
#include "x/w/file_dialog.H"
#include "x/w/label.H"
#include "catch_exceptions.H"
#include "messages.H"

#include <x/config.H>
#include <x/messages.H>
#include <x/locale.H>
#include <x/to_string.H>
#include <x/imbue.H>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <iterator>

#ifndef CREATORDIR
#define CREATORDIR PKGDATADIR "/creator"
#endif


static x::xml::doc new_theme_file()
{
	auto d=x::xml::doc::create();

	auto lock=d->writelock();

	lock->create_child()->element({"theme"})->attribute({"version", "1"});

	return d;
}

appObj::init_args::init_args()
	: configfile{x::configdir("cxxwcreator@libcxx.com") + "/windows"},
	  theme{new_theme_file()},
	  label_filter
	{
	 []
	 (ONLY IN_THREAD,
	  const auto &filter_info)
	 {
		 for (const auto c:filter_info.new_contents)
			 if (c <= ' ' || c == '<' || c == '>' ||
			     c == '&')
				 return;

		 filter_info.update();
	 }
	}
{
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

	auto main_generator=
		x::w::const_uigenerators::create(CREATORDIR "/main.xml", pos,
						 catalog);

	args.elements.main_generator=main_generator;

	args.elements.main_window=x::w::main_window::create
		(config,
		 [&]
		 (const auto &mw)
		 {
			 x::w::uielements ui;

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

			 mb->generate("mainmenubar", main_generator, ui);

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

			 args.elements.file_save_menu_handle=
				 ui.get_listitemhandle("file_save");

			 // Initial contents.

			 x::w::gridlayoutmanager lm=mw->get_layoutmanager();
			 lm->generate("main", main_generator, ui);

			 args.elements.status=ui.get_element("status");

			 appObj::dimension_elements_initialize
				 (args.elements, ui, args);

			 appObj::colors_elements_initialize
				 (args.elements, ui, args);
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

	x::imbue imbued{x::locale::base::global(), o};

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

static inline auto value_validator(const x::w::input_field &field,
				   bool allownan,
				   const char *errmsg,
				   void (appObj::*callback)(ONLY IN_THREAD))
{
	return field->set_validator
		([allownan, errmsg]
		 (ONLY IN_THREAD,
		  const std::string &value,
		  const auto &me,
		  const auto &trigger)
		 -> std::optional<std::string> {

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
			x::imbue i_locale{x::locale::base::global(), i};

			i >> v;

			if (!i.fail())
			{
				i.get();
				if (i.eof())
				{
					if (v >= 0 && v < 10000)
					{
						double_value=
							appObj::fmtdblval(v);
						return ret;
					}
				}
			}
			ret.reset();

			me->stop_message(cxxwlibmsg(errmsg));
			return std::nullopt;
		},
		 []
		 (const std::string &v) -> std::string
		 {
			 if (v == "inf")
				 return _("inf");

			 return v;
		 },
		 [callback]
		 (ONLY IN_THREAD,
		  const std::optional<std::string> &v)
		 {
			 appinvoke(callback, IN_THREAD);
		 });
}

static inline auto dimension_new_name_validator(const x::w::input_field &field)
{
	return field->set_validator
		([]
		 (ONLY IN_THREAD,
		  const std::string &value,
		  const auto &me,
		  const auto &trigger)
		 -> std::optional<std::string>
		 {
			 return value;
		 },
		 []
		 (const std::string &v) -> std::string
		 {
			 return v;
		 },
		 []
		 (ONLY IN_THREAD,
		  const std::optional<std::string> &v)
		 {
			 appinvoke(&appObj::dimension_field_updated, IN_THREAD);
		 });
}

static inline auto dimension_value_validator(const x::w::input_field &field)
{
	return value_validator(field, true,
			       _txt("Invalid millimeter value"),
			       &appObj::dimension_value_entered);
}

static inline auto dimension_scale_value_validator(const x::w::input_field
						   &field)
{
	return value_validator(field, false, _txt("Invalid scale value"),
			       &appObj::dimension_scale_value_entered);
}

static auto color_scale_value_validator(const x::w::input_field &field)
{
	return value_validator(field, true,
			       _txt("Invalid scaling value"),
			       &appObj::color_updated);
}

struct all_gradient_values : x::w::linear_gradient_values,
			     x::w::radial_gradient_values {};

static const x::w::validated_input_field<double>
color_gradient_value_validator(const x::w::input_field &field,
			       double all_gradient_values::*default_value)
{
	return field->set_string_validator
		([default_value]
		 (ONLY IN_THREAD,
		  const std::string &value,
		  double *parsed_value,
		  const x::w::input_field &field,
		  const auto &trigger) -> std::optional<double>
		 {
			 if (parsed_value)
			 {
				 auto s=appObj::fmtdblval(*parsed_value);

				 std::istringstream i{s};

				 x::imbue imbued{x::locale::base::global(), i};

				 i >> *parsed_value;

				 return *parsed_value;
			 }

			 if (value.empty())
			 {
				 all_gradient_values default_values;

				 return default_values.*default_value;
			 }

			 field->stop_message(_("Invalid value"));

			 return std::nullopt;
		 },
		 [default_value]
		 (double v) -> std::string
		 {
			 all_gradient_values default_values;

			 return appObj::fmtdblval(v);
		 },
		 []
		 (ONLY IN_THREAD, const std::optional<double> &v)
		 {
			 appinvoke(&appObj::color_updated, IN_THREAD);
		 });
}

appObj::appObj(init_args &&args)
	: app_elements_t{std::move(args.elements)},
	  configfile{args.configfile},
	  theme{args.theme},
	  themename{args.filename},

	  /////////////////////////////////////////////////////////////////////
	  // Dimension element callbacks.
	  dimension_new_name_validated{dimension_new_name_validator
				       (dimension_new_name)},
	  dimension_value_validated{dimension_value_validator
				    (dimension_value)},
	  dimension_scale_value_validated{dimension_scale_value_validator
					  (dimension_scale_value)},
	  color_scaled_r_validated(color_scale_value_validator
				   (color_scaled_page_r)),
	  color_scaled_g_validated(color_scale_value_validator
				   (color_scaled_page_g)),
	  color_scaled_b_validated(color_scale_value_validator
				   (color_scaled_page_b)),
	  color_scaled_a_validated(color_scale_value_validator
				   (color_scaled_page_a)),


	  color_linear_x1_validated(color_gradient_value_validator
				    (color_linear_x1,
				     &all_gradient_values::x1)),
	  color_linear_y1_validated(color_gradient_value_validator
				    (color_linear_y1,
				     &all_gradient_values::y1)),
	  color_linear_x2_validated(color_gradient_value_validator
				    (color_linear_x2,
				     &all_gradient_values::x2)),
	  color_linear_y2_validated(color_gradient_value_validator
				    (color_linear_y2,
				     &all_gradient_values::y2)),
	  color_linear_width_validated(color_gradient_value_validator
				       (color_linear_width,
					&all_gradient_values
					::linear_gradient_values::fixed_width)),
	  color_linear_height_validated(color_gradient_value_validator
					(color_linear_height,
					 &all_gradient_values
					 ::linear_gradient_values::fixed_height)
					),

	  color_radial_inner_x_validated(color_gradient_value_validator
					 (color_radial_inner_x,
					  &all_gradient_values::inner_center_x)
					 ),
	  color_radial_inner_y_validated(color_gradient_value_validator
					 (color_radial_inner_y,
					  &all_gradient_values::inner_center_y)
					 ),
	  color_radial_inner_radius_validated(color_gradient_value_validator
					      (color_radial_inner_radius,
					       &all_gradient_values
					       ::inner_radius)),
	  color_radial_outer_x_validated(color_gradient_value_validator
					 (color_radial_outer_x,
					  &all_gradient_values
					  ::outer_center_x)),
	  color_radial_outer_y_validated(color_gradient_value_validator
					 (color_radial_outer_y,
					  &all_gradient_values
					  ::outer_center_y)),
	  color_radial_outer_radius_validated(color_gradient_value_validator
					    (color_radial_outer_radius,
					     &all_gradient_values::outer_radius)
					      ),
	  color_radial_fixed_width_validated(color_gradient_value_validator
					     (color_radial_fixed_width,
					      &all_gradient_values::
					      radial_gradient_values::
					      fixed_width)
					     ),
	  color_radial_fixed_height_validated(color_gradient_value_validator
					      (color_radial_fixed_height,
					       &all_gradient_values::
					       radial_gradient_values::
					       fixed_height))
{
	loaded_file();
	main_window->get_menubar()->show();
	main_window->show_all();
}

void appObj::loaded_file()
{
	update_title();
	enable_disable_menus();
	dimension_initialize();
	colors_initialize();
}

// Update the main window title's after loading or saving a file.

void appObj::update_title()
{
	std::string title=_("LibCXXW UI Creator");

	auto n=themename.get();

	n=n.substr(n.rfind('/')+1);

	if (!n.empty())
	{
		title += " - ";
		title += n;
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

	while (running)
		eventqueue->pop()(me);
}

void appObj::update_theme(ONLY IN_THREAD, const x::w::busy &mcguffin,
			  bool (appObj::*validator)(ONLY IN_THREAD),
			  void (appObj::*callback)(const update_callback_t &))
{
	if (!(this->*validator)(IN_THREAD))
		return;

	(this->*callback)
		(x::make_function<bool (const x::xml::doc &)>
		 ([this]
		  (const x::xml::doc &new_theme)
		  {
			  try {
				  // Try to reparse the proposed theme file.
				  //
				  // We save and reread it with xinclude
				  // enable, and then try to parse that
				  // version.
				  std::stringstream s;

				  new_theme->readlock()
					  ->save_to(std::ostreambuf_iterator
						    {s.rdbuf()}, true);

				  auto n=themename.get();

				  if (n.empty())
					  n="theme.xml";

				  auto test_theme=x::xml::doc::create
					  (std::istreambuf_iterator
					   {s.rdbuf()},
					   std::istreambuf_iterator<char>{},
					   n,
					   "noblanks xinclude");

				  (void)x::w::uigenerators
					  ::create(test_theme);

				  theme=new_theme;
				  edited=true;
				  enable_disable_menus();
			  } catch (const x::exception &e)
			  {
				  std::ostringstream o;

				  o << _("This change cannot be made for the"
					 " following reason:\n\n")
				    << e;

				  main_window->stop_message(o.str());
				  return false;
			  }
			  return true;
		  }));
}

void appObj::file_save_event(ONLY IN_THREAD)
{
	do_file_save_event(IN_THREAD, &appObj::only_save);
}

void appObj::do_file_save_event(ONLY IN_THREAD,
				void (appObj::*what_to_do_next)())
{
	auto filename=themename.get();

	if (!filename.empty())
	{
		do_file_save(filename, what_to_do_next);
		return;
	}

	do_file_save_as_event(IN_THREAD, what_to_do_next);
}

void appObj::file_save_as_event(ONLY IN_THREAD)
{
	do_file_save_as_event(IN_THREAD, &appObj::only_save);
}

void appObj::do_file_save_as_event(ONLY IN_THREAD,
				   void (appObj::*what_to_do_next)())
{
	what_to_do_after_save_as(IN_THREAD)=what_to_do_next;
	main_window->get_dialog("filesave@creator.w.libcxx.com")
		->dialog_window->show_all();
}

void appObj::save_dialog_closed(ONLY IN_THREAD,
				const std::string &filename)
{
	do_check_and_file_save(filename, what_to_do_after_save_as(IN_THREAD));
}

void appObj::do_check_and_file_save(std::string filename,
				    void (appObj::*what_to_do_next)())
{
	// Warn if the file exists.

	if (access(filename.c_str(), 0) == 0)
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

	if (filename.find('.', filename.rfind('/')+1) == filename.npos)
		filename += ".xml";

	do_file_save(filename, what_to_do_next);
}

void appObj::do_file_save(const std::string &filename,
			  void (appObj::*what_to_do_next)())
{
	theme.get()->readlock()->save_file(filename, true);

	themename=filename;
	edited=false;
	update_title();
	enable_disable_menus();
	(this->*what_to_do_next)();
}

void appObj::only_save()
{
	status->update(_("File saved"));
}

void appObj::enable_disable_menus()
{
	file_save_menu_handle->enabled(themename.get().size() > 0 &&
				       edited.get());
}

void appObj::file_quit_event(ONLY IN_THREAD)
{
	ifnotedited(&appObj::stoprunning,
		    _("Save And Quit"),
		    _("Quit Only"),
		    _("Cancel"));
}

void appObj::file_new_event(ONLY IN_THREAD)
{
	ifnotedited(&appObj::new_file,
		    _("Save Changes"),
		    _("Discard Changes"),
		    _("Cancel"));
}

void appObj::file_open_event(ONLY IN_THREAD)
{
	ifnotedited(&appObj::open_file,
		    _("Save Changes"),
		    _("Discard Changes"),
		    _("Cancel"));
}

void appObj::open_file()
{
	main_window->get_dialog("fileopen@creator.w.libcxx.com")
		->dialog_window->show_all();
}

void appObj::open_initial_file(ONLY IN_THREAD,
			       const std::string &filename)
{
	using x::exception;

	try {
		open_dialog_closed(IN_THREAD, filename);
	} REPORT_EXCEPTIONS(main_window);
}

void appObj::open_dialog_closed(ONLY IN_THREAD,
				const std::string &filename)
{
	theme=load_file(filename);
	themename=filename;
	loaded_file();

	auto n=themename.get();

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

void appObj::ifnotedited(void (appObj::*whattodo)(),
			 const char *ok_label,
			 const char *ok2_label,
			 const char *cancel_label)
{
	if (!edited.get())
	{
		(this->*whattodo)();
		return;
	}

	main_window->create_ok2_cancel_dialog
		({"alert@creator.w.libcxx.com", true},
		 "alert",
		 []
		 (const auto &f)
		 {
			 f->create_label(_("Some changes have not been saved"));
		 },
		 [whattodo]
		 (ONLY IN_THREAD,
		  const auto &ignore)
		 {
			 appinvoke(&appObj::do_file_save_event,
				   IN_THREAD, whattodo);
		 },
		 [whattodo]
		 (ONLY IN_THREAD,
		  const auto &ignore)
		 {
			 appinvoke(whattodo);
		 },
		 []
		 (ONLY IN_THREAD,
		  const auto &ignore)
		 {
		 },
		 ok_label,
		 ok2_label,
		 cancel_label)->dialog_window->show_all();
	return;
}

void appObj::new_file()
{
	theme=new_theme_file();
	themename="";
	edited=false;
	status->update(_("New theme file created"));
	loaded_file();
}

void appObj::stoprunning()
{
	eventqueue->event([]
			  (const app &a)
			  {
				  a->running=false;
			  });
}
