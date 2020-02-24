#ifndef creator_app_H
#define creator_app_H

#include <x/obj.H>
#include <x/ref.H>
#include <x/refptr_traits.H>
#include <x/singletonptr.H>
#include <x/functional.H>
#include <x/functionalrefptr.H>
#include <x/eventqueue.H>
#include <x/mpobj.H>
#include <x/functional.H>
#include <x/xml/doc.H>
#include "x/w/main_window.H"
#include "x/w/uigenerators.H"
#include "x/w/file_dialogfwd.H"
#include "x/w/focusable_container.H"
#include "x/w/input_field.H"
#include "x/w/image_button.H"
#include "x/w/button.H"
#include "x/w/label.H"
#include "x/w/standard_comboboxlayoutmanagerfwd.H"
#include "x/w/listitemhandle.H"
#include "x/w/color_picker.H"
#include "x/w/impl/connection_threadfwd.H"
#include "x/w/impl/uixmlparser.H"

#include "creator/appelements.inc.H"
#include <variant>
#include <optional>
#include <string>
#include <map>
#include <vector>

class appObj;

typedef x::ref<appObj> app;

//! Singleton application class.

//! Inherits from UI elements, the UI elements in the app window, mostly
//! the main menu.

class appObj : public app_elements_t, virtual public x::obj {

	//! My configuration file

	const std::string configfile;

public:
	//! Constructor
	appObj();

private:

	struct LIBCXX_HIDDEN init_args;

	static init_args create_init_args() LIBCXX_HIDDEN;

	//! Parameters for an internal constructor
	struct init_args {

		std::string configfile;

		std::string filename;
		x::xml::doc theme;
		app_elements_tptr elements;

		x::functionref<x::w::input_field_filter_callback_t
			       > label_filter;

		init_args();
	};

	//! Internal constructor
	appObj(init_args &&);

	//! UI event queue type
	typedef x::eventqueue< x::functionref<void ()>> eventqueue_t;

	//! UI events
	const eventqueue_t eventqueue=eventqueue_t::create();

public:
	//! Destructor
	~appObj();

	////////////////////////////////////////////////////

	//! Construct an xpath for an element in the theme file.

	//! They all follow the "/theme/{type}[@id="{name}"]" pattern.

	static x::xml::doc::base::xpath
	get_xpath_for(const x::xml::doc::base::readlock &lock,
		      const char *type,
		      const std::string &id);
private:

	//! Update everything, after a new file is loaded or created.
	void loaded_file();

	//! Update the application title

	//! (include the name of the current theme)

	void update_title();

	//! Disable/enable menu items based on what things are now.

	void enable_disable_menus();
public:
	//! Format double value to three deicmal points.
	static std::string fmtdblval(double d);

	//! Main application loop
	void mainloop();

	//! Currently loaded theme file
	x::mpobj<x::xml::doc> theme;

	//! What's returned by create_update()

	//! It returns a new write lock on the new, cloned, theme document,
	//! and the node for the created empty <type id={id}> placeholder.
	//!
	//! Nothing gets returned for an error.

	typedef std::optional<std::tuple<x::xml::doc::base::writelock,
					 x::xml::doc::base::createnode>
			      > create_update_t;

	//! Add or update a theme element.

	//! Common logic when creating or updating something in the theme.
	//!
	//! Takes a "dim", or "color", etc..., its @id value and whether this
	//! should be a new element, and constructs a
	//!
	//! <type id={id}>
	//!
	//! node.
	//!
	//! Returns nothing for an error.

	create_update_t create_update(const char *type,
				      const std::string &id,
				      bool is_new);

	//! Common logic when creating a new element

	//! Inserts the new element's id into the list of existing_ids,
	//! and a combo-box with the list.
	static size_t
	update_new_element(const std::string &new_id,
			   std::vector<std::string> &existing_ids,
			   const x::w::focusable_container &id_combo);

	//! Signature of a callback used by update_theme.

	typedef x::function<bool (const x::xml::doc &)> update_callback_t;

	//! Update the theme

