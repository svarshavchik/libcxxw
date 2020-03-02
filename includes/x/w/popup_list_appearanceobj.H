/*
** Copyright 2019-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_popup_list_appearanceobj_h
#define x_w_popup_list_appearanceobj_h

#include <x/w/popup_list_appearancefwd.H>
#include <x/w/list_appearanceobj.H>

LIBCXXW_NAMESPACE_START

//! All properties inherited by a \ref popup_list_appearance "popup list appearance object".

//! That's in addition to the ones that are inherited from the
//! \ref list_appearance "list appearance properties" object.

struct popup_list_appearance_properties {

	//! Constructor

	popup_list_appearance_properties();

	//! Destructor

	~popup_list_appearance_properties();

#include <x/w/popup_list_appearance.inc.H>

};

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
