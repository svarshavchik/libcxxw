/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include <x/exception.H>
#include "x/w/namespace.H"
#include <iostream>

#include "mockrichtext.H"

using namespace LIBCXX_NAMESPACE::w;

struct fragment_metric {
	std::u32string string;
	dim_t initial_width_lr;
	dim_t initial_width_rl_trailing_lr;
	dim_t initial_width_rl;
	dim_t width;
	richtexthorizinfo_t horiz_info;

	bool operator==(const fragment_metric &) const=default;
};

std::ostream &operator<<(std::ostream &o, const std::vector<fragment_metric> &v)
{
	const char *p="";
	o << "{";
	p="";

	for (const auto &m:v)
	{
		o << p;
		p=",";
		o << std::endl << "\t";

		o << "{U\"" << std::string{m.string.begin(), m.string.end()}
		<< "\", " << m.initial_width_lr << ", "
		<< m.initial_width_rl_trailing_lr << ", "
		<< m.initial_width_rl << ", "
		<< m.width
		<< "," << std::endl << "\t{"
		<< std::endl << "\t\t{";

		for (size_t i=0; i < m.horiz_info.size(); i++)
		{
			o << (i == 0 ? "": ", ")
			  << m.horiz_info.width(i);
		}
		o << "}," << std::endl << "\t\t{";
		for (size_t i=0; i < m.horiz_info.size(); i++)
		{
			o << (i == 0 ? "": ", ")
			  << m.horiz_info.kerning(i);
		}
		o << "}" << std::endl
		  << "\t}}";
	}
	o << std::endl << "}";
	return o;
}

std::vector<fragment_metric>
get_metrics(const LIBCXX_NAMESPACE::ref<richtext_implObj> &impl)
{
	std::vector<fragment_metric> ret;

	impl->paragraphs.for_paragraphs
		(0,
		 [&]
		 (auto &p)
		 {
			 p->fragments.for_fragments
				 ([&]
				  (auto &f)
				  {
					  fragment_metric m
						  {
						   f->string.get_string(),
						   f->initial_width_lr,
						   f->initial_width_rl_trailing_lr,
						   f->initial_width_rl,
						   f->width,
						   f->horiz_info
						  };

					  ret.push_back(std::move(m));
				  });
			 return true;
		 });

	return ret;
}

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

