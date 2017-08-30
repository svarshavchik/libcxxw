/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_menulayoutmanagerobj_h
#define x_w_menulayoutmanagerobj_h

#include <x/w/menulayoutmanagerfwd.H>
#include <x/w/listlayoutmanagerobj.H>
#include <x/w/menufactoryfwd.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see menulayoutmanager

class menulayoutmanagerObj : public listlayoutmanagerObj {

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	menulayoutmanagerObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~menulayoutmanagerObj() LIBCXX_HIDDEN;

	//! Container used by plain text menu item overloads.

	//! Variadic overloads collect text parameters as a vector of
	//! <text_param, menuitem_type_t> tuples.

	typedef std::vector<std::tuple<menuitem_type_t, text_param>
			    > text_items_t;

	//! Common variadic function for inserting plain text menu items.

	inline void collect_text_items(text_items_t &vec)
	{
	}

	//! Common variadic function for inserting plain text menu items.

	template<typename ...Args>
	inline void collect_text_items(text_items_t &vec,
				       const std::vector<text_param> &s,
				       Args && ...args)
	{
		for (const auto &t:s)
			vec.emplace_back(menuitem_type_t{}, t);
		collect_text_items(vec, std::forward<Args>(args)...);
	}

	//! Common variadic function for inserting plain text menu items.

	template<typename ...Args>
	inline void collect_text_items(text_items_t &vec,
				       const text_items_t &s,
				       Args && ...args)
	{
		vec.insert(vec.end(), s.begin(), s.end());
		collect_text_items(vec, std::forward<Args>(args)...);
	}

	//! Common variadic function for inserting plain text menu items.

	template<typename ...Args>
	inline void collect_text_items(text_items_t &vec,
				       const menuitem_type_t &t,
				       const text_param &s,
				       Args && ...args)
	{
		vec.emplace_back(t, s);
		collect_text_items(vec, std::forward<Args>(args)...);
	}

	//! Common variadic function for inserting plain text menu items.

	template<typename ...Args>
	inline void collect_text_items(text_items_t &vec,
				       const text_param &s,
				       Args && ...args)
	{
		vec.emplace_back(menuitem_plain{}, s);
		collect_text_items(vec, std::forward<Args>(args)...);
	}

	//! Override append_item().

	//! Invoke append_menu_item().

	factory append_item() override;

	//! Append a new menu item
	menufactory append_menu_item();

	//! Override append_item().

	//! Reimplement in terms of append_menu_item().

	void append_item(const std::vector<text_param> &items) override;

	//! Append a new menu item

	//! \overload
	template<typename Arg, typename ...Args>
	inline void append_menu_item(Arg &&arg, Args && ...args)
	{
		text_items_t vec;

		collect_text_items(vec,
				   std::forward<Arg>(arg),
				   std::forward<Args>(args)...);

		do_append_menu_item(vec);
	}

	//! Append a new menu item

	//! \internal

	void do_append_menu_item(const text_items_t &v);

	//! Override insert_item().

	//! Invoke insert_menu_item().

	factory insert_item(size_t item_number) override;

	//! Insert a new menu item.
	menufactory insert_menu_item(size_t item_number);

	//! Override insert_item().

	//! Reimplement in terms of insert_menu_item().

	void insert_item(size_t item_number,
			 const std::vector<text_param> &items) override;

	//! Insert a new menu item

	//! \overload
	template<typename Arg, typename ...Args>
	inline void insert_menu_item(size_t item_number,
				     Arg && arg,
				     Args && ...args)
	{
		text_items_t vec;

		collect_text_items(vec,
				   std::forward<Arg>(arg),
				   std::forward<Args>(args)...);

		do_insert_menu_item(item_number, vec);
	}

	//! Append a new menu item

	//! \internal

	void do_insert_menu_item(size_t item_number, const text_items_t &v);

	//! Override replace_item().

	//! Invoke replace_menu_item().

	factory replace_item(size_t item_number) override;

	//! Replace menu items.
	menufactory replace_menu_item(size_t item_number);

	//! Override replace_item().

	//! Reimplement in terms of replace_menu_item().

	void replace_item(size_t item_number,
			  const std::vector<text_param> &items) override;

	//! Replace menu items.

	//! \overload
	template<typename Arg,
		 typename ...Args>
	inline void replace_menu_item(size_t item_number,
				      Arg && arg,
				      Args && ...args)
	{
		text_items_t vec;

		collect_text_items(vec,
				   std::forward<Arg>(arg),
				   std::forward<Args>(args)...);

		do_replace_menu_item(item_number, vec);
	}

	//! Append a new menu item

	//! \internal

	void do_replace_menu_item(size_t item_number, const text_items_t &v);


      	//! Remove a menu item
	inline void remove_menu_item(size_t item_number)
	{
		// Perfectly adequate implementation in the listlayoutmanager.
		remove_item(item_number);
	}

	//! Override replace_all_item().

	//! Invoke replace_all_menu_item().

	factory replace_all_items() override;

	//! Replace all existing menu items.

	menufactory replace_all_menu_items();

	//! Override replace_all_item().

	//! Reimplement in terms of replace_all_menu_items().

	void replace_all_items(const std::vector<text_param> &items) override;

	//! Replace all existing menu items.

	//! \overload
	template<typename Arg, typename ...Args>
	inline void replace_all_menu_items(Arg &&arg, Args && ...args)
	{
		text_items_t vec;

		collect_text_items(vec,
				   std::forward<Arg>(arg),
				   std::forward<Args>(args)...);

		do_replace_all_menu_items(vec);
	}

	//! Replace_All a new menu item

	//! \internal

	void do_replace_all_menu_items(const text_items_t &v);

	//! Change an existing menu item's shortcut.
	void update(size_t item_number,
		    const menuitem_type_t &new_type);
};

LIBCXXW_NAMESPACE_END

#endif
