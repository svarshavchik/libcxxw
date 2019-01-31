/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef fontpattern_impl_H
#define fontpattern_impl_H

#include "fonts/fontconfigfwd.H"
#include "fonts/fontpatternobj.H"
#include <x/mpobj.H>
#include <fontconfig/fontconfig.h>

LIBCXXW_NAMESPACE_START

namespace fontconfig {
#if 0
}
#endif

//! Font pattern implementation.

class patternObj::implObj : virtual public obj {

public:
	//! Constructor
	implObj(const config &configArg);

	//! Constructor

	implObj(const config &configArg,
		FcPattern *pArg,
		bool autodestroyArg);

	//! Destructor
	~implObj();

	//! The fontconfig library.

	const config c;

	//! Thread-safe container for the FcPattern wrapper
	typedef mpobj<FcPattern * const> p_t;

	//! The pattern.
	mutable p_t p;

	//! Whether this object owns the FcPattern, and the destructor must destroy it.

	const bool autodestroy;

		//! Clone this pattern.

	pattern clone() const;

	//! Convert the pattern into a form that can be used with parse_pattern()

	std::string unparse() const;

	//! Add a value to a pattern.
	void add_integer(const std::string_view &object, int val, bool append=false);
	//! Add a value to a pattern.
	void add_double(const std::string_view &object, double val, bool append=false);
	//! Add a value to a pattern.
	void add_bool(const std::string_view &object, bool val, bool append=false);
	//! Add a value to a pattern.
	void add_string(const std::string_view &object, const std::string_view &val,
			bool append=false);
	//! Add a value to a pattern.
	void add_charset(const std::string_view &object, const const_charset &val,
			 bool append=false);

	//! Get a value.
	bool get_integer(const std::string_view &object, int &value, size_t index=0)
		const;
	//! Get a value.
	bool get_double(const std::string_view &object, double &value,
			size_t index=0)
		const;
	//! Get a value.
	bool get_bool(const std::string_view &object, bool &value, size_t index=0)
		const;
	//! Get a value.
	bool get_string(const std::string_view &object, std::string &value,
			size_t index=0)
		const;
	//! Get a value.
	bool get_charset(const std::string_view &object,
			 const_charsetptr &value, size_t index=0);

	//! Delete property

	//! Bool return value indicates whether it existed.
	bool del(const std::string_view &object);

	//! Substitute default patterns parameters.
	void substitute_matchpattern();

	//! Substitute default font parameters.
	void substitute_matchfont();

	//! Substitute default scanning parameters.
	void substitute_matchscan();

	//! Substitute miscellaneous default values.
	void substitute_defaults();
};

//! Internal lock.

class LIBCXX_HIDDEN pattern_lock : public patternObj::implObj::p_t::lock {

 public:
	pattern_lock(const patternObj::implObj *me);
	~pattern_lock();
};

#if 0
{
#endif
}

LIBCXXW_NAMESPACE_END

#endif
