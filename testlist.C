/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/obj.H>

#include "x/w/main_window.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/listlayoutmanager.H"

#include <vector>
#include <sstream>

using namespace LIBCXX_NAMESPACE::w;

std::vector<std::tuple<size_t, size_t, bool>> invocations;

void testlist2(const listlayoutmanager &tlm)
{
	auto callback_factory=[counter=0]
		()
		mutable
		{
			return [this_counter=counter++]
			(const auto &info)
			{
				invocations.emplace_back(this_counter,
							 info.item_number,
							 info.selected);
			};
		};

	// A (0)
	// B (1)
	// C (2)
	tlm->append_items({callback_factory(), "A",
				callback_factory(), "B",
				callback_factory(), "C"});

	// A (0) *
	// B (1) *
	// C (2) *
	// [0, 0, 1]
	// [1, 1, 1]
	// [2, 2, 1]
	tlm->selected(0, true, {});
	tlm->selected(1, true, {});
	tlm->selected(2, true, {});

	// NO-OP
	tlm->selected(0, true, {});
	tlm->selected(1, true, {});
	tlm->selected(2, true, {});


	// A (0)
	// B (1)
	// C (2)

	// [0, 0, 0]
	// [1, 1, 0]
	// [2, 2, 0]

	tlm->unselect();

	tlm->unselect();

	// D (3)
	// E (4)
	// C (2)
	tlm->replace_items(0,
			   {callback_factory(), "D",
					   callback_factory(), "E"});

	// D (3) *
	// E (4) *
	// C (2)

	// [3, 0, 1]
	// [4, 1, 1]

	tlm->selected(0, true, {});
	tlm->selected(1, true, {});

	// D (3) *
	// E (4) *
	// F (5)
	// C (2)
	tlm->insert_items(2, {
			callback_factory(), "F"});


	// D (3) *
	// E (4) *
	// F (5) *
	// C (2) *

	// [5, 2, 1]
	// [2, 3, 1]

	tlm->selected(2, true, {});
	tlm->selected(3, true, {});

	// E (4) *
	// F (5) *
	// C (2) *

	// [3, 0, 0]
	tlm->remove_item(0);


	// E (4) *
	// F (5) *
	// G (6)
	// H (7)

	// [2, 2, 0]

	tlm->replace_items(2, { callback_factory(), "G",
				callback_factory(), "H"
				});

	// E (4) *
	// F (5) *
	// G (6) *
	// H (7) *

	// [6, 2, 1]
	// [7, 3, 1]

	tlm->selected(2, true, {});
	tlm->selected(3, true, {});

	// I (8)
	// J (9)
	// K (10)

	// [4, 0, 0]
	// [5, 1, 0]
	// [6, 2, 0]
	// [7, 3, 0]

	tlm->replace_all_items({callback_factory(), "I",
				callback_factory(), "J",
				callback_factory(), "K"});

	// I (8) *
	// J (9) *
	// K (10) *

	// [8, 0, 1]
	// [9, 1, 1]
	// [10, 2, 1]
	tlm->selected(0, true, {});
	tlm->selected(1, true, {});
	tlm->selected(2, true, {});

	std::ostringstream o;

	for (const auto &t:invocations)
		o << "[" << std::get<0>(t)
		  << ", " << std::get<1>(t)
		  << ", " << std::get<2>(t)
		  << "]\n";

	auto s=o.str();

	if (s !=
	    "[0, 0, 1]\n"
	    "[1, 1, 1]\n"
	    "[2, 2, 1]\n"
	    "[0, 0, 0]\n"
	    "[1, 1, 0]\n"
	    "[2, 2, 0]\n"
	    "[3, 0, 1]\n"
	    "[4, 1, 1]\n"
	    "[5, 2, 1]\n"
	    "[2, 3, 1]\n"
	    "[3, 0, 0]\n"
	    "[2, 2, 0]\n"
	    "[6, 2, 1]\n"
	    "[7, 3, 1]\n"
	    "[4, 0, 0]\n"
	    "[5, 1, 0]\n"
	    "[6, 2, 0]\n"
	    "[7, 3, 0]\n"
	    "[8, 0, 1]\n"
	    "[9, 1, 1]\n"
	    "[10, 2, 1]\n")
		throw EXCEPTION("Unexpected results");
}

void testlist()
{
	auto main_window=main_window
		::create([]
			 (const auto &main_window)
			 {
				 gridlayoutmanager
				     layout=main_window->get_layoutmanager();
				 gridfactory factory=
				     layout->append_row();

				 new_listlayoutmanager nlm{highlighted_list};

				 factory->create_focusable_container
				 ([]
				  (const auto &l) {
					 testlist2(l->get_layoutmanager());
				 },
				  nlm);
			 });
}

int main(int argc, char **argv)
{
	try {
		testlist();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
