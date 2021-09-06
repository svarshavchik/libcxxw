/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef creator_appgenerator_functionobj_h
#define creator_appgenerator_functionobj_h

#include "appgenerator_functionfwd.H"
#include "x/w/gridlayoutmanagerfwd.H"
#include "x/w/text_param.H"
#include <x/obj.H>
#include <x/xml/writelock.H>

//! Implementation object.

//! \see appgenerator_function

class appgenerator_functionObj: virtual public x::obj {

public:
	//! Constructor
	appgenerator_functionObj();

	//! Destructor
	~appgenerator_functionObj();

	//! Return a description for this function.
	virtual x::w::text_param description(description_format fmt) const=0;

	//! Create a UI for editing this generator function's parameter.

	//! Gets a grid layout manager as a parameter. Appends rows with
	//! two cells, a row gets appended for each parameter, or property.
	//!
	//! The first cell in each row is the name of the parameter, the
	//! second cell is the widget that edits this parameter.
	//!
	//! \return a callable object that returns a boolean value, which
	//! gets called when "Save" is pressed in the UI. A \c true value
	//! gets returned if all entered values are valid, and the generator
	//! function's parameters have been updated. A \c false return value
	//! indicates that some parameter failed validation.

	virtual generator_create_ui_ret_t
	create_ui(const x::w::gridlayoutmanager &) const=0;

	//! Clone this generator function

	//! A layout or a factory definition maintains a list of its
	//! generator functions, kept as constant objects. A new
	//! generator function gets added to a parser by cloning the
	//! constant object, and adding it to the
	//! \ref appgenerator_functions "list of generator functions".

	virtual appgenerator_function clone() const=0;

	//! Save this function as XML

	//! The XML write lock is position on the parent factory or layout,
	//! this used create_child() to create a new child element, when
	//! done the write lock's position is returned to what it was.

	virtual void save(const x::xml::writelock &lock) const=0;
};

#endif
