/*
** Copyright 2017-2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef background_color_elementfwd_H
#define background_color_elementfwd_H

#include "background_colorfwd.H"
#include "connection_threadfwd.H"
#include "defaultthemefwd.H"
#include "x/w/rgb.H"
#include "x/w/element_state.H"
#include "x/w/elementobj.H"
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Attach a tag to a background_colorObj

//! \see background_color_elementObj

class LIBCXX_HIDDEN background_color_element_implObj : virtual public obj {

	//! Wrapped background color
	background_color color;

	//! The background color was computed for its element that's this wide

	dim_t background_color_element_width=0;

	//! The background color was computed for its element that's this tall

	dim_t background_color_element_height=0;

public:

	//! Constructor
	background_color_element_implObj(const background_color &color);

	//! Set new background color
	void do_update(IN_THREAD_ONLY,
		       const background_color &new_color);

	// Retrieve the current color.

	const background_color get(IN_THREAD_ONLY);

	//! Update the background color
	void theme_updated(IN_THREAD_ONLY, const defaulttheme &new_theme);

	//! Initialize after construction
	void initialize(IN_THREAD_ONLY);

	//! The size of the background color's element.
	void set_background_color_for_element(IN_THREAD_ONLY,
					      elementObj::implObj &);

	//! The background color was recalculated.

	//! Invoked by set_background_color_for_element() and
	//! theme_updated().

	virtual void background_color_was_recalculated(IN_THREAD_ONLY);

	//! Destructor
	~background_color_element_implObj();
};

//! Helper class for using tags to attach multiple background color objects to elements

template<typename tag=void>
class LIBCXX_HIDDEN background_color_element
	: public background_color_element_implObj {

 public:

	using background_color_element_implObj::background_color_element_implObj;

	~background_color_element();

	//! Return my element.

	virtual elementObj::implObj &background_color_element_impl()=0;

	//! Call do_update(), then set_background_color_for_element().
	void update(IN_THREAD_ONLY,
		    const background_color &new_color);

	//! If the element is visible, invoke do_update().

	void set_background_color_for_element(IN_THREAD_ONLY);
};

//! Attach one background color mixin.

//! The constructor forwards its first argument, the initial background color
//! to the background_color_element_implObj.

template<typename element_impl, typename tag=void>
class LIBCXX_HIDDEN background_color_1elementObj
	: public element_impl,
	  public background_color_element<tag> {

 public:
	//! Constructor

	template<typename ...Args>
		background_color_1elementObj(const background_color &color,
					     Args && ...args);
	//! Constructor

	//! Uses the already-constructed element_impl superclass to
	//! create_background_color()
	template<typename ...Args>
		background_color_1elementObj(const color_arg &color,
					     Args && ...args);

	//! Destructor
	~background_color_1elementObj();

	//! Override initialize()

	//! Makes sure the background color gets updated properly, in the event
	//! of the default theme changing after construction.

	void initialize(IN_THREAD_ONLY) override;

	//! Set new background color
	void update(IN_THREAD_ONLY,
		    const background_color &new_color);

	//! Recalculate the background color, after a theme change.

	//! If the display element is visible, call
	//! set_background_color_for_element().

	void theme_updated(IN_THREAD_ONLY, const defaulttheme &new_theme)
		override;

	//! Override process_updated_position.

	//! If the display element is visible, call
	//! set_background_color_for_element().

	void process_updated_position(IN_THREAD_ONLY) override;

	//! Override invoke_element_state_updates

	//! If becoming visible, call set_background_color_for_element().
	//! This makes sure we do this work before the display window
	//! becomes visible.
	void invoke_element_state_updates(IN_THREAD_ONLY,
					  element_state::state_update_t)
		override;

	//! Implement background_color_element_impl().

	elementObj::implObj &background_color_element_impl() override;
};

template<typename ...args> struct background_colors_element_types;

//! Helper template used by background_color_elementObj

template<typename base, typename arg>
struct LIBCXX_HIDDEN background_colors_element_types<base, arg> {

	typedef background_color_1elementObj<base, arg> type;
};

//! Helper template used by background_color_elementObj

template<typename base, typename arg1, typename arg2, typename ...args>
struct LIBCXX_HIDDEN background_colors_element_types<base, arg1, arg2, args...>
{

	typedef background_color_1elementObj
		<typename
		 background_colors_element_types<base, arg2, args...>
		 ::type, arg1> type;
};

//! Template for attaching one or more background_colors, as mixins, to a display element.

//! The first template parameter is a subclass of elementObj::implObj.
//! background_color_elementObj multiply-inherits from the template
//! parameter and a \ref background_color_element_implObj via an intermediate
//! background_color_element template, using the tag specified by the optional
//! second template parameter (which defaults to \c void ).
//!
//! The tag is used to create a distinct type only, and is not otherwise used.
//! Additional template parameters may follow, specifying additional tags.
//! This result in multiple inheritances, each one is referenced as
//! background_color_element<tag>.
//!
//! The end result is a multiple inheritance from the first template parameter
//! and background_color_element<T>, with T specified by the template
//! parameters to background_color_elementObj. If none are specified, beyond
//! the parent class, a single inheritance from background_color_element<void>
//! results.
//!
//! Each background_color_element<T> inherits from
//! \ref background_color_element_implObj "background_color_element_implObj",
//! which implements methods for each mixed-in background color.
//!
//! \par Constructors
//!
//! The initial set of parameters to the constructor are the initial
//! \ref background_color "background_color" objects, one for each
//! background_color_element<T> mixin. The remaining constructor parameters
//! get forwarded to the parent elementObj::implObj superclass.
//!
//! \par Background color recalculations
//!
//! Background color processing occurs whenever it's possible for a
//! background_color to change due to the element's size changing (i.e.
//! gradient background color).
//!
//! The \ref background_color_element_recalculated template mixin defines
//! a callback for those cases.

template<typename base, typename arg1=void, typename ...args>
using background_color_elementObj=
	typename background_colors_element_types<base, arg1, args...>::type;

LIBCXXW_NAMESPACE_END

#endif
