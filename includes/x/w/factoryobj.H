/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_factoryobj_h
#define x_w_factoryobj_h

#include <x/w/factoryfwd.H>
#include <x/w/elementfwd.H>
#include <x/w/new_layoutmanagerfwd.H>
#include <x/w/new_focusable_layoutmanagerfwd.H>
#include <x/w/containerfwd.H>
#include <x/w/containerobj.H>
#include <x/w/canvasfwd.H>
#include <x/w/border_argfwd.H>
#include <x/w/alignment.H>
#include <x/w/text_paramfwd.H>
#include <x/w/shortcutfwd.H>
#include <x/w/labelfwd.H>
#include <x/w/focusable_labelfwd.H>
#include <x/w/buttonfwd.H>
#include <x/w/input_fieldfwd.H>
#include <x/w/imagefwd.H>
#include <x/w/image_buttonfwd.H>
#include <x/w/radio_groupfwd.H>
#include <x/w/focusable_containerfwd.H>
#include <x/w/progressbarfwd.H>
#include <x/w/scrollbarfwd.H>
#include <x/w/dim_axis_arg.H>
#include <x/w/date_input_fieldfwd.H>
#include <x/w/date_input_field_configfwd.H>
#include <x/w/color_pickerfwd.H>
#include <x/w/color_picker_configfwd.H>
#include <x/w/font_pickerfwd.H>
#include <x/w/font_picker_configfwd.H>
#include <x/obj.H>
#include <x/functional.H>

#include <utility>

LIBCXXW_NAMESPACE_START

//! A factory of display elements.

//! \see factory

class factoryObj : virtual public obj {

public:

	//! Creating a new element, return its parent container implementation object.

	//! \internal

	virtual container_impl get_container_impl()=0;

	//! Return the container's element implementation object.

	//! This is used for ancillary purposes, such as constructing
	//! theme-related metadata.
	//!
	//! \internal

	virtual elementObj::implObj &get_element_impl()=0;

	//! Return parent container implementation object of newly created element.

	//! Used for an internal sanity check. The default implementation
	//! calls get_container_impl(). A factory that creates a new container
	//! for each new display element returns the same return value as the
	//! last call to get_container_impl().
	//!
	//! \internal.

	virtual container_impl last_container_impl();

	//! An internal call to create().

	void created_internally(const element &);

private:
	//! Invoked by factory's methods, after creating a new element.

	virtual void created(const element &) LIBCXX_HIDDEN=0;
public:
	//! A creator that takes a factory parameter.

	typedef void factory_creator_t(const factory &);

	//! Constructor
	factoryObj() LIBCXX_HIDDEN;

	//! Destructor
	~factoryObj() LIBCXX_HIDDEN;

	//! Create an empty \ref canvas.

	//! A default usage of create_canvas() that specifies a completely
	//! unbound canvas. This is used to fill in unused cells in
	//! a \ref gridlayoutmanager "grid layout manager".

	canvas create_canvas();

	//! Create an empty \ref canvas "canvas".
	template<typename functor>
	canvas create_canvas(functor &&creator,
			     const dim_axis_arg &horiz,
			     const dim_axis_arg &vert)
	{
		return do_create_canvas(make_function<void (const canvas &)>
					(creator),
					horiz, vert);
	}

	//! Create an empty \ref canvas "canvas".

	canvas do_create_canvas(const function<void (const canvas &)> &creator,
				const dim_axis_arg &horiz,
				const dim_axis_arg &vert);

	//! Create a nested container

	template<typename functor>
	container create_container(functor &&creator,
				   const new_layoutmanager &layout_manager)
	{
		return do_create_container(make_function<void (const container
							       &)>
					   (creator),
					   layout_manager);
	}

	//! Create a container with a border around an element.

	template<typename creator>
	container create_bordered_element(creator &&c,
					  const border_arg &border,
					  const dim_arg &hpad=0,
					  const dim_arg &vpad=0)
	{
		return do_create_bordered_element(make_function
						  <void (const factory &)>
						  (std::forward<creator>(c)),
						  border, hpad, vpad);
	}

	//! Type-erased create_bordered_element()

	container do_create_bordered_element(const function<void (const factory
								  &)>
					     &creator,
					     const border_arg &border,
					     const dim_arg &hpad,
					     const dim_arg &vpad);
	//! Create a nested container

	container do_create_container(const function<void (const container &)>
				      &creator,
				      const new_layoutmanager &layout_manager)
		LIBCXX_PUBLIC;

