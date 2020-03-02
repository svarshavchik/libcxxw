/*
** Copyright 2018-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_toolboxlayoutmanagerobj_h
#define x_w_toolboxlayoutmanagerobj_h

#include <x/w/toolboxlayoutmanagerfwd.H>
#include <x/w/toolboxfactoryfwd.H>
#include <x/w/layoutmanagerobj.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see toolboxlayoutmanager

class toolboxlayoutmanagerObj: public layoutmanagerObj {

public:
	class LIBCXX_HIDDEN implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! A lock on the toolbox's contents.

	mpobj_lock<toolbox_info, std::recursive_mutex> toolbox_lock;

	//! Constructor
	toolboxlayoutmanagerObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~toolboxlayoutmanagerObj() LIBCXX_HIDDEN;

	//! Return a factory to append more tool elements.

	toolboxfactory append_tools();

	//! Return a factory to insert new tools before an existing tool.

	toolboxfactory insert_tools(size_t);

	//! Remove a tool
	void remove_tool(size_t n);

	//! Remove tools

	void remove_tools(size_t first_tool, size_t n);

	//! How many tools exist.
	size_t size() const;

	//! Implement generate()

	//! \see uigenerators

	void generate(const std::string_view &name,
		      const const_uigenerators &generators,
		      uielements &elements) override;

	using layoutmanagerObj::generate;
};

LIBCXXW_NAMESPACE_END

#endif
