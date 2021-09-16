#ifndef creator_app_H
#define creator_app_H

#include <x/obj.H>
#include <x/ref.H>
#include <x/singletonptr.H>
#include <x/functional.H>
#include <x/functionalrefptr.H>
#include <x/eventqueue.H>
#include <x/mpobj.H>
#include <x/functional.H>
#include <x/xml/doc.H>
#include <x/xml/readlock.H>
#include <x/xml/writelock.H>
#include <x/xml/xpath.H>
#include "x/w/main_window.H"
#include "x/w/dialog.H"
#include "x/w/uigenerators.H"
#include "x/w/file_dialogfwd.H"
#include "x/w/focusable_container.H"
#include "x/w/input_field.H"
#include "x/w/input_field_filter.H"
#include "x/w/image_button.H"
#include "x/w/button.H"
#include "x/w/label.H"
#include "x/w/canvas.H"
#include "x/w/standard_comboboxlayoutmanagerfwd.H"
#include "x/w/listitemhandle.H"
#include "x/w/color_picker.H"
#include "x/w/impl/connection_threadfwd.H"
#include "x/w/impl/uixmlparser.H"
#include "x/w/screen_positions.H"
#include "font_picker/font_picker_preview.H"
#include "messages.H"

#include "creator/appelements.inc.H"
#include "creator/uicompiler_generatorsfwd.H"
#include "creator/uicompilerfwd.H"
#include "creator/appgenerator_functionsobj.H"
#include "creator/appgenerator_savefwd.H"
#include <variant>
#include <optional>
#include <string>
#include <string_view>
#include <map>
#include <concepts>
#include <vector>

#ifndef CREATORDIR
#define CREATORDIR PKGDATADIR "/creator"
#endif

#ifndef UICOMPILERDIR
#define UICOMPILERDIR PKGDATADIR "/creator"
#endif

class appObj;

typedef x::ref<appObj> app;

typedef const app_elements_t const_app_elements_t;

//! Collection of standard values of various font properties.

struct standard_font_values_t {

	x::w::font::values_t standard_weights,
		standard_slants,
		standard_widths,
		standard_spacings;
	std::vector<unsigned> standard_point_sizes;

	standard_font_values_t();
};

//! Status of the loaded appearance file

struct edited_info_t {

	//! Name of the currently loaded filename

	std::string themename;

	//! Whether we update_themed successfully.

	//! This is true when the theme has been updated. Saving it resets
	//! it back to false.
	bool updated=false;

	//! Something was modified, but not update_themed.

	bool modified=false;

	//! Comparison operator
	bool operator==(const edited_info_t &o) const=default;

	//! Constructor
	edited_info_t(const std::string &themename)
		: themename{themename}
	{
	}

	//! The theme file was updated.
	void updated_theme()
	{
		updated=true;
	}

	//! The theme file was saved with this name.

	void saved(const std::string &filename)
	{
		*this=edited_info_t{filename};
	}

	//! Whether the theme file needs saving.
	bool need_saving() const
	{
		return updated || modified;
	}

	//! Whether the current theme can be saved.

	//! If a modification is in progress, it won't be picked up by save()
	//! so we will not enable it.

	bool can_save() const
	{
		return !modified;
	}
};

//! A lambda that updates the theme file.

//! The lambda takes an edited_info_t reference as a parameter, and can
//! modify it.

template<typename F>
concept update_functor = requires(F f, edited_info_t &info) {
	{
		f(info)
	} -> std::same_as<void>;
};

extern x::functionref<x::w::input_field_filter_callback_t> get_label_filter();

//! Singleton application class.

//! Inherits from UI elements, the UI elements in the app window, mostly
//! the main menu.

class appObj : public const_app_elements_t, virtual public x::obj {

	//! My configuration file

	const std::string configfile;

public:
	//! Constructor
	appObj();

private:

	struct LIBCXX_HIDDEN init_args;

	static init_args create_init_args() LIBCXX_HIDDEN;

	//! Container for known appearance types.
	typedef std::map<std::string, const x::xml::const_readlock
			 > appearance_types_t;

	//! Parameters for an internal constructor
	struct init_args : standard_font_values_t {

		std::string configfile;

		std::string filename;
		x::xml::doc theme;
		x::w::const_uigeneratorsptr cxxwui_generators;
		app_elements_tptr elements;
		app_generator_elements_tptr generator_elements;
		appearance_types_t appearance_types;

		uicompiler uicompiler_info;
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
	//!
	//! "type" may contain "|"-delimited list, this results in an
	//! xpath. I.e. xpath_for("layout|factory", "xx") becomes:
	//!
	//! \code
	//!   /theme/layout[@id="xx"]|/theme/factory[@id="xx"]
	//! \endcode

	static std::string xpath_for(const std::string_view &type,
				     const std::string &id);

	//! Call xpath_for(), then return this xpath for the given lock.

	static x::xml::xpath
	get_xpath_for(const x::xml::readlock &lock,
		      const char *type,
		      const std::string &id);

private:

	//! Update everything, after a new file is loaded or created.
	void loaded_file(ONLY IN_THREAD);

	//! Update the application title

	//! (include the name of the current theme)

	void update_title();

	//! Disable/enable menu items based on what things are now.

	void enable_disable_menus(const edited_info_t &info);

