/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include <x/exception.H>
#include "x/w/namespace.H"
#include <iostream>

#define x_w_impl_richtext_richtextmeta_H
#define x_w_impl_fonts_freetypefontfwd_H
#define x_w_impl_fonts_freetypefont_H

LIBCXXW_NAMESPACE_START

class richtextmeta {

public:

	char c;
	bool rl=false;

	bool operator==(const richtextmeta &o) const
	{
		return c == o.c && rl == o.rl;
	}

	bool operator!=(const richtextmeta &o) const { return !operator==(o); }

	richtextmeta *operator->()
	{
		return this;
	}

	const richtextmeta *operator->() const
	{
		return this;
	}

	template<typename iter_type>
	void load_glyphs(iter_type b, iter_type e,
			 char32_t unprintable_char) const
	{
	}

	template<typename iter_type, typename lambda_type>
	void glyphs_size_and_kernings(iter_type b,
				      iter_type e,
				      lambda_type &&lambda,
				      char32_t prev_char,
				      char32_t unprintable_char) const
	{
		while (b != e)
		{
		 lambda(16, 16, -(int)(prev_char & 15), 0);

		 prev_char = *b;

		 ++b;
		}
	}
};

typedef richtextmeta freetypefont;

LIBCXXW_NAMESPACE_END

#include "richtext/richtextstring.C"
#include "richtext/richtextstring2.C"
#include "assert_or_throw.C"

LIBCXXW_NAMESPACE_START

const richtextstring::resolved_fonts_t &richtextstring::resolve_fonts() const
{
	resolved_fonts=get_meta();

	return resolved_fonts;
}

LIBCXXW_NAMESPACE_END

using namespace LIBCXX_NAMESPACE::w;

struct insert_test_case {

	const char *testname;

	const char32_t *initial_s;
	std::unordered_map<size_t, richtextmeta> initial_meta;

	size_t insert_pos;
	const char32_t *insert_s;
	std::unordered_map<size_t, richtextmeta> insert_meta;

	const char32_t *expect_s;
	std::vector<std::pair<size_t, richtextmeta>> expect_meta;
};

const struct insert_test_case insert_test_cases[]={
	{
		"Basic insert",

		U"",
		{},

		0,
		U"ab",
		{ {0, {'a'}}},

		U"ab",
		{ {0, {'a'}}},
	},

	{
		"Insert empty at start",
		U"ab",
		{ {0, {'a'}}},

		0,
		U"",
		{},

		U"ab",
		{ {0, {'a'}}},
	},

	{
		"Insert empty in middle",
		U"ab",
		{ {0, {'a'}}},

		1,
		U"",
		{},

		U"ab",
		{ {0, {'a'}}},
	},

	{
		"Insert empty at end",
		U"ab",
		{ {0, {'a'}}},

		2,
		U"",
		{},

		U"ab",
		{ {0, {'a'}}},
	},

	{
		"Insert new meta in beginning",
		U"abc",
		{ {0, {'a'}}, {2, {'c'}}},

		0,
		U"xy",
		{ {0, {'x'}}, {1, {'y'}}},

		U"xyabc",
		{ {0, {'x'}}, {1, {'y'}}, {2, {'a'}}, {4, {'c'}}},
	},

	{
		"Insert new meta in middle #1",
		U"abc",
		{ {0, {'a'}}, {2, {'c'}}},

		1,
		U"xy",
		{ {0, {'x'}}, {1, {'y'}}},

		U"axybc",
		{ {0, {'a'}}, {1, {'x'}}, {2, {'y'}}, {3, {'a'}}, {4, {'c'}}},
	},

	{
		"Insert new meta in middle #2",
		U"abc",
		{ {0, {'a'}}, {2, {'c'}}},

		2,
		U"xy",
		{ {0, {'x'}}, {1, {'y'}}},

		U"abxyc",
		{ {0, {'a'}}, {2, {'x'}}, {3, {'y'}}, {4, {'c'}}},
	},

	{
		"Insert new meta at end",
		U"abc",
		{ {0, {'a'}}, {2, {'c'}}},

		3,
		U"xy",
		{ {0, {'x'}}, {1, {'y'}}},

		U"abcxy",
		{ {0, {'a'}}, {2, {'c'}}, {3, {'x'}}, {4, {'y'}}},
	},

	{
		"Insert new meta in beginning",
		U"abc",
		{ {0, {'a'}}, {2, {'c'}}},

		0,
		U"xy",
		{ {0, {'a'}}, {1, {'c'}}},

		U"xyabc",
		{ {0, {'a'}}, {1, {'c'}}, {2, {'a'}}, {4, {'c'}}},
	},

	{
		"Insert existing meta in middle #1",
		U"abc",
		{ {0, {'a'}}, {2, {'c'}}},

		1,
		U"xy",
		{ {0, {'a'}}, {1, {'c'}}},

		U"axybc",
		{ {0, {'a'}}, {2, {'c'}}, {3, {'a'}}, {4, {'c'}}},
	},

	{
		"Insert existing meta in middle #2",
		U"abc",
		{ {0, {'a'}}, {2, {'c'}}},

		2,
		U"xy",
		{ {0, {'a'}}, {1, {'c'}}},

		U"abxyc",
		{ {0, {'a'}}, {3, {'c'}}},
	},

	{
		"Insert existing meta at end",
		U"abc",
		{ {0, {'a'}}, {2, {'c'}}},

		3,
		U"xy",
		{ {0, {'a'}}, {1, {'c'}}},

		U"abcxy",
		{ {0, {'a'}}, {2, {'c'}}, {3, {'a'}}, {4, {'c'}}},
	},
};

