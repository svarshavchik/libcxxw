/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef theme_parser_lock_H
#define theme_parser_lock_H

#include <x/xml/doc.H>
#include <x/locale.H>
#include "theme_parser_lockfwd.H"

LIBCXXW_NAMESPACE_START

// Internal lock used when processing the XML file.

// We also need to carry around the "C" locale, in order to parse the
// numerical values in the XML file. It's convenient to swallow
// the XML document's read lock, and attach a locale object, in this manner.

class theme_parser_lock : public xml::doc::base::readlock {

public:

	const_locale c_locale;

	//! Constructor
	theme_parser_lock(const xml::doc::base::readlock &l);

	//! Constructor
	theme_parser_lock(const xml::doc::base::readlock &l,
			  const const_locale &c_locale);
	//! Destructor
	~theme_parser_lock();

	//! Clone the lock.
	theme_parser_lock clone() const;
};

LIBCXXW_NAMESPACE_END
#endif
