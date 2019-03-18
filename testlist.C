/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/obj.H>
#include <x/mpobj.H>
#include <x/config.H>

#include "x/w/main_window.H"
#include "x/w/screen_positions.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/tablelayoutmanager.H"
#include "x/w/label.H"
#include "x/w/button.H"
#include "x/w/canvas.H"
#include "x/w/screen.H"
#include "x/w/image_param_literals.H"
#include "x/w/connection.H"

#include <vector>
#include <sstream>

class close_flagObj : public LIBCXX_NAMESPACE::obj {

public:
	LIBCXX_NAMESPACE::mpcobj<bool> flag;

	close_flagObj() : flag{false} {}
	~close_flagObj()=default;

	void close()
	{
		LIBCXX_NAMESPACE::mpcobj<bool>::lock lock{flag};

		*lock=true;
		lock.notify_all();
	}
};

typedef LIBCXX_NAMESPACE::ref<close_flagObj> close_flag_ref;

typedef LIBCXX_NAMESPACE::mpobj<std::vector<std::tuple<size_t, size_t, bool>>
				> invocations_t;

invocations_t invocations;

class flagObj : virtual public LIBCXX_NAMESPACE::obj {

public:

	typedef LIBCXX_NAMESPACE::mpcobj<bool> value_t;

	value_t value=false;

	void signal()
	{
		value_t::lock lock{value};

		*lock=true;
		lock.notify_all();
	}

	void wait()
	{
		value_t::lock lock{value};

		lock.wait([&] { return *lock; });
	}
};

void flush(const LIBCXX_NAMESPACE::w::element &e)
{
	auto flag=LIBCXX_NAMESPACE::ref<flagObj>::create();

	e->in_thread_idle([flag]
			  (THREAD_CALLBACK)
			  {
				  flag->signal();
			  });

	flag->wait();
}