void inserttest()
{
	for (const auto &test:insert_test_cases)
	{
		richtextstring rts{test.initial_s, test.initial_meta};

		rts.insert(test.insert_pos,
			   richtextstring{test.insert_s,
					   test.insert_meta});

		if (rts.get_string() != test.expect_s ||
		    rts.get_meta() != test.expect_meta)
			throw EXCEPTION("Test " << test.testname << " failed");
	}
}

struct erase_substr_test_case {

	const char *testname;

	const char32_t *initial_s;
	std::unordered_map<size_t, richtextmeta> initial_meta;

	size_t erase_pos;
	size_t erase_count;

	const char32_t *erase_expect_s;
	std::vector<std::pair<size_t, richtextmeta>> erase_expect_meta;

	const char32_t *substr_expect_s;
	std::vector<std::pair<size_t, richtextmeta>> substr_expect_meta;
};

const struct erase_substr_test_case erase_substr_test_cases[]={
	{
		"Empty erase #1",

		U"abcde",
		{ {0, {'a'}}, {2, {'c'}}, {4, {'e'}} },

		0, 0,

		U"abcde",
		{ {0, {'a'}}, {2, {'c'}}, {4, {'e'}} },

		U"",
		{},
	},

	{
		"Empty erase #2",

		U"abcde",
		{ {0, {'a'}}, {2, {'c'}}, {4, {'e'}} },

		2, 0,

		U"abcde",
		{ {0, {'a'}}, {2, {'c'}}, {4, {'e'}} },

		U"",
		{},
	},

	{
		"Empty erase #3",

		U"abcde",
		{ {0, {'a'}}, {2, {'c'}}, {4, {'e'}} },

		4, 0,

		U"abcde",
		{ {0, {'a'}}, {2, {'c'}}, {4, {'e'}} },

		U"",
		{},
	},

	{
		"Empty erase #4",

		U"abcde",
		{ {0, {'a'}}, {2, {'c'}}, {4, {'e'}} },

		5, 0,

		U"abcde",
		{ {0, {'a'}}, {2, {'c'}}, {4, {'e'}} },

		U"",
		{},
	},

	{
		"Erase initial 1",

		U"abcde",
		{ {0, {'a'}}, {2, {'c'}}, {4, {'e'}} },

		0, 1,

		U"bcde",
		{ {0, {'a'}}, {1, {'c'}}, {3, {'e'}} },

		U"a",
		{ {0, {'a'}}},
	},

	{
		"Erase initial 2",

		U"abcde",
		{ {0, {'a'}}, {2, {'c'}}, {4, {'e'}} },

		0, 2,

		U"cde",
		{ {0, {'c'}}, {2, {'e'}} },

		U"ab",
		{ {0, {'a'}}},
	},

	{
		"Erase middle 1",

		U"abcde",
		{ {0, {'a'}}, {2, {'c'}}, {4, {'e'}} },

		1, 1,

		U"acde",
		{ {0, {'a'}}, {1, {'c'}}, {3, {'e'}} },

		U"b",
		{ {0, {'a'}}},
	},

	{
		"Erase middle 2",

		U"abcde",
		{ {0, {'a'}}, {2, {'c'}}, {4, {'e'}} },

		1, 2,

		U"ade",
		{ {0, {'a'}}, {1, {'c'}}, {2, {'e'}} },

		U"bc",
		{ {0, {'a'}}, {1, {'c'}}},
	},

	{
		"Erase middle 3",

		U"abcde",
		{ {0, {'a'}}, {2, {'c'}}, {4, {'e'}} },

		1, 3,

		U"ae",
		{ {0, {'a'}}, {1, {'e'}} },

		U"bcd",
		{ {0, {'a'}}, {1, {'c'}}},
	},

	{
		"Erase tail 1",

		U"abcde",
		{ {0, {'a'}}, {2, {'c'}}, {4, {'e'}} },

		4, 1,

		U"abcd",
		{ {0, {'a'}}, {2, {'c'}} },

		U"e",
		{ {0, {'e'}}},
	},

	{
		"Erase tail 2",

		U"abcde",
		{ {0, {'a'}}, {2, {'c'}}, {4, {'e'}} },

		3, 2,

		U"abc",
		{ {0, {'a'}}, {2, {'c'}} },

		U"de",
		{ {0, {'c'}}, {1, {'e'}}},
	},

	{
		"Erase tail 3",

		U"abcde",
		{ {0, {'a'}}, {2, {'c'}}, {4, {'e'}} },

		2, 3,

		U"ab",
		{ {0, {'a'}} },

		U"cde",
		{ {0, {'c'}}, {2, {'e'}}},
	},

	{
		"Erase all",

		U"abcde",
		{ {0, {'a'}}, {2, {'c'}}, {4, {'e'}} },

		0, 5,

		U"",
		{ },

		U"abcde",
		{ {0, {'a'}}, {2, {'c'}}, {4, {'e'}} },
	},
};

