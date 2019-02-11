/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef font_picker_impl_h
#define font_picker_impl_h

#include "x/w/font_picker.H"
#include "x/w/font_picker_configfwd.H"
#include "x/w/impl/popup/popupfwd.H"
#include "x/w/container.H"
#include "x/w/button.H"
#include "x/w/label.H"
#include "x/w/font.H"
#include "x/w/input_field.H"
#include "x/w/textlabelobj.H"
#include "x/w/listlayoutmanagerfwd.H"
#include "x/w/standard_comboboxlayoutmanagerfwd.H"
#include "x/w/impl/connection_threadfwd.H"
#include "font_picker/font_family_group_entry.H"
#include "font_picker/font_picker_previewfwd.H"
#include "image_button_internalfwd.H"
#include <x/functionalrefptr.H>
#include <x/weakptr.H>

LIBCXXW_NAMESPACE_START

#pragma GCC visibility push(hidden)
#include "font_picker/font_picker.inc.H"
#pragma GCC visibility pop

struct font_picker_impl_init_params;
struct LIBCXX_HIDDEN font_picker_int_option_info;

//! Internal implementation object.

class LIBCXX_HIDDEN font_pickerObj::implObj : virtual public obj {

 public:
	//! Available font family groups.

	//! This vector is sorted by entry->id.

	const std::vector<const_font_family_group_entry> families;

	//! The popup button in the font picker.
	const image_button_internal popup_button;

	//! The fields in the popup
	const font_picker_popup_fields popup_fields;

	//! Constructor
	implObj(const font_picker_impl_init_params &init_params);

	//! Destructor
	~implObj();

	//! Set from the config
	const bool selection_required;

	//! Set from the config
	const bool foundry_required;

	//! Font family combobox updated.
	void update_font_family(ONLY IN_THREAD,
				const listlayoutmanager &);

	//! save_font_size(), update_font_options(), then compute_new_preview()
	void update_font_size(ONLY IN_THREAD,
			      unsigned new_point_size);
 private:
	//! Update current_font accordingly.
	void save_font_size(ONLY IN_THREAD,
			    double new_point_size);

	//! Most recently entered point size
	font current_font_thread_only;

	//! Current callback.
	functionptr<font_picker_callback_t> callback_thread_only;

 public:
	THREAD_DATA_ONLY(current_font);
	THREAD_DATA_ONLY(callback);

 private:
	//! The contents of the combobox.

	//! This is the families vector, but with most recently selected
	//! fonts at the beginning, and a separator element (null ptr entry).
	//!
	//! Initialized in the constructor. Modified only by the connection
	//! thread.
	std::vector<const_font_family_group_entryptr> sorted_families;

 public:

	//! Update the sorted list of families, and create the combo-box items.

	//! Computes:
	//!
	//! - sorted list of font families managed by the font picker popup
	//!
	//! - the corresponding rows in the font family combo-box.
	//!
	//! The sorted list is returned, and the combo-box rows are placed into
	//! the items parameter that's passed by reference.
	//!
	//! Both the returned vector, and the initialized items parameter,
	//! will always be the same size.
	//!
	//! The first row in the combo-box is a prompt to pick a font family.
	//! The first value in the sorted lits is a null font family ptr.
	//!
	//! The current_family_sel is the currently selected font family, from
	//! the current value of the combobo-box. If not null, the
	//! new_current_family_sel_index gets set to the index of the currently
	//! selected font family in the returned vector.
	//!
	//! Invoked from the constructor. The returned vector sets the
	//! sorted_families class member, and items sets the initial contents
	//! of the font family combo-box.

	static std::vector<const_font_family_group_entryptr>
		compute_font_family_combobox_items
		(bool selection_required,
		 const std::vector<const_font_family_group_entry> &families,
		 const std::vector<font_picker_group_id> &most_recently_used,
		 std::vector<font_picker_group_id> &validated_mru,
		 std::vector<list_item_param> &items,
		 const const_font_family_group_entryptr &current_family_sel,
		 std::optional<size_t> &new_current_family_sel_index);

	//! The font weight combo-box's value has changed.

	//! A font option has changed

	//! A selection callback gets installed in create_font_picker() that
	//! invokes update_font_option(), for each option.

	void updated_font_option(ONLY IN_THREAD,
				 const font_picker_int_option_info &option,
				 const standard_comboboxlayoutmanager &lm);

	typedef void update_font_properties_t
		(const listlayoutmanager &,
		 const std::vector<list_item_param> &,
		 const std::optional<size_t> &);

 private:

	//! Update display elements for a newly-selected font family.

	//! Updates the font size combo-box with the available font sizes
	//! of the current font family, or a default list of font sizes if
	//! the default font family is scalable.
	//!
	//! Calls update_font_options().
	//!
	//! This overload is called from the font family callback.
	void update_font_properties(ONLY IN_THREAD,
				    const listlayoutmanager &);

	//! Update display elements for a newly-selected font family.

	//! This overload is called from the constructor in order to initialize
	//! everything by using non-IN_THREAD listlayoutmanager methods.

	void update_font_properties(const const_font_family_group_entryptr &,
				    unsigned,
				    const function<update_font_properties_t> &);

	//! Update font options.

	//! Updates the combo-boxes for the remaining font options, preserving
	//! their current selection, but reflecting the available options
	//! for the currently-selected font family+point size.
	//!
	//! This overload is invoked only from the connection thread.

	void update_font_options(ONLY IN_THREAD);