void testlist2(const LIBCXX_NAMESPACE::w::listlayoutmanager &tlm)
{
	auto callback_factory=[counter=0]
		()
		mutable
		{
			return [this_counter=counter++]
			(THREAD_CALLBACK,
			 const auto &info)
			{
				invocations_t::lock lock{invocations};

				lock->emplace_back(this_counter,
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

	tlm->selected(0, true);
	tlm->selected(1, true);
	tlm->selected(2, true);

	// NO-OP
	tlm->selected(0, true);
	tlm->selected(1, true);
	tlm->selected(2, true);


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

	tlm->selected(0, true);
	tlm->selected(1, true);

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

	tlm->selected(2, true);
	tlm->selected(3, true);

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

	tlm->selected(2, true);
	tlm->selected(3, true);

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
	tlm->selected(0, true);
	tlm->selected(1, true);
	tlm->selected(2, true);
}

void testlist3(const LIBCXX_NAMESPACE::w::element &e)
{
	flush(e);
	invocations_t::lock lock{invocations};

	std::ostringstream o;

	for (const auto &t:*lock)
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

void testlist1()
{
	LIBCXX_NAMESPACE::w::containerptr c;

	auto main_window=LIBCXX_NAMESPACE::w::main_window
		::create([&]
			 (const auto &main_window)
			 {
				 LIBCXX_NAMESPACE::w::gridlayoutmanager
				     layout=main_window->get_layoutmanager();
				 LIBCXX_NAMESPACE::w::gridfactory factory=
				     layout->append_row();

				 LIBCXX_NAMESPACE::w::new_listlayoutmanager
					 nlm{LIBCXX_NAMESPACE::w
					     ::highlighted_list};

				 c=factory->create_focusable_container
				 ([]
				  (const auto &c) {
					 testlist2(c->get_layoutmanager());
				 }, nlm);
			 });

	testlist3(c);
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

#include "testlistoptions.H"

auto create_process_table(const LIBCXX_NAMESPACE::w::main_window &mw,
			  const LIBCXX_NAMESPACE::w::screen_positions &pos,
			  const LIBCXX_NAMESPACE::w::gridfactory &f,
			  const testlistoptions &options)
{
	LIBCXX_NAMESPACE::w::new_tablelayoutmanager
		ntlm{[]
		    (const LIBCXX_NAMESPACE::w::factory &f, size_t i)
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

	ntlm.restore(pos, "list");
	ntlm.selection_type=LIBCXX_NAMESPACE::w::no_selection_type;
	ntlm.columns=5;

	ntlm.requested_col_widths={{0, 100}};
	ntlm.col_alignments={
			    {0, LIBCXX_NAMESPACE::w::halign::center},
			    {1, LIBCXX_NAMESPACE::w::halign::right},
			    {2, LIBCXX_NAMESPACE::w::halign::right},
			    {3, LIBCXX_NAMESPACE::w::halign::right},
			    {4, LIBCXX_NAMESPACE::w::halign::right},
	};

	ntlm.column_borders={
			    {1, "thin_0%"},
			    {2, "thin_dashed_0%"},
			    {3, "thin_dashed_0%"},
			    {4, "thin_dashed_0%"},
	};

	ntlm.adjustable_column_widths=options.adjustable->value;

	if (options.width->isSet())
		ntlm.table_width=ntlm.maximum_table_width=options.width->value;

	if (options.maximum_table_width->isSet())
		ntlm.maximum_table_width=options.maximum_table_width->value;

	if (options.minwidth->isSet())
		ntlm.minimum_column_widths=
			{
			 {1, 30},
			 {2, 30},
			 {3, 30},
			 {4, 30}
			};

	auto c=f->create_focusable_container
		([&]
		 (const LIBCXX_NAMESPACE::w::focusable_container &c)
		 {
			 LIBCXX_NAMESPACE::w::listlayoutmanager lm=
				 c->get_layoutmanager();

			 std::vector<LIBCXX_NAMESPACE::w::list_item_param>
				 items;

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

	mw->appdata=c;

	return c;
}

void listtest(const LIBCXX_NAMESPACE::w::screen &default_screen,
	      const LIBCXX_NAMESPACE::w::main_window &main_window,
	      const LIBCXX_NAMESPACE::w::screen_positions &pos,
	      const testlistoptions &options)
{
	auto [original_theme, original_scale, original_options]
		=default_screen->get_connection()->current_theme();

	LIBCXX_NAMESPACE::w::gridlayoutmanager
		layout=main_window->get_layoutmanager();
	LIBCXX_NAMESPACE::w::gridfactory factory=layout->append_row();

	factory->halign(LIBCXX_NAMESPACE::w::halign::fill);
	factory->colspan(2);
	auto l=create_process_table(main_window, pos, factory, options);

	factory=layout->append_row();

	auto b=factory->create_button("Bigger/Smaller");

	b->on_activate
		([=]
		 (THREAD_CALLBACK,
		  const auto &trigger,
		  const auto &busy)
		 mutable
		 {
			 original_scale=original_scale == 100 ? 200:100;

			 default_screen->get_connection()
				 ->set_theme(original_theme,
					     original_scale,
					     original_options,
					     true);
		 });
	factory->create_canvas();
	factory=layout->append_row();

	b=factory->create_button("Reorder");

	b->on_activate
		([l]
		 (ONLY IN_THREAD,
		  const auto &trigger,
		  const auto &busy)
		 {
			 LIBCXX_NAMESPACE::w::listlayoutmanager
				 lm=l->get_layoutmanager();

			 size_t n=lm->size();

			 std::vector<size_t> i;

			 i.resize(n);

			 std::generate(i.begin(), i.end(),
				       [n=0]
				       ()
				       mutable
				       {
					       return n++;
				       });

			 std::random_shuffle(i.begin(), i.end());
			 lm->resort_items(IN_THREAD, i);
		 });
	factory->create_canvas();

	factory=layout->append_row();
	b=factory->create_button("Swap Header");

	b->on_activate
		([l, flag=false]
		 (ONLY IN_THREAD,
		  const auto &trigger,
		  const auto &busy)
		 mutable
		 {
			 LIBCXX_NAMESPACE::w
				 ::tablelayoutmanager
				 lm=l->get_layoutmanager();

			 (void)lm->header(1);

			 lm->replace_header(1)
				 ->create_label
				 (flag ? "CPU":"CPU %")
				 ->show();

			 flag=!flag;
		 });
	factory->create_canvas();
}

static const char * const lorem_ipsum[]=
	{
	 "Lorem ipsum",
	 "dolor sit amet",
	 "consectetur",
	 "adipisicing elit",
	 "sed do eiusmod",
	 "tempor incididunt",
	 "ut labore",
	 "et dolore magna"
	 "aliqua"
	};

static std::string next_lorem_ipsum()
{
	static size_t counter=0;

	size_t i=counter;

	if (++i > sizeof(lorem_ipsum)/sizeof(lorem_ipsum)[0])
		i=0;

	counter=i;

	return lorem_ipsum[i];
}

void listhiertest(const LIBCXX_NAMESPACE::w::main_window &main_window)
{
	LIBCXX_NAMESPACE::w::gridlayoutmanager
		layout=main_window->get_layoutmanager();
	LIBCXX_NAMESPACE::w::gridfactory factory=layout->append_row();

	LIBCXX_NAMESPACE::w::new_listlayoutmanager
		nlm{LIBCXX_NAMESPACE::w::highlighted_list};

	auto custom=nlm.appearance->clone();

	nlm.appearance=custom;

	nlm.columns=2;
	nlm.requested_col_widths={{1, 100}};
	nlm.row_alignments={{0, LIBCXX_NAMESPACE::w::valign::middle}};
	custom->h_padding=0;
	nlm.width(LIBCXX_NAMESPACE::w::dim_axis_arg{75});
	nlm.height(LIBCXX_NAMESPACE::w::dim_axis_arg{100});

	nlm.selection_type=
		[]
		(ONLY IN_THREAD,
		 const LIBCXX_NAMESPACE::w::listlayoutmanager &ll,
		 size_t i,
		 const LIBCXX_NAMESPACE::w::callback_trigger_t &trigger,
		 const LIBCXX_NAMESPACE::w::busy &mcguffin)
		{
			size_t i_indent=ll->hierindent(i);

			size_t s=ll->size();

			size_t e;

			for (e=i; ++e < s; )
			{
				if (ll->hierindent(e) <= i_indent)
					break;
			}

			if (e-i > 1)
			{
				ll->remove_items(i+1, e-i-1);
				return;
			}

			LIBCXX_NAMESPACE::w::hierindent new_indent{++i_indent};

			ll->insert_items(i+1, {
					new_indent,
					"scroll-right1"_image,
					next_lorem_ipsum(),
					new_indent,
					"scroll-right1"_image,
					next_lorem_ipsum(),
					new_indent,
					"scroll-right1"_image,
					next_lorem_ipsum(),
					new_indent,
					"scroll-right1"_image,
					next_lorem_ipsum()
				});
		};

	factory->create_focusable_container
		([]
		 (const auto &fc)
		 {
			 LIBCXX_NAMESPACE::w::listlayoutmanager ll=
				 fc->get_layoutmanager();

			 ll->append_items({LIBCXX_NAMESPACE::w::image_param
					   {"scroll-right1"},
					   lorem_ipsum[0]});
		 },
		 nlm);
}

void testlist(const testlistoptions &options)
{
	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	LIBCXX_NAMESPACE::w::containerptr c;

	auto close_flag=close_flag_ref::create();

	auto configfile=
		LIBCXX_NAMESPACE::configdir("testlist@libcxx.com") + "/windows";

	LIBCXX_NAMESPACE::w::screen_positions pos{configfile};

	auto default_screen=LIBCXX_NAMESPACE::w::screen::create();

	LIBCXX_NAMESPACE::w::main_window_config config;

	config.screen_position(pos, "name");
	auto main_window=default_screen->create_mainwindow
		(config,
		 [&]
		 (const auto &main_window)
		 {
			 if (options.hier->value)
				 listhiertest(main_window);
			 else
				 listtest(default_screen, main_window, pos,
					  options);
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

	LIBCXX_NAMESPACE::mpcobj<bool>::lock lock{close_flag->flag};
	lock.wait([&] { return *lock; });

	if (main_window->appdata)
	{
		main_window->save(pos);

		LIBCXX_NAMESPACE::w::tablelayoutmanager tlm=
			LIBCXX_NAMESPACE::w::focusable_container(main_window->appdata)
			->get_layoutmanager();
		tlm->save("list", pos);

		pos.save(configfile);
	}
}

int main(int argc, char **argv)
{
	try {
		LIBCXX_NAMESPACE::property
			::load_property(LIBCXX_NAMESPACE_STR "::themes",
					"themes", true, true);
		testlistoptions options;

		options.parse(argc, argv);
		if (options.test->value)
		{
			testlist1();
			return 0;
		}
		testlist(options);
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