	//! Compute the logic to enable and disable update/delete/reset buttons

	//! The individual tabs tell us:
	//!
	//! - is_updating: whether an existing value is being updated.
	//!
	//! - has_valid_value: whether the widgets on the tab are valid.
	//!
	//! - unchanged_value: only when is_updating and has_valid_value:
	//!   whether the valid value is the same as the original value
	//!
	//! - the updated, delete, and reset buttons.

	static void enable_disable_urd(bool is_updating,
				       bool has_valid_value,
				       bool unchanged_value,
				       const x::w::button &u_button,
				       const x::w::button &d_button,
				       const x::w::button &r_button);

	//! Compute the logic to enable and disable update/delete/reset buttons

	//! Template function overload that computes is_updating,
	//! has_valid_value, and unchanged_value parameters based on a state
	//! of a locked status object.
	//!
	//! The first template parameter is a lock, dereferencing it produces:
	//!
	//! - current_selection: a std::optional indicating the presence of
	//! an existing object whose value is getting updated, else this must
	//! be a new object.
	//!
	//! - save_params: a std::optional with the value of the new or
	//! updated object.
	//!
	//! The optional save_params object must have a defined == operator
	//! for comparing it to the current selection object.

	template<typename lock_type>
	static inline void enable_disable_urd(lock_type &lock,
					      const x::w::button &u_button,
					      const x::w::button &d_button,
					      const x::w::button &r_button)
	{
		enable_disable_urd((lock->current_selection ? true:false),
				   (lock->save_params ? true:false),
				   (lock->save_params &&
				    lock->current_selection &&
				    *lock->save_params ==
				    *lock->current_selection),
				   u_button,
				   d_button,
				   r_button);
	}

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

	typedef std::optional<std::tuple<x::xml::writelock,
					 x::xml::createnode>
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
	//!
	//! "type" is a "dim" or "color". When creating a layout or a factory,
	//! which share the same id namespace, "type" is "layout|factory".
	//!
	//! "new_type" is the same as type, or just "layout" or "factory",
	//! when creating that one.
	//!
	//! doc_lock is the lock on the new document being created.

	create_update_t
	create_update_with_new_document(const char *type,
					const char *new_type,
					const std::string &id,
					const x::xml::writelock &doc_lock,
					bool is_new);

	//! Create a new document, then call create_update_with_new_document

	//! "lock" is the lock on the current document, theme.get().
	//! this document is cloned() and returned.

	create_update_t create_update(const char *type,
				      const char *new_type,
				      const std::string &id,
				      const x::xml::readlock &lock,
				      bool is_new);

	//! Add or update a theme element.

	//! \overload
	//!
	//! The same value for "type" and "new_type".

	create_update_t create_update(const char *type,
				      const std::string &id,
				      bool is_new);

	//! New ID and name parameter to update_new_element

	//! If description is an empty vector, it's considered to be the
	//! same as the id. This allows for convenient uniform initialization
	//! syntax to be used for this parameter.

	struct new_element_t {
		std::string id;
		std::vector<std::string> description;
	};

	//! Common logic when creating a new element

	//! Inserts the new element's id into the list of existing_ids,
	//! and the description into the combo-box that shows the list.
	static size_t
	update_new_element(const new_element_t &new_element,
			   std::vector<std::string> &existing_ids,
			   const x::w::focusable_container &id_combo);

	//! Invoke a callback before auto-selecting the new element

	//! Invokes the closure before calling autoselect() on the combo-box.
	template<typename F>
	static inline size_t
	update_new_element(const new_element_t &new_element,
			   std::vector<std::string> &existing_ids,
			   const x::w::focusable_container &id_combo,
			   F &&f)
	{
		return do_update_new_element(new_element, existing_ids,
					     id_combo,
					     x::make_function<void (size_t)>
					     (std::forward<F>(f)));
	}

	//! Type-erase update_new_element
	static size_t
	do_update_new_element(const new_element_t &new_element,
			      std::vector<std::string> &existing_ids,
			      const x::w::focusable_container &id_combo,
			      const x::function<void (size_t)> &f);


	//! Signature of the callback used by update_theme.

	typedef std::optional<
		std::tuple<
			x::xml::readlock,
			x::functionref<void (appObj *, const x::ref<x::obj> &)>
			>> update_callback_t;

	//! Callable object used by update_theme()
	typedef x::functionref<update_callback_t (appObj *)
		> get_updatecallback;

	//! Callable object used by update_theme()
	typedef x::functionptr<update_callback_t (appObj *)
		> get_updatecallbackptr;

	//! Update the theme

	//! Invoked by various update and delete buttons.

	//! Receives a get_updatecallbackptr, a callable object that returns
	//! a possibly-null callable object. This gets called immediately.
	//!
	//! A null return from the callback results in no further option
	//! (UI values fail validation).
	//!
	//! Otherwise, the returned object returns a second callable object.
	//! The second callable object gets invoked in the main execution
	//! thread, via appinvoke, and not the connection thread that
	//! invoked update_theme() (which resumes processing, immediately.
	//! The second callable object receives an appObj pointer, it gets
	//! executed via appinvoke().
	//!
	//! The second callable object returns a tuple with two values:
	//!
	//! 1) An XML read lock,
	//!
	//! 2) A third callable object.
	//!
	//! The XML read lock is a lock on an updated XML document that reflects
	//! the updated UI value.
	//!
	//! If the updated XML document fails to be loaded by the uigenerator
	//! (invalid value), no further action takes place. Otherwise it gets
	//! installed as the new theme, and the third callable object gets
	//! invoked.
	//!
	//! The third callable object gets invoked with two parameters:
	//! an appObj pointer (also being executed via appinvoke);
	//! and an opaque mcguffin.
	//!
	//! The opaque mcguffin is the busy mcguffin for the connection thread
	//! that blocks all pointer and keyboard event processing. The
	//! busy mcguffin gets constructed by update_theme, up front, and this
	//! is the means to block pointer and keyboard event processing until
	//! the entire update process is complete.