	//! Invoked by various update buttons.
	//!
	//! Invoke the update's button callback, passing to it two callbacks:
	//!
	//! A) Validate input fields and verify that they're valid.
	//!
	//! B) If the first callbacks returns true, the second callback
	//! gets invoked, and gets an opaque closure, that
	//! receives an x::xml::doc as a parameter. This callback is
	//! is responsible for invoking the closure to
	//! "propose" its update.
	//!
	//! The closure returns true to indicate that the updated was valid,
	//! and the callback is free to then apply the update to its UI.
	//!
	//! The closure must receive an x::xml::doc without any locks, because
	//! it will be parsed.
	void update_theme(ONLY IN_THREAD,
			  const x::w::busy &mcguffin,
			  bool (appObj::*validator)(ONLY IN_THREAD),
			  void (appObj::*callback)(ONLY IN_THREAD,
						   const update_callback_t &));

	//! Filename
	x::mpobj<std::string> themename;

	//! Whether the theme file has been modified
	x::mpobj<bool> edited=false;

	//! Dimensions page: validated new name field
	const x::w::validated_input_field<
		std::string> dimension_new_name_validated,

	//! Dimensions page: validated value field.
		dimension_value_validated,

	//! Dimensions page: validated scale field.
		dimension_scale_value_validated;

	const x::w::validated_input_field<std::optional<double>>
	//! Colors page: validated R scale value.
		color_scaled_r_validated,

	//! Colors page: validated G scale value.
		color_scaled_g_validated,

	//! Colors page: validated B scale value.
		color_scaled_b_validated,

	//! Colors page: validated A scale value.
		color_scaled_a_validated;

	//! Colors page: linear gradient value
	const x::w::validated_input_field<
		double> color_linear_x1_validated,

	//! Colors page: linear gradient value
		color_linear_y1_validated,

	//! Colors page: linear gradient value
		color_linear_x2_validated,

	//! Colors page: linear gradient value
		color_linear_y2_validated,

	//! Colors page: linear gradient value
		color_linear_width_validated,

	//! Colors page: linear gradient value
		color_linear_height_validated,

	//! Colors page: radial gradient value
		color_radial_inner_x_validated,

	//! Colors page: radial gradient value
		color_radial_inner_y_validated,

	//! Colors page: radial gradient value
		color_radial_inner_radius_validated,

	//! Colors page: radial gradient value
		color_radial_outer_x_validated,

	//! Colors page: radial gradient value
		color_radial_outer_y_validated,

	//! Colors page: radial gradient value
		color_radial_outer_radius_validated,

	//! Colors page: radial gradient value
		color_radial_fixed_width_validated,

	//! Colors page: radial gradient value
		color_radial_fixed_height_validated;

	//////////////////////////////////////////////////////////////////////
	//
	// Events from the UI. They invoke appevent to send the event to
	// main thread for execution.

	friend void appevent(const x::functionref<void (const app &)> &);

	//! File save event.
	void file_save_event(ONLY IN_THREAD);

	//! File save as event
	void file_save_as_event(ONLY IN_THREAD);

private:
	//! We know what we want to do after saving the file.
	void do_file_save_event(ONLY IN_THREAD,
				void (appObj::*what_to_do_next)());

	//! We know what we want to do after save-as dialog closes.
	void do_file_save_as_event(ONLY IN_THREAD,
				   void (appObj::*what_to_do_next)());
public:
	//! File new event
	void file_new_event(ONLY IN_THREAD);
	//! File save dialog closed
	void do_check_and_file_save(std::string,
				    void (appObj::*what_to_do_next)());

	//! File save dialog closed, checked for overwriting, ok to save now
	void do_file_save(const std::string &filename,
			  void (appObj::*what_to_do_next)());

	//! What do_file_save calls when only saving a file, and nothing more.
	void only_save();

	//! File open event
	void file_open_event(ONLY IN_THREAD);

	//! Show the file open dialog
	void open_file();

	//! Load the initial file, that was passed in as a parameter.

	void open_initial_file(ONLY IN_THREAD,
			       const std::string &filename);

	//! File open dialog was closed
	void open_dialog_closed(ONLY IN_THREAD,
				const std::string &filename);

	//! Load the theme file into x::xml::doc

