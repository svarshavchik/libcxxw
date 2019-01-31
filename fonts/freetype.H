/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef freetype_H
#define freetype_H

#include <x/logger.H>
#include <x/chrcasecmp.H>

#include "fonts/freetypefwd.H"
#include "fonts/freetypeobj.H"
#include "x/w/screen.H"

LIBCXXW_NAMESPACE_START

const char *freetype_error(FT_Error err) LIBCXX_HIDDEN;

//! Base class for \ref freetype "graphic context"s.

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::freetype::base

class LIBCXX_HIDDEN freetypeBase {
public:

	//! Returns a singleton instance of the freetype library

	static freetype create();

	//! Object factory for freetype objects

	template<typename ref_type> class objfactory {

	public:

		//! This returns the singleton instance of the library

		static inline ref_type create()
		{
			return freetypeBase::create();
		}
	};
};

//! A wrapper for some object owned by the freetype library.

//! Make sure only a single thread accesses any freetype object originating
//! from the same FT_Library instance.
//!
//! This template wraps some freetype-related object, and requires a lock
//! to be acquired on the freetype library instance, before accessing the
//! object.

template<typename obj_type>
class LIBCXX_HIDDEN freetypeObj::ft_object {

	//! The locked object.

	obj_type obj;

	//! The library the object came from.
	const const_freetype library;

 public:

	//! Constructor

	//! The first argument is the library, the remaining arguments
	//! construct the locked object.
	template<typename ...Args>
		ft_object(const const_freetype &libraryArg, Args && ...args)
		: obj{std::forward<Args>(args)...},
		library{libraryArg}
	{
	}

	//! Return the freetype library.

	const const_freetype &get_library() const
	{
		return library;
	}

	//! A lock on the underlying object.

	//! Acquires a lock on the library, before accessing the object.

	class lock : public freetypeObj::library_t::lock {

		typedef freetypeObj::library_t::lock library_lock_t;
		ft_object<obj_type> &o;
	public:

		//! Constructor

		lock(ft_object<obj_type> &oArg)
			: library_lock_t(oArg.library->library), o(oArg)
		{
		}

		//! Destructor

		~lock()=default;

		//! With the lock, we can return the library object

		FT_Library &library()
		{
			return library_lock_t::operator*();
		}

		//! With the lock, we can return the library object

		const FT_Library &library() const
		{
			return library_lock_t::operator*();
		}

		//! Access the locked object.
		obj_type &operator*()
		{
			return o.obj;
		}

		//! Access the locked object.
		obj_type *operator->()
		{
			return &o.obj;
		}

		//! Access the locked object.
		const obj_type &operator*() const
		{
			return o.obj;
		}

		//! Access the locked object.
		const obj_type *operator->() const
		{
			return &o.obj;
		}
	};

	//! Read-only locked on the library object.

	class const_lock : public freetypeObj::library_t::lock {

		//! Convenience typedef

		typedef freetypeObj::library_t::lock library_lock_t;
		const ft_object<obj_type> &o;
	public:

		//! The constructor acquires the lock.
		const_lock(const ft_object<obj_type> &oArg)
			: library_lock_t(oArg.library->library), o(oArg)
		{
		}

		//! Destructor
		~const_lock()=default;

		//! With the lock, we can return the library object

		const FT_Library &library() const
		{
			return library_lock_t::operator*();
		}

		//! Access the locked object.

		const obj_type &operator*() const
		{
			return o.obj;
		}

		//! Access the locked object.
		const obj_type *operator->() const
		{
			return &o.obj;
		}
	};

};

//! A wrapper for an Ft_Bitmap object.

class LIBCXX_HIDDEN freetypeObj::ftbitmap {

 public:

	//! Convenient typedef alias.

	typedef ft_object<FT_Bitmap> bitmap_t;

	//! The locked bitmap
	bitmap_t bitmap;

	//! Constructor
	ftbitmap(const const_freetype &libraryArg);

	//! Destructor
	~ftbitmap();

	ftbitmap(const ftbitmap &)=delete;
	ftbitmap(ftbitmap &&)=delete;

	ftbitmap &operator=(const ftbitmap &)=delete;
	ftbitmap &operator=(ftbitmap &&)=delete;
};

LIBCXXW_NAMESPACE_END

#endif
