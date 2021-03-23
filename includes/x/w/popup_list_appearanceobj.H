/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_popup_list_appearanceobj_h
#define x_w_popup_list_appearanceobj_h

#include <x/w/popup_list_appearancefwd.H>
#include <x/w/list_appearanceobj.H>
#include <x/w/popup_list_appearance.inc.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see popup_list_appearance

class popup_list_appearanceObj : public list_appearanceObj,
				 public popup_list_appearance_properties {

public:
	//! Constructor
	popup_list_appearanceObj();

	//! Destructor
	~popup_list_appearanceObj();

	//! Copy constructor
	popup_list_appearanceObj(const popup_list_appearanceObj &);

	//! Clone this object.
	template<typename F> inline const_popup_list_appearance modify(F &&f)
		const
	{
		return do_modify(make_function
				 <void(const popup_list_appearance &)>
				 (std::forward<F>(f)));
	}

	//! Type-erased modify().
	const_popup_list_appearance
	do_modify(const function<void(const popup_list_appearance &)> &) const;
};

LIBCXXW_NAMESPACE_END

#endif
