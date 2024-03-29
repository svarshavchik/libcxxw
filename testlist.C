/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include <x/property_properties.H>
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
#include "x/w/button_event.H"
#include "x/w/motion_event.H"
#include "x/w/canvas.H"
#include "x/w/screen.H"
#include "x/w/image_param_literals.H"
#include "x/w/listitemhandle.H"
#include "x/w/connection.H"

#include "focus/label_for.H"
#include "listlayoutmanager/listlayoutmanager_impl.H"
#include "listlayoutmanager/list_element_impl.H"
#include <vector>
#include <random>
#include <sstream>
#include <X11/keysym.h>

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
	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

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

	guard(main_window->connection_mcguffin());

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

#include "listlayoutmanager/list_element_impl.H"
#include "listlayoutmanager/listlayoutmanager_impl.H"
#include "connection.H"

auto create_process_table(const LIBCXX_NAMESPACE::w::main_window &mw,
			  const LIBCXX_NAMESPACE::w::gridfactory &f,
			  const testlistoptions &options)
{
	LIBCXX_NAMESPACE::w::new_tablelayoutmanager ntlm{
		{[](const auto &f)
		{
			f->create_label("Process")->show();
		},
		 [](const auto &f)
		{
			f->create_label("CPU %")->show();
		},
		 [](const auto &f)
		{
			f->create_label("RAM %")->show();
		},
		 [](const auto &f)
		{
			f->create_label("Disk I/O (Mbps)")->show();
		},
		 [](const auto &f)
		{
			f->create_label("Net I/O (Mbps)")->show();
		}}};

	if (options.adjustable->value)
	{
		ntlm.adjustable("list");
	}
	ntlm.selection_type=LIBCXX_NAMESPACE::w::no_selection_type;

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

	if (options.width->is_set())
		ntlm.table_width=ntlm.maximum_table_width=options.width->value;

	if (options.maximum_table_width->is_set())
		ntlm.maximum_table_width=options.maximum_table_width->value;

	if (options.minwidth->is_set())
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

void listtable(const LIBCXX_NAMESPACE::w::screen &default_screen,
	      const LIBCXX_NAMESPACE::w::main_window &main_window,
	      const testlistoptions &options)
{
	auto [original_theme, original_scale, original_options]
		=default_screen->get_connection()->current_theme();

	LIBCXX_NAMESPACE::w::gridlayoutmanager
		layout=main_window->get_layoutmanager();
	LIBCXX_NAMESPACE::w::gridfactory factory=layout->append_row();

	factory->halign(LIBCXX_NAMESPACE::w::halign::fill);
	factory->colspan(2);
	auto l=create_process_table(main_window, factory, options);

	factory=layout->append_row();

	auto b=factory->create_button("Bigger/Smaller");

	b->on_activate
		([=]
		 (ONLY IN_THREAD,
		  const auto &trigger,
		  const auto &busy)
		 mutable
		 {
			 original_scale=original_scale == 100 ? 200:100;

			 default_screen->get_connection()
				 ->set_theme(IN_THREAD,
					     original_theme,
					     original_scale,
					     original_options,
					     true,
					     {"theme"});
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

			 std::random_device rd;
			 std::mt19937 g{rd()};
			 std::shuffle(i.begin(), i.end(), g);
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

static size_t lorem_ipsum_idx=(size_t)-1;


static std::string next_lorem_ipsum()
{
	size_t i=lorem_ipsum_idx;

	if (++i >= sizeof(lorem_ipsum)/sizeof(lorem_ipsum)[0])
		i=0;

	lorem_ipsum_idx=i;

	return lorem_ipsum[i];
}

void make_context_menu(const LIBCXX_NAMESPACE::w::listlayoutmanager &m,
		       const std::optional<size_t> &selected)
{
	std::ostringstream o;

	if (selected)
	{
		o << *selected;
	}
	else
	{
		o << "none";
	}

	m->append_items
		({
			[]
			(THREAD_CALLBACK, const auto &ignore)
			{
				std::cout << "Help selected" << std::endl;
			},
			"Help (" + o.str() + ")",
			[]
			(THREAD_CALLBACK, const auto &ignore)
			{
				std::cout << "About selected" << std::endl;
			},
			"About (" + o.str() + ")",
		});
}

static LIBCXX_NAMESPACE::mpobj<std::optional<size_t>
			       > selection_when_context_menu_opened;

static LIBCXX_NAMESPACE::w::container
show_context_menu(ONLY IN_THREAD,
		  const LIBCXX_NAMESPACE::w::container &c)
{
	auto selected=
		LIBCXX_NAMESPACE::w::listlayoutmanager{c->get_layoutmanager()}
		->selected();

	selection_when_context_menu_opened=selected;

	auto context_menu=c->create_popup_menu
		([&]
		 (const auto &lm) {
			make_context_menu(lm, selected);
		});

	context_menu->show();

	return context_menu;
}

void listhiertest(const LIBCXX_NAMESPACE::w::main_window &main_window)
{
	LIBCXX_NAMESPACE::w::gridlayoutmanager
		layout=main_window->get_layoutmanager();
	LIBCXX_NAMESPACE::w::gridfactory factory=layout->append_row();

	LIBCXX_NAMESPACE::w::new_listlayoutmanager
		nlm{LIBCXX_NAMESPACE::w::highlighted_list};

	nlm.appearance=nlm.appearance->modify
		([]
		 (const auto &custom)
		 {
			 custom->h_padding=0;
		 });

	nlm.columns=2;
	nlm.requested_col_widths={{1, 100}};
	nlm.row_alignments={{0, LIBCXX_NAMESPACE::w::valign::middle}};
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

static LIBCXX_NAMESPACE::w::focusable_container
create_plain_list(const testlistoptions &opts,
		  const LIBCXX_NAMESPACE::w::gridfactory &factory)
{
	LIBCXX_NAMESPACE::w::new_listlayoutmanager
		new_list{opts.bullets->value
			 ? LIBCXX_NAMESPACE::w::bulleted_list
			 : LIBCXX_NAMESPACE::w::highlighted_list};


	if (opts.multiple->value)
		new_list.selection_type=
			LIBCXX_NAMESPACE::w::multiple_selection_type;

	if (opts.rows->is_set())
	{
		size_t min=opts.rows->value, max=min;

		if (opts.maxrows->is_set())
		{
			max=opts.maxrows->value;
		}

		new_list.height(min, max);
	}
	else if (opts.maxrows->is_set())
	{
		auto v=opts.maxrows->value;
		new_list.height(v); //
	}
	else if (opts.height->is_set())
	{
		auto v=opts.height->value;
		new_list.height(LIBCXX_NAMESPACE::w::dim_axis_arg{v});
	}

	new_list.selection_changed=
		[]
		(ONLY IN_THREAD,
		 const LIBCXX_NAMESPACE::w::list_item_status_info_t &info)
		{
			std::cout << "Item #" << info.item_number << " was ";

			std::cout << (info.selected ? "selected":"unselected");

			std::cout << std::endl;
		};

	new_list.current_list_item_changed=
		[]
		(ONLY IN_THREAD,
		 const LIBCXX_NAMESPACE::w::list_item_status_info_t &info)
		{
			std::cout << "Item #" << info.item_number << " was ";

			std::cout << (info.selected ? "highlighted"
				      : "unhighlighted");

			std::cout << std::endl;
		};

	LIBCXX_NAMESPACE::w::focusable_container list_container=
		factory->create_focusable_container
		([&]
		 (const auto &list_container)
		 {
			 LIBCXX_NAMESPACE::w::listlayoutmanager
				 l=list_container->get_layoutmanager();

			 l->append_items({next_lorem_ipsum()});

			 l->append_items({
					  LIBCXX_NAMESPACE::w::text_param
					  {next_lorem_ipsum()}
				 });
		 },
		 new_list);

	list_container->show();
	return list_container;
}

struct plain_list_info_s {
	bool shown=false;
	std::optional<size_t> item_selected;
};

typedef LIBCXX_NAMESPACE::mpcobj<plain_list_info_s> plain_list_info_t;

plain_list_info_t plain_list_info;

static inline auto plain_list(const LIBCXX_NAMESPACE::w::main_window
			      &main_window,
			      const testlistoptions &opts)
{
	LIBCXX_NAMESPACE::w::gridlayoutmanager
		layout=main_window->get_layoutmanager();

	LIBCXX_NAMESPACE::w::gridfactory factory=layout->append_row();

	factory->rowspan(6);

	auto list_container=create_plain_list(opts, factory);

	list_container->install_contextpopup_callback
		([current_popup=LIBCXX_NAMESPACE::w::containerptr{}]
		 (ONLY IN_THREAD,
		  const LIBCXX_NAMESPACE::w::container &c,
		  const auto &t,
		  const auto &m)
		mutable {
			current_popup=show_context_menu(IN_THREAD, c);
		});

	list_container->on_state_update
		([]
		 (ONLY IN_THREAD,
		  const auto &s,
		  const auto &)
		 {
			 if (s.shown)
			 {
				 plain_list_info_t::lock lock{plain_list_info};

				 lock->shown=s.shown;
				 lock.notify_all();
			 }
		 });

	list_container->listlayout()->on_current_list_item_changed
		([]
		 (ONLY IN_THREAD,
		  const auto &info)
		 {
			 if (info.selected)
			 {
				 plain_list_info_t::lock lock{plain_list_info};

				 lock->item_selected=info.item_number;
				 lock.notify_all();
			 }
		 });
	auto insert_row=
		factory->halign(LIBCXX_NAMESPACE::w::halign::fill)
		.create_button("Insert New Row");

	insert_row->show();

	factory=layout->append_row();

	auto append_row=factory->halign(LIBCXX_NAMESPACE::w::halign::fill)
		.create_button("Append New Row");

	append_row->show();

	factory=layout->append_row();
	auto remove_row=factory->halign(LIBCXX_NAMESPACE::w::halign::fill)
		.create_button("Remove Row");

	remove_row->show();

	factory=layout->append_row();
	auto replace_row=
		factory->halign(LIBCXX_NAMESPACE::w::halign::fill)
		.create_button("Replace Row");

	replace_row->show();

	factory=layout->append_row();

	auto reset=
		factory->halign(LIBCXX_NAMESPACE::w::halign::fill)
		.create_button("Reset");

	reset->show();

	factory=layout->append_row();

	auto show_me=
		factory->halign(LIBCXX_NAMESPACE::w::halign::fill)
		.create_button("Show Selected Items");

	show_me->show();

	insert_row->on_activate
		([list_container, counter=0]
		 (ONLY IN_THREAD,
		  const LIBCXX_NAMESPACE::w::callback_trigger_t &trigger,
		  const LIBCXX_NAMESPACE::w::busy &busy_mcguffin)
		 mutable
		 {
			 auto l=list_container->listlayout();

			 l->insert_items
				 (0, {next_lorem_ipsum()
				 });

			 l->on_status_update
				 (0,
				  [counter]
				  (ONLY IN_THREAD,
				   const LIBCXX_NAMESPACE::w
				   ::list_item_status_info_t
				   &info)
				  {
					  std::cout << "Item factory: "
						  "item #"
						    << counter
						    << (info.selected ?
							" is":
							" is not")
						    << " selected at"
						    << " position "
						    << info.item_number
						    << std::endl;
				  });
			 ++counter;
		 });

	append_row->on_activate
		([list_container]
		 (ONLY IN_THREAD,
		  const LIBCXX_NAMESPACE::w::callback_trigger_t &trigger,
		  const LIBCXX_NAMESPACE::w::busy &busy_mcguffin)
		 {
			 auto l=list_container->listlayout();

			 // insert_items() and append_items() take
			 // a std::vector of list_item_param-s as parameters.
			 //
			 // Each list_item_param is constructible with either
			 // an explicit LIBCXX_NAMESPACE::w::text_param, or with a
			 // std::string (UTF-8) or std::u32string (unicode).
			 //
			 // A plain const char pointer will work as well.

			 auto ret=l->append_items
				 ({next_lorem_ipsum(),
				   LIBCXX_NAMESPACE::w::get_new_items{}});

			 ret.handles.at(0)->on_status_update
				 ([]
				  (ONLY IN_THREAD,
				   const LIBCXX_NAMESPACE::w
				   ::list_item_status_info_t &info)
				  {
					  if (info.trigger.index() ==
					      LIBCXX_NAMESPACE::w
					      ::callback_trigger_initial)
						  return;

					  std::cout <<
						  "Appended item selected: "
						    << info.selected
						    << std::endl;
				  });
		 });

	remove_row->on_activate
		([list_container]
		 (ONLY IN_THREAD,
		  const LIBCXX_NAMESPACE::w::callback_trigger_t &trigger,
		  const LIBCXX_NAMESPACE::w::busy &busy_mcguffin)
		 {
			 auto l=list_container->listlayout();

			 // If the list is non-empty, remove the first list
			 // item.

			 if (l->size() == 0)
				 return;
			 l->remove_item(0);
		 });

	replace_row->on_activate
		([list_container]
		 (ONLY IN_THREAD,
		  const LIBCXX_NAMESPACE::w::callback_trigger_t &trigger,
		  const LIBCXX_NAMESPACE::w::busy &busy_mcguffin)
		 {
			 auto l=list_container->listlayout();

			 if (l->size() == 0)
				 return;

			 // replace_items() works like insert_items(), except
			 // that the existing item gets removed.

			 l->replace_items(0, {next_lorem_ipsum()});
		 });

	reset->on_activate
		([list_container]
		 (ONLY IN_THREAD,
		  const LIBCXX_NAMESPACE::w::callback_trigger_t &trigger,
		  const LIBCXX_NAMESPACE::w::busy &busy_mcguffin)
		 {
			 auto l=list_container->listlayout();

			 lorem_ipsum_idx=(size_t)-1;

			 // replace_all_items() is equivalent to removing
			 // all items from the list, then append_items().
			 //
			 // This effectively sets the new list of items.
			 //
			 l->replace_all_items({ next_lorem_ipsum(),
						next_lorem_ipsum()});
		 });

	show_me->on_activate
		([list_container]
		 (ONLY IN_THREAD,
		  const LIBCXX_NAMESPACE::w::callback_trigger_t &trigger,
		  const LIBCXX_NAMESPACE::w::busy &busy_mcguffin)
		 {
			 auto l=list_container->listlayout();

			 // This callback gets executed by the library's
			 // internal connection thread, so this lock is
			 // not really necessary, since only the connection
			 // thread modifies the list.
			 //
			 // Acquiring a list_lock blocks other execution
			 // threads from accessing the list. The lock freezes
			 // the list state, so that the list's contents can
			 // be examined and modified, with the list's contents
			 // remaining consistent.

			 LIBCXX_NAMESPACE::w::list_lock lock{l};

			 auto s=l->size();
			 size_t n=0;

			 for (size_t i=0; i<s; ++i)
				 if (l->selected(i))
				 {
					 ++n;
					 std::cout << "Item #"
						   << i << " is selected."
						   << std::endl;

				 }

			 std::cout << "Total # of selected items: " << n
				   << std::endl;
		 });

	factory=layout->append_row();

	factory->colspan(2);
	auto second_list_container=create_plain_list(opts, factory);

	auto l=second_list_container->listlayout();

	l->selection_type(LIBCXX_NAMESPACE::w::single_selection_type);

	l->on_selection_changed
		([]
		 (ONLY IN_THREAD,
		  const LIBCXX_NAMESPACE::w::list_item_status_info_t &info)
		 {
			 std::cout << "Item #" << info.item_number << " was ";

			 std::cout << (info.selected ? "selected":"unselected");

			 std::cout << " in list 2";
			 std::cout << std::endl;
		 });

	l->on_current_list_item_changed
		([]
		 (ONLY IN_THREAD,
		  const LIBCXX_NAMESPACE::w::list_item_status_info_t &info)
		 {
			 std::cout << "Item #" << info.item_number << " was ";

			 std::cout << (info.selected ? "highlighted"
				       : "unhighlighted");

			 std::cout << " in list 2";
			 std::cout << std::endl;
		 });

	return list_container;
}

void settle_down(const LIBCXX_NAMESPACE::w::main_window &mw)
{
	LIBCXX_NAMESPACE::mpcobj<bool> flag{false};

	mw->in_thread_idle([&]
			   (ONLY IN_THREAD)
	{
		LIBCXX_NAMESPACE::mpcobj<bool>::lock lock{flag};

		*lock=true;
		lock.notify_all();
	});

	LIBCXX_NAMESPACE::mpcobj<bool>::lock lock{flag};
	lock.wait([&]
	{
		return *lock;
	});
}

void testlist(const testlistoptions &options)
{
	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	LIBCXX_NAMESPACE::w::containerptr c;

	auto close_flag=close_flag_ref::create();

	auto default_screen=LIBCXX_NAMESPACE::w::screen::create();

	LIBCXX_NAMESPACE::w::main_window_config config{"main"};

	LIBCXX_NAMESPACE::w::focusable_containerptr mainlist;

	auto main_window=default_screen->create_mainwindow
		(config,
		 [&]
		 (const auto &main_window)
		 {
			 if (options.hier->value)
				 listhiertest(main_window);
			 else if (options.table->value)
				 listtable(default_screen, main_window,
					   options);
			 else
				 mainlist=plain_list(main_window, options);
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

	if (options.callback->value)
	{
		alarm(30);

		plain_list_info_t::lock lock{plain_list_info};

		lock.wait([&] { return lock->shown; });

		mainlist->in_thread
			([=]
			 (ONLY IN_THREAD)
			 {
				 auto ll=mainlist->listlayout();

				 ll->set_modified();

				 auto impl=
					 ll->impl->list_element_singleton->impl;

				 LIBCXX_NAMESPACE::w::key_event
					 ke{0,
					    mainlist->get_screen()
					    ->get_connection()
					    ->impl
					    ->keysyms_info(IN_THREAD)};
				 ke.keypress=true;
				 ke.keysym=XK_Home;

				 impl->process_key_event(IN_THREAD, ke);
			 });

		lock.wait([&] { return lock->item_selected &&
					lock->item_selected == 0; });

		mainlist->listlayout()->insert_items(0, {"Foo"});

		lock.wait([&] { return lock->item_selected &&
					lock->item_selected == 1; });
		mainlist->listlayout()->remove_item(0);
		lock.wait([&] { return lock->item_selected &&
					lock->item_selected == 1; });
		return;
	}

	if (options.test->value)
	{
		alarm(30);
		settle_down(main_window);
		main_window->in_thread
			([mainlist]
			 (ONLY IN_THREAD)
			{
				LIBCXX_NAMESPACE::w::listlayoutmanager lm=
					mainlist->get_layoutmanager();

				auto impl=lm->impl->list_element_singleton
					->impl;

				auto &keysyms=
					impl->get_screen()->get_connection()
					->impl->keysyms_info(IN_THREAD);

				LIBCXX_NAMESPACE::w::button_event_redirect_info
					redirect_info;

				LIBCXX_NAMESPACE::w::button_event
					be{0, keysyms, 3, true, 1,
					redirect_info};

				LIBCXX_NAMESPACE::w::motion_event
					me{be,
					LIBCXX_NAMESPACE::w::motion_event_type
					::button_action_event,
					0, 0};

				impl->report_motion_event(IN_THREAD, me);
				impl->process_button_event
					(IN_THREAD, be, 0);
			});
		settle_down(main_window);

		auto selected=selection_when_context_menu_opened.get();

		if (! (selected && *selected == 0))
		{
			throw EXCEPTION("Did not have item 0 selected when "
					"context menu opened");
		}
		return;
	}
	LIBCXX_NAMESPACE::mpcobj<bool>::lock lock{close_flag->flag};
	lock.wait([&] { return *lock; });
}

int main(int argc, char **argv)
{
	x::property::load_property("x::w::themes", "./themes", true, false);
	try {
		testlistoptions options;

		options.parse(argc, argv);
		if (options.test->value)
		{
			testlist1();
		}
		testlist(options);
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
