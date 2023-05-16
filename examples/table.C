/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/obj.H>
#include <x/appid.H>

#include <x/w/main_window.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/gridfactory.H>
#include <x/w/tablelayoutmanager.H>
#include <x/w/label.H>

#include "close_flag.H"

std::string x::appid() noexcept
{
	return "table.examples.w.libcxx.com";
}

struct mondata {
	std::string process;
	int cpu;
	int ram;
	int diskio;
	int netio;
};

static mondata processes[]=
	{
	 {"Compiler", 40, 12, 3, 0},
	 {"Idle", 35, 0, 0, 0},
	 {"Updater", 12, 4, 2, 2},
	 {"Editor", 4, 7, 0, 0},
	 {"Torrent Downloader", 5, 6, 4, 4},
	 {"Backup", 4, 5, 5, 0},
	};

auto create_process_table(const x::w::main_window &mw,
			  const x::w::gridfactory &f)
{
	x::w::new_tablelayoutmanager ntlm{

		x::w::table_headers(
			{
				{"Process", {}},
				{"CPU %", {}},
				{"RAM %", {}},
				{"Disk I/O (Mbps)", {}},
				{"Net I/O (Mbps)", {}}
			})
	};

	// The table layout manager inherits from the list layout manager.
	// Set the selection_type to no-selection_type if the table or the
	// list layout manager is not meant to be used for selecting
	// an element from the list, setting the selection_type to
	// no_selection_type
	ntlm.selection_type=x::w::no_selection_type;

	// No need to set columns, inherited from new_listlayoutmanager.
	// new_tablelayoutmanager's constructor initializes it from the number
	// of the header factories
	//
	// ntlm.columns = 5

	// The col_alignments and column_borders are inherited from
	// new_listlayoutmanager, and can be also set in a
	// new_listlayoutmanager, too.

	ntlm.col_alignments={
			    {0, x::w::halign::center},
			    {1, x::w::halign::right},
			    {2, x::w::halign::right},
			    {3, x::w::halign::right},
			    {4, x::w::halign::right},
	};

	// Borders before the corresponding column. Border #1 specifies the
	// border that separates columns 0 and 1.

	ntlm.column_borders={
			    {1, "thin_0%"},
			    {2, "thin_dashed_0%"},
			    {3, "thin_dashed_0%"},
			    {4, "thin_dashed_0%"},
	};

	auto c=f->create_focusable_container
		([&]
		 (const x::w::focusable_container &c)
		 {
			 x::w::tablelayoutmanager lm=c->tablelayout();

			 // The tablelayoutmanager inherits from the
			 // listlayoutmanager. Just like with selection lists,
			 // list_item_parms specify new items to be added to
			 // the list.
			 //
			 // Initialize the table.

			 std::vector<x::w::list_item_param> items;

			 items.reserve((std::end(processes)-
					std::begin(processes)) * 5);

			 for (const auto &p:processes)
			 {
				 std::ostringstream cpu, ram, diskio, netio;

				 cpu << p.cpu << "%";

				 ram << p.ram << "%";

				 diskio << p.diskio << " Mbps";

				 netio << p.netio << " Mbps";

				 std::string fields[5]={
							p.process,
							cpu.str(),
							ram.str(),
							diskio.str(),
							netio.str()
				 };

				 items.insert(items.end(),
					      std::begin(fields),
					      std::end(fields));
			 }
			 lm->append_items(items);
		 },
		 ntlm);

	return c;
}

void testlist()
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=x::w::main_window::create
		([&]
		 (const auto &main_window)
		 {
			 auto layout=main_window->gridlayout();
			 x::w::gridfactory factory=
				     layout->append_row();

			 create_process_table(main_window, factory);
			 main_window->set_window_title("Table");
		 });

	guard(main_window->connection_mcguffin());

	main_window->on_disconnect([]
				   {
					   exit(1);
				   });

	main_window->on_delete
		([close_flag]
		 (THREAD_CALLBACK,
		  const auto &ignore)
		 {
			 close_flag->close();
		 });

	main_window->show_all();

	close_flag->wait();
}

int main(int argc, char **argv)
{
	try {
		testlist();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
