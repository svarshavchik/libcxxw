/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef creator_setting_handler_h
#define creator_setting_handler_h

#include "creator/setting_handlerfwd.H"
#include "creator/create_ui_info.H"
#include "creator/appgenerator_savefwd.H"
#include "creator/parse_parameterfwd.H"
#include <x/xml/readlockfwd.H>
#include "x/w/connection_threadfwd.H"

#include <unordered_map>
#include <string>
#include <string_view>

// Setting-specific behavior.

// Each possible type of a setting implements create_ui() which receives
// the setting's current value.
//
// create_ui() should use the given factory to create a single widget for
// the setting's value.
//
// It returns a closure that gets called to retrieve the setting's (possibly)
// updated value. It should return std::nullopt if the setting's value failed
// validation. The closure receives a bool parameter, which is normally true,
// and the closure should report an error if the validation fails. A false
// parameter indicates that the closure should only return std::nullopt
// and without reporting an error. This is used when the closure gets used
// to retrieve the current value for secondary purposes. Normally the closure
// gets called when attempting to save the generator's new value, and full
// validation should occur, and an error should be reported; but sometimes UI
// work needs to know the current value, and it's ok if it's not currently
// set.
//
// define_additional_parameters() gets called after parsing all generator
// parameters, and it receives a map of all XML elements that have not been
// recognized as a valid parameter. The handler can look through this for
// anything of interest, if found it gets removed. The layoutmanager_type
// setting looks to see if there's a <config> value for the layoutmanager,
// and loads one if it exists.
//
// flag_value() returns an indication whether this setting is a flag, that is
// a boolean value.
//
// load() loads the parameter's value. The default implementation calls
// get_text() and stores it in the string_value.
//
// load_from_parent_element() gets called with the parent element's XML node.
// This is used to read the existing @id attribute. It returns true if
// this parameter occurs multiple times, and false otherwise.
//
// saved_element() gets called after the element, and its value, are saved
// The write lock is positioned at the saved element's XML node. This is used
// to write additional content. For example, the layoutmanager_type handler,
// used by a <container> will checkif any <config> values were specified, then
// proceed and write the <config> section.
//
// This is also used by the text_param setting to add type="theme_text"
// attribute.

struct setting_handler {

	// Map <parameter> types to setting handlers

	static const std::unordered_map<std::string_view,
					const setting_handler *
					> parameter_types;

	// Map object <member> types to setting handlers
	static const std::unordered_map<std::string_view,
					const setting_handler *
					> member_types;

	virtual setting_create_ui_ret_t create_ui(ONLY IN_THREAD,
						  const create_ui_info &info)
		const=0;

	virtual
	void define_additional_parameters(parameter_value &value,
					  std::unordered_map<std::string,
					  x::xml::readlock>
					  &unparsed_additional_parameters)
		const;

	virtual bool load_from_parent_element(
		const x::xml::readlock &lock,
		const parse_parameterObj &parameter,
		parameter_value &value) const;

	virtual void load(const x::xml::readlock &lock,
			  parameter_value &value) const;

	// Default logic for saving a parameter, used when <xpath>.</xpath>
	// was not specified:
	//
	//     <parameter_name>string_value</parameter_name>

	virtual void save(const setting_save_info &save_info,
			  appgenerator_save &info) const;

	virtual void saved_element(const setting_save_info &save_info,
				   appgenerator_save &info) const;

	virtual bool flag_value() const;
};

#endif
