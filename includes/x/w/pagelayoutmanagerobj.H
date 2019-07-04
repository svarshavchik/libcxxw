/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_pagelayoutmanagerobj_h
#define x_w_pagelayoutmanagerobj_h

#include <x/w/pagelayoutmanagerfwd.H>
#include <x/w/pagefactoryfwd.H>
#include <x/w/elementfwd.H>
#include <x/w/layoutmanagerobj.H>

#include <optional>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see pagelayoutmanager

class pagelayoutmanagerObj : public layoutmanagerObj {

public:
	class LIBCXX_HIDDEN implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	pagelayoutmanagerObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~pagelayoutmanagerObj() LIBCXX_HIDDEN;

	//! Append new elements
	pagefactory append();

	//! Insert elements before an existing one.
	pagefactory insert(size_t);

	//! Remove an widget.
	void remove(size_t);

	//! How many pages there are right now.
	size_t pages() const;

	//! Look up the element's current index position.
	std::optional<size_t> lookup(const element &) const;

	//! Return the specified page widget.
	element get(size_t) const;

	//! Return the currently opened- element, if there is one.

	std::optional<size_t> opened() const;

	//! Switch to the given widget.
	void open(size_t);

	//! Un-switch, don't show any widget.
	void close();

	//! Implement generate()

	//! \see uigenerators

	void generate(const std::string_view &name,
		      const const_uigenerators &generators,
		      uielements &elements) override;

	using layoutmanagerObj::generate;
};

LIBCXXW_NAMESPACE_END

#endif