	//! Create a label

	label create_label(const text_param &text);

	//! Create a label with custom properties

	//! Specifies custom alignment, or word wrapping.

	label create_label(const text_param &text,
			   const label_config &props);

	//! Create a focusable label.

	focusable_label create_focusable_label(const text_param &text);

	//! Create a focusable label.

	focusable_label create_focusable_label(const text_param &text,
					       const focusable_label_config &);

	//! Create an button with a label, and a normal border.
	button create_normal_button_with_label(const text_param &text);

	//! Create an button with a label, and a normal border.

	//! \overload
	button create_normal_button_with_label(const text_param &text,
					       const label_config &config);

	//! Create an button with a label, and a normal border.

	//! \overload
	button create_normal_button_with_label(const text_param &text,
					       const shortcut &key_shortcut);

	//! Create an button with a label, and a normal border.

	//! \overload
	button create_normal_button_with_label(const text_param &text,
					       const shortcut &key_shortcut,
					       const label_config &config);

	//! Create an button with a label, and a special border.
	button create_special_button_with_label(const text_param &text);

	//! Create an button with a label, and a special border.

	//! \overload
	button create_special_button_with_label(const text_param &text,
						const label_config &config);

	//! Create an button with a label, and a special border.

	//! \overload
	button create_special_button_with_label(const text_param &text,
						const shortcut &key_shortcut);

	//! Create an button with a label, and a special border.

	//! \overload
	button create_special_button_with_label(const text_param &text,
						const shortcut &key_shortcut,
						const label_config &config);

	//! Create an button with custom contents and a normal border.
	template<typename functor>
	button create_normal_button(functor &&f)
	{
		return do_create_normal_button(make_function<factory_creator_t>
					       (std::forward<functor>(f))
					       );
	}

	//! Create an button with custom contents and a normal border.

	//! \overload
	template<typename functor>
	button create_normal_button(functor &&f,
				    const shortcut &shortcut_key)
	{
		return do_create_normal_button(make_function<factory_creator_t>
					       (std::forward<functor>(f)),
					       shortcut_key);
	}

	//! Create an button with custom contents and a special border.
	template<typename functor>
	button create_special_button(functor &&f)
	{
		return do_create_special_button(make_function<factory_creator_t>
						(std::forward<functor>(f))
						);
	}

	//! Create an button with custom contents and a special border.

	//! \overload
	template<typename functor>
	button create_special_button(functor &&f,
				     const shortcut &shortcut_key)
	{
		return do_create_special_button(make_function<factory_creator_t>
						(std::forward<functor>(f)),
						shortcut_key);
	}

	//! Create a button with a custom border.

	template<typename functor>
	button create_button(const border_arg &arg, functor &&f)
	{
		return do_create_button(arg, make_function<factory_creator_t>
					(std::forward<functor>(f)));
	}
	//! Create a button with a custom border.

	template<typename functor>
	button create_button(const border_arg &arg, functor &&f,
			     const shortcut &shortcut_key)
	{
		return do_create_button(arg, make_function<factory_creator_t>
					(std::forward<functor>(f)),
					shortcut_key);
	}

	//! Helper for creating buttons.
	button do_create_normal_button(const function<factory_creator_t> &);

	//! Helper for creating buttons.
	button do_create_special_button(const function<factory_creator_t> &);

	//! Helper for creating buttons.
	button do_create_normal_button(const function<factory_creator_t> &,
				       const shortcut &);

	//! Helper for creating buttons.
	button do_create_special_button(const function<factory_creator_t> &,
					const shortcut &);

	//! Helper for creating buttons.
	button do_create_button(const border_arg &,
				const function<factory_creator_t> &);

	//! Helper for creating buttons.
	button do_create_button(const border_arg &,
				const function<factory_creator_t> &,
				const shortcut &);

	//! Helper for creating buttons.
	button create_button_with_label(const border_arg &theme_border,
					const text_param &text,
					const shortcut &shortcut_key);

	//! Helper for creating buttons.
	button create_button_with_label(const border_arg &theme_border,
					const text_param &text,
					const shortcut &shortcut_key,
					const label_config &config);

	//! Create a text input field

	input_field
	create_input_field(const text_param &text);

	//! Create a text input field

	input_field
	create_input_field(const text_param &text,
			   const input_field_config &config);

	//! Create a date input field
	date_input_field create_date_input_field();

