/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/obj.H>
#include <x/config.H>

#include <x/w/main_window.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/gridfactory.H>
#include <x/w/tablelayoutmanager.H>
#include <x/w/label.H>
#include <x/w/screen_positions.H>

#include "close_flag.H"

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

// My application data, attached to the main window

class my_appdataObj : virtual public x::obj {

public:
	// The adjustable table container.

	const x::w::focusable_container main_table;

	my_appdataObj(const x::w::focusable_container &main_table)
		: main_table{main_table}
	{
	}
};

typedef x::ref<my_appdataObj> my_appdata;

auto create_process_table(const x::w::main_window &mw,
			  const x::w::gridfactory &f,
			  const x::w::screen_positions &saved_positions)
{
	x::w::new_tablelayoutmanager
		ntlm{[]
		     (const x::w::factory &f, size_t i)
		     {
			     static const char * const titles[]=
				     {
				      "Process",
				      "CPU %",
				      "RAM %",
				      "Disk I/O (Mbps)",
				      "Net I/O (Mbps)",
				     };

			     f->create_label(titles[i])->show();
		     }};


	ntlm.selection_type=x::w::no_selection_type;
	ntlm.columns=5;

	ntlm.col_alignments={
			    {0, x::w::halign::center},
			    {1, x::w::halign::right},
			    {2, x::w::halign::right},
			    {3, x::w::halign::right},
			    {4, x::w::halign::right},
	};

	ntlm.column_borders={
			    {1, "thin_0%"},
			    {2, "thin_dashed_0%"},
			    {3, "thin_dashed_0%"},
			    {4, "thin_dashed_0%"},
	};

	// Enable adjustable columns.
	ntlm.adjustable_column_widths=true;

	// Initial table width, in millimeters
	ntlm.table_width=200;

	// Maximum table width, in millimeters.
	ntlm.maximum_table_width=500;

	// Restore this table's prior column widths.

	ntlm.restore(saved_positions, "main_table");

	auto c=f->create_focusable_container
		([&]
		 (const x::w::focusable_container &c)
		 {
			 x::w::tablelayoutmanager lm=
				 c->get_layoutmanager();

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

	// My configuration file.

	auto configfile=
		x::configdir("table2@examples.w.libcxx.com") + "/windows";

	x::w::screen_positions pos{configfile};

	x::w::main_window_config config;

	config.screen_position(pos, "main");

	auto main_window=x::w::main_window::create
		(config,
		 [&]
		 (const auto &main_window)
		 {
			 x::w::gridlayoutmanager
				 layout=main_window->get_layoutmanager();
			 x::w::gridfactory factory=
				     layout->append_row();

			 auto c=create_process_table(main_window, factory,
						     pos);
			 main_window->set_window_title("Table");

			 // Use main_window's appdata member to store
			 // application-specific data. In our case we'll
			 // use it to store the table container.

			 main_window->appdata=my_appdata::create(c);
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

	// Save the final position and size of the main window.

	main_window->save(pos);

	// Save the final adjusted column widths.

	my_appdata appdata=main_window->appdata;

	x::w::tablelayoutmanager tlm=appdata->main_table->get_layoutmanager();

	tlm->save("main_table", pos);

	pos.save(configfile);
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