	void update_theme(ONLY IN_THREAD,
			  const x::w::busy &mcguffin,
			  get_updatecallbackptr
			  (appObj::*callback)(ONLY IN_THREAD));

private:
	//! Invoked by update_theme() in the main execution thread.
	void update_theme2(const x::functionref<update_callback_t(appObj *)
			   > &,
			   const x::ref<x::obj> &mcguffin);

	//! The current status of what's being update
	x::mpobj<edited_info_t> current_edited_info;

public:
	//! update status of what's being edited
	template<update_functor F>
	void update(F &&f)
	{
		do_update(x::make_function<void (edited_info_t &)>
			  (std::forward<F>(f)));
	}
private:
	//! Type-erased update().
	void do_update(const x::function<void (edited_info_t &)> &);
public:
	//! All supported appearance types, by their name.
	const appearance_types_t appearance_types;

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

	const x::w::validated_input_field<
		std::variant<std::string, double>
		//! Borders page: border_size

		> border_width_validated,

	//! Borders page: border_size
		border_height_validated,

	//! Borders page: border_size
		border_hradius_validated,

	//! Borders page: border_size
		border_vradius_validated;

	//! Borders page: scaled border size
	const x::w::validated_input_field<
		std::optional<unsigned>
		> border_width_scale_validated,
	//! Borders page: scaled border size
		border_height_scale_validated,
	//! Borders page: scaled border size
		border_hradius_scale_validated,
	//! Borders page: scaled border size
		border_vradius_scale_validated;

	//! Borders page: border dashes
	const x::w::validated_input_field<
		std::vector<double>
		> border_dashes_field_validated;

	//! Validator for the point size field
	const x::w::validated_input_field<std::optional<double>
					  > font_point_size_validated;

	//! Collection of standard values of various font properties.

	const standard_font_values_t standard_font_values;

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
				void (appObj::*what_to_do_next)
				(ONLY IN_THREAD));

	//! We know what we want to do after save-as dialog closes.
	void do_file_save_as_event(ONLY IN_THREAD,
				   void (appObj::*what_to_do_next)
				   (ONLY IN_THREAD));
public:
	//! We might be very busy.

	//! Shows the "wait" cursor, until the connection thread is idle.

	void busy();

	//! File new event
	void file_new_event(ONLY IN_THREAD);
	//! File save dialog closed
	void do_check_and_file_save(ONLY IN_THREAD,
				    std::string,
				    void (appObj::*what_to_do_next)
				    (ONLY IN_THREAD));

	//! File save dialog closed, checked for overwriting, ok to save now
	void do_file_save(ONLY IN_THREAD, const std::string &filename,
			  void (appObj::*what_to_do_next)(ONLY IN_THREAD));

	//! What do_file_save calls when only saving a file, and nothing more.
	void only_save(ONLY IN_THREAD);

	//! File open event
	void file_open_event(ONLY IN_THREAD);

	//! Show the file open dialog
	void open_file(ONLY IN_THREAD);

	//! Load the initial file, that was passed in as a parameter.

	//! An empty string indicates no file was passed in.
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

	//! Help/About event
	void help_about(ONLY IN_THREAD);

private:
	//! Stashed pointer to pick up after "Save As" closes.

	void (appObj::*what_to_do_after_save_as_thread_only)(ONLY IN_THREAD);

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

	void ifnotedited(ONLY IN_THREAD,
			 void (appObj::*whattodo)(ONLY IN_THREAD),
			 const char *save_label,
			 const char *nosave_label,
			 const char *nosave_label_without_shortcut,
			 const char *cancel_label,
			 const char *nosave_shortcut);

	//! The end of the road for a file_quit_event
	void stoprunning(ONLY IN_THREAD);

	//! Create a new file
	void new_file(ONLY IN_THREAD);

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

	void dimension_initialize(ONLY IN_THREAD);

private:
	/////////////////////////////////////////////////////////////////////
	// metadata about the dimensions page.

	//! Original dimension parameters

	struct dimension_orig_params {
		size_t index; //!< Which original dimension
		std::optional<size_t> scale_from; //!< If not empty
		std::string value; //!< Original value
	};

	//! New values entered for the new or updated dimension.

	struct dimension_save_params {

		std::string dimension_new_name; //!< When creating a new one.

		std::optional<size_t> scale_from; //!< If not empty

		std::string value; //!< Dimension value

		//! Whether the saved parameters are the same as the orig ones

		bool operator==(const dimension_orig_params &orig)
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

public:
	//! Thread-safe container for dimension metadata.

