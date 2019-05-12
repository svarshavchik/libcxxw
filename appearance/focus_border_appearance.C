/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/focus_border_appearance.H"

LIBCXXW_NAMESPACE_START

focus_border_appearance_properties::focus_border_appearance_properties()
{
}

focus_border_appearance_properties::~focus_border_appearance_properties()=default;

focus_border_appearanceObj::focus_border_appearanceObj()=default;

focus_border_appearanceObj::~focus_border_appearanceObj()=default;

focus_border_appearanceObj::focus_border_appearanceObj
(const focus_border_appearanceObj &o)
	: focus_border_appearance_properties{o}
{
}

const_focus_border_appearance focus_border_appearanceObj
::do_modify(const function<void (const focus_border_appearance &)> &closure) const
{
	auto copy=focus_border_appearance::create(*this);
	closure(copy);
        return copy;
}

const const_focus_border_appearance &focus_border_appearance_base::none_theme()
{
	static const const_focus_border_appearance config=
		const_focus_border_appearance::create();

	return config;
}

static inline const_focus_border_appearance make_theme()
{
	return focus_border_appearance::base::none_theme()->modify
		([]
		 (const auto &appearance)
		 {
			 appearance->focusoff_border="inputfocusoff_border";
			 appearance->focuson_border="inputfocuson_border";
		 });
}

const const_focus_border_appearance &focus_border_appearance_base::theme()
{
	static const const_focus_border_appearance config=make_theme();

	return config;
}

static inline const_focus_border_appearance make_menu_theme()
{
	return focus_border_appearance::base::none_theme()->modify
		([]
		 (const auto &appearance)
		 {
			 appearance->focusoff_border=
				 "menu_inputfocusoff_border";
			 appearance->focuson_border=
				 "menu_inputfocuson_border";
		 });
}

const const_focus_border_appearance &focus_border_appearance_base::menu_theme()
{
	static const const_focus_border_appearance config=make_menu_theme();

	return config;
}

static inline const_focus_border_appearance make_thin_theme()
{
	return focus_border_appearance::base::none_theme()->modify
		([]
		 (const auto &appearance)
		 {
			 appearance->focusoff_border=
				 "thin_inputfocusoff_border";
			 appearance->focuson_border=
				 "thin_inputfocuson_border";
		 });
}

const const_focus_border_appearance &focus_border_appearance_base::thin_theme()
{
	static const const_focus_border_appearance config=make_thin_theme();

	return config;
}

static inline const_focus_border_appearance make_visible_thin_theme()
{
	return focus_border_appearance::base::thin_theme()->modify
		([]
		 (const auto &appearance)
		 {
			 appearance->focusoff_border=
				 "thin_inputfocusoff_border_color2";
		 });
}

const const_focus_border_appearance
&focus_border_appearance_base::visible_thin_theme()
{
	static const const_focus_border_appearance config=
		make_visible_thin_theme();

	return config;
}

static inline const_focus_border_appearance make_slider_theme()
{
	return focus_border_appearance::base::none_theme()->modify
		([]
		 (const auto &appearance)
		 {
			 appearance->focusoff_border=
				 "pane_slider_focusoff_border";
			 appearance->focuson_border=
				 "pane_slider_focuson_border";
		 });
}

const const_focus_border_appearance
&focus_border_appearance_base::slider_theme()
{
	static const const_focus_border_appearance config=
		make_slider_theme();

	return config;
}

static inline const_focus_border_appearance make_combobox_button_theme()
{
	return focus_border_appearance::base::none_theme()->modify
		([]
		 (const auto &appearance)
		 {
			 appearance->focusoff_border=
				 "comboboxbuttonfocusoff_border";
			 appearance->focuson_border=
				 "comboboxbuttonfocuson_border";
		 });
}

const const_focus_border_appearance
&focus_border_appearance_base::combobox_button_theme()
{
	static const const_focus_border_appearance config=
		make_combobox_button_theme();

	return config;
}

static inline const_focus_border_appearance make_input_field_theme()
{
	return focus_border_appearance::base::none_theme()->modify
		([]
		 (const auto &appearance)
		 {
			 appearance->focusoff_border=
				 "texteditfocusoff_border";
			 appearance->focuson_border=
				 "texteditfocuson_border";
		 });
}

const const_focus_border_appearance
&focus_border_appearance_base::input_field_theme()
{
	static const const_focus_border_appearance config=
		make_input_field_theme();

	return config;
}

static inline const_focus_border_appearance make_list_theme()
{
	return focus_border_appearance::base::none_theme()->modify
		([]
		 (const auto &appearance)
		 {
			 appearance->focusoff_border=
				 "listfocusoff_border";
			 appearance->focuson_border=
				 "listfocuson_border";
		 });
}

const const_focus_border_appearance
&focus_border_appearance_base::list_theme()
{
	static const const_focus_border_appearance config=
		make_list_theme();

	return config;
}

static inline const_focus_border_appearance make_visible_list_theme()
{
	return focus_border_appearance::base::list_theme()->modify
		([]
		 (const auto &appearance)
		 {
			 appearance->focusoff_border=
				 "listvisiblefocusoff_border";
		 });
}

const const_focus_border_appearance
&focus_border_appearance_base::visible_list_theme()
{
	static const const_focus_border_appearance config=
		make_visible_list_theme();

	return config;
}

static inline const_focus_border_appearance make_attached_button_theme()
{
	return focus_border_appearance::base::none_theme()->modify
		([]
		 (const auto &appearance)
		 {
			 appearance->focusoff_border=
				 "element_attached_popup_button_focusoff_border";
			 appearance->focuson_border=
				 "element_attached_popup_button_focuson_border";
		 });
}

const const_focus_border_appearance
&focus_border_appearance_base::attached_button_theme()
{
	static const const_focus_border_appearance config=
		make_attached_button_theme();

	return config;
}

static inline const_focus_border_appearance make_scrollbar_theme()
{
	return focus_border_appearance::base::none_theme()->modify
		([]
		 (const auto &appearance)
		 {
			 appearance->focusoff_border=
				 "scrollbarfocusoff_border";

			 appearance->focuson_border=
				 "scrollbarfocuson_border";
		 });
}

const const_focus_border_appearance
&focus_border_appearance_base::scrollbar_theme()
{
	static const const_focus_border_appearance config=
		make_scrollbar_theme();

	return config;
}

LIBCXXW_NAMESPACE_END
