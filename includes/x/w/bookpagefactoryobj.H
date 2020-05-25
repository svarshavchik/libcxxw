/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_bookpagefactoryobj_h
#define x_w_bookpagefactoryobj_h

#include <x/w/booklayoutmanagerfwd.H>
#include <x/w/bookpagefactoryfwd.H>
#include <x/w/bookpage_appearancefwd.H>
#include <x/w/factoryfwd.H>
#include <x/w/alignment.H>
#include <x/w/shortcutfwd.H>
#include <x/w/text_paramfwd.H>
#include <x/w/alignment.H>
#include <x/w/concepts.H>
#include <x/functional.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see bookpagefactory

class bookpagefactoryObj : virtual public obj {

public:

	//! Constructor
	bookpagefactoryObj() LIBCXX_HIDDEN;

	//! Destructor
	~bookpagefactoryObj() LIBCXX_HIDDEN;

	//! Appearance of the new page's tab.

	const_bookpage_appearance appearance;

	//! Set the appearance.

	//! Convenience function for the benefit of the theme file.

	bookpagefactoryObj &set_appearance(const const_bookpage_appearance &);

	//! Add another page to the book.

	template<factory_processor tab_factory,
		 factory_processor page_factory>
	inline auto &add(tab_factory &&tf,
			 page_factory &&pf)
	{
		do_add(make_function<void (const factory &)>
		       (std::forward<tab_factory>(tf)),
		       make_function<void (const factory &)>
		       (std::forward<page_factory>(pf)));
		return *this;
	}

	//! Add another page to the book.

	//! \overload
	template<factory_processor tab_factory,
		 factory_processor page_factory>
	inline auto &add(tab_factory &&tf,
			 page_factory &&pf,
			 const create_bookpage_args_t &args)
	{
		do_add(make_function<void (const factory &)>
		       (std::forward<tab_factory>(tf)),
		       make_function<void (const factory &)>
		       (std::forward<page_factory>(pf)), args);

		return *this;
	}

	//! Add another page to the book.

	//! \overload

	template<factory_processor page_factory>
	inline auto &add(const text_param &label,
			 page_factory &&pf)
	{
		do_add(label,
		       make_function<void (const factory &)>
		       (std::forward<page_factory>(pf)));
		return *this;
	}

	//! Add another page to the book.

	//! \overload
	template<factory_processor page_factory>
	inline auto &add(const text_param &label,
			 page_factory &&pf,
			 const create_bookpage_with_label_args_t &args)
	{
		do_add(label,
		       make_function<void (const factory &)>
		       (std::forward<page_factory>(pf)), args);

		return *this;
	}

	//! Type-erased add().

	void do_add(const text_param &,
		    const function<void (const factory &)> &);

	//! Type-erased add().

	void do_add(const text_param &,
		    const function<void (const factory &)> &,
		    const create_bookpage_with_label_args_t &);


	//! Type-erased add().

	void do_add(const function<void (const factory &)> &,
		    const function<void (const factory &)> &);

	//! Type-erased add().

	virtual void do_add(const function<void (const factory &)> &,
			    const function<void (const factory &)> &,
			    const create_bookpage_args_t &)=0;

	//! Set next page's alignment

	virtual bookpagefactoryObj &halign(LIBCXXW_NAMESPACE::halign)=0;

	//! Set next page's alignment

	virtual bookpagefactoryObj &valign(LIBCXXW_NAMESPACE::valign)=0;
};

LIBCXXW_NAMESPACE_END

#endif