	typedef x::mptobj<dimension_info_s,
			  std::recursive_mutex> dimension_info_t;
private:
	//! Enable or disable the reset button.
	void dimension_enable_disable_buttons(dimension_info_t::lock &lock);

	//! Reset button pressed
	void dimension_reset();

	//! Reset all fields to their original values, for the current dim
	void dimension_reset_values(dimension_info_t::lock &lock);
public:

	//! Current dimension data
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
	//! Save/update dimension, invoked from update_theme().
	get_updatecallbackptr dimension_update(ONLY IN_THREAD);

private:

	//! Continue dimension_update processing, in the main execution thread
	update_callback_t dimension_update2(dimension_info_t::lock &lock);

	//! Finish dimension_update processing, in the main execution thread
	void dimension_update2(dimension_info_t::lock &lock,
			       const std::string &id,
			       bool is_new,
			       const x::ref<x::obj> &busy_mcguffin);
public:
	//! Delete dimension, invoked from update_theme().
	get_updatecallbackptr dimension_delete(ONLY IN_THREAD);

private:

	//! Continue dimension_delete processing, in the main execution thread
	update_callback_t dimension_delete2(dimension_info_t::lock &lock);

	//! Finish dimension_delete processing, in the main execution thread
	void dimension_delete2(dimension_info_t::lock &lock,
			       size_t index,
			       const x::ref<x::obj> &busy_mcguffin);
public:

	/////////////////////////////////////////////////////////////////
	//
	// Colors page.

	//! Installs callbacks.

	static void colors_elements_initialize(app_elements_tptr &,
					       x::w::uielements &,
					       init_args &);

	//! Initialize the colors page after loading/creating a new theme.

	void colors_initialize(ONLY IN_THREAD);

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
		loaded_color_t loaded_color; //!< The original color
	};

	//! Validated and saved parameters.
	struct colors_save_params {

		std::string color_new_name; //!< When creating a new one.
		loaded_color_t color_new_value; //!< The new value

		//! Whether the saved parameters are the same as the orig ones

		bool operator==(const colors_orig_params &orig)
		{
			return color_new_value == orig.loaded_color;
		}
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

	typedef x::mptobj<colors_info_s,
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

	//! Reset button
	void color_reset(ONLY IN_THREAD);

	//! A new color was selected from the color name combo-box.

	//! Or the Reset button was pressed. Always invokes color_reset_values
	//! when done.
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

	//! Save/update color, invoked from update_theme().

	get_updatecallbackptr color_update(ONLY IN_THREAD);

private:

	//! Continue color_update(), in the main execution thread.
	update_callback_t color_update2(colors_info_t::lock &lock);

	//! Finalize color_update(), in the main execution thread.
	void color_update2(colors_info_t::lock &,
			   const std::string &,
			   bool,
			   const x::ref<x::obj> &);

	//! Finish color_update(), in the connection thread.
	void color_update3(ONLY IN_THREAD, const x::ref<x::obj> &busy_mcguffin);

public:
	//! Delete color, invoked from update_theme().
	get_updatecallbackptr color_delete(ONLY IN_THREAD);

private:

	//! Continue color_delete(), in the main execution thread.
	update_callback_t color_delete2(colors_info_t::lock &lock);

	//! Finish color_delete(), in the main execution thread.
	void color_delete2(colors_info_t::lock &lock,
			   size_t index,
			   const x::ref<x::obj> &busy_mcguffin);

	//! The app is running.

	//! Cleared by file_quit_event(), to stop the main execution thread.
	bool running=true;
public:

	/////////////////////////////////////////////////////////////////////
	//
	// Borders page.

	//! Installs callbacks.

	static void borders_elements_initialize(app_elements_tptr &,
						x::w::uielements &,
						init_args &);

	//! Initialize the dimension page after loading/creating a new theme.

	void borders_initialize(ONLY IN_THREAD);

public:
	//! Parsed border

	struct loaded_border_t {

		//! Parsed border value
		std::string from;

		//! Parsed border value
		std::string color;
		//! Parsed border value
		std::string color2;

		//! Parsed border value
		std::string width;
		//! Parsed border value
		std::optional<unsigned> width_scale;

		//! Parsed border value
		std::string height;
		//! Parsed border value
		std::optional<unsigned> height_scale;

		//! Parsed border value
		std::string hradius;
		//! Parsed border value
		std::optional<unsigned> hradius_scale;

		//! Parsed border value
		std::string vradius;
		//! Parsed border value
		std::optional<unsigned> vradius_scale;

		//! Parsed border value
		std::optional<bool> rounded;

		//! Parsed border value
		std::optional<std::vector<double>> dashes;

		//! Comparison operator

		bool operator==(const loaded_border_t &other) const
		{
			return from == other.from &&
				color == other.color &&
				color2 == other.color2 &&
				width == other.width &&
				width_scale == other.width_scale &&
				height == other.height &&
				height_scale == other.height_scale &&
				hradius == other.hradius &&
				hradius_scale == other.hradius_scale &&
				vradius == other.vradius &&
				vradius_scale == other.vradius_scale &&
				rounded == other.rounded &&
				dashes == other.dashes;
		}
	};

	//! Original border parameters

	struct border_orig_params {
		size_t index; //!< Which original border

		loaded_border_t loaded_border; //!< The original border
	};

	//! Validated and saved parameters.