void erasesubstrtest()
{
	for (const auto &test:erase_substr_test_cases)
	{
		richtextstring rts{test.initial_s, test.initial_meta};

		richtextstring substr{rts, test.erase_pos, test.erase_count};

		rts.erase(test.erase_pos, test.erase_count);

		if (rts.get_string() != test.erase_expect_s ||
		    rts.get_meta() != test.erase_expect_meta)
			throw EXCEPTION("Erase test " << test.testname << " failed");

		if (substr.get_string() != test.substr_expect_s ||
		    substr.get_meta() != test.substr_expect_meta)
			throw EXCEPTION("Substring test " << test.testname << " failed");
	}
}

struct render_order_test_case {
	const char *testname;

	const char32_t *logical_s;
	std::unordered_map<size_t, richtextmeta> logical_meta;

	const char32_t *render_s;
	std::vector<std::pair<size_t, richtextmeta>> render_meta;
};

const struct render_order_test_case render_order_test_cases[]={
	{
	 "Empty right-to-left",
	 U"",
	 {},
	 U"",
	 {}
	},
	{
	 "One right-to-left, even chars",
	 U"abcdef",
	 { {0, {'a', false}},
	   {1, {'b', true}},
	   {5, {'c', false}}
	 },
	 U"aedcbf",
	 { {0, {'a', false}},
	   {1, {'b', true}},
	   {5, {'c', false}}
	 },
	},
	{
	 "One right-to-left, odd chars",
	 U"abcdefg",
	 { {0, {'a', false}},
	   {1, {'b', true}},
	   {6, {'c', false}}
	 },
	 U"afedcbg",
	 { {0, {'a', false}},
	   {1, {'b', true}},
	   {6, {'c', false}}
	 },
	},
	{
	 "Two right-to-lefts, even chars",
	 U"abcdef",
	 { {0, {'a', false}},
	   {1, {'b', true}},
	   {3, {'c', true}},
	   {5, {'d', false}}
	 },
	 U"aedcbf",
	 { {0, {'a', false}},
	   {1, {'c', true}},
	   {3, {'b', true}},
	   {5, {'d', false}}
	 },
	},
	{
	 "Two right-to-lefts, odd chars",
	 U"abcdefg",
	 { {0, {'a', false}},
	   {1, {'b', true}},
	   {3, {'c', true}},
	   {6, {'d', false}}
	 },
	 U"afedcbg",
	 { {0, {'a', false}},
	   {1, {'c', true}},
	   {4, {'b', true}},
	   {6, {'d', false}}
	 },
	},
};

