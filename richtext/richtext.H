/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef richtext_H
#define richtext_H

#include "richtext/richtextstring.H"
#include "richtext/richtextfwd.H"
#include "richtext/richtextobj.H"
#include "x/w/metrics/axis.H"
#include "x/w/alignment.H"

LIBCXXW_NAMESPACE_START

class richtextBase {

public:

	static richtext create(richtextstring &string,
			       halign alignment, dim_t initial_width);

	template<typename ref_type>
	class objfactory {

	public:
		static inline ref_type create(richtextstring &string,
					      halign alignment,
					      dim_t initial_width)
		{
			return richtextBase::create(string, alignment,
						    initial_width);
		}
	};
};

LIBCXXW_NAMESPACE_END

#endif
