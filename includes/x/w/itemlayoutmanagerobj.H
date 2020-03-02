/*
** Copyright 2018-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_itemlayoutmanagerobj_h
#define x_w_itemlayoutmanagerobj_h

#include <x/w/itemlayoutmanagerfwd.H>
#include <x/w/item_button_appearancefwd.H>
#include <x/w/layoutmanagerobj.H>
#include <x/w/factoryfwd.H>
#include <x/w/elementfwd.H>
#include <x/functional.H>
#include <x/functionalrefptrfwd.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see itemlayoutmanager

class itemlayoutmanagerObj: public layoutmanagerObj {

public:
	class LIBCXX_HIDDEN implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	itemlayoutmanagerObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~itemlayoutmanagerObj() LIBCXX_HIDDEN;

	//! Number of existing items
	size_t size() const;

	//! Remove an existing item
	void remove_item(size_t);

	//! Remove existing items

	//! Multiple items version of remove_item().
	void remove_items(size_t, size_t);

	//! Return an existing item
	element get_item(size_t) const;

	//! Add a new item
	template<typename F, typename ...Args>
	inline void append_item(F &&f, Args && ...args)
	{
		do_append(make_function<void (const factory &)>
			  (std::forward<F>(f)),
			  std::forward<Args>(args)...);
	}

	//! Type-erased append_item().
	void do_append(const function<void (const factory &)> &callback);

	//! Type-erased append_item().
	void do_append(const function<void (const factory &)> &callback,
		       const const_item_button_appearance &appearance);

	//! Insert a new item
	template<typename F, typename ...Args>
	inline void insert_item(size_t i, F &&f, Args && ...args)
	{
		do_insert(i, make_function<void (const factory &)>
			  (std::forward<F>(f)),
			  std::forward<Args>(args)...);
	}

	//! Type-erased insert_item().
	void do_insert(size_t i,
		       const function<void (const factory &)> &callback);

	//! Type-erased insert_item().
	void do_insert(size_t i,
		       const function<void (const factory &)> &callback,
		       const const_item_button_appearance &appearance);

	//! Install a new callback

	void on_remove(const itemlayout_callback_t &callback);

	//! Implement generate()

	//! \see uigenerators

	void generate(const std::string_view &name,
		      const const_uigenerators &generators,
		      uielements &elements) override;

	using layoutmanagerObj::generate;
};

LIBCXXW_NAMESPACE_END

#endif
