/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_file_dialog_appearanceobj_h
#define x_w_file_dialog_appearanceobj_h

#include <x/w/file_dialog_appearancefwd.H>
#include <x/w/pane_layout_appearancefwd.H>
#include <x/w/pane_appearancefwd.H>
#include <x/w/list_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/font_arg.H>
#include <x/w/rgb.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Actual properties inherited by \ref file_dialog_appearance "INSERT_LIBX_NAMESPACE::w::file_dialog_appearance".

struct file_dialog_appearance_properties {

	//! Constructor
	file_dialog_appearance_properties();

	//! Destructor
	~file_dialog_appearance_properties();

	//! Custom appearance of the directory/file pane container
	const_pane_layout_appearance filedir_pane_appearance;

	//! Custom appearance of the directory pane
	const_pane_appearance dir_pane_appearance;

	//! Custom appearance of the file pane
	const_pane_appearance file_pane_appearance;

	//! Custom appearance of the directory pane's list

	const_list_appearance dir_pane_list_appearance;

	//! Custom appearance of the file pane's list

	const_list_appearance file_pane_list_appearance;

	//! Font used for filenames
	font_arg filedir_filename_font;

	//! Font used for file dates
	font_arg filedir_filedate_font;

	//! Font used for file sizes
	font_arg filedir_filesize_font;

	//! Font used for the current directory name
	font_arg filedir_directoryfont;

	//! Current directory, the currently highlighted part
	text_color_arg filedir_highlight_fg;

	//! Current directory, the currently highlighted part
	text_color_arg filedir_highlight_bg;

};

//! Implementation object.

//! \see file_dialog_appearance

class file_dialog_appearanceObj: public file_dialog_appearance_properties,
				 virtual public obj {

public:
	//! Constructor
	file_dialog_appearanceObj();

	//! Destructor
	~file_dialog_appearanceObj();

	//! Copy constructor
	file_dialog_appearanceObj(const file_dialog_appearanceObj &);

	//! Create a copy of this object.

	file_dialog_appearance clone() const;
};

LIBCXXW_NAMESPACE_END

#endif
