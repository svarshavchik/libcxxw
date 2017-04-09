/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_factoryobj_h
#define x_w_factoryobj_h

#include <x/w/factoryfwd.H>
#include <x/w/elementfwd.H>
#include <x/w/new_layoutmanagerfwd.H>
#include <x/w/containerfwd.H>
#include <x/w/containerobj.H>
#include <x/w/canvasfwd.H>
#include <x/w/metrics/axis.H>
#include <x/w/metrics/mmaxisfwd.H>
#include <x/w/border_infommfwd.H>
#include <x/w/alignment.H>
#include <x/w/text_paramfwd.H>
#include <x/w/labelfwd.H>
#include <x/w/actionbuttonfwd.H>
#include <x/w/input_fieldfwd.H>
#include <x/obj.H>
#include <x/functional.H>

LIBCXXW_NAMESPACE_START

//! A factory of display elements.

//! \see factory

class factoryObj : virtual public obj {

public:

	//! This factory is for the given container.

	const ref<containerObj::implObj> container_impl;

	//! An internal call to create().

	void created_internally(const element &) LIBCXX_HIDDEN;

private:
	//! Invoked by factory's methods, after creating a new element.

	virtual void created(const element &)=0;
public:
	//! Constructor
	factoryObj(const ref<containerObj::implObj> &container_impl)
		LIBCXX_HIDDEN;

	//! Destructor
	~factoryObj() LIBCXX_HIDDEN;

	//! Create an empty \ref canvas "canvas".
	template<typename functor>
	canvas create_canvas(functor &&creator,
			     const metrics::mmaxis &horiz,
			     const metrics::mmaxis &vert,
			     halign h=halign::center,
			     valign v=valign::middle)
	{
		return do_create_canvas(make_function<void (const canvas &)>
					(creator),
					horiz, vert, h, v);
	}

	//! Create an empty \ref canvas "canvas".

	canvas do_create_canvas(const function<void (const canvas &)> &creator,
				const metrics::mmaxis &horiz,
				const metrics::mmaxis &vert,
				halign h,
				valign v);

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
				      const new_layoutmanager &layout_manager);

	//! Create a label

	label create_label(const text_param &text,
			   halign alignment=halign::left);

	//! Create a label that word-wraps the text to the given width

	//! The initial width of the label is specified in millimeters.
	//! The label's text gets word-wrapped accordingly, and the label's
	//! width will adjust to match the width of its column.

	label create_label(const text_param &text,
			   double widthmm,
			   halign alignment=halign::left);

	//! Create an action button with a label, and a normal border.
	actionbutton
	create_normal_actionbutton_with_label(const text_param &text,
					      halign alignment=halign::left);

	//! Create an action button with a label, and a special border.
	actionbutton
	create_special_actionbutton_with_label(const text_param &text,
					       halign alignment=halign::left);

	//! Create an action button with custom contents and a normal border.
	template<typename functor>
	actionbutton create_normal_actionbutton(functor &&f)
	{
		return do_create_normal_actionbutton(make_function<void
						     (const factory &)>
						     (std::forward<function>(f))
						     );
	}

	//! Create an action button with custom contents and a special border.
	template<typename functor>
	actionbutton create_special_actionbutton(functor &&f)
	{
		return do_create_normal_actionbutton(make_function<void
						     (const factory &)>
						     (std::forward<function>(f))
						     );
	}

	//! Helper for creating action buttons.
	actionbutton do_create_normal_actionbutton(const function<
						   void (const factory &)> &);

	//! Helper for creating action buttons.
	actionbutton do_create_special_actionbutton(const function<
						    void (const factory &)> &);

	//! Helper for creating action buttons.
	actionbutton do_create_actionbutton(const char *,
					    const function<
					    void (const factory &)> &);

	//! Helper for creating action buttons.
	actionbutton
	create_actionbutton_with_label(const char *theme_border,
				       const text_param &text,
				       halign alignment=halign::left);

	//! Create a text input field

	input_field
	create_input_field(const text_param &text);

	//! Create a text input field

	input_field
	create_input_field(const text_param &text,
			   const input_field_config &config);
};

LIBCXXW_NAMESPACE_END

#endif
