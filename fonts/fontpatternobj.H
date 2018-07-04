/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef fontpatternobj_H
#define fontpatternobj_H

#include <x/ptrfwd.H>
#include <x/obj.H>

#include <string_view>
#include "x/w/impl/fonts/fontcharsetfwd.H"
#include "fonts/fontpatternfwd.H"
#include "fonts/fontsortedlistfwd.H"
#include "x/w/screenobj.H"

LIBCXXW_NAMESPACE_START

namespace fontconfig {
#if 0
}
#endif

//! Information about a font.

class LIBCXX_HIDDEN patternObj : virtual public obj {

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	patternObj(const ref<implObj> &implArg);

	//! Destructor
	~patternObj();

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
			 const_charsetptr &value, size_t index=0)
		const;

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

	//! Return a list of fonts sorted by best match.
	sortedlist match(bool trim=true);

	//! Return this pattern's point size for the given screen.

	//! Returns 0 if no point size is defined (scalable font?)
	size_t get_point_size(const ref<screenObj::implObj> &screen) const;
};

#if 0
{
#endif
}

LIBCXXW_NAMESPACE_END

#endif