	struct border_save_params {
		std::string border_new_name;  //!< When creating a new one.
		loaded_border_t border_new_value; //!< The new value

		//! Check if the new value is unchanged
		bool operator==(const border_orig_params &orig) const
		{
			return border_new_value == orig.loaded_border;
		}
	};

	//! Borders metadata
	struct border_info_s {

		//! Cached: selected index of an existing border.
		std::optional<border_orig_params> current_selection;

		//! The existing borders.
		std::vector<std::string> ids;

		//! This is updated every time something changes on the
		//! borders page. The "Save" button simply takes this to
		//! the bank.

		std::optional<border_save_params> save_params;
	};

	//! Thread-safe container for borders metadata.

	typedef x::mptobj<border_info_s, std::recursive_mutex> border_info_t;

	//! Current border data
	border_info_t border_info;

	//! Internally the border size gets stored as a variant parameter.

	//! Convert it to visual representation.
	static std::string border_format_size(const std::variant<std::string,
					      double> &);
private:
	//! Initialize the dimension page after loading/creating a new theme.

	void borders_initialize(ONLY IN_THREAD, border_info_t::lock &lock);
public:
	//! Something was selected or unselected in the border combo-box

	void border_selected(ONLY IN_THREAD,
			     const
			     x::w::standard_combobox_selection_changed_info_t
			     &);

	//! "Reset" button
	void border_reset(ONLY IN_THREAD);

	//! A new border was selected in the border combo-box

	//! Or the Reset button was pressed. Always invokes border_reset_values
	//! when done.
	void border_selected_locked(ONLY IN_THREAD,
				    border_info_t::lock &lock);

	//! Reset all fields to their original values, for the current border
	void border_reset_values(ONLY IN_THREAD,
				 border_info_t::lock &lock);

	//! Enable/disable elements
	void border_enable_disable_buttons(ONLY IN_THREAD,
					   border_info_t::lock &lock);

	//! Enable/disable elements

	//! Invoked from UI callback.
	static void border_enable_disable(ONLY IN_THREAD);

	//! Enable/disable elements

	//! Invoked from UI callback.
	static void border_enable_disable_later();

	//! Save/update border, invoked from update_theme().

	get_updatecallbackptr border_update(ONLY IN_THREAD);

private:

	//! Continue border_update(), in the main execution thread.

	update_callback_t border_update2(border_info_t::lock &lock);

	//! Finalize border_update(), in the main execution thread.

	void border_update2(border_info_t::lock &lock,
			    const std::string &id,
			    bool is_new,
			    const x::ref<x::obj> &busy_mcguffin);

	//! Finish border_update(), in the connection thread.
	void border_update3(ONLY IN_THREAD,
			    const x::ref<x::obj> &busy_mcguffin);

public:
	//! Delete border, invoked from update_theme().
	get_updatecallbackptr border_delete(ONLY IN_THREAD);

private:

	//! Continue border_delete(), in the main execution thread.
	update_callback_t border_delete2(border_info_t::lock &lock);

	//! Finish border_delete(), in the main execution thread.
	void border_delete2(border_info_t::lock &lock,
			    size_t index,
			    const x::ref<x::obj> &busy_mcguffin);

public:


	/////////////////////////////////////////////////////////////////////
	//
	// Fonts page.

	//! Installs callbacks.

	static void fonts_elements_initialize(app_elements_tptr &,
					      x::w::uielements &,
					      init_args &);

	//! Initialize the dimension page after loading/creating a new theme.

	void fonts_initialize(ONLY IN_THREAD);

public:
	//! Font size can be specified as point size, or scaled

	//! Note: enum values in same order as the corresponding
	//! font_size_type combo-box values.

	enum class font_size_type_t
		{
		 point_size,
		 scaled_size,
		 scale
		};

	//! Parsed font

	struct loaded_font_t {

		//! Parsed font value
		std::string from;

		//! Parsed font value
		std::string family;

		//! Parsed font value
		font_size_type_t size_type=font_size_type_t::point_size;

		//! Parsed font value
		std::optional<double> size;

		//! Parsed font value
		std::string foundry;

		//! Parsed font value
		std::string style;

		//! Parsed font value
		std::string weight;

		//! Parsed font value
		std::string spacing;

		//! Parsed font value
		std::string slant;

		//! Parsed font value
		std::string width;

		bool operator==(const loaded_font_t &other) const
		{
			return from == other.from &&
				family == other.family &&
				size_type == other.size_type &&
				size == other.size &&
				foundry == other.foundry &&
				style == other.style &&
				weight == other.weight &&
				spacing == other.spacing &&
				slant == other.slant &&
				width == other.width;
		}
	};

	//! Original font parameters

	struct font_orig_params {
		size_t index; //!< Which original font

		loaded_font_t loaded_font; //!< The original font
	};

	//! Validated and saved parameters.

	struct font_save_params {
		std::string font_new_name;  //!< When creating a new one.
		loaded_font_t font_new_value; //!< The new value

		//! Check if the new value is unchanged
		bool operator==(const font_orig_params &orig) const
		{
			return font_new_value == orig.loaded_font;
		}
	};

	//! Fonts metadata
	struct font_info_s {

		//! Cached: selected index of an existing font.
		std::optional<font_orig_params> current_selection;

		//! The existing fonts.
		std::vector<std::string> ids;