static void renderordertest()
{
	for (const auto &test:render_order_test_cases)
	{
		richtextstring rts{test.logical_s, test.logical_meta};

		richtextstring rts_orig=rts;

		rts.render_order();

		if (rts.get_string() != test.render_s ||
		    rts.get_meta() != test.render_meta)
			throw EXCEPTION("Test " << test.testname << " failed");

		richtextstring rts2{rts, 0, rts.get_string().size()};

		rts2.logical_order();

		if (rts2.get_string() != rts_orig.get_string() ||
		    rts2.get_meta() != rts_orig.get_meta())
			throw EXCEPTION("Test " << test.testname
					<< " (reset) failed");
	}
}

void kerningtest()
{
	static const struct {

		richtextstring test_string, prev, next;
		unicode_bidi_level_t paragraph_embedding_level;
		std::vector<int16_t> kernings;
	} tests[]={

		   // 0: basic test: left to right, previous line ends in a
		   // different font.

		   {{U"12345678",
			   {
			    {0, {0}},
			    {4, {1}},
			   }},

		    {U"1234567",
		     {
		      {0, {0}},
		      {4, {1}},
		     }},
		    {U"4567123",
		     {
		      {0, {0}},
		      {4, {1}},
		     }},
		    UNICODE_BIDI_LR,
		    {0, -1, -2, -3, 0, -5, -6, -7}
		   },

		   // 1: basic test: left to right, previous line ends in the
		   // same font.

		   {{U"12345678",
			   {
			    {0, {1}},
			    {4, {0}},
			   }},

		    {U"1234567",
		     {
		      {0, {0}},
		      {4, {1}},
		     }},
		    {U"4567123",
		     {
		      {0, {0}},
		      {4, {1}},
		     }},
		    UNICODE_BIDI_LR,
		    {-7, -1, -2, -3, 0, -5, -6, -7}
		   },

		   // 2: Right to left, next line is read for the leading
		   // character's kerning.
		   {{U"12345678",
		     {
		      {0, {0, 1}},
		      {4, {1, 1}},
		     }},
		    {U"1234567",
		     {
		      {0, {1, 1}},
		      {4, {0, 1}},
		     }},
		    {U"4567123",
		     {
		      {0, {1, 1}},
		      {4, {0, 1}},
		     }},
		    UNICODE_BIDI_LR,
		    {-3, -1, -2, -3, 0, -5, -6, -7},
		   },

		   // 3: Right to left: next line ends in a different font.

		   {{U"12345678",
		     {
		      {0, {0, 1}},
		      {4, {1, 1}},
		     }},
		    {U"1234567",
		     {
		      {0, {1}},
		      {4, {0}},
		     }},
		    {U"4567123",
		     {
		      {0, {0, 1}},
		      {4, {1, 1}},
		     }},
		    UNICODE_BIDI_LR,
		    {0, -1, -2, -3, 0, -5, -6, -7},
		   },

		   // 4: right to left, next line's leading right to left
		   // text ends in a different font.
		   {{U"12345678",
		     {
		      {0, {0, 1}},
		      {4, {1, 1}},
		     }},
		    {U"1234567",
		     {
		      {0, {1}},
		      {4, {0}},
		     }},
		    {U"4567123",
		     {
		      {0, {1, 1}},
		      {4, {0, 0}},
		     }},
		    UNICODE_BIDI_LR,
		    {0, -1, -2, -3, 0, -5, -6, -7},
		   },

		   // 5: right to left, next line's right to left text ends
		   // with a matching font.

		   {{U"12345678",
		     {
		      {0, {0, 1}},
		      {4, {1, 1}},
		     }},
		    {U"1234567",
		     {
		      {0, {1}},
		      {4, {0}},
		     }},
		    {U"4567123",
		     {
		      {0, {0, 1}},
		      {3, {0, 0}},
		     }},
		    UNICODE_BIDI_LR,
		    {-6, -1, -2, -3, 0, -5, -6, -7}
		   },

		   // 6: line is only partially right to left.

		   {{U"12345678",
		     {
		      {0, {0, 1}},
		      {4, {1, 0}},
		     }},
		    {U"1234567",
		     {
		      {0, {1}},
		      {4, {0}},
		     }},
		    {U"4567123",
		     {
		      {0, {0, 1}},
		      {3, {0, 0}},
		     }},
		    UNICODE_BIDI_LR,
		    {0, -1, -2, -3, 0, -5, -6, -7}
		   },

		   // Right to left paragraph embedding level tests


		   // 7: basic test: left to right, previous line ends in the
		   // same font, but it's not entirely left to right.

		   {{U"12345678",
			   {
			    {0, {0}},
			   }},

		    {U"1234567",
		     {
		      {0, {1, 1}},
		      {4, {0}},
		     }},
		    {U"4567123",
		     {
		      {0, {0}},
		     }},
		    UNICODE_BIDI_RL,
		    {0, -1, -2, -3, -4, -5, -6, -7}
		   },

		   // 8: previous line is entirely left to right, but this
		   // line isn't.

		   {{U"12345678",
			   {
			    {0, {0}},
			    {4, {0, 1}},
			   }},

		    {U"1234567",
		     {
		      {0, {0}},
		      {4, {0}},
		     }},
		    {U"4567123",
		     {
		      {0, {0}},
		     }},
		    UNICODE_BIDI_RL,
		    {0, -1, -2, -3, 0, -5, -6, -7}
		   },

		   // 9: previous line and this line are entirely left to
		   // right.

		   {{U"12345678",
			   {
			    {0, {0}},
			   }},

		    {U"1234567",
		     {
		      {0, {0}},
		     }},
		    {U"4567123",
		     {
		      {0, {0}},
		     }},
		    UNICODE_BIDI_RL,
		    {-7, -1, -2, -3, -4, -5, -6, -7}
		   },

		   // 10: right to left, different fonts.

		   {{U"12345678",
			   {
			    {0, {0, 1}},
			   }},

		    {U"1234567",
		     {
		      {0, {0, 1}},
		      {4, {0, 0}},
		     }},
		    {U"4567123",
		     {
		      {0, {0}},
		      {4, {1, 1}},
		     }},
		    UNICODE_BIDI_RL,
		    {0, -1, -2, -3, -4, -5, -6, -7}
		   },

		   // 11: right to left, same fonts

		   {{U"12345678",
			   {
			    {0, {0, 1}},
			   }},

		    {U"1234567",
		     {
		      {0, {0, 1}},
		      {4, {0, 0}},
		     }},
		    {U"4567123",
		     {
		      {0, {0}},
		      {4, {0, 1}},
		     }},
		    UNICODE_BIDI_RL,
		    {-3, -1, -2, -3, -4, -5, -6, -7}
		   },
	};

	int i=0;

	for (const auto &t:tests)
	{
		std::vector<dim_t> widths;
		std::vector<int16_t> kernings;

		auto test_string=t.test_string;

		test_string.compute_width(&t.prev, &t.next,
					  t.paragraph_embedding_level, 0,
					  widths, kernings);

		if (kernings != t.kernings)
			throw EXCEPTION("kerningtest " << i << " failed");
		++i;
	}
}

int main()
{
	try {
		inserttest();
		erasesubstrtest();
		renderordertest();
		kerningtest();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