	//! And do a minimal sanity check.
	static x::xml::doc load_file(const std::string &filename);

	//! File quit event.
	void file_quit_event(ONLY IN_THREAD);

private:
	//! Stashed pointer to pick up after "Save As" closes.

	void (appObj::*what_to_do_after_save_as_thread_only)();

public:
	THREAD_DATA_ONLY(what_to_do_after_save_as);

	//! Save dialog has closed
	void save_dialog_closed(ONLY IN_THREAD, const std::string &filename);

private:

	//! Check if the theme file has been edited before doing something.

	//! If it's edited, show a dialog with the given buttons. If not
	//! then invoke whattodo.
	//!
	//! The buttons save the file first, then do whattodo, go ahead and
	//! do whattodo without saving, and cancel.

	void ifnotedited(void (appObj::*whattodo)(),
			 const char *ok_label,
			 const char *ok2_label,
			 const char *cancel_label);

	//! The end of the road for a file_quit_event
	void stoprunning();

	//! Create a new file
	void new_file();

	//////////////////////////////////////////////////////////////////
	//
	// appdimension.C

	//! Install elements on the dimension page

	//! Called after generating the elements on the dimension page.
	//! Extracts the individual fields from the uielements, and places
	//! them into app_elements.
	//!
	//! Installs callbacks.

	static void dimension_elements_initialize(app_elements_tptr &,
						  x::w::uielements &,
						  init_args &);

	//! Initialize the dimension page after loading/creating a new theme.

	void dimension_initialize();

private:
	/////////////////////////////////////////////////////////////////////
	// metadata about the dimensions page.

	//! New values entered for the new or updated dimension.

	struct dimension_save_params {

		std::string dimension_new_name; //!< When creating a new one.

		std::optional<size_t> scale_from; //!< If not empty

		std::string value; //!< Dimension value
	};

	//! Original dimension parameters

	struct dimension_orig_params {
		size_t index; //!< Which original dimension
		std::optional<size_t> scale_from; //!< If not empty
		std::string value; //!< Original value

		//! Whether the saved parameters are the same as the orig ones

		bool operator==(const dimension_save_params &orig)
		{
			return scale_from == orig.scale_from &&
				value == orig.value;
		}
	};

	//! Dimension metadata
	struct dimension_info_s {

		//! Cached: selected index of an existing dimension.
		std::optional<dimension_orig_params> current_selection;

		//! Cached: selected index of an existing "from" dimension.
		std::optional<size_t> from_index;

		//! The existing dimensions.
		std::vector<std::string> ids;

		//! Updated dimension save params.

		//! This is updated every time something changes on the
		//! dimension. The "Save" button simply takes this to the bank.
		std::optional<dimension_save_params> save_params;
	};

	//! Thread-safe container for dimension metadata.

	typedef x::mpobj<dimension_info_s,
			 std::recursive_mutex> dimension_info_t;

	//! Whether something was edited but not saved
	bool dimension_unsaved_values(dimension_info_t::lock &lock);

	//! Enable or disable the reset button.
	void dimension_enable_disable_buttons(dimension_info_t::lock &lock);

	//! Reset button pressed
	void dimension_reset();

	//! Reset all fields to their original values, for the current dim
	void dimension_reset_values(dimension_info_t::lock &lock);
public:

	dimension_info_t dimension_info;

	//! Something was selected or unselected in the dimension combo-box

	void dimension_selected(ONLY IN_THREAD,
				const
				x::w::standard_combobox_selection_changed_info_t
				&);

	//! Selected an existing from dimension (maybe)
	void dimension_from_selected
	(ONLY IN_THREAD,
	 const
	 x::w::standard_combobox_selection_changed_info_t &);

	//! Selected the dimension value option
	void dimension_value_option_selected(ONLY IN_THREAD);

	//! Selected the dimension scale option
	void dimension_scale_option_selected(ONLY IN_THREAD);

	//! A dimension value was entered.
	void dimension_value_entered(ONLY IN_THREAD);

	//! A scaled dimension value was entered.
	void dimension_scale_value_entered(ONLY IN_THREAD);

	//! One of the dimension fields was changed.
	void dimension_field_updated(ONLY IN_THREAD);

private:

