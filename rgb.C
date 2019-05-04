/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/rgb.H"
#include "defaulttheme.H"
#include "messages.H"
#include <sstream>
#include <iomanip>
#include <charconv>


LIBCXXW_NAMESPACE_START

rgb::rgb(const std::string_view &s) : rgb{}
{
	auto b=s.begin(), e=s.end();

	while (b != e)
	{
		if (*b == ',' || *b == ' ' || *b == '\t')
		{
			++b;
			continue;
		}

		size_t i;

		for (i=0; i<4; ++i)
			if (*b == rgb_channels[i][0])
				break;

		if ( i >= 4 || ++b == e || *b != '=')
			throw EXCEPTION(_("Cannot parse color"));


		rgb_component_t v=0;

		auto p=++b;

		auto ret=std::from_chars(&*p, &*e, v, 16);

		if (ret.ec != std::errc{})
			throw EXCEPTION(_("Cannot parse color"));

		b += (ret.ptr-&*p);

		// 2-digit hex value, scale it up.
		if (b-p <= 2)
			v |= (v << 8);

		(this->*(rgb_fields[i]))=v;
	}
}

rgb::operator std::string() const
{
	std::string s;

	s.reserve(30);

	char hex[8]="0000";

	for (size_t i=0; i<4; ++i)
	{
		if (i)
			s += ", ";
		s += rgb_channels[i];
		s += '=';

		auto ret=std::to_chars(hex+4, hex+8,
				       (this->*(rgb_fields[i])),
				       16);

		if (ret.ec != std::errc{}) // Shouldn't happen.
			throw EXCEPTION(_("Cannot parse color"));

		s.append(ret.ptr-4, ret.ptr);
	}

	return s;
}

std::ostream &operator<<(std::ostream &o, const rgb &r)
{
	return o << std::string{r};
}

LIBCXXW_NAMESPACE_END
