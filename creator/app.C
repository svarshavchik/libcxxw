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
		// into the sorted_available_uigenerators list.
		//
		// Also store them in the uigenerators_lookup, so they
		// can be looked up by layout/factory and its name.

		switch (gen->type_category.type) {
		case appuigenerator_type::layoutmanager:
		case appuigenerator_type::factory:

			if (gen->type_category.category.empty())
				break; // Generic factory

			uicompiler_info
				->sorted_available_uigenerators.push_back(name);

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
	std::sort(uicompiler_info->sorted_available_uigenerators.begin(),
		  uicompiler_info->sorted_available_uigenerators.end());

	uicompiler_info->sorted_available_uigenerators.shrink_to_fit();
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

// Double value input field validator.

static auto optional_double_validator_closure()
{
	return []
		(ONLY IN_THREAD,
		 const std::string &value,
		 const x::w::input_field &me,
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
				me->stop_message(_("Invalid value"));
				return std::nullopt;
			}
		}

		if (parsed_value < 0)
		{
			me->stop_message(_("Value cannot be"
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

template<typename field_type>
static auto optional_double_validator(field_type &&field,
				      void (appObj::*validated_cb)(ONLY))
{
	return field->set_validator
		(optional_double_validator_closure(),
		 optional_double_formatter_closure(),
		 optional_double_new_value_closure(validated_cb));
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
			 return appObj::fmtdblval(v);
		 },
		 []
		 (ONLY IN_THREAD, const std::optional<double> &v)
		 {
			 appinvoke(&appObj::color_updated, IN_THREAD);
		 });
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

static const x::w::validated_input_field<
	std::variant<std::string, double>
	> border_size_validator(const x::w::focusable_container &field)
{
	x::w::editable_comboboxlayoutmanager lm=field->get_layoutmanager();

	return lm->set_validator
		([field=x::make_weak_capture(field)]
		 (ONLY IN_THREAD,
		  const std::string &value,
		  const auto &me,
		  const auto &trigger)
		 {
			 std::optional<std::variant<std::string, double>>
				 ret{value};

			 auto got=field.get();

			 if (!got)
			 {
				 // Being destroyed

				 return ret;
			 }

			 auto &[field]=*got;

			 x::w::editable_comboboxlayoutmanager lm=
				 field->get_layoutmanager();

			 if (lm->selected())
			 {
				 // Some item is selected, don't bother
				 // checking.
				 return ret;
			 }

			 if (value.empty())
			 {
				 return ret;
			 }

			 // Nothing is selected, must be a mm value.

			 std::istringstream i{x::trim(value)};

			 double v;

			 i >> v;

			 if (!i.fail() && (i.get(), i.eof()))
			 {
				 if (v >= 0 && v < 10000)
				 {
					 auto s=appObj::fmtdblval(v);

					 std::istringstream i{s};

					 i >> v;

					 ret=v;

					 return ret;
				 }
			 }

			 field->stop_message(_("Invalid size, must be a "
					       "defined dimension or a "
					       "numeric value in millimeters"));

			 ret.reset();
			 return ret;
		 },
		 []
		 (const auto &v)
		 {
			 return appObj::border_format_size(v);
		 },
		 []
		 (ONLY IN_THREAD, const auto &ignore)
		 {
			 appObj::border_enable_disable_later();
		 });
}

// Validator for the "scaled" fields on the border page, that have an
// optional "unsigned" value.

static const x::w::validated_input_field<std::optional<unsigned>>
border_size_scale_validator(const x::w::input_field &field)
{
	return field->set_validator
		([]
		 (ONLY IN_THREAD,
		  const std::string &value,
		  const auto &me,
		  const auto &trigger)
		 -> std::optional<std::optional<unsigned>> {

			std::optional<std::optional<unsigned>> ret;

			if (value.empty())
			{
				ret.emplace();

				appObj::border_enable_disable_later();
				return ret;
			}

			unsigned v;

			std::istringstream i{x::trim(value)};

			i >> v;

			if (!i.fail() && (i.get(), i.eof()))
			{
				if (v >= 0 && v < 10000)
				{
					ret.emplace()=v;
					appObj::border_enable_disable_later();
					return ret;
				}
			}

			me->stop_message(_("Scale value must be a non-negative"
					   " integer"));
			return ret;
		},
		 []
		 (const std::optional<unsigned> &v) -> std::string
		 {
			 std::string s;

			 if (v)
				 s=x::value_string<unsigned>
					 ::to_string(*v, x::locale::base::c());

			 return s;
		 });
}

static inline x::w::validated_input_field<std::vector<double>>
create_border_dashes_field_validator(const x::w::input_field &field)
{
	return field->set_validator
		([]
		 (ONLY IN_THREAD,
		  const std::string &value,
		  const auto &me,
		  const auto &trigger)
		 -> std::optional<std::vector<double>> {

			std::optional<std::vector<double>> ret;

			auto &v=ret.emplace();

			std::istringstream i{value};

			while (1)
			{
				auto c=i.peek();

				if (c == ' ' || c == '\t' ||
				    c == ';' ||
				    c == '\n' || c == '\r')
				{
					i.get();
					continue;
				}

				if (i.eof())
				{
					appObj::border_enable_disable_later();
					return ret;
				}

				double n;

				if (!(i >> n))
					break;

				std::istringstream rounded{appObj::fmtdblval(n)};

				x::imbue im{x::locale::base::c(), rounded};

				if (!(rounded >> n))
					break;

				if (n <= 0)
					break;
				v.push_back(n);
			}

			me->stop_message(_("Cannot parse a list of non-negative "
					   "values in millimeters"));
			return ret;
		},
		 []
		 (const std::optional<std::vector<double>> &v) -> std::string
		 {
			 std::ostringstream o;

			 const char *p="";

			 if (v)
			 {
				 for (auto n:*v)
				 {
					 o << p << appObj::fmtdblval(n);
					 p="; ";
				 }
			 }

			 return o.str();
		 });
}

appObj::appObj(init_args &&args)
	: const_app_elements_t{std::move(args.elements)},
	  configfile{args.configfile},
	  theme{args.theme},
	  current_edited_info{args.filename},
	  appearance_types{std::move(args.appearance_types)},
	  /////////////////////////////////////////////////////////////////////
	  // Dimension element callbacks.
	  dimension_new_name_validated{dimension_new_name_validator
				       (dimension_new_name)},
	  dimension_value_validated{dimension_value_validator
				    (dimension_value)},
	  dimension_scale_value_validated{dimension_scale_value_validator
					  (dimension_scale_value)},
	  color_scaled_r_validated{optional_double_validator
				   (color_scaled_page_r,
				    &appObj::color_updated)},
	  color_scaled_g_validated{optional_double_validator
				   (color_scaled_page_g,
				    &appObj::color_updated)},
	  color_scaled_b_validated{optional_double_validator
				   (color_scaled_page_b,
				    &appObj::color_updated)},
	  color_scaled_a_validated{optional_double_validator
				   (color_scaled_page_a,
				    &appObj::color_updated)},


	  color_linear_x1_validated{color_gradient_value_validator
				    (color_linear_x1,
				     &all_gradient_values::x1)},
	  color_linear_y1_validated{color_gradient_value_validator
				    (color_linear_y1,
				     &all_gradient_values::y1)},
	  color_linear_x2_validated{color_gradient_value_validator
				    (color_linear_x2,
				     &all_gradient_values::x2)},
	  color_linear_y2_validated{color_gradient_value_validator
				    (color_linear_y2,
				     &all_gradient_values::y2)},
	  color_linear_width_validated{color_gradient_value_validator
				       (color_linear_width,
					&all_gradient_values
					::linear_gradient_values::fixed_width)},
	  color_linear_height_validated{color_gradient_value_validator
					(color_linear_height,
					 &all_gradient_values
					 ::linear_gradient_values::fixed_height)
	  },

	  color_radial_inner_x_validated{color_gradient_value_validator
					 (color_radial_inner_x,
					  &all_gradient_values::inner_center_x)
	  },
	  color_radial_inner_y_validated{color_gradient_value_validator
					 (color_radial_inner_y,
					  &all_gradient_values::inner_center_y)
	  },
	  color_radial_inner_radius_validated{color_gradient_value_validator
					      (color_radial_inner_radius,
					       &all_gradient_values
					       ::inner_radius)},
	  color_radial_outer_x_validated{color_gradient_value_validator
					 (color_radial_outer_x,
					  &all_gradient_values
					  ::outer_center_x)},
	  color_radial_outer_y_validated{color_gradient_value_validator
					 (color_radial_outer_y,
					  &all_gradient_values
					  ::outer_center_y)},
	  color_radial_outer_radius_validated{color_gradient_value_validator
					      (color_radial_outer_radius,
					       &all_gradient_values::outer_radius)
	  },
	  color_radial_fixed_width_validated{color_gradient_value_validator
					     (color_radial_fixed_width,
					      &all_gradient_values::
					      radial_gradient_values::
					      fixed_width)
	  },
	  color_radial_fixed_height_validated{color_gradient_value_validator
					      (color_radial_fixed_height,
					       &all_gradient_values::
					       radial_gradient_values::
					       fixed_height)
	  },

	  border_width_validated{border_size_validator(border_width)},
	  border_height_validated{border_size_validator(border_height)},
	  border_hradius_validated{border_size_validator(border_hradius)},
	  border_vradius_validated{border_size_validator(border_vradius)},

	  border_width_scale_validated{border_size_scale_validator
			  (border_width_scale)},
	  border_height_scale_validated{border_size_scale_validator
			  (border_height_scale)},
	  border_hradius_scale_validated{border_size_scale_validator
			  (border_hradius_scale)},
	  border_vradius_scale_validated{border_size_scale_validator
			  (border_vradius_scale)},
	  border_dashes_field_validated{create_border_dashes_field_validator
					(border_dashes_field)},
	  font_point_size_validated(optional_double_validator
				    (font_size->editable_comboboxlayout(),
				     &appObj::font_enable_disable)),
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

	try {
		// Try to reparse the proposed theme file.

		(void)x::w::uigenerators::create(new_theme);

		theme=new_theme;

		update([]
		       (auto &info)
		{
			info.updated_theme();
		});

		callback2(this, mcguffin);
		status->update(_("Update saved."));

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

		xpath->to_node(1); // Remove existing element
		doc_lock->remove();
	}

	xpath=get_xpath_for(doc_lock, type, "");

	auto new_node=new_element(doc_lock, xpath);

	return std::tuple{doc_lock,
			new_node->element({new_type})->create_child()
			->attribute({"id", id})};
}

size_t
appObj::update_new_element(const new_element_t &new_element,
			   std::vector<std::string> &existing_ids,
			   const x::w::focusable_container &id_combo)
{
	return update_new_element(new_element, existing_ids, id_combo,
				  []
				  (auto ignore)
				  {
				  });
}

size_t
appObj::do_update_new_element(const new_element_t &new_element,
			      std::vector<std::string> &existing_ids,
			      const x::w::focusable_container &id_combo,
			      const x::function<void (size_t)> &callback)
{
	// Move the focus here first.
	id_combo->request_focus();

	auto insert_pos=std::lower_bound(existing_ids.begin(),
					 existing_ids.end(),
					 new_element.id);

	x::w::standard_comboboxlayoutmanager id_lm=
		id_combo->get_layoutmanager();

	size_t p=insert_pos-existing_ids.begin();

	auto i=p+1;
	// Pos 0 is new dimension

	existing_ids.insert(insert_pos, new_element.id);

	std::vector<std::string> default_description{new_element.id};

	auto &new_item_description=new_element.description.empty()
		? default_description:new_element.description;

	id_lm->insert_items(i, {new_item_description.begin(),
				new_item_description.end()});
	callback(i);
	id_lm->autoselect(i);

	return i;
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