	//! Enable or disable the reset button.
	void dimension_update_enable_disable(dimension_info_t::lock &lock);

	//! Invoked by dimension_field_updated() to process fields' contents.

	bool dimension_update_save_params(ONLY IN_THREAD,
					  dimension_info_t::lock &lock);

public:
	//! Validate parameters for save/update
	bool dimension_validate(ONLY IN_THREAD);

	//! Save/update dimension
	void dimension_update(ONLY IN_THREAD,
			      const update_callback_t &callback);

	//! Verify that it's ok to delete the current shown dimension
	bool dimension_ok_to_delete(ONLY IN_THREAD);

	//! Delete dimension
	void dimension_delete(ONLY IN_THREAD,
			      const update_callback_t &callback);

	/////////////////////////////////////////////////////////////////
	//
	// Colors page.

	//! Installs callbacks.

	static void colors_elements_initialize(app_elements_tptr &,
					       x::w::uielements &,
					       init_args &);

	//! Initialize the dimension page after loading/creating a new theme.

	void colors_initialize();

	//! We load the gradient colors as is.
	typedef std::map<size_t, std::string> loaded_color_gradient_t;

	//! Loaded linear gradient
	struct loaded_linear_gradient : x::w::linear_gradient_values {

		loaded_color_gradient_t gradient;

		//! Comparison operator
		inline bool operator==(const loaded_linear_gradient &o) const
		{
			return x::w::linear_gradient_values::operator==(o) &&
				gradient == o.gradient;
		}

		//! Comparison operator
		inline bool operator!=(const loaded_linear_gradient &o) const
		{
			return !operator==(o);
		}
	};

	//! Loaded radial gradient
	struct loaded_radial_gradient : x::w::radial_gradient_values {

		loaded_color_gradient_t gradient;

		//! Comparison operator
		inline bool operator==(const loaded_radial_gradient &o) const
		{
			return x::w::radial_gradient_values::operator==(o) &&
				gradient == o.gradient;
		}

		//! Comparison operator
		inline bool operator!=(const loaded_radial_gradient &o) const
		{
			return !operator==(o);
		}
	};


	//! Loaded color

	//! Verbatim loaded rgb value, or a parsed scaled color, or a gradient.

	typedef std::variant<x::w::rgb, x::w::ui::parsed_scaled_color,
			     loaded_linear_gradient,
			     loaded_radial_gradient> loaded_color_t;

	//! Original dimension parameters

	struct colors_orig_params {
		size_t index; //!< Which original dimension
		loaded_color_t loaded_color;
	};

	//! Validated and saved parameters.
	struct colors_save_params {

		std::string color_new_name; //!< When creating a new one.
		loaded_color_t color_new_value; //!< The new value
	};

	//! Colors metadata
	struct colors_info_s {

		//! Cached: selected index of an existing color.
		std::optional<colors_orig_params> current_selection;

		//! Cached: selected index of an existing colors.
		//! std::optional<colors_orig_params> current_selection;

		//! Cached: selected index of an existing "from" colors.
		std::optional<size_t> from_index;

		//! The existing colors.
		std::vector<std::string> ids;

		//! Updated colors save params.

		//! This is updated every time something changes on the
		//! colors. The "Save" button simply takes this to the bank.

		std::optional<colors_save_params> save_params;
	};

	//! Thread-safe container for colors metadata.

	typedef x::mpobj<colors_info_s,
			 std::recursive_mutex> colors_info_t;

	//! Current colors
	colors_info_t colors_info;

	//! Something was selected or unselected in the color combo-box

	//! Invokes either color_unselected() or color_reset().

	void color_selected(ONLY IN_THREAD,
			    const
			    x::w::standard_combobox_selection_changed_info_t
			    &info);

private:

	//! color_selected() was invoked for an unselection

	//! Before a new color, or "New Color" gets selected in the combo-box
	//! the previous selection gets unselected.

	void color_unselected_locked(ONLY IN_THREAD,
				     colors_info_t::lock &lock);

	//! A new color was selected from the color name combo-box.

