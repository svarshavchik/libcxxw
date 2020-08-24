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
		<< m.initial_width_rl
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

void rlmetricstest()
{
	richtextmeta lr, rl;

	rl.rl=true;

	auto impl=LIBCXX_NAMESPACE::ref<richtext_implObj>::create
		(richtextstring{
				U"12 34 56 78 11 22 33 44",
				{
				 {0, rl},
				 {12, lr},
				},
		},
		 halign::left,
		 UNICODE_BIDI_RL);

	impl->finish_initialization();

	{
		paragraph_list my_paragraphs{*impl};

		auto p=*impl->paragraphs.get_paragraph(0);
		fragment_list my_fragments{my_paragraphs, *p};

		auto f=p->get_fragment(0);
		f->split(my_fragments, 12, f->split_rl);

		f=p->get_fragment(0);
		f->split(my_fragments, 6, f->split_lr);

		f=p->get_fragment(2);
		f->split(my_fragments, 6, f->split_rl);
	}

	if (auto m=get_metrics(impl); m != std::vector<fragment_metric>{
			{U"11 22 ", 46, 74, 46,
				{
					{16, 16, 16, 16, 16, 16},
					{0, -1, -1, -16, -2, -2}
				}},
			{U"33 44", 42, 38, 26,
				{
					{16, 16, 16, 16, 16},
					{-16, -3, -3, -16, -4}
				}},
			{U" 43 21", 30, 48, 48,
				{
					{16, 16, 16, 16, 16, 16},
					{-5, -16, -4, -3, -16, -2}
				}},
			{U" 87 65", 26, 48, 48,
				{
					{16, 16, 16, 16, 16, 16},
					{0, -16, -8, -7, -16, -6}
				}}
		})
	{
		std::cerr << "Unexpected rlmetricstest result:"
			  << std::endl
			  << m << std::endl;
		throw EXCEPTION("rlmetricstest: test 1 failed");
	}
}

int main()
{
	try {
		inserttest();
		erasesubstrtest();
		renderordertest();
		kerningtest();
		rlmetricstest();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
