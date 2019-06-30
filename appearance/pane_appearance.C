/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/pane_appearance.H"
#include "x/w/scrollbar_appearance.H"
#include "x/w/scrollbar.H"
#include <x/singleton.H>

LIBCXXW_NAMESPACE_START

pane_appearance_properties::pane_appearance_properties()
	: size{50},
	  left_padding{"grid_horiz_padding"},
	  right_padding{left_padding},
	  top_padding{"grid_vert_padding"},
	  bottom_padding{top_padding},
	  horizontal_alignment{halign::left},
	  vertical_alignment{valign::top},
	  pane_scrollbar_visibility{scrollbar_visibility::automatic_reserved},
	  horizontal_scrollbar{scrollbar_appearance::base::theme()},
	  vertical_scrollbar{scrollbar_appearance::base::theme()}
{
}

pane_appearance_properties::~pane_appearance_properties()=default;

pane_appearanceObj::pane_appearanceObj()=default;

pane_appearanceObj::~pane_appearanceObj()=default;

pane_appearanceObj::pane_appearanceObj
(const pane_appearanceObj &o)
	: pane_appearance_properties{o}
{
}

const_pane_appearance pane_appearanceObj
::do_modify(const function<void (const pane_appearance &)> &closure) const
{
	auto copy=pane_appearance::create(*this);
	closure(copy);
        return copy;
}

namespace {
#if 0
}
#endif

struct pane_appearance_base_themeObj : virtual public obj {

	const const_pane_appearance config=const_pane_appearance::create();

};

#if 0
{
#endif
}

const_pane_appearance pane_appearance_base::theme()
{
	return singleton<pane_appearance_base_themeObj>::get()->config;
}

static const_pane_appearance create_focusable_list()
{
	auto appearance=pane_appearance::base::theme()->modify
		([]
		 (const auto &appearance)
		 {
			 appearance->pane_scrollbar_visibility=
				 scrollbar_visibility::never;
			 appearance->left_padding=
				 appearance->right_padding=
				 appearance->top_padding=
				 appearance->bottom_padding=0;
			 appearance->horizontal_alignment=halign::fill;
			 appearance->vertical_alignment=valign::fill;
		 });
	return appearance;
}

namespace {
#if 0
}
#endif

struct pane_appearance_base_focusable_listObj : virtual public obj {

	const const_pane_appearance config=create_focusable_list();
};

#if 0
{
#endif
}

const_pane_appearance pane_appearance_base::focusable_list()
{
	return singleton<pane_appearance_base_focusable_listObj>::get()->config;
}

static const_pane_appearance create_file_dialog_dir()
{
	auto appearance=pane_appearance_base::focusable_list()->modify
		([]
		 (const auto &appearance)
		 {
			 appearance->size=25;
		 });

	return appearance;
}

static const_pane_appearance create_file_dialog_file()
{
	auto appearance=pane_appearance_base::focusable_list()->modify
		([]
		 (const auto &appearance)
		 {
			 appearance->size=75;
		 });

	return appearance;
}

namespace {
#if 0
}
#endif

struct pane_appearance_base_file_dialog_dirObj : virtual public obj {

	const const_pane_appearance config=create_file_dialog_dir();
};

#if 0
{
#endif
}

const_pane_appearance pane_appearance_base::file_dialog_dir()
{
	return singleton<pane_appearance_base_file_dialog_dirObj>::get()->config;
}

namespace {
#if 0
}
#endif

struct pane_appearance_base_file_dialog_fileObj : virtual public obj {

	const const_pane_appearance config=create_file_dialog_file();
};

#if 0
{
#endif
}

const_pane_appearance pane_appearance_base::file_dialog_file()
{
	return singleton<pane_appearance_base_file_dialog_fileObj>::get()->config;
}

LIBCXXW_NAMESPACE_END
