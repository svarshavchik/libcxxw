/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/obj.H>

#include "x/w/main_window.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/textlistlayoutmanager.H"

#include <vector>
#include <sstream>

using namespace LIBCXX_NAMESPACE::w;

std::vector<std::tuple<size_t, size_t, bool>> invocations;

void testlist2(const textlistlayoutmanager &tlm)
{
	tlm->callback_factory([counter=0]
			      ()
			      mutable
			      {
				      return [this_counter=counter++]
					      (list_lock &lock,
					       size_t i,
					       bool selected)
				      {
					      invocations.emplace_back
						      (this_counter, i,
						       selected);
				      };
			      });

	// [0, 0, 1]
	// [1, 1, 1]
	// [2, 2, 1]
	// [0, 0, 0]
	// [1, 1, 0]
	// [2, 2, 0]

	tlm->append_item("A", "B", "C");
	tlm->selected(0, true, {});
	tlm->selected(1, true, {});
	tlm->selected(2, true, {});
	tlm->selected(0, true, {});
	tlm->selected(1, true, {});
	tlm->selected(2, true, {});

	tlm->unselect();
	tlm->unselect();

	// [3, 0, 1]
	// [4, 1, 1]
	// [5, 2, 1]
	// [2, 3, 1]

	tlm->replace_item(0, "D", "E");
	tlm->selected(0, true, {});
	tlm->selected(1, true, {});

	tlm->insert_item(2, "F");
	tlm->selected(2, true, {});
	tlm->selected(3, true, {});

	// [6, 2, 1]
	// [7, 3, 1]

	tlm->remove_item(0);
	tlm->replace_item(2, "G", "H");
	tlm->selected(2, true, {});
	tlm->selected(3, true, {});

	// [8, 0, 1]
	// [9, 1, 1]
	// [10, 2, 1]
	tlm->replace_all_items({"I", "J", "K"});
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
	    "[6, 2, 1]\n"
	    "[7, 3, 1]\n"
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

				 new_listlayoutmanager nlm{text_list};

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
