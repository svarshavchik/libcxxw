/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef paragraph_list_H
#define paragraph_list_H

#include "richtext/richtextparagraph.H"
#include "richtext/richtext_impl.H"
#include "connection_thread.H"
#include "x/w/screenfwd.H"

LIBCXXW_NAMESPACE_START

class richtextObj;
class fragment_list;

//! All the \ref richtext "rich text paragraphs".

//! A character with a UNICODE_LB_MANDATORY always begins a new paragraph.
//! This helper class operates on richtextparagraphObj's. This is the only class
//! that has full access to a paragraph's paragraphs_t.

class LIBCXX_HIDDEN paragraph_list {

public:

	//! My rich text.

	richtextObj::implObj &text;

 private:

	//! Flag: if set, the destructor calls recalculate_size()

	bool size_changed=false;
 public:

	//! Constructor
	paragraph_list(richtextObj::implObj &textArg);

	//! Destructor

	//! If size_changed is set, recalculate_size() gets invoked.

	~paragraph_list();

	//! Create a new paragraph object.

	//! Creates a new paragraph at the given position. Sets its
	//! my_paragraph_number, and increments it for all following
	//! paragraphs.

	richtextparagraph insert_new_paragraph(size_t insert_pos);

	//! Add new paragraph to the rich text object.
	richtextparagraph append_new_paragraph();

	//! Remove the given paragraph from richtext

	//! Decrements all subsequent paragraphs' my_paragraph_number.
	void erase(size_t paragraph_number);

 private:
	//! Recalculate the width and the height of this text .

	//! The width is the maximum width of all paragraphs. The last paragraph's
	//! below baseline is our below_baseline. Its above_baseline, and
	//! all above+below of all other paragraphs, is our above_baseline.
	//! Also calculates maximum_width_if_one_line and
	//! height_of_tallest_fragment.

	void recalculate_size();
 public:
	friend class fragment_list;

	//! Invoke all paragraphs' rewrap() method.
	bool rewrap(IN_THREAD_ONLY, dim_t width);

	//! Invoke all paragraphs' unwrap() method.
	bool unwrap(IN_THREAD_ONLY);

	//! Invoke all paragraphs's fragments' theme_updated().

	//! Takes care of updating the Y position of everything.
	void theme_updated(IN_THREAD_ONLY);
};

LIBCXXW_NAMESPACE_END

#endif
