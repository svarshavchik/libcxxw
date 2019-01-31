/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef scrollbar_icon_set_h
#define scrollbar_icon_set_h

#include "x/w/namespace.H"
#include "iconfwd.H"
#include "iconobj.H"
#include "x/w/connection_threadfwd.H"
#include "icon_images_set_elementfwd.H"

LIBCXXW_NAMESPACE_START

//! Tags for scrollbar's icons.

template<typename set_type>
struct LIBCXX_HIDDEN scrollbar_icons_tags {
	//! Scroll in the low direction button.
	struct low;

	//! Scroll in the high direction button.
	struct high;

	//! Starting part of the slider handlebar.
	struct handlebar_start;

	//! The middle part of the slider handlebra.
	struct handlebar;

	//! The end part of the slider handlebar.
	struct handlebar_end;
};

//! Regular icons
struct LIBCXX_HIDDEN scrollbar_normal;

//! Button down/click icons
struct LIBCXX_HIDDEN scrollbar_pressed;

//! Parameter to icon_images_set constructor for scrollbars.
typedef std::tuple<icon,icon,icon,icon,icon,
		   icon,icon,icon,icon,icon> scrollbar_icons_tuple_t;

#define scrollbar_icon_tuple_t_get(n)		\
		std::get<0>(n),			\
		std::get<1>(n),			\
		std::get<2>(n),			\
		std::get<3>(n),			\
		std::get<4>(n),			\
		std::get<5>(n),			\
		std::get<6>(n),			\
		std::get<7>(n),			\
		std::get<8>(n),			\
		std::get<9>(n)

//! Typedef alias for inheriting all scrollbar icons.
template<typename element_impl>
using scrollbar_icon_images_set_t=icon_images_set_elementObj
	<element_impl,
	 scrollbar_icons_tags<scrollbar_normal>::low,
	 scrollbar_icons_tags<scrollbar_normal>::high,
	 scrollbar_icons_tags<scrollbar_normal>::handlebar_start,
	 scrollbar_icons_tags<scrollbar_normal>::handlebar,
	 scrollbar_icons_tags<scrollbar_normal>::handlebar_end,
	 scrollbar_icons_tags<scrollbar_pressed>::low,
	 scrollbar_icons_tags<scrollbar_pressed>::high,
	 scrollbar_icons_tags<scrollbar_pressed>::handlebar_start,
	 scrollbar_icons_tags<scrollbar_pressed>::handlebar,
	 scrollbar_icons_tags<scrollbar_pressed>::handlebar_end>;

LIBCXXW_NAMESPACE_END

#endif
