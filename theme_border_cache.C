/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "theme_border_cache.H"
#include "current_border_impl.H"
#include "border_impl.H"
#include "connection.H"
#include "screen.H"
#include "defaulttheme.H"

LIBCXXW_NAMESPACE_START

theme_border_cacheObj::theme_border_cacheObj()
	: map{map_t::create()}
{
}

theme_border_cacheObj::~theme_border_cacheObj()=default;


class LIBCXX_HIDDEN theme_current_border_implObj
	: public current_border_implObj {

	const ref<screenObj::implObj> screen;
	const std::string name;

	defaulttheme current_theme_thread_only;

 public:
	THREAD_DATA_ONLY(current_theme);

	theme_current_border_implObj(const ref<screenObj::implObj> &screen,
				     const std::string &name)
		: theme_current_border_implObj(screen, name,
					       current_theme_t::lock
					       {screen->current_theme})
	{
	}

	theme_current_border_implObj(const ref<screenObj::implObj> &screen,
				     const std::string &name,
				     current_theme_t::lock &&lock)

		: current_border_implObj((*lock)
					 ->get_theme_border(name,border_info())
					 ),
		screen(screen),
		name(name),
		current_theme_thread_only(*lock)
	{
	}

	~theme_current_border_implObj()=default;

	void theme_updated(IN_THREAD_ONLY, const defaulttheme &new_theme)
	{
		// This theme border object can be attached to multiple
		// border display elements. Go through the motions of
		// creating a new border object only the first time we're
		// called, here.

		auto &t=current_theme(IN_THREAD);

		if (new_theme == t)
			return;

		t=new_theme;

		border(IN_THREAD)=
			new_theme->get_theme_border(name, border(IN_THREAD));
	}
};

current_border_impl screenObj::implObj
::get_theme_border(const std::experimental::string_view &name)
{
	std::string name_s{name};

	return theme_borders->map
		->find_or_create
		(name_s,
		 [&, this]
		 {
			 return ref<theme_current_border_implObj>
				 ::create(ref<implObj>(this), name_s);
		 });
}

LIBCXXW_NAMESPACE_END