	//! Create a date input field
	date_input_field create_date_input_field(const date_input_field_config &);

	//! Create a display element that shows an image.
	image create_image(const std::string &name);

	//! Create a display element that shows an image.

	//! "name" specifies a scalable SXG image, that's sized as specified.
	//!
	//! Either the width or the height may be given as 0, which computes
	//! the size to preserve the image's aspect ratio.

	image create_image(const std::string &name,
			   const dim_arg &width,
			   const dim_arg &height);

	//! Create a checkbox
	image_button create_checkbox(const checkbox_config &config={});

	//! Create a checkbox with a label factory
	template<typename functor,
		 typename sfinae=decltype(std::declval<functor &&>()
					  (std::declval<const factory &>()))>
	image_button create_checkbox(functor &&f,
				     const checkbox_config &config={})
	{
		return do_create_checkbox(make_function<factory_creator_t>
					  (std::forward<functor>(f)),
					  config);
	}

	//! Create a radio button
	image_button create_radio(const radio_group &group,
				  const radio_config &config={});

	//! Create a radio button with a label factory
	template<typename functor,
		 typename sfinae=decltype(std::declval<functor &&>()
					  (std::declval<const factory &>()))>
	image_button create_radio(const radio_group &group,
				  functor &&f,
				  const radio_config &config={})
	{
		return do_create_radio(group,
				       make_function<factory_creator_t>
				       (std::forward<functor>(f)),
				       config);
	}

	//! Type-erased create_checkbox().
	image_button do_create_checkbox(const function<factory_creator_t>&,
					const checkbox_config &config);

	//! Type-erased create_radio().
	image_button do_create_radio(const radio_group &,
				     const function<factory_creator_t>&,
				     const radio_config &);

	//! Create a focusable container.

	template<typename functor>
	focusable_container
	create_focusable_container(functor &&creator,
				   const new_focusable_layoutmanager
				   &layout_manager)
	{
		return do_create_focusable_container(make_function<void
						     (const focusable_container
						      &)>(creator),
						     layout_manager);
	}

	//! Create a focusable container
	virtual focusable_container
	do_create_focusable_container(const function<void
				      (const focusable_container
				       &)> &creator,
				      const new_focusable_layoutmanager
				      &layout_manager) LIBCXX_PUBLIC;

	//! Create a progress bar

	template<typename functor>
	inline progressbar create_progressbar(functor &&creator)
	{
		return create_progressbar(std::forward<functor>(creator), {});
	}

	//! Create a progress bar

	template<typename functor, typename ...Args>
	inline progressbar create_progressbar(functor &&creator,
					      const progressbar_config &config,
					      Args && ...args)
	{
		return do_create_progressbar(make_function<void
					     (const progressbar &)>
					     (creator),
					     config,
					     std::forward<Args>(args)...);
	}

	//! Create a progress bar
	progressbar do_create_progressbar(const function<void
					  (const progressbar &)> &,
					  const progressbar_config &config)
		LIBCXX_PUBLIC;

	//! Create a progress bar
	progressbar do_create_progressbar(const function<void
					  (const progressbar &)> &,
					  const progressbar_config &config,
					  const new_layoutmanager
					  &layout_manager) LIBCXX_PUBLIC;

	//! Create a horizontal scrollbar
	scrollbar create_horizontal_scrollbar(const scrollbar_config &config,
					      dim_arg minimum_size=0)
		LIBCXX_PUBLIC;

	//! Create a horizontal scrollbar

	//! \overload
	scrollbar create_horizontal_scrollbar(const scrollbar_config &config,
					      const scrollbar_cb_t &callback,
					      dim_arg minimum_size=0)
		LIBCXX_PUBLIC;

	//! Create a vertical scrollbar
	scrollbar create_vertical_scrollbar(const scrollbar_config &config,
					    dim_arg minimum_size=0)
		LIBCXX_PUBLIC;

	//! Create a vertical scrollbar

	//! \overload
	scrollbar create_vertical_scrollbar(const scrollbar_config &config,
					    const scrollbar_cb_t &callback,
					    dim_arg minimum_size=0)
		LIBCXX_PUBLIC;

	//! Create a color picker
	color_picker create_color_picker();

	//! Create a color picker
	color_picker create_color_picker(const color_picker_config &);

	//! Create a font picker
	font_picker create_font_picker();

	//! Create a font picker
	font_picker create_font_picker(const font_picker_config &);
};

LIBCXXW_NAMESPACE_END

#endif
