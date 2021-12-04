/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef creator_object_appgenerator_functionobj_h
#define creator_object_appgenerator_functionobj_h

#include "creator/object_appgenerator_functionfwd.H"
#include "creator/appgenerator_functionfwd.H"
#include "creator/appgenerator_functionobj.H"
#include "creator/parse_parameterfwd.H"
#include "creator/app.H"
#include "x/w/text_paramfwd.H"
#include "x/w/gridlayoutmanagerfwd.H"

#include <x/xml/writelockfwd.H>

//! Implementation object.

//! \see object_appgenerator_function

struct object_appgenerator_functionObj : public appgenerator_functionObj {

	//! Name of the object that has this member.

	const std::string object_name;

	//! The object's member being implemented here.

	const parse_parameter member;

	//! Its value.

	const object_generator_value value;

	//! Constructor
	object_appgenerator_functionObj(const std::string &object_name,
					const parse_parameter &member,
					parameter_value value);

	//! Destructor
	~object_appgenerator_functionObj();

	//! Implement clone()

	//! Calls clone_me().
	appgenerator_function clone() const override;

	//! Duplicate this object.
	object_appgenerator_function clone_me() const;

	//! Implement description()
	x::w::text_param description(description_format fmt) const override;

	//! Implement create_ui()
	generator_create_ui_ret_t create_ui(ONLY IN_THREAD,
					    const x::w::main_window &mw,
					    const x::w::gridlayoutmanager &glm)
		const override;

	//! Implement has_ui()
	bool has_ui() const override;

	//! Implement create_ui()
	generator_create_ui_ret_t create_ui(ONLY IN_THREAD, appObj *app,
					    const x::w::main_window &mw,
					    const x::w::gridlayoutmanager &glm)
		const;

	//! Implement save()
	void save(const x::xml::writelock &lock,
		  appgenerator_save &info) const override;
};

#endif
