/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef richtext_linebreak_info_H
#define richtext_linebreak_info_H

#include "x/w/namespace.H"
#include "richtext/richtext_linebreak_infofwd.H"
#include "x/w/impl/richtext/richtextstring.H"
#include <courier-unicode.h>
#include <queue>
#include <tuple>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN richtext_linebreak_calculate;

//! Run the unicode linebreak algorithm on one or more richtextstrings.

//! Additionally, detect changes in left-to-right rendering, and
//! make sure that linebreaking is allowed at the change of direction
//! point. Whenever the rendering direction changes, UNICODE_LB_NONE is
//! overridden to UNICODE_LB_ALLOWED.
//!
//! Writes the linebreaking values to a preallocated array of shorts.
//! The constructor takes a pointer to a vector of unicode_lbs, of size "todo".
//! The initial #skip characters get skipped, when writing the linebreaking
//! algorithm.
//!
//! Invoke the () operator, with one or more richtextstrings, followed by
//! finish().
class richtext_linebreak_info
	: private unicode::linebreak_callback_base {

	//! Running skip count
	size_t skip;

	//! Remaining todo
	size_t todo;

	//! Writing unicode linebreaking values.
	unicode_lb *ptr;

	//! Queues up richtextstrings.

	//! unicode linebreak algorithm does not guarantee a one for one
	//! callback. We need to pair each linebreak value with the richtext
	//! metadata, so as soon as each richtextstring comes in we grab the
	//! beginning an th ending iterator value to its metadata, and the
	//! total size of the richtext string, and queue them up.
	//!
	//! Each callback invokation logically works its way through each
	//! meta vector, until the entire metadata from a single richtextstring
	//! gets consumed; the queue then gets poppped and we're ready for the
	//! next range, if any.
	std::queue<std::tuple<
			   richtextstring::meta_t::const_iterator,
			   richtextstring::meta_t::const_iterator,
			   size_t>
		   > metadata;

	//! Current index in the metadata.
	size_t meta_index;

	//! Current text direction.
	bool rl;

public:
	//! Constructor
	richtext_linebreak_info(size_t skipArg,
				size_t todoArg,
				unicode_lb *ptrArg);

	//! Destructor
	~richtext_linebreak_info();

private:
	//! Callback from the courier-unicode library
	int callback(int value) override;

public:
	//! Compute linebreaks for.
	void operator()(const richtextstring &s);

	//! use finish() to conclude the job.
	using unicode::linebreak_callback_base::finish;
};

LIBCXXW_NAMESPACE_END;
#endif
