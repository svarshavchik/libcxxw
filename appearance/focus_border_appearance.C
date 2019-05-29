/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/focus_border_appearance.H"
#include <x/singleton.H>

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

namespace {
#if 0
}
#endif

struct focus_border_appearance_base_none_themeObj : virtual public obj {

	const const_focus_border_appearance config=const_focus_border_appearance::create();

};

#if 0
{
#endif
}

const_focus_border_appearance focus_border_appearance_base::none_theme()
{
	return singleton<focus_border_appearance_base_none_themeObj>::get()->config;
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

namespace {
#if 0
}
#endif

struct focus_border_appearance_base_themeObj : virtual public obj {

	const const_focus_border_appearance config=make_theme();
};

#if 0
{
#endif
}

const_focus_border_appearance focus_border_appearance_base::theme()
{
	return singleton<focus_border_appearance_base_themeObj>::get()->config;
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

namespace {
#if 0
}
#endif

struct focus_border_appearance_base_menu_themeObj : virtual public obj {

	const const_focus_border_appearance config=make_menu_theme();
};

#if 0
{
#endif
}

const_focus_border_appearance focus_border_appearance_base::menu_theme()
{
	return singleton<focus_border_appearance_base_menu_themeObj>::get()->config;
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

namespace {
#if 0
}
#endif

struct focus_border_appearance_base_thin_themeObj : virtual public obj {

	const const_focus_border_appearance config=make_thin_theme();
};

#if 0
{
#endif
}

const_focus_border_appearance focus_border_appearance_base::thin_theme()
{
	return singleton<focus_border_appearance_base_thin_themeObj>::get()->config;
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

namespace {
#if 0
}
#endif

struct focus_border_appearance_base_visible_thin_themeObj : virtual public obj {

	const const_focus_border_appearance config=make_visible_thin_theme();

};

#if 0
{
#endif
}

const_focus_border_appearance
focus_border_appearance_base::visible_thin_theme()
{
	return singleton<focus_border_appearance_base_visible_thin_themeObj>::get()->config;
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

namespace {
#if 0
}
#endif

struct focus_border_appearance_base_slider_themeObj : virtual public obj {

	const const_focus_border_appearance config=make_slider_theme();

};

#if 0
{
#endif
}

const_focus_border_appearance
focus_border_appearance_base::slider_theme()
{
	return singleton<focus_border_appearance_base_slider_themeObj>::get()->config;
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

namespace {
#if 0
}
#endif

struct focus_border_appearance_base_combobox_button_themeObj : virtual public obj {

	const const_focus_border_appearance config=make_combobox_button_theme();

};

#if 0
{
#endif
}

const_focus_border_appearance
focus_border_appearance_base::combobox_button_theme()
{
	return singleton<focus_border_appearance_base_combobox_button_themeObj>::get()->config;
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

namespace {
#if 0
}
#endif

struct focus_border_appearance_base_input_field_themeObj : virtual public obj {

	const const_focus_border_appearance config=make_input_field_theme();

};

#if 0
{
#endif
}

const_focus_border_appearance
focus_border_appearance_base::input_field_theme()
{
	return singleton<focus_border_appearance_base_input_field_themeObj>::get()->config;
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

namespace {
#if 0
}
#endif

struct focus_border_appearance_base_list_themeObj : virtual public obj {

	const const_focus_border_appearance config=make_list_theme();

};

#if 0
{
#endif
}

const_focus_border_appearance
focus_border_appearance_base::list_theme()
{
	return singleton<focus_border_appearance_base_list_themeObj>::get()->config;
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

namespace {
#if 0
}
#endif

struct focus_border_appearance_base_visible_list_themeObj : virtual public obj {

	const const_focus_border_appearance config=make_visible_list_theme();

};

#if 0
{
#endif
}

const_focus_border_appearance
focus_border_appearance_base::visible_list_theme()
{
	return singleton<focus_border_appearance_base_visible_list_themeObj>::get()->config;
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

namespace {
#if 0
}
#endif

struct focus_border_appearance_base_attached_button_themeObj : virtual public obj {

	const const_focus_border_appearance config=make_attached_button_theme();

};

#if 0
{
#endif
}

const_focus_border_appearance
focus_border_appearance_base::attached_button_theme()
{
	return singleton<focus_border_appearance_base_attached_button_themeObj>::get()->config;
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

namespace {
#if 0
}
#endif

struct focus_border_appearance_base_scrollbar_themeObj : virtual public obj {

	const const_focus_border_appearance config=make_scrollbar_theme();

};

#if 0
{
#endif
}

const_focus_border_appearance
focus_border_appearance_base::scrollbar_theme()
{
	return singleton<focus_border_appearance_base_scrollbar_themeObj>::get()->config;
}

static inline const_focus_border_appearance make_dateeditbutton_theme()
{
	return focus_border_appearance::base::none_theme()->modify
		([]
		 (const auto &appearance)
		 {
			 appearance->focusoff_border=
				 "dateeditbuttonfocusoff_border";

			 appearance->focuson_border=
				 "dateeditbuttonfocuson_border";
		 });
}

namespace {
#if 0
}
#endif

struct focus_border_appearance_base_dateeditbutton_themeObj : virtual public obj {

	const const_focus_border_appearance config=make_dateeditbutton_theme();

};

#if 0
{
#endif
}

const_focus_border_appearance
focus_border_appearance_base::dateeditbutton_theme()
{
	return singleton<focus_border_appearance_base_dateeditbutton_themeObj>::get()->config;
}

LIBCXXW_NAMESPACE_END
