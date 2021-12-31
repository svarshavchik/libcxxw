/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_screen_positions_confighandleobj_h
#define x_w_screen_positions_confighandleobj_h

#include <x/w/impl/screen_positions_confighandlefwd.H>
#include <x/xml/writelockfwd.H>
#include <x/xml/readlockfwd.H>

LIBCXXW_NAMESPACE_START

//! Implementation object

//! \see screen_positions_handle

class screen_positionsObj::config_handleObj : virtual public obj {

	//! The XML configuration file
	const ref<implObj> impl;

	//! My XPath
	const std::string xpath;

	//! My namespace and type.
	const ref<widget_typeObj> widget_type;

	//! My unique label in the window.
	const std::string name;

	//! Position the existing readlock on the xpath

	void position(const xml::readlock &l) const;
public:
	//! Constructor
	config_handleObj(const ref<implObj> &impl,
			 const std::string &xpath,
			 const ref<widget_typeObj> &widget_type,
			 const std::string_view &name);

	//! Destructor
	~config_handleObj();

	//! Return a lock for reading the existing configuration
	xml::readlock config() const;

	//! Return a lock for creating a new configuration

	xml::writelock newconfig(bool remove_existing=true);
};

LIBCXXW_NAMESPACE_END

#endif
