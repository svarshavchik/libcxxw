/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/pane_appearance.H"
#include "x/w/scrollbar_appearance.H"
#include "x/w/scrollbar.H"

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

pane_appearance pane_appearanceObj::clone() const
{
	return pane_appearance::create(*this);
}

const const_pane_appearance &pane_appearance_base::theme()
{
	static const const_pane_appearance config=
		const_pane_appearance::create();

	return config;
}

static const_pane_appearance create_focusable_list()
{
	auto appearance=pane_appearance::base::theme()->clone();

	appearance->pane_scrollbar_visibility=scrollbar_visibility::never;
	appearance->left_padding=
		appearance->right_padding=
		appearance->top_padding=
		appearance->bottom_padding=0;
	appearance->horizontal_alignment=halign::fill;
	appearance->vertical_alignment=valign::fill;

	return appearance;
}

const const_pane_appearance &pane_appearance_base::focusable_list()
{
	static const const_pane_appearance obj=create_focusable_list();

	return obj;
}

static const_pane_appearance create_file_dialog_dir()
{
	auto appearance=pane_appearance_base::focusable_list()->clone();

	appearance->size=30;

	return appearance;
}

static const_pane_appearance create_file_dialog_file()
{
	auto appearance=pane_appearance_base::focusable_list()->clone();

	appearance->size=50;

	return appearance;
}

const const_pane_appearance &pane_appearance_base::file_dialog_dir()
{
	static const const_pane_appearance config=create_file_dialog_dir();

	return config;
}

const const_pane_appearance &pane_appearance_base::file_dialog_file()
{
	static const const_pane_appearance config=create_file_dialog_file();

	return config;
}

LIBCXXW_NAMESPACE_END
