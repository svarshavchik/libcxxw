#include "libcxxw_config.h"
#include <x/property_properties.H>
#include <iostream>
#include <string>
#include <algorithm>

#include <x/ymd.H>

using namespace LIBCXX_NAMESPACE;

void testfilter(const std::u32string &date_format,
		const std::u32string &initial_contents,
		size_t starting_pos,
		size_t n_delete,
		const std::u32string &str,
		size_t &result_starting_pos,
		size_t &result_n_delete,
		std::u32string &result)
{
	const size_t size=initial_contents.size();

#define CURRENT_INPUT_FIELD_CONTENTS() (initial_contents)

#include "date_input_field/date_input_field_filter.H"

	result=new_string;
	result_starting_pos=starting_pos;
	result_n_delete=n_delete;
}

void runtests()
{
	auto y=ymd{}.get_year();

	char32_t y2cslash[8]={'1','.','1','.',
			      (char32_t)( '0' + ((y / 10) % 10)),
			      (char32_t)('0' + (y % 10)), '/', 0};

	auto os="01.01."+std::to_string(y);

	std::u32string uos{os.begin(), os.end()};

	const struct {
		const char32_t *date_format;
		const char32_t *initial_contents;
		size_t starting_pos;
		size_t n_delete;
		const char32_t *str;
		size_t result_starting_pos;
		size_t result_n_delete;
		const char32_t *result;
	} testcases[]=
		  {
		   {
		    U"mm/dd/YYYY", U"", 0, 0, U"1",
		    0, 0, U"1"
		   },

		   {U"mm/dd/YYYY", U"", 0, 0, U"12",
		    0, 0, U"12/"
		   },

		   {U"mm/dd/YYYY", U"12", 0, 1, U"12",
		    (size_t)-1, 0, U""
		   },

		   {U"mm/dd/YYYY", U"12", 0, 2, U"01",
		    0, 2, U"01/"
		   },

		   {U"mm/dd/YYYY", U"12/", 0, 3, U"01",
		    0, 3, U"01/"
		   },

		   // 6

		   {U"mm/dd/YYYY", U"", 0, 0, U"1/",
		    0, 0, U"01/"
		   },

		   {U"mm/dd/YYYY", U"01/02/", 5, 1, U"",
		    4, 2, U""
		   },

		   {U"mm/dd/YYYY", U"01/0", 3, 1, U"",
		    3, 1, U""
		   },

		   {U"mm/dd/YYYY", U"", 0, 0, U"1/2/",
		    0, 0, U"01/02/"
		   },

		   {U"mm.dd.YYYY", U"", 0, 0, y2cslash,
		    0, 0, uos.c_str(),
		   },

		   // 11

		   {U"mm/dd/YYYY", U"01/", 3, 0, U"/",
		    3, 0, U""
		   },

		  };

	int counter=0;

	for (const auto &t:testcases)
	{
		++counter;

		size_t starting_pos=(size_t)-1;
		size_t n_delete=0;
		std::u32string result;

		testfilter(t.date_format, t.initial_contents,
			   t.starting_pos,
			   t.n_delete,
			   t.str, starting_pos, n_delete, result);

		if (result != t.result || starting_pos != t.result_starting_pos
		    || n_delete != t.result_n_delete)
		{
			std::cerr << "Test " << counter << " failed"
				  << std::endl;
			exit(1);
		}
	}
}

int main()
{
	x::property::load_property("x::w::themes", "./themes", true, false);
	locale::create("en_US.UTF-8")->global();

	runtests();
	exit(0);
}
