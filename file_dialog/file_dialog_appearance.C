/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/file_dialog_appearance.H"
#include "x/w/pane_layout_appearance.H"
#include "x/w/list_appearance.H"
#include "x/w/pane_appearance.H"

LIBCXXW_NAMESPACE_START

file_dialog_appearance_properties::file_dialog_appearance_properties()
	: filedir_pane_appearance{pane_layout_appearance::base::theme()},
	  dir_pane_appearance{pane_appearance::base::file_dialog_dir()},
	  file_pane_appearance{pane_appearance::base::file_dialog_file()},
	  dir_pane_list_appearance{list_appearance::base::list_pane_theme()},
	  file_pane_list_appearance{list_appearance::base::list_pane_theme()},
	  filedir_filename_font{theme_font{"filedir_filename"}},
	  filedir_filedate_font{theme_font{"filedir_filedate"}},
	  filedir_filesize_font{theme_font{"filedir_filesize"}},
	  filedir_directoryfont{theme_font{"filedir_directoryfont"}},
	  filedir_highlight_fg{theme_color{"filedir_highlight_fg"}},
	  filedir_highlight_bg{theme_color{"filedir_highlight_bg"}}
{
}

file_dialog_appearance_properties::~file_dialog_appearance_properties()=default;

file_dialog_appearanceObj::file_dialog_appearanceObj()=default;

file_dialog_appearanceObj::~file_dialog_appearanceObj()=default;

file_dialog_appearanceObj::file_dialog_appearanceObj
(const file_dialog_appearanceObj &o)
	: file_dialog_appearance_properties{o}
{
}

file_dialog_appearance file_dialog_appearanceObj::clone() const
{
	return file_dialog_appearance::create(*this);
}

const const_file_dialog_appearance &file_dialog_appearance_base::theme()
{
	static const const_file_dialog_appearance config=
		const_file_dialog_appearance::create();

	return config;
}

LIBCXXW_NAMESPACE_END
