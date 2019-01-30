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
	std::ostringstream y4os;

	auto y=ymd{}.get_year();

	y4os << y;

	std::string y4s=y4os.str();
	std::u32string uy4s = U"01.01." +
		std::u32string{y4s.begin(), y4s.end()};

	char32_t y2cslash[8]={'1','.','1','.',
			      (char32_t)( '0' + ((y / 10) % 10)),
			      (char32_t)('0' + (y % 10)), '/', 0};

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
		    0, 0, uy4s.c_str()
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
	locale::create("en_US.UTF-8")->global();

	runtests();
	exit(0);
}
