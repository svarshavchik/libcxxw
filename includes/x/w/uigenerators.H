/*
** Copyright 2019-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_uigenerators_h
#define x_w_uigenerators_h

#include <x/w/uigeneratorsfwd.H>
#include <x/w/uigeneratorsobj.H>
#include <x/w/screen_positions.H>
#include <x/functionalrefptr.H>
#include <x/vector.H>
#include <x/ref.H>
#include <x/optional_args.H>
#include <x/refptr_traits.H>
#include <x/messages.H>
#include <x/xml/docfwd.H>
#include <string_view>

LIBCXXW_NAMESPACE_START

//! Base class for \ref uigenerators "parsed UI generators".

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::generators::base

class uigeneratorsBase {

public:

	//! Optional create() parameters.

	typedef optional_args<explicit_refptr<const_screen_positions>,
			      explicit_refptr<const_messages>> create_args_t;

	//! Load template from file.
	static const_uigenerators create(const std::string_view &filename,
					 const create_args_t &args);

	//! Load template from a parsed XML document

	static const_uigenerators create(const xml::doc &parsed_xml,
					 const create_args_t &args);

	//! Factory for objects.

	//! Implements custom create().
	template<typename T> class objfactory {

	public:

		//! create() for uigenerators

		//! Always return a const_uigenerators handle.

		template<typename ...Args>
		static inline const_uigenerators
		create(const std::string_view &filename,
		       Args && ...args)
		{
			return uigeneratorsBase::create(filename,
							{std::forward<Args>
							 (args)...});
		}
		//! Always return a const_uigenerators handle.

		template<typename ...Args>
		static inline const_uigenerators
		create(const xml::doc &parsed_xml,
		       Args && ...args)
		{
			return uigeneratorsBase::create(parsed_xml,
							{std::forward<Args>
							 (args)...});
		}
	};
};

LIBCXXW_NAMESPACE_END

#endif