	//! Or the Reset button was pressed.
	void color_selected_locked(ONLY IN_THREAD,
				   colors_info_t::lock &);
public:
	//! Reset all fields to their original values, for the current color
	void color_reset_values(ONLY IN_THREAD, colors_info_t::lock &lock);

	//! The currently displayed color has been updated in some way.
	void color_updated(ONLY IN_THREAD);

private:

	//! The currently displayed color has been updated in some way.
	void color_updated_locked(ONLY IN_THREAD,
				  colors_info_t::lock &);

	//! Invoke by color_updated_locked to create colors_save_params

	//! color_updated_locked() creates a new colors_save_params, if needed
	//! and invokes this. \c false gets returned if the color parameters
	//! are not valid; color_updated_locked() clears save_params

	bool color_updated_locked(ONLY IN_THREAD,
				  colors_info_t::lock &lock,
				  colors_save_params &);

	//! Update the status of update, reset, and delete buttons

	//! Whether they should be enabled or disabled is determined by the
	//! current state.
	void color_enable_disable_buttons(ONLY IN_THREAD,
					  colors_info_t::lock &lock);

	//! Invoked by "Add" button on a color gradient tab.

	//! Adds another row for a gradient color. Returns the input field
	//! for the gradient number input field

	x::w::input_field
	color_add_gradient_row(ONLY IN_THREAD,
			       const x::w::container &container,
			       const x::w::gridlayoutmanager &glm);

	//! Determine whether the gradient "Add" button should be enabled.

	//! Disable the "Add" button if there's any existing gradient
	//! color that failed validation.

	void color_add_enable_disable(ONLY IN_THREAD,
				      const x::w::container &container,
				      const x::w::gridlayoutmanager &glm);

	//! Enable or disable the "Add" button.

	//! Calls color_add_enable_disable_button(), then color_updated().

	void color_add_enable_disable(ONLY IN_THREAD,
				      const x::w::container &container,
				      const x::w::gridlayoutmanager &glm,
				      bool enabled);
private:

	//! Change the status of the Add button.

	//! Receives either color_linear_page_values_grid's or
	//! color_radial_page_values_grid's layout manager, and enables
	//! or disables the "Add" button.

	void color_add_button_enable_disable(ONLY IN_THREAD,
					     const x::w::gridlayoutmanager &glm,
					     bool enable_disable);

public:
	//! Helper class for creating new gradient color rows.

	struct color_create_gradient_row;

	//! Helper function for extracting gradient values and colors.

	template<typename F>
	static bool parse_gradient_rows(const x::w::container &container,
					const x::w::gridlayoutmanager &glm,
					F &&f)
	{
		return do_parse_gradient_rows(container, glm,
					      x::make_function<void
					      (size_t,
					       const std::string &)>
					      (std::forward<F>(f)));
	}

	static bool do_parse_gradient_rows(const x::w::container &container,
					   const x::w::gridlayoutmanager &glm,
					   const x::function<void
					   (size_t,
					    const std::string &)> &parser);
	//! Validate parameters for save/update
	bool color_validate(ONLY IN_THREAD);

	//! Save/update color
	void color_update(ONLY IN_THREAD,
			  const update_callback_t &callback);

	//! Verify that it's ok to delete the current shown color
	bool color_ok_to_delete(ONLY IN_THREAD);

	//! Delete color
	void color_delete(ONLY IN_THREAD,
			  const update_callback_t &callback);

private:

	//! The app is running.

	//! Cleared by file_quit_event(), to stop the main execution thread.
	bool running=true;

};

// The singleton instance of the app object.
typedef x::singletonptr<appObj> appsingleton;

//! app events

//! Constructs the app singleton
//! then (attempt to) instantiate the app singleton,
//! then put an event closure into the event queue, to be executed
//! by the main application thread that loops and plucks events
//! off the event queue.

void appevent(const x::functionref<void (const app &)> &);

// Helper for invoking something in the app object.

template<typename T, typename ...Args>
static inline void appinvoke(T t, Args && ...args)
{
	appsingleton singleton;

	if (!singleton)
		return;

	auto &me=*singleton;

	std::invoke(t, &me, std::forward<Args>(args)...);
}

#endif