		//! This is updated every time something changes on the
		//! fonts page. The "Save" button simply takes this to
		//! the bank.

		std::optional<font_save_params> save_params;

		//! Whether a call to font_enable_disable is already scheduled

		bool enable_disable_scheduled=false;
	};

	//! Thread-safe container for fonts metadata.

	typedef x::mptobj<font_info_s, std::recursive_mutex> font_info_t;

	//! Current font data
	font_info_t font_info;

private:
	//! Initialize the dimension page after loading/creating a new theme.

	void fonts_initialize(ONLY IN_THREAD, font_info_t::lock &lock);
public:
	//! Something was selected or unselected in the font combo-box

	void font_selected(ONLY IN_THREAD,
			     const
			     x::w::standard_combobox_selection_changed_info_t
			     &);

	//! "Reset" button
	void font_reset(ONLY IN_THREAD);

	//! A new font was selected in the font combo-box

	//! Or the Reset button was pressed. Always invokes font_reset_values
	//! when done.
	void font_selected_locked(ONLY IN_THREAD,
				    font_info_t::lock &lock);

	//! Reset all fields to their original values, for the current font
	void font_reset_values(ONLY IN_THREAD,
				 font_info_t::lock &lock);

	//! Enable/disable elements
	void font_enable_disable_buttons(ONLY IN_THREAD,
					   font_info_t::lock &lock);

	//! Enable/disable elements

	//! Invoked from UI callback.
	static void font_enable_disable_later(ONLY IN_THREAD);

	//! Invoked from UI callback.
	static void font_enable_disable_now(ONLY IN_THREAD);

	//! Invoked from UI callback.
	void font_enable_disable(ONLY IN_THREAD);
	//! Save/update font, invoked from update_theme().

	get_updatecallbackptr font_update(ONLY IN_THREAD);

private:

	//! Continue font_update(), in the main execution thread.

	update_callback_t font_update2(font_info_t::lock &lock);

	//! Finalize font_update(), in the main execution thread.

	void font_update2(font_info_t::lock &lock,
			    const std::string &id,
			    bool is_new,
			    const x::ref<x::obj> &busy_mcguffin);

	//! Finish font_update(), in the connection thread.
	void font_update3(ONLY IN_THREAD,
			    const x::ref<x::obj> &busy_mcguffin);

public:
	//! Delete font, invoked from update_theme().
	get_updatecallbackptr font_delete(ONLY IN_THREAD);

private:

	//! Continue font_delete(), in the main execution thread.
	update_callback_t font_delete2(font_info_t::lock &lock);

	//! Finish font_delete(), in the main execution thread.
	void font_delete2(font_info_t::lock &lock,
					    size_t index,
					    const x::ref<x::obj> &busy_mcguffin);

public:
	/////////////////////////////////////////////////////////////////////
	//
	// Appearance page.

	//! Installs callbacks.

	static void appearances_elements_initialize(app_elements_tptr &elements,
						    x::w::uielements &ui,
						    init_args &args,
						    const x::w::main_window &,
						    const x::messages &,
						    const
						    x::w::screen_positions &);

	//! Placeholder for an \ref appearance_value_t "appearance value".
	struct reset {

		//! Always equal.
		bool operator==(const reset &) const { return true; }
	};

	//! An appearance field value is just a string, for our purposes.

	//! Except when it's not, <reset />s in optionals and vectors
	//! is an alternative.

	typedef std::variant<reset, std::u32string> appearance_value_t;

	//! An appearance field, and its value

	struct appearance_fv {

		std::string field;
		appearance_value_t value;

		bool operator==(const appearance_fv &other) const
		{
			return field == other.field && value == other.value;
		}

		//! Delegating constructor (sort of)
		template<typename F, typename V>
		appearance_fv(F &&f, V && v)
			: field{std::forward<F>(f)},
			  value{std::forward<V>(v)}
		{
		}

		//! Add this field and value to a list_item_param.

		//! Used to convert a field/value to a table row, in the
		//! appearance_value container.
		//!
		//! push_back()s two values here.
		void format(std::vector<x::w::list_item_param> &v) const;
	};

	//! Original appearance parameters

	struct appearance_orig_params {
		size_t index; //!< Which original appearance

		std::string from; //!< Appearance based on.

		std::vector<appearance_fv> values; //! Values.
	};

	//! Validated and saved parameters.

	struct appearance_save_params {
		std::string appearance_new_name;  //!< When creating a new one.

		// Existing appearance:

		std::string from; //!< Appearance based on.

		std::vector<appearance_fv> values; //!< Values.

		//! Which value is currently highlighted, if any.

		std::optional<size_t> current_value;

		//! Whether the current_value is being updated.

		bool current_value_update=false;

		//! Check if the new value is unchanged

		bool operator==(const appearance_orig_params &orig) const
		{
			return from == orig.from && values == orig.values;
		}
	};

	//! All fields in the appearance object.

	struct appearance_field {

		//! Either "type[name]" or "ref[name}"

		//! This field is either "name" (like color_arg, dim-arg)
		//! or a reference to another appearance type, i.e.
		//! "scrollbar".

		std::string typeref;

		//! Description

		std::string descr;

		//! Whether this is a vector
		bool multiple;

		//! Whether this value can be '<reset>'

		//! Implied by "multiple"
		bool reset;
	};

