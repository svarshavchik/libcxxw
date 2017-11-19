/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include <x/exception.H>
#include "x/w/namespace.H"
#include <iostream>

#define richtextmeta_H

LIBCXXW_NAMESPACE_START

class richtextmeta {

public:

	char c;

	bool operator==(const richtextmeta &o) const
	{
		return c == o.c;
	}

	bool operator!=(const richtextmeta &o) const { return !operator==(o); }
};
LIBCXXW_NAMESPACE_END

#include "richtext/richtextstring.C"
#include "assert_or_throw.C"

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

int main()
{
	try {
		inserttest();
		erasesubstrtest();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
