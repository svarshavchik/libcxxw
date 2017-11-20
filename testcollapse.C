#include "libcxxw_config.h"
#include <iostream>
#include <sstream>
#include <x/exception.H>
#include <x/ref.H>
#include <x/property_properties.H>
#include "calculate_borders.H"
#include <vector>
#include <map>
#include <tuple>

#include "metrics_grid_axisrange.C"
#include "metrics_grid_pos.C"

using namespace LIBCXX_NAMESPACE::w;

struct my_cell_data {
	int grid_element;
	dim_t width, height;

	metrics::grid_pos pos;

	my_cell_data(int grid_element,
		     dim_t width,
		     dim_t height)
		: grid_element(grid_element),
		  width(width),
		  height(height),
		  pos(metrics::grid_pos::create()) {}
};

struct my_cell_obj : public my_cell_data, virtual public LIBCXX_NAMESPACE::obj
{
	my_cell_obj(const struct my_cell_data &d)
		: my_cell_data(d) {}

};

typedef LIBCXX_NAMESPACE::ref<my_cell_obj> my_cell;

void test1()
{
	//!   +----+----+----+----+----+----+
	//!   |  1 |  2 |    |    |  5 |  6 |
	//!   +----+----+    |    +----+----+
	//!   |  7 |  8 |    |    |  9 | 10 |
	//!   +----+----+    |    +----+----+
	//!   |   11    |    |    |   12    |
	//!   +----+----+  3 |  4 +----+----+
	//!   |   13    |    |    |   14    |
	//!   +----+----+    |    +----+----+
	//!   | 15 | 16 |    |    | 17 | 18 |
	//!   +----+----+    |    +----+----+
	//!   | 19 | 20 |    |    | 21 | 22 |
	//!   +----+----+----+----+----+----+

	std::vector<std::vector<my_cell_data>> grid_data={
		{ {1, {1}, {1}}, {2, {1}, {1}},
		  {3, {1}, {6}}, {4, {1}, {6}}, {5, {1}, {1}}, {6, {1}, {1}}},
		{ {7, {1}, {1}}, {8, {1}, {1}},
		  {9, {1}, {1}}, {10, {1}, {1}}},
		{ {11, {2}, {1}}, {12, {2}, {1}}},
		{ {13, {2}, {1}}, {14, {2}, {1}}},
		{ {15, {1}, {1}}, {16, {1}, {1}},
		  {17, {1}, {1}}, {18, {1}, {1}}},
		{ {19, {1}, {1}}, {20, {1}, {1}},
		  {21, {1}, {1}}, {22, {1}, {1}}},
	};

	std::vector<std::vector<my_cell>> grid;

	for (const auto &r:grid_data)
	{
		grid.push_back(std::vector<my_cell>());
		auto &rr=*--grid.end();

		for (const auto &c:r)
			rr.push_back(my_cell::create(c));
	}

	std::ostringstream h;

	std::multimap< std::tuple<metrics::grid_xy,
				  metrics::grid_xy>, std::string> v;

	calculate_borders(grid,
			  // v_lambda()
			  [&]
			  (my_cell *left,
			   my_cell *right,
			   metrics::grid_xy column_number,
			   metrics::grid_xy row_start,
			   metrics::grid_xy row_end)
			  {
				  std::ostringstream o;

				  if (left)
					  o << (*left)->grid_element;
				  o << "-";
				  if (right)
					  o << (*right)->grid_element;

				  o << ": c=" << column_number
				    << ", r1=" << row_start
				    << ", r2=" << row_end << "\n";

				  v.insert({ {row_start, column_number},
						  { o.str() }});
			  },
			  // h_lambda
			  [&]
			  (my_cell *above,
			   my_cell *below,
			   metrics::grid_xy row_number,
			   metrics::grid_xy col1,
			   metrics::grid_xy col2)
			  {
				  if (above)
					  h << (*above)->grid_element;
				  h << "-";
				  if (below)
					  h << (*below)->grid_element;
				  h << ": r=" << row_number
				    << ", c=" << col1
				    << "-" << col2
				    << "\n";
			  });

	{
		std::ostringstream o;

		for (const auto &row:grid)
		{
			const char *sep="{";

			for (const auto &col:row)
			{
				const auto &h=col->pos->horiz_pos;
				const auto &v=col->pos->vert_pos;
				o << sep << "[" << col->grid_element << ": x="
				  << h.start << ", y="
				  << v.start << ", w="
				  << h.end-h.start+1 << ", h="
				  << v.end-v.start+1 << "]";
				sep=", ";
			}
			o << "}\n";
		}

		auto s=o.str();

		if (s !=
		    "{[1: x=1, y=1, w=1, h=1], [2: x=3, y=1, w=1, h=1], [3: x=5, y=1, w=1, h=11], [4: x=7, y=1, w=1, h=11], [5: x=9, y=1, w=1, h=1], [6: x=11, y=1, w=1, h=1]}\n"
		    "{[7: x=1, y=3, w=1, h=1], [8: x=3, y=3, w=1, h=1], [9: x=9, y=3, w=1, h=1], [10: x=11, y=3, w=1, h=1]}\n"
		    "{[11: x=1, y=5, w=3, h=1], [12: x=9, y=5, w=3, h=1]}\n"
		    "{[13: x=1, y=7, w=3, h=1], [14: x=9, y=7, w=3, h=1]}\n"
		    "{[15: x=1, y=9, w=1, h=1], [16: x=3, y=9, w=1, h=1], [17: x=9, y=9, w=1, h=1], [18: x=11, y=9, w=1, h=1]}\n"
		    "{[19: x=1, y=11, w=1, h=1], [20: x=3, y=11, w=1, h=1], [21: x=9, y=11, w=1, h=1], [22: x=11, y=11, w=1, h=1]}\n")
			throw EXCEPTION("Collapsed layout #1 is wrong:\n" <<
					o.str());
	}

	std::ostringstream ov;

	for (const auto &vv:v)
		ov << vv.second;

	if (ov.str() !=
	    "-1: c=0, r1=1, r2=1\n"
	    "1-2: c=2, r1=1, r2=1\n"
	    "2-3: c=4, r1=1, r2=1\n"
	    "3-4: c=6, r1=1, r2=11\n"
	    "4-5: c=8, r1=1, r2=1\n"
	    "5-6: c=10, r1=1, r2=1\n"
	    "6-: c=12, r1=1, r2=1\n"
	    "-7: c=0, r1=3, r2=3\n"
	    "7-8: c=2, r1=3, r2=3\n"
	    "8-3: c=4, r1=3, r2=3\n"
	    "4-9: c=8, r1=3, r2=3\n"
	    "9-10: c=10, r1=3, r2=3\n"
	    "10-: c=12, r1=3, r2=3\n"
	    "-11: c=0, r1=5, r2=5\n"
	    "11-3: c=4, r1=5, r2=5\n"
	    "4-12: c=8, r1=5, r2=5\n"
	    "12-: c=12, r1=5, r2=5\n"
	    "-13: c=0, r1=7, r2=7\n"
	    "13-3: c=4, r1=7, r2=7\n"
	    "4-14: c=8, r1=7, r2=7\n"
	    "14-: c=12, r1=7, r2=7\n"
	    "-15: c=0, r1=9, r2=9\n"
	    "15-16: c=2, r1=9, r2=9\n"
	    "16-3: c=4, r1=9, r2=9\n"
	    "4-17: c=8, r1=9, r2=9\n"
	    "17-18: c=10, r1=9, r2=9\n"
	    "18-: c=12, r1=9, r2=9\n"
	    "-19: c=0, r1=11, r2=11\n"
	    "19-20: c=2, r1=11, r2=11\n"
	    "20-3: c=4, r1=11, r2=11\n"
	    "4-21: c=8, r1=11, r2=11\n"
	    "21-22: c=10, r1=11, r2=11\n"
	    "22-: c=12, r1=11, r2=11\n")
		throw EXCEPTION("Vertical borders #1 are wrong:\n"
				<< ov.str());

	if (h.str() !=
	    "-1: r=0, c=1-1\n"
	    "-2: r=0, c=3-3\n"
	    "-3: r=0, c=5-5\n"
	    "-4: r=0, c=7-7\n"
	    "-5: r=0, c=9-9\n"
	    "-6: r=0, c=11-11\n"
	    "1-7: r=2, c=1-1\n"
	    "2-8: r=2, c=3-3\n"
	    "5-9: r=2, c=9-9\n"
	    "6-10: r=2, c=11-11\n"
	    "7-11: r=4, c=1-1\n"
	    "8-11: r=4, c=3-3\n"
	    "9-12: r=4, c=9-9\n"
	    "10-12: r=4, c=11-11\n"
	    "11-13: r=6, c=1-3\n"
	    "12-14: r=6, c=9-11\n"
	    "13-15: r=8, c=1-1\n"
	    "13-16: r=8, c=3-3\n"
	    "14-17: r=8, c=9-9\n"
	    "14-18: r=8, c=11-11\n"
	    "15-19: r=10, c=1-1\n"
	    "16-20: r=10, c=3-3\n"
	    "17-21: r=10, c=9-9\n"
	    "18-22: r=10, c=11-11\n"
	    "19-: r=12, c=1-1\n"
	    "20-: r=12, c=3-3\n"
	    "3-: r=12, c=5-5\n"
	    "4-: r=12, c=7-7\n"
	    "21-: r=12, c=9-9\n"
	    "22-: r=12, c=11-11\n")
		throw EXCEPTION("Horizontal borders #1 are wrong:\n"
				<< h.str());
}

int main()
{
	try {
		LIBCXX_NAMESPACE::property
			::load_property(LIBCXX_NAMESPACE_STR "::themes",
					"themes", true, true);
		test1();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
}