	//! Appearances metadata
	struct appearance_info_s {

		//! Cached: selected index of an existing appearance.
		std::optional<appearance_orig_params> current_selection;

		//! New appearance type
		std::string new_appearance_type;

		//! The existing appearances.
		std::vector<std::string> ids;

		//! The existing appearance types
		std::vector<std::string> types;

		//! Cached possible appearances based on values for the currently shown type

		std::vector<std::string> current_defaults;

		//! Cached fields, in order, of the currently shown appearance.

		std::map<std::string, appearance_field> current_fields;

		//! This is updated every time something changes on the
		//! appearances page. The "Save" button simply takes this to
		//! the bank.

		appearance_save_params save_params;
	};

	//! Thread-safe container for appearances metadata.

	typedef x::mptobj<appearance_info_s,
			  std::recursive_mutex> appearance_info_t;

	//! Current appearance data
	appearance_info_t appearance_info;

	//! Initialize the colors page after loading/creating a new theme.

	void appearances_initialize(ONLY IN_THREAD);

	//! Something was selected or unselected in the appearance combo-box

	//! Reset all appearance fields.
	void appearance_reset(ONLY IN_THREAD);

	//! Something was selected or unselected in the new appearance type combo-box

	void new_appearance_type_selected
	(ONLY IN_THREAD,
	 const
	 x::w::standard_combobox_selection_changed_info_t &info);

	//! Enable/disable elements

	//! Invoked from UI callback.
	void appearance_enable_disable_buttons(ONLY IN_THREAD,
					       appearance_info_t::lock &lock);

	//! Enable/disable individual fields on the "Create" submenu.

	//! Invoked from UI callback. Once a field value is set, it is disabled
	//! in the Create submenu, unless it's a multiple field.
	void appearance_enable_disable_new_fields(ONLY IN_THREAD,
						  appearance_info_t::lock &lock)
		;

	//! The appearance's description in the appearance_name combo-box.
	new_element_t appearance_name_and_description(const std::string &n);

	//! Create a new appearance object, invoked from update_theme().
	get_updatecallbackptr appearance_create(ONLY IN_THREAD);

	//! Currently selected appearance value has changed.

	void appearance_value_selected(ONLY IN_THREAD,
				       appearance_info_t::lock &,
				       const x::w::list_item_status_info_t &);

	//! Selection made from the appearance value popup menu
	void appearance_on_value_move_up(ONLY IN_THREAD);

	//! Selection made from the appearance value popup menu
	void appearance_on_value_move_down(ONLY IN_THREAD);

	//! Selection made from the appearance value popup menu
	void appearance_on_value_update(ONLY IN_THREAD);

	//! Selection made from the appearance value popup menu
	void appearance_on_value_create(ONLY IN_THREAD);

	//! Selection made from the appearance value popup menu
	void appearance_on_value_delete(ONLY IN_THREAD);

	//! Get new appearance name

	//! Retrieve the name from the input field.
	//!
	//! The lock parameter enforces the same locking order.
	std::string get_new_appearance_name(appearance_info_t::lock &);
private:
	//! Selected the option for the new value

	//! Callback from the radio button next to the new value, on the
	//! appearance dialog. Enables or disables the new value input field

	void appearance_value_option_selected(ONLY IN_THREAD, size_t i);

	//! "Create" selected from appearance submenu, create the dialog
	void appearance_value_edit_create(ONLY IN_THREAD, const std::string &);

	//! "Update" selected from appearance submenu, create the dialog
	void appearance_value_edit_update(ONLY IN_THREAD);

	//! Common code for  appearance_value_edit_create and update.
	void appearance_value_edit(ONLY IN_THREAD,
				   appearance_info_t::lock &lock,
				   const std::string &n,
				   const appearance_value_t &v);

	//! Continue appearance_create(), in the main execution thread.
	update_callback_t appearance_create2(appearance_info_t::lock &lock);

	//! Finish appearance_create(), in the main execution thread.
	void appearance_create2(appearance_info_t::lock &lock,
				const std::string &new_appearance_name,
				const x::ref<x::obj> &busy_mcguffin);



	//! Save/update appearance, invoked from update_theme().

	get_updatecallbackptr appearance_update(ONLY IN_THREAD);

	//! Continue appearance_update(), in the main execution thread.

	update_callback_t appearance_update2(appearance_info_t::lock &lock);

	//! Finalize appearance_update(), in the main execution thread.
	void appearance_update2(appearance_info_t::lock &lock,
				const std::string &id,
				const x::ref<x::obj> &busy_mcguffin);

	//! Finish appearance_update(), in the connection thread.
	void appearance_update3(ONLY IN_THREAD,
				const x::ref<x::obj> &busy_mcguffin);


	//! Returns the description of the current appearance type.

	//! The parameter is the current_selection.
	x::xml::readlock appearance_current_value(appearance_info_t::lock &);

	//! Find the currently shown appearance type

	//! Return value of appearance_current_value() is the parameter.
	appearance_types_t::const_iterator
	appearance_current_value_type(const x::xml::readlock &);

	//! Find the currently shown appearance type

	//! \overload
	appearance_types_t::const_iterator
	appearance_current_value_type(appearance_info_t::lock &);

public:
	//! Save button in the new value appearance button.

	//! The new value is a standard combo-box, that specifies the new
	//! value by its index.

