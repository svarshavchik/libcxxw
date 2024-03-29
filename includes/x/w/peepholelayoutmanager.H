/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_peepholelayoutmanager_h
#define x_w_peepholelayoutmanager_h

#include <x/w/peepholelayoutmanagerfwd.H>
#include <x/w/peepholelayoutmanagerobj.H>
#include <x/w/peephole_appearancefwd.H>
#include <x/w/layoutmanager.H>
#include <x/w/new_layoutmanager.H>
#include <x/w/new_focusable_layoutmanagerfwd.H>
#include <x/w/peephole_style.H>
#include <x/w/scrollbar.H>
#include <x/w/factory.H>
#include <x/w/border_arg.H>
#include <x/w/dim_arg.H>
#include <x/w/alignment.H>
#include <x/w/element.H>

#include <x/functionalrefptr.H>
#include <optional>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC new_peepholelayoutmanager;
class LIBCXX_PUBLIC new_scrollable_peepholelayoutmanager;

//! Create a new container that uses the \ref peepholelayoutmanager "peephole layout manager".

//! Passing this object to a \ref factory "factory"'s create_container() creates
//! a lightweight
//! peephole for another widget. No borders or scroll-bars get
//! created.
//!
//! Use \ref new_scrollable_peepholelayoutmanager "new_scrollable_peepholelayoutmanager"
//! to create a more fleshed out peephole, with scroll-bars and a border.
//!
//! Inherits from peephole_style, which sets most peephole properties.

class new_peepholelayoutmanager : public new_layoutmanager,
				  public peephole_style {

public:
	//! Peepholed widget factory.
	functionref<void (const factory &)> peepholed_factory;

	//! Constructor
	new_peepholelayoutmanager(const functionref<void (const factory &)> &);

	//! Down-convert the scrollable version to the non-scrollable one.

	new_peepholelayoutmanager(const new_scrollable_peepholelayoutmanager &);
	//! Destructor
	~new_peepholelayoutmanager();

private:
	//! Internal implementation
	layout_impl create(const container_impl &) const override;

	//! Internal implementation
	container create(const container_impl &,
			 const function<void(const container &)> &)
		const override;
};

//! Create a new container that uses the \ref peepholelayoutmanager "peephole layout manager", with provided scroll-bars.

//! Passing this object to a \ref factory "factory"'s
//! create_focusable_container()
//! creates a peephole for another widget.
//! Like \ref new_peepholelayoutmanager "new_peepholelayoutmanager", but
//! with a border and operational scroll-bars.
//!
//! The constructors to \c new_peepholelayoutmanager and
//! \c new_scrollable_peepholelayoutmanager have one required parameter:
//! a closure that takes a \ref factory "factory" object.
//! The closure must use the factory to create exactly
//! one widget. This becomes the widget in the peephole.
//! The widget can be a container with many widgets.
//!
//! Inherits from peephole_style, which sets most peephole properties.
//! Additionally, the peephole's border, optional padding, and scroll-bar
//! visibility controls are set here.

class new_scrollable_peepholelayoutmanager : public new_focusable_layoutmanager,
					     public peephole_style {

public:
	//! Peepholed widget factory.
	functionref<void (const factory &)> peepholed_factory;

	//! Constructor
	new_scrollable_peepholelayoutmanager(const functionref<void
					     (const factory &)> &);

	//! Destructor
	~new_scrollable_peepholelayoutmanager();

	//! Copy constructor

	new_scrollable_peepholelayoutmanager
	(const new_scrollable_peepholelayoutmanager &);

	//! Assignment operator

	new_scrollable_peepholelayoutmanager &
	operator=(const new_scrollable_peepholelayoutmanager &);

	//! Horizontal scroll-bar visibility
	scrollbar_visibility horizontal_scrollbar=
		scrollbar_visibility::automatic;

	//! Vertical scroll-bar visibility
	scrollbar_visibility vertical_scrollbar=
		scrollbar_visibility::automatic;

	//! Appearance of the scrollable peephole

	const_peephole_appearance appearance;

private:
	//! Internal implementation
	focusable_container create(const container_impl &,
				   const function<void
				   (const focusable_container &)>
				   &creator) const override
		LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