	//! Update font options

	//! This overload is called from the constructor in order to initialize
	//! everything by using non-IN_THREAD listlayoutmanager methods,
	//! and called by the first overload.
	void update_font_options(const function<update_font_properties_t> &);

	//! Update font options

	//! This overload gets called with the current family group, and it
	//! uses it to decide which values, for each options, are not found
	//! anywhere within the current family, for the currently selected
	//! point size

	void update_font_options(const function<update_font_properties_t> &,
				 const const_font_family_group_entryptr &,
				 unsigned);

	//! Reset the font options as specified in the font_spec.

	//! Sets each font option combo-box to the value specified in the
	//! font_spec.
	//!
	//! This is called from popup_closed() to reset each font option
	//! to the official font's value, undo-ing the changes that were
	//! done while the popup was option (if "Ok" was pressed, the font
	//! options were already saved, and this ends up doing nothing).

	void reset_font_options(const font &font_spec,
				const function<void (const listlayoutmanager &,
						     size_t)> &set_func);

	//! A font option has changed

	//! \overload
	void updated_font_option(ONLY IN_THREAD,
				 const standard_comboboxlayoutmanager &lm,
				 font::values_t (*get_standard_values)(),
				 font &(font::*set_function)(int));

 public:

	//! Update the preview label based on the current font options.
	void compute_new_preview(ONLY IN_THREAD);

	//! Official value of the font selector popup

	//! Besides the official font object, we also save the values
	//! of the font family combo-box and the validated input field.
	//!
	//! This is because they may be different than the default font
	//! properties.

	struct official_values_t {

		font official_font;
		text_param official_font_label;

		const_font_family_group_entryptr saved_font_group;
		double saved_font_size=0;
	};

	//! Finish initializing this object.

	//! Some things cannot be taken care of in the constructor.
	//! The validator for point size editable combo-box captures
	//! this implementation object so that it can call our
	//! update_font_size(), and our update_font_properties() needs to have
	//! point size validator.
	//!
	//! Additionally, we would like to take care of the messy details of
	//! supplying our font picker element object to the callback.
	//!
	//! finish_initialization() gets called after the public object gets
	//! constructed, and it safely stashes away all the missing pieces...

	void finish_initialization(const font_picker &,
				   const validated_input_field<unsigned>
				   &new_font_size_validator);

 private:
	//! Validator for popup_fields.font_size

	validated_input_fieldptr<unsigned> font_size_validator;

	//! Our public object
	weakptr<font_pickerptr> public_object;

 public:

	//! Replace the font shown in the popup with this font.
	void set_font(ONLY IN_THREAD, const font &f);

	//! The popup is closed. Restore the official font properties.
	void popup_closed(ONLY IN_THREAD);

	//! The "Ok" button is clicked.

	//! Set the official font properties, and invoke_callback().
	void set_official_font(ONLY IN_THREAD,
			       const callback_trigger_t &trigger);

	//! Invoke the callback

	//! The parameter is a mutable reference, and is disposable (may
	//! modify the font before passing it to the callback).

	void invoke_callback(ONLY IN_THREAD,
			     official_values_t &official_values,
			     const callback_trigger_t &trigger);

	//! Adjust the official font values before invoking the callback.

	void adjust_font_for_callback(official_values_t &official_values)
		const;

	//! Update most recently used font family groups

	//! Displays these fonts first, in the font family combo-box.

	void most_recently_used(ONLY IN_THREAD,
				const std::vector<font_picker_group_id> &mru);

	//! The mutex-protected container for official_values_t

	typedef mpobj<official_values_t> official_font_t;

	//! The official, saved font.
	official_font_t official_font;

	//! Validated most recently used vector
	mpobj<std::vector<font_picker_group_id>> validated_most_recently_used;

	//! The label display element, that shows the name of the currnt font.
	const label current_font_shown;
};

//! Metadata for each int-based font picker option.

struct LIBCXX_HIDDEN font_picker_int_option_info {

	//! The field in the \ref font "font" object with the option's value.

	int font::*font_field;

	//! Return all valid values for the option.
	font::values_t (*valid_values)();

	//! \ref font "font" method that sets the value
	font &(font::*set_value)(int);

	//! The combo-box in the font picker popup that shows the value.
	focusable_container font_picker_popup_fields::*value_combobox;

	//! fontconfig library's option name
	const char *fontconfig_name;

	//! The "please select" text for this option in its combo-box.
	const char *select_prompt;
};

//! A list of all font picker options that are represented by an \c int value.

extern const font_picker_int_option_info font_picker_int_option_infos[3];

//! Parameters to \ref font_pickerObj::implObj "font picker implementation object"'s constructor.

struct LIBCXX_HIDDEN font_picker_impl_init_params {

	const image_button_internal &popup_button;
	const label &current_font_shown;
	const font_picker_popup_fieldsptr &popup_fields;
	standard_comboboxlayoutmanager font_family_lm;
	const font_picker_config &config;
	font_pickerObj::implObj::official_values_t initial_official_values;

	font initial_font;

	std::vector<const_font_family_group_entry> families;
	std::vector<const_font_family_group_entryptr> sorted_families;

	std::vector<font_picker_group_id> most_recently_used;

	font_picker_impl_init_params(const image_button_internal &popup_button,
				     const label &current_font_shown,
				     const font_picker_popup_fieldsptr
				     &popup_fields,
				     const font_picker_config &config);
};

LIBCXXW_NAMESPACE_END

#endif