	void appearance_new_value_std_combo(ONLY IN_THREAD,
					    const std::string &field_name,
					    const std::vector<
					    std::u32string> &);

	//! Save button in the new value appearance button.

	//! The new value is an editable combo-box, that specifies the new
	//! value in its input field.

	void appearance_new_value_edit_combo(ONLY IN_THREAD,
					     const std::string &field_name);
private:
	//! Save button in the new value appearance button.

	//! The new value is a free-form input field.

	void appearance_new_value_input(ONLY IN_THREAD,
					const std::string &field_name);

	//! Save button in the new appearance dialog.

	void appearance_new_value_save_and_hide(ONLY IN_THREAD,
						const std::string &,
						const std::u32string &);
	//! Delete appearance, invoked from update_theme().
	get_updatecallbackptr appearance_delete(ONLY IN_THREAD);

	//! Continue appearance_delete(), in the main execution thread.
	update_callback_t appearance_delete2(appearance_info_t::lock &lock);

	//! Finish appearance_delete(), in the main execution thread.
	void appearance_delete2(appearance_info_t::lock &lock,
				size_t index,
				const x::ref<x::obj> &busy_mcguffin);
public:
	/////////////////////////////////////////////////////////////////////
	//
	// Generator page.

	// The main generator list.

	const appgenerator_functions current_generators;

	//! Installs callbacks.

	static void generators_elements_initialize
	(app_elements_tptr &common_elements,
	 x::w::uielements &ui,
	 const uicompiler &uiptr);

	//! Initialize the colors page after loading/creating a new theme.

	void generators_initialize(ONLY IN_THREAD);

	//! The generator's description in the generator_name combo-box.
	new_element_t generator_name_and_description(const std::string &n);

	//! Something was selected or unselected in the generator combo-box

	//! Reset all generator fields.
	void generator_reset(ONLY IN_THREAD);

	//! Return the state of the new generator name and type fields.

	std::tuple<std::string,
		   std::optional<size_t>>
	get_new_generator_name_and_type(generator_info_lock &);

	//! Enable/disable elements

	//! Invoked from UI callback.

	void generator_enable_disable_buttons(ONLY IN_THREAD,
					      generator_info_lock &lock);

	void generator_enable_disable_buttons(ONLY IN_THREAD,
					      generator_info_lock &lock,
					      const x::w::listlayoutmanager &);

	//! Create new generator button clicked
	get_updatecallbackptr generator_new_create_clicked(ONLY IN_THREAD);

	//! Continue generator_new_create_clicked(), in the main execution thread.
	update_callback_t generator_new_create_clicked2(generator_info_lock
							&lock);

	//! Delete generator, invoked from update_theme().
	get_updatecallbackptr generator_delete(ONLY IN_THREAD);

	//! Continue generator_delete(), in the main execution thread.
	update_callback_t generator_delete2(generator_info_lock &lock);

	//! Finish generator_delete(), in the main execution thread.
	void generator_delete2(generator_info_lock &lock,
			       size_t index,
			       const x::ref<x::obj> &busy_mcguffin);

private:

	//! compiler_lookup() callback parameter.

	typedef void compiler_lookup_cb_t(const const_uicompiler_generators &);

	//! Find the current generator's compiler.

	//! For the given generator #n, read from the theme file, look up
	//! its actual definition, and its compiler, and invoke the callback
	//! function with this information.
	//!
	//! The passed-in this_generator gets positioned to the XML node
	//! for the specified generator and the callback gets invoked with
	//! the generator's compiler as its parameter.

	template<typename F>
	void compiler_lookup(generator_info_lock &lock,
			      size_t n,
			      const x::xml::readlock &this_generator,
			      F && f)
	{
		do_compiler_lookup(lock, n,
				    this_generator,
				    x::make_function<compiler_lookup_cb_t>
				    (std::forward<F>(f)));
	}

	//! type-erased compiler_lookup().
	void do_compiler_lookup(generator_info_lock &lock,
				 size_t n,
				 const x::xml::readlock &this_generator,
				 const x::function<compiler_lookup_cb_t> &cb);

	//! Return the compiler specified by the this_generator's XML node.

	const_uicompiler_generators
	compiler_lookup(const x::xml::readlock &this_generator);
public:

	//! Save/update generator, invoked from update_theme().

	get_updatecallbackptr generator_update(ONLY IN_THREAD);

	//! Continue generator_update(), in the main execution thread.

	update_callback_t generator_update2(generator_info_lock &lock);

	//! Finalize generator_update(), in the main execution thread.
	void generator_update2(generator_info_lock &lock,
			       const x::ref<x::obj> &busy_mcguffin);

	//! Finish generator_update(), in the connection thread.
	void generator_update3(ONLY IN_THREAD,
			       const x::ref<x::obj> &busy_mcguffin);

	//! Automatically create a new layout or a factory

	//! Called when saving/updating new generator. If a generator
	//! function references another layour or factory, this checks
	//! if it doesn't exist, and automatically creates it.

	void generator_autocreate_layout_or_factory
	(const x::xml::writelock &lock,
	 appgenerator_save &save_info,
	 const char *layout_or_factory,
	 const std::string &type,
	 const std::string &id);
};

// The singleton instance of the app object.
typedef x::singletonptr<appObj> appsingleton;

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
