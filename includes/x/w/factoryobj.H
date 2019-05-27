/*
** Copyright 2017-2019 Double Precision, Inc.
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
#include <x/w/image_button_appearancefwd.H>
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
#include <x/optional_args.H>
#include <utility>

LIBCXXW_NAMESPACE_START

//! A factory of widgets.

//! \see factory

class factoryObj : virtual public obj {

public:

	//! Creating a new element, return its parent container implementation object.

	//! \internal

	virtual container_impl get_container_impl()=0;

	//! Return the container's widget implementation object.

	//! This is used for ancillary purposes, such as constructing
	//! theme-related metadata.
	//!
	//! \internal

	virtual elementObj::implObj &get_element_impl()=0;

	//! Return parent container implementation object of newly created widget.

	//! Used for an internal sanity check. The default implementation
	//! calls get_container_impl(). A factory that creates a new container
	//! for each new widget returns the same return value as the
	//! last call to get_container_impl().
	//!
	//! \internal.

	virtual container_impl last_container_impl();

	//! An internal call to create().

	void created_internally(const element &);

private:
	//! Invoked by factory's methods, after creating a new widget.

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

	//! Create a simple button with a text label.
	button create_button(const text_param &text);

	//! Create a simple button with a text label.
	button create_button(const text_param &text,
			     const create_button_with_label_args_t &args);

	//! Create a button using a callback to populate its contents.
	template<typename functor,
		 typename=std::void_t<decltype(std::declval<functor &&>()
					       (std::declval<const factory &>())
					       )>>

	button create_button(functor &&f)
	{
		return do_create_button(make_function<factory_creator_t>
					(std::forward<functor>(f)));
	}

	//! Create a button using a callback to populate its contents.
	template<typename functor,
		 typename=std::void_t<decltype(std::declval<functor &&>()
					       (std::declval<const factory &>())
					       )>>
	button create_button(functor &&f,
			     const create_button_args_t &args)
	{
		return do_create_button(make_function<factory_creator_t>
					(std::forward<functor>(f)),
					args);
	}

	//! Type-erased create_button()
	button do_create_button(const function<factory_creator_t> &creator);

	//! Type-erased create_button()
	button do_create_button(const function<factory_creator_t> &creator,
				const create_button_args_t &args);

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

	//! Create a widget that shows an image.
	image create_image(const std::string &name);

	//! Create a widget that shows an image.

	//! "name" specifies a scalable SXG image, that's sized as specified.
	//!
	//! Either the width or the height may be given as 0, which computes
	//! the size to preserve the image's aspect ratio.

	image create_image(const std::string &name,
			   const dim_arg &width,
			   const dim_arg &height);

	//! Optional arguments to create_checkbox() and create_radio().

	typedef optional_args<functionref<factory_creator_t>,
			      const_image_button_appearance
			      > image_button_args_t;

	//! Create a checkbox
	template<typename ...Args>
	inline image_button create_checkbox(Args && ...args)
	{
		return do_create_checkbox({std::forward<Args>(args)...});
	}

	//! Type-erased create_checkbox().

	image_button do_create_checkbox(const image_button_args_t &args);

	//! Create a radio button
	template<typename ...Args>
	inline image_button create_radio(const radio_group &group,
					 Args && ...args)
	{
		return do_create_radio(group,
				       { std::forward<Args>(args)...});
	}

	//! Create a radio button
	image_button do_create_radio(const radio_group &group,
				     const image_button_args_t &args);

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
		return do_create_progressbar(make_function<void
					     (const progressbar &)>
					     (std::forward<functor>(creator)));
	}

	//! Option argument to create_progressbar().

	typedef optional_argconstrefs<progressbar_config,
				      new_layoutmanager
				      > create_progressbar_args_t;
	//! Create a progress bar

	template<typename functor, typename Arg, typename ...Args>
	inline progressbar create_progressbar(functor &&creator,
					      Arg &&arg,
					      Args && ...args)
	{
		return do_create_progressbar(make_function<void
					     (const progressbar &)>
					     (creator),
					     {
					      std::forward<Arg>(arg),
					      std::forward<Args>(args)...
					     });
	}

	//! Type-erased create_progressbar()
	progressbar do_create_progressbar(const function<void
					  (const progressbar &)> &)
		LIBCXX_PUBLIC;

	//! Type-erased create_progressbar()
	progressbar do_create_progressbar(const function<void
					  (const progressbar &)> &,
					  const create_progressbar_args_t &)
		LIBCXX_PUBLIC;

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
