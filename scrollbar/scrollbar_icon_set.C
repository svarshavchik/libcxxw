/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "scrollbar/scrollbar_icon_set.H"
#include "icon.H"

LIBCXXW_NAMESPACE_START

void scrollbar_icon_set::initialize(IN_THREAD_ONLY)
{
	scroll_low_icon=scroll_low_icon->initialize(IN_THREAD);
	scroll_high_icon=scroll_high_icon->initialize(IN_THREAD);

	handlebar_start_icon=handlebar_start_icon->initialize(IN_THREAD);
	handlebar_icon=handlebar_icon->initialize(IN_THREAD);
	handlebar_end_icon=handlebar_end_icon->initialize(IN_THREAD);
}

void scrollbar_icon_set::theme_updated(IN_THREAD_ONLY,
				       const defaulttheme &new_theme)
{
	scroll_low_icon=scroll_low_icon->theme_updated(IN_THREAD,
						       new_theme);
	scroll_high_icon=scroll_high_icon->theme_updated(IN_THREAD,
							 new_theme);

	handlebar_start_icon=handlebar_start_icon->theme_updated(IN_THREAD,
								 new_theme);
	handlebar_icon=handlebar_icon->theme_updated(IN_THREAD,
						     new_theme);
	handlebar_end_icon=handlebar_end_icon->theme_updated(IN_THREAD,
							     new_theme);
}

LIBCXXW_NAMESPACE_END