void replacesubstrtest()
{
	static const struct {
		richtextstring string;
		richtextstring other;
		size_t  replace_pos;
		size_t  other_pos, other_size;
		richtextstring result;
	} tests[]={
		{
			// Test 1
			{U"123456",
			 {
				 {0, {'a'}},
			 }},
			{U"789",
			 {
				 {0, {'b'}},
			 }},
			0,
			0, 3,
			{U"789456",
			 {
				 {0, {'b'}},
				 {3, {'a'}},
			 }}

		},
		{
			// Test 2
			{U"123456",
			 {
				 {0, {'a'}},
			 }},
			{U"789",
			 {
				 {0, {'b'}},
			 }},
			1,
			0, 3,
			{U"178956",
			 {
				 {0, {'a'}},
				 {1, {'b'}},
				 {4, {'a'}},
			 }}

		},
		{
			// Test 3
			{U"123456",
			 {
				 {0, {'a'}},
			 }},
			{U"789",
			 {
				 {0, {'b'}},
			 }},
			3,
			0, 3,
			{U"123789",
			 {
				 {0, {'a'}},
				 {3, {'b'}},
			 }}

		},
		{
			// Test 4
			{U"123456",
			 {
				 {0, {'a'}},
				 {2, {'b'}},
				 {5, {'c'}},
			 }},
			{U"789",
			 {
				 {0, {'d'}},
			 }},
			2,
			0, 3,
			{U"127896",
			 {
				 {0, {'a'}},
				 {2, {'d'}},
				 {5, {'c'}},
			 }}

		},
		{
			// Test 5
			{U"123456",
			 {
				 {0, {'a'}},
			 }},
			{U"67890",
			 {
				 {0, {'x'}},
				 {1, {'b'}},
				 {4, {'y'}},
			 }},
			2,
			1, 3,
			{U"127896",
			 {
				 {0, {'a'}},
				 {2, {'b'}},
				 {5, {'a'}},
			 }}

		},
	};

	size_t i=0;

	for (const auto &t:tests)
	{
		++i;
		auto string=t.string;

		string.replace(t.replace_pos, t.other,
			       t.other_pos, t.other_size);

		if (string != t.result)
			throw EXCEPTION("replacesubstrtest #" << i <<
					" failed");
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

void rlmetricstest()
{
	richtextmeta lr{0}, rl{0};

	rl.rl=true;

	richtext_options options;

	options.paragraph_embedding_level=UNICODE_BIDI_RL;

	auto impl=LIBCXX_NAMESPACE::ref<richtext_implObj>::create
		(richtextstring{
				U" 87 65 43 2111 22 33 44",
				{
				 {0, rl},
				 {12, lr},
				},
		}, options);

	impl->finish_initialization();

	{
		paragraph_list my_paragraphs{*impl};

		auto p=*impl->paragraphs.get_paragraph(0);
		fragment_list my_fragments{my_paragraphs, *p};

		auto f=p->get_fragment(0);
		f->split(my_fragments, 12, f->split_rl, false);

		f=p->get_fragment(0);
		f->split(my_fragments, 6, f->split_lr, false);

		f=p->get_fragment(2);
		f->split(my_fragments, 6, f->split_rl, false);
	}

	if (auto m=get_metrics(impl); m != std::vector<fragment_metric>{
			{U"11 22 ", 46, 80, 46, 80,
				{
					{16, 16, 16, 16, 16, 16},
					{0, -1, -1, -10, -2, -2}
				}},
			{U"33 44", 42, 50, 32, 60,
				{
					{16, 16, 16, 16, 16},
					{-10, -3, -3, -10, -4}
				}},
			{U" 43 21", 36, 48, 48, 67,
				{
					{16, 16, 16, 16, 16, 16},
					{-5, -10, -4, -3, -10, -2}
				}},
			{U" 87 65", 32, 48, 48, 55,
				{
					{16, 16, 16, 16, 16, 16},
					{0, -10, -8, -7, -10, -6}
				}}
		})
	{
		std::cerr << "Unexpected rlmetricstest result:"
			  << std::endl
			  << m << std::endl;
		throw EXCEPTION("rlmetricstest: test 1 failed");
	}
}

void rewraptest()
{
	richtextmeta lr{0}, rl{0};

	rl.rl=true;

	const struct {
		richtextstring string;
		dim_t wrap_to_width;
		unicode_bidi_level_t lr;
		std::vector<fragment_metric> wrapped;
	} testcases[]=
		  {
		   // Test 1
		   {
		    {U"1234 567",
		     {
		      {0, lr}
		     }
		    },
		    50,
		    UNICODE_BIDI_LR,
		    {
		     {U"1234 ", 70, 70, 70, 70,
		      {
		       {16, 16, 16, 16, 16},
		       {0, -1, -2, -3, -4}
		      }},
		     {U"567", 37, 27, 27, 37,
		      {
		       {16, 16, 16},
		       {-10, -5, -6}
		      }}
		    },
		   },

		   // Test 2

		   {
		    {U"12 34 56 78",
		     {
		      {0, lr}
		     }
		    },
		    76,
		    UNICODE_BIDI_LR,
		    {
		     {U"12 34 ", 45, 76, 45, 76,
		      {
		       {16, 16, 16, 16, 16, 16},
		       {0, -1, -2, -10, -3, -4}
		      }},
		     {U"56 78", 37, 42, 27, 52,
		      {
		       {16, 16, 16, 16, 16},
		       {-10, -5, -6, -10, -7}
		      }}
		    }
		   },

		   // Test 3

		   {
		    {U"12 34 56 78",
		     {
		      {0, lr}
		     }
		    },
		    75,
		    UNICODE_BIDI_LR,
		    {
		     {U"12 ", 45, 45, 45, 45,
		      {
		       {16, 16, 16},
		       {0, -1, -2}
		      }},
		     {U"34 56 ", 41, 58, 31, 68,
		      {
		       {16, 16, 16, 16, 16, 16},
		       {-10, -3, -4, -10, -5, -6}
		      }},
		     {U"78", 25, 15, 15, 25,
		      {
		       {16, 16},
		       {-10, -7}
		      }}
		    }
		   },
		   // Test 4
		   {
		    {U" 43 2156 78",
		     {
		      {0, rl},
		      {6, lr},
		     }
		    },
		    90,
		    UNICODE_BIDI_LR,
		    {
		     {U" 43 21", 36, 48, 48, 67,
		      {
		       {16, 16, 16, 16, 16, 16},
		       {0, -10, -4, -3, -10, -2}
		      }},
		     {U"56 78", 37, 52, 37, 52,
		      {
		       {16, 16, 16, 16, 16},
		       {0, -5, -6, -10, -7}
		      }}
		    }
		   },
		   // Test 5
		   {
		    {U"11 22 22 33",
		     {
		      {0, lr},
		      {3, rl},
		      {8, lr},
		     }
		    },
		    85,
		    UNICODE_BIDI_LR,
		    {
		     {U"11 ", 46, 46, 46, 46,
		      {
		       {16, 16, 16},
		       {0, -1, -1}
		      }},
		     {U"22 22 ", 36, 16, 16, 80,
		      {
		       {16, 16, 16, 16, 16, 16},
		       {0, -2, -2, -10, -2, 0}
		      }},
		     {U"33", 29, 19, 19, 29,
		      {
		       {16, 16},
		       {-10, -3}
		      }}
		    }
		   },
		   // Test 6
		   {
		    {U"11 22 22 33",
		     {
		      {0, lr},
		      {3, rl},
		      {8, lr},
		     }
		    },
		    72,
		    UNICODE_BIDI_LR,
		    {
		     {U"11 ", 46, 46, 46, 46,
		      {
		       {16, 16, 16},
		       {0, -1, -1}
		      }},
		     {U"22 22", 36, 48, 48, 64,
		      {
		       {16, 16, 16, 16, 16},
		       {0, -2, -2, -10, -2}
		      }},
		     {U" 33", 16, 35, 16, 35,
		      {
		       {16, 16, 16},
		       {0, -10, -3}
		      }}
		    }
		   },
		   // Test 7
		   {
		    {U"11 11 11 33",
		     {
		      {0, rl},
		      {8, lr},
		     }
		    },
		    110,
		    UNICODE_BIDI_LR,
		    {
		     {U"11 11 11", 37, 48, 48, 103,
		      {
		       {16, 16, 16, 16, 16, 16, 16, 16},
		       {0, -1, -1, -10, -1, -1, -10, -1}
		      }},
		     {U" 33", 16, 35, 16, 35,
		      {
		       {16, 16, 16},
		       {0, -10, -3}
		      }}
		    }
		   },
		   // Test 8
		   {
		    {U"11 11 11 11",
		     {
		      {0, rl},
		     }
		    },
		    70,
		    UNICODE_BIDI_LR,
		    {
		     {U" 11", 37, 48, 48, 37,
		      {
		       {16, 16, 16},
		       {-1, -10, -1}
		      }},
		     {U" 11", 37, 48, 48, 37,
		      {
		       {16, 16, 16},
		       {-1, -10, -1}
		      }},
		     {U"11 11", 37, 48, 48, 67,
		      {
		       {16, 16, 16, 16, 16},
		       {0, -1, -1, -10, -1}
		      }}
		    }
		   },


		   // Test 9
		   {
		    {U"1234 567",
		     {
		      {0, lr}
		     }
		    },
		    50,
		    UNICODE_BIDI_RL,
		    {
		     {U"1234 ", 70, 70, 70, 70,
		      {
		       {16, 16, 16, 16, 16},
		       {0, -1, -2, -3, -4}
		      }},
		     {U"567", 37, 27, 27, 37,
		      {
		       {16, 16, 16},
		       {-10, -5, -6}
		      }}
		    },
		   },

		   // Test 10
		   {
		     {U"98 765 4321",
		     {
		      {0, rl}
		     }
		    },
		    96,
		    UNICODE_BIDI_RL,
		    {
		     	{U" 4321", 61, 80, 80, 61,
			 {
			  {16, 16, 16, 16, 16},
			  {-5, -10, -4, -3, -2}
			 }},
			{U"98 765", 41, 64, 64, 56,
			 {
			  {16, 16, 16, 16, 16, 16},
			  {0, -9, -8, -10, -7, -6}
			 }}
		    },
		   },

		   // Test 11
		   {
		    {U"98 765 4321",
		     {
		      {0, rl}
		     }
		    },
		    16,
		    UNICODE_BIDI_RL,
		    {
		     {U" 4321", 61, 80, 80, 61,
		      {
		       {16, 16, 16, 16, 16},
		       {-5, -10, -4, -3, -2}
		      }},
		     {U" 765", 41, 64, 64, 41,
		      {
		       {16, 16, 16, 16},
		       {-8, -10, -7, -6}
		      }},
		     {U"98", 23, 32, 32, 23,
		      {
		       {16, 16},
		       {0, -9}
		      }}
		    },
		   },
		   // Test 12
		   {
		    {U"98 765 4321",
		     {
		      {0, rl}
		     }
		    },
		    97,
		    UNICODE_BIDI_RL,
		    {
		     {U" 765 4321", 61, 80, 80, 97,
		      {
		       {16, 16, 16, 16, 16, 16, 16, 16, 16},
		       {-8, -10, -7, -6, -5, -10, -4, -3, -2}
		      }},
		     {U"98", 23, 32, 32, 23,
		      {
		       {16, 16},
		       {0, -9}
		      }}
		    },
		   },

		   // Test 13
		   {
		    {U" 1122 2211 ",
		     {
		      {0, rl},
		      {3, lr},
		      {8, rl},
		     }
		    },
		    90,
		    UNICODE_BIDI_RL,
		    {
		     {U"11 ", 16, 16, 16, 46,
		      {
		       {16, 16, 16},
		       {0, -1, -1}
		      }},
		     {U"22 22", 44, 64, 44, 64,
		      {
		       {16, 16, 16, 16, 16},
		       {0, -2, -2, -10, -2}
		      }},
		     {U" 11", 37, 48, 48, 37,
		      {
		       {16, 16, 16},
		       {0, -10, -1}
		      }}
		    },
		   },


		   // Test 14
		   {
		    {U"11 1122 22",
		     {
		      {0, rl},
		      {5, lr},
		     }
		    },
		    6,
		    UNICODE_BIDI_RL,
		    {
			    {U"22 ", 44, 44, 44, 44,
			     {
				     {16, 16, 16},
				     {0, -2, -2}
			     }},
			    {U"22", 30, 20, 20, 30,
			     {
				     {16, 16},
				     {-10, -2}
			     }},

			    {U" 11", 37, 48, 48, 37,
			     {
				     {16, 16, 16},
				     {-1, -10, -1}
			     }},
			    {U"11", 31, 32, 32, 31,
			     {
				     {16, 16},
				     {0, -1}
			     }}
		    },
		   },
		  };

	size_t casenum=0;

	for (const auto &t:testcases)
	{
		++casenum;

		richtext_options options;

		options.paragraph_embedding_level=t.lr;

		auto impl=LIBCXX_NAMESPACE::ref<richtext_implObj>::create
			((richtextstring)t.string, options);

		impl->finish_initialization();
		{
			impl->rewrap(t.wrap_to_width);
		}
		if (auto m=get_metrics(impl); m != t.wrapped)
		{
			std::cerr << "Test case " << casenum << " failed"
				  << std::endl
				  << "Expected:" << std::endl
				  << t.wrapped
				  << std::endl
				  << "Actual: " << std::endl
				  << m << std::endl;
			throw EXCEPTION("rewraptest failed");
		}
	}
}

void gettest()
{
	richtextmeta lr{0}, rl{0};

	rl.rl=true;

	const struct {
		richtextstring string;
		dim_t wrap_to_width;
		unicode_bidi_level_t lr;

		std::vector<std::u32string> fragments;

		std::vector<std::tuple<size_t, size_t, std::u32string>> tests;

	} testcases[]=
		{
			// Test 1
			{
				{U"11222 22 233",
				 {
					 {0, lr},
					 {2, rl},
					 {10, lr},
				 }
				},
				80,
				UNICODE_BIDI_LR,
				{
					U"11",
					U" 22 2",
					U"22233",
				},

				{
					{1, 11, U"1222 22 23"},
					{1, 8,  U"12 22 2"},
					{0, 2, U"1122 2"},
					{0, 6, U"11"},
					{5, 11, U"222 22 3"},
					{5, 9, U"22 22 "},
				},

			},
			// Test 2
			{
				{U"11222 22 233\n11222 22 233",
				 {
					 {0, lr},
					 {2, rl},
					 {10, lr},
					 {15, rl},
					 {23, lr},
				 }
				},
				88,
				UNICODE_BIDI_LR,
				{
					U"11",
					U" 22 2",
					U"22233\n",
					U"11",
					U" 22 2",
					U"22233",
				},
				{
					{11, 14, U"3\n1"},
				},
			},
			// Test 3

			{
				{U"11222 22 2\n222 22 233",
				 {
					 {0, lr},
					 {2, rl},
					 {10, lr},
					 {11, rl},
					 {19, lr},
				 }
				},
				80,
				UNICODE_BIDI_LR,
				{
					U"11",
					U" 22 2",
					U"222\n",
					U" 22 2",
					U"22233",
				},
				{
					{0, 20, U"11222 22 2\n222 22 23"},
					{6, 10, U"222 22 2"},
				},
			},
			// Test 4
			{
				{U"222 22 211\n33222 22 2",
				 {
					 {0, lr},
					 {8, rl},
					 {11, lr},
					 {13, rl},
				 }
				},
				80,
				UNICODE_BIDI_RL,
				{
					U"11",
					U"222 ",
					U"\n22 2",
					U" 22 2",
					U"33222",
				},
				{
					{0, 20, U"222 22 211\n3222 22 2"},
					{2, 10, U" 22 2"},
					{10, 20, U"\n3222 22 2"},
				},
			},
			// Test 5
			{
				{U"222 22 211\n33222 22 2",
				 {
					 {0, rl},
					 {8, lr},
					 {10, rl},
					 {11, lr},
					 {13, rl},
				 }
				},
				70,
				UNICODE_BIDI_RL,
				{
					U" 211",
					U" 22",
					U"\n222",
					U" 22 2",
					U"222",
					U"33"
				},
				{
					{0, 11, U"222 221\n"},
					{1, 11, U"222 2211\n"},
					{4, 13, U"222 22\n 2"},
				},
			},

			// Test 6
			{
				{U"1122 2 222\n222 22 2",
				 {
					 {0, lr},
					 {2, rl},
				 }
				},
				80,
				UNICODE_BIDI_RL,
				{
					U" 2 222",
					U"\n1122",
					U" 22 2",
					U"222",
				},
				{
					{0, 10, U"1122 2 222"},
					{0, 9, U"122 2 222"},
					{0, 11, U"1122 2 222\n"},
				},
			},
			// Test 7
			{
				{U"111 11 122\n222 22 2",
				 {
					 {0, lr},
					 {8, rl},
				 }
				},
				40,
				UNICODE_BIDI_RL,
				{
					U"22",
					U"111 ",
					U"11 ",
					U"\n1",
					U" 2",
					U" 22",
					U"222",
				},
				{
					{0, 10, U"111 11 122"},
					{0, 11, U"111 11 122\n"},
					{0, 13, U"111 11 122\n 2"},
					{0, 10, U"111 11 122"},
					{5, 10, U"111 11 1"},
					{4, 10, U"11 11 1"},
					{10,18, U"\n22 22 2"},
				},
			},
			// Test 8
			{
				{U"111 11 122",
				 {
					 {0, rl},
				 }
				},
				200,
				UNICODE_BIDI_RL,
				{
					U"111 11 122",
				},
				{
					{0, 3, U"122"},
				}
			},

			// Test 9
			{
				{U"2211 111 111",
				 {
					 {0, rl},
					 {2, lr},
				 }
				},
				100,
				UNICODE_BIDI_RL,
				{
					U"11 111 ",
					U"22111",
				},
				{
					{6, 11, U"211 111 111"},
					{6, 10, U"11 111 111"},
					{6, 9, U"11 111 11"},
				}
			},
			// Test 10
			{
				{U"221122 2222 222 22222",
				 {
					 {0, rl},
					 {2, lr},
					 {4, rl},
				 }
				},
				120,
				UNICODE_BIDI_RL,
				{
					U" 22222",
					U" 2222 222",
					U"221122",
				},
				{
					{0, 20, U"21122 2222 222 22222"},
					{0, 19, U"1122 2222 222 22222"},
					{0, 18, U"122 2222 222 22222"},
					{0, 17, U"22 2222 222 22222"},
					{0, 16, U"2 2222 222 22222"},
				}
			},
		};

	size_t casenum=0;

	for (const auto &t:testcases)
	{
		++casenum;

		richtext_options options;

		options.paragraph_embedding_level=t.lr;

		auto str=richtext::create((richtextstring)t.string,
					  options);

		str->rewrap(t.wrap_to_width);

		std::vector<std::u32string> actual_fragments;

		str->read_only_lock
			([&]
			 (auto &lock)
			{
				paragraph_list my_paragraphs{**lock};

				auto p=*(*lock)->paragraphs.get_paragraph(0);
				fragment_list my_fragments{my_paragraphs, *p};

				auto f=&*p->get_fragment(0);

				while (f)
				{
					actual_fragments.push_back
						(f->string.get_string());

					f=f->next_fragment();
				}
			});

		if (actual_fragments != t.fragments)
		{
			std::ostringstream o;

			o << "Expected lines:" << std::endl;

			for (auto &f:t.fragments)
			{
				o << "   \"" << std::string{f.begin(), f.end()}
					<< "\"" << std::endl;
			}
			o << "Actual lines:" << std::endl;
			for (auto &f:actual_fragments)
			{
				o << "   \"" << std::string{f.begin(), f.end()}
					<< "\"" << std::endl;
			}
			throw EXCEPTION("gettest #" << casenum << " failed:\n"
					<< o.str());
		}
		auto b=str->begin(), e=str->end();

		if (b->pos() != 0 && e->pos() != t.string.size()-1)
			throw EXCEPTION("gettext #" << casenum << "failed: "
					"unexpected begin/end positions.");

		size_t n=0;

		for (const auto &[start, end, result] : t.tests)
		{
			++n;

			b=b->pos(start);
			e=e->pos(end);

			auto res=b->get(e).get_string();

			if (res != result)
			{
				std::string res_s{res.begin(), res.end()};
				std::string result_s{result.begin(),
					result.end()};
				throw EXCEPTION("gettext #" << casenum
						<< " failed test "
						<< n << ": result is \""
						<< res_s
						<< "\" instead of \""
						<< result_s
						<< "\"");
			}
		}

		richtextstring unwrapped_string;

		str->read_only_lock
			([&]
			 (auto &lock)
			{
				(*lock)->unwrap();

				(*lock)->paragraphs.for_paragraphs
					(0,
					 [&]
					 (const auto &p)
					 {
						 auto s=p->get_fragment(0)
							 ->string;

						 if (s.size() > 1 &&
						     s.get_string()[0] == '\n'
						     &&
						     t.lr != UNICODE_BIDI_LR)
						 {
							 richtextstring
								 r{s,
								 1,
								 s.size()-1};

							 r += richtextstring
							 {s, 0, 1};

							 s=r;
						 }

						 unwrapped_string += s;
						 return true;
					 });
			});

		if (unwrapped_string != t.string)
				throw EXCEPTION("gettext #" << casenum
						<< " failed test "
						<< n
						<< ": did not unwrap back to "
						"the original string");

	}
}

int main()
{
	try {
		inserttest();
		erasesubstrtest();
		replacesubstrtest();
		kerningtest();
		rlmetricstest();
		rewraptest();
		gettest();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
