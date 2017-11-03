/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_textlistlayoutmanager_h
#define x_w_textlistlayoutmanager_h

#include <x/w/textlistlayoutmanagerfwd.H>
#include <x/w/textlistlayoutmanagerobj.H>
#include <x/w/listlayoutmanagerbase.H>
#include <x/w/text_param.H>
#include <x/w/image_param.H>
#include <x/w/shortcut.H>

LIBCXXW_NAMESPACE_START

struct list_item_param : public list_item_param_base {

	using list_item_param_base::list_item_param_base;

	typedef list_item_param_base variant_t;

	template<typename Arg1, typename ...Args,
		 typename=std::void_t<text_param_constructible
				      <Arg1, Args...>>>
	list_item_param(Arg1 &&arg1,
			Args && ...args)
		: list_item_param_base{text_param{std::forward<Arg1>(arg1),
				std::forward<Args>(args)...}}
	{
	}

	template<typename Arg1, typename=
		 std::enable_if<is_list_item_status_change_callback<Arg1>
				::value>>
	list_item_param(Arg1 &&arg1)
		: list_item_param_base{list_item_status_change_callback{
			std::forward<Arg1>(arg1)}}
	{
	}

	~list_item_param();

	list_item_param(const list_item_param &);
	list_item_param(list_item_param &&);

	list_item_param &operator=(const list_item_param &);
	list_item_param &operator=(list_item_param &&);
};

LIBCXXW_NAMESPACE_END

#endif
