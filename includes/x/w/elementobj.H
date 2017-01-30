/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_elementobj_h
#define x_w_elementobj_h

#include <x/w/elementfwd.H>
#include <x/w/screenfwd.H>
#include <x/w/element_statefwd.H>
#include <x/w/picturefwd.H>
#include <x/w/rgbfwd.H>
#include <x/obj.H>

#include <experimental/string_view>

LIBCXXW_NAMESPACE_START

//! Methods common to all display elements.

//! \see element

class elementObj : virtual public obj {

public:
	class implObj;

	const ref<implObj> impl;

	//! Application data.

	//! The library doesn't use this opaque pointer in any way.

	ptr<obj> appdata;

	//! Constructor
	elementObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~elementObj() LIBCXX_HIDDEN;

	//! Return my screen

	screen get_screen();

	//! Return my screen

	const_screen get_screen() const;

	//! Show this display element, and all elements inside it.

	void show_all();

	//! Hide this display element, and all elements inside it.

	void hide_all();

	//! Show this display element only.

	void show();

	//! Hide this display element only.

	void hide();

	//! Use a standard background color for this display element.

	//! If there's no background color of the given name, use the
	//! default value.

	void set_background_color(const std::experimental::string_view &name,
				  const rgb &default_value);

	//! Use a custom background color for this display element.

	void set_background_color(const const_picture &background_color);

	//! Remove this display element's custom background color.

	void remove_background_color();

	//! Install a state update callback.

	template<typename functor_type>
	ref<obj> on_state_update(functor_type &&functor)
	{
		return do_on_state_update(element_state_update_handler_t
					  ::create(std::forward<functor_type>
						   (functor)));
	}

	//! Install a state update callback.

	ref<obj> do_on_state_update(const element_state_update_handler_t &);
};

LIBCXXW_NAMESPACE_END

#endif
