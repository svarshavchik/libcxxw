/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_background_color_elementfwd_H
#define x_w_impl_background_color_elementfwd_H

#include <x/w/impl/background_colorfwd.H>
#include <x/w/impl/updated_position_infofwd.H>
#include <x/w/connection_threadfwd.H>
#include <x/w/defaultthemefwd.H>
#include <x/w/rgb.H>
#include <x/w/element_state.H>
#include <x/w/elementobj.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC background_color_element_implObj;

template<typename element_impl, typename tag=void>
class LIBCXX_PUBLIC background_color_1elementObj;
template<typename tag=void>
class LIBCXX_PUBLIC background_color_element;


//! Attach a tag to a background_colorObj

//! \see background_color_elementObj

class background_color_element_implObj : virtual public obj {

	//! Wrapped background color
	background_color color;

	//! The background color was computed for its widget that's this wide

	dim_t background_color_element_width=0;

	//! The background color was computed for its widget that's this tall

	dim_t background_color_element_height=0;

public:

	//! Constructor
	background_color_element_implObj(const background_color &color);

	//! Set new background color
	void do_update(ONLY IN_THREAD,
		       const background_color &new_color,
		       elementObj::implObj &e);

	// Retrieve the current color.

	const background_color get(ONLY IN_THREAD);

	//! Update the background color
	void theme_updated(ONLY IN_THREAD,
			   const const_defaulttheme &new_theme,
			   elementObj::implObj &);

	//! The size of the background color's widget.
	void set_background_color_for_element(ONLY IN_THREAD,
					      elementObj::implObj &);

	//! The background color was recalculated.

	//! Invoked by set_background_color_for_element() and
	//! theme_updated().

	virtual void background_color_was_recalculated(ONLY IN_THREAD);

	//! Destructor
	~background_color_element_implObj();
};

//! Helper class for using tags to attach multiple background color objects to elements

template<typename tag>
class background_color_element
	: public background_color_element_implObj {

 public:

	using background_color_element_implObj::background_color_element_implObj;

	~background_color_element();

	//! Return my widget.

	virtual elementObj::implObj &background_color_element_impl()=0;

	//! Call do_update(), then set_background_color_for_element().
	void update(ONLY IN_THREAD,
		    const background_color &new_color);

	//! If the widget is visible, invoke do_update().

	void set_background_color_for_element(ONLY IN_THREAD);
};

//! Attach one background color mixin.

//! The constructor forwards its first argument, the initial background color
//! to the background_color_element_implObj.

template<typename element_impl, typename tag>
class background_color_1elementObj
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

	void initialize(ONLY IN_THREAD) override;

	//! Set new background color
	void update(ONLY IN_THREAD,
		    const background_color &new_color);

	//! Recalculate the background color, after a theme change.

	//! If the widget is visible, call
	//! set_background_color_for_element().

	void theme_updated(ONLY IN_THREAD, const const_defaulttheme &new_theme)
		override;

	//! Override process_updated_position.

	//! If the widget is visible, call
	//! set_background_color_for_element().

	void process_updated_position(ONLY IN_THREAD) override;

	//! Override invoke_element_state_updates

	//! If becoming visible, call set_background_color_for_element().
	//! This makes sure we do this work before the display window
	//! becomes visible.
	void invoke_element_state_updates(ONLY IN_THREAD,
					  element_state::state_update_t)
		override;

	//! Implement background_color_element_impl().

	elementObj::implObj &background_color_element_impl() override;
};

template<typename ...args> struct LIBCXX_PUBLIC background_colors_element_types;

//! Helper template used by background_color_elementObj

template<typename base, typename arg>
struct background_colors_element_types<base, arg> {

	typedef background_color_1elementObj<base, arg> type;
};

//! Helper template used by background_color_elementObj

template<typename base, typename arg1, typename arg2, typename ...args>
struct background_colors_element_types<base, arg1, arg2, args...>
{

	typedef background_color_1elementObj
		<typename
		 background_colors_element_types<base, arg2, args...>
		 ::type, arg1> type;
};

/*! Template for attaching one or more background_colors, as mixins, to a widget.

\code
#include <INSERT_LIBX_NAMESPACE/w/impl/background_color_element.H>

struct color_tag;

class my_implObj: public INSERT_LIBX_NAMESPACE::w::background_color_elementObj< // ...,
                                                      color_tag> {

    // ...

    typedef INSERT_LIBX_NAMESPACE::w::background_color_elementObj< // ...,
                                        color_tag> superclass_t;
};

my_implObj::my_implObj(const INSERT_LIBX_NAMESPACE::w::color_arg &amp;color, // ...
                       ) : superclass_t{color, // ...
                                        }
{
     // ...

     INSERT_LIBX_NAMESPACE::w::background_color bg=INSERT_LIBX_NAMESPACE::w::background_color_element<color_tag>::get(IN_THREAD);

     INSERT_LIBX_NAMESPACE::w::const_picture p=bg->get_current_color(IN_THREAD);
}
\endcode

The first template parameter is a subclass of elementObj::implObj.
background_color_elementObj multiply-inherits from the template
parameter and a \ref background_color_element_implObj via an intermediate
background_color_element template, using the tag specified by the optional
second template parameter (which defaults to \c void ).

The tag is used to create a distinct type only, and is not otherwise used.
Additional template parameters may follow, specifying additional tags.
This result in multiple inheritances, each one is referenced as
background_color_element<tag>.

The end result is a multiple inheritance from the first template parameter
and background_color_element<T>, with T specified by the remaining template
parameters to background_color_elementObj. If none are specified, beyond
the parent class, a single inheritance from background_color_element<void>
results.

Each background_color_element<T> inherits from
\ref background_color_element_implObj "background_color_element_implObj",
which implements methods for each mixed-in background color.

\par Constructors

The initial set of parameters to the constructor are
\ref color_arg "color_arg"s, one for each
background_color_element<T> mixin. The remaining constructor parameters
get forwarded to the parent elementObj::implObj superclass.

\par Background color recalculations

Background color processing occurs whenever it's possible for a
background_color to change due to the element's size changing (i.e.
gradient background color).

The \ref background_color_element_recalculated template mixin defines
a callback for those cases.

\par Obtaining the current color

\c background_color_element<T>::get(IN_THREAD) returns the associated tag's
\ref background_color "background_color".

*/

template<typename base, typename arg1=void, typename ...args>
using background_color_elementObj=
	typename background_colors_element_types<base, arg1, args...>::type;

LIBCXXW_NAMESPACE_END

#endif
