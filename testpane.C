/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/obj.H>
#include <x/config.H>
#include <x/singletonptr.H>
#include "x/w/main_window.H"
#include "x/w/screen_positions.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/focusable_container.H"
#include "x/w/panelayoutmanager.H"
#include "x/w/panefactory.H"
#include "x/w/pane_appearance.H"
#include "x/w/button.H"
#include "x/w/label.H"
#include "x/w/standard_comboboxlayoutmanager.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/tablelayoutmanager.H"
#include "x/w/canvas.H"
#include "x/w/uigenerators.H"

#include <string>
#include <iostream>
#include <sstream>

#include "testpane.inc.H"

struct my_appObj : virtual public LIBCXX_NAMESPACE::obj {

public:

	LIBCXX_NAMESPACE::w::const_uigenerators theme;

	my_appObj() : theme{LIBCXX_NAMESPACE::w::uigenerators
			    ::create("testpane_theme.xml")}
	{
	}
};

typedef LIBCXX_NAMESPACE::singletonptr<my_appObj> my_app;

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

static void create_pane(const LIBCXX_NAMESPACE::w::panelayoutmanager &lm,
			const LIBCXX_NAMESPACE::w::new_panelayoutmanager &nplm)
{
	LIBCXX_NAMESPACE::w::panefactory f=lm->append_panes();

	for (size_t i=0; i<nplm.restored_sizes.size(); ++i)
	{
		f->create_label("Lorem ipsum "
				"dolor sit amet\n"
				"consectetur "
				"adipisicing elit sed\n"
				"do eiusmod "
				"tempor incididunt ut\n"
				"labore et "
				"dolore magna\n"
				"aliqua")->show();
	}
}

static void insert(const LIBCXX_NAMESPACE::w::container &c,
		   LIBCXX_NAMESPACE::w::scrollbar_visibility v)
{
	LIBCXX_NAMESPACE::w::panelayoutmanager lm=c->get_layoutmanager();

	LIBCXX_NAMESPACE::w::panefactory f=lm->insert_panes(0);

	my_app app;

	switch (v) {
	case LIBCXX_NAMESPACE::w::scrollbar_visibility::never:
		f->appearance=app->theme->lookup_appearance("insert_never");
		break;
	case LIBCXX_NAMESPACE::w::scrollbar_visibility::always:
		f->appearance=app->theme->lookup_appearance("insert_always");
		break;
	case LIBCXX_NAMESPACE::w::scrollbar_visibility::automatic:
		f->appearance=app->theme->lookup_appearance("insert_automatic");
		break;
	case LIBCXX_NAMESPACE::w::scrollbar_visibility::automatic_reserved:
		f->appearance=app->theme->lookup_appearance("insert_automatic_reserved");
		break;
	}

	f->create_label("Lorem ipsum\n"
			"dolor sit amet\n"
			"consectetur\n"
			"adipisicing elit sed\n"
			"do eiusmod\n"
			"tempor incididunt ut\n"
			"labore et\n"
			"dolore magna\n"
			"aliqua")->show();

	LIBCXX_NAMESPACE::w::label l=lm->get(0);
}

static void append(const LIBCXX_NAMESPACE::w::container &c,
		   LIBCXX_NAMESPACE::w::scrollbar_visibility v)
{
	LIBCXX_NAMESPACE::w::panelayoutmanager lm=c->get_layoutmanager();

	LIBCXX_NAMESPACE::w::panefactory f=lm->append_panes();

	auto appearance=f->appearance->modify
		([v]
		 (const auto &appearance)
		 {
			 appearance->background_color="100%";
			 appearance->left_padding=appearance->right_padding=
				 appearance->top_padding=
				 appearance->bottom_padding=2;
			 appearance->pane_scrollbar_visibility=v;
		 });

	f->appearance=appearance;

	f->create_label("Lorem ipsum "
			"dolor sit amet\n"
			"consectetur "
			"adipisicing elit sed\n"
			"do eiusmod "
			"tempor incididunt ut\n"
			"labore et "
			"dolore magna\n"
			"aliqua")->show();
}

static void remove_first(const LIBCXX_NAMESPACE::w::container &c)
{
	LIBCXX_NAMESPACE::w::panelayoutmanager lm=c->get_layoutmanager();

	if (lm->size() > 0)
		lm->remove_pane(0);
}

static void remove_last(const LIBCXX_NAMESPACE::w::container &c)
{
	LIBCXX_NAMESPACE::w::panelayoutmanager lm=c->get_layoutmanager();

	auto s=lm->size();

	if (s > 0)
		lm->remove_pane(s-1);
}

static void replace_first(const LIBCXX_NAMESPACE::w::container &c,
			  LIBCXX_NAMESPACE::w::scrollbar_visibility v)
{
	LIBCXX_NAMESPACE::w::panelayoutmanager lm=c->get_layoutmanager();

	LIBCXX_NAMESPACE::w::panefactory f=lm->replace_panes(0);

	auto appearance=f->appearance->modify
		([v]
		 (const auto &appearance)
		 {
			 appearance->background_color="100%";
			 appearance->left_padding=appearance->right_padding=
				 appearance->top_padding=
				 appearance->bottom_padding=2;
			 appearance->pane_scrollbar_visibility=v;
			 appearance->vertical_alignment=
				 LIBCXX_NAMESPACE::w::valign::bottom;
		 });

	f->appearance=appearance;

	f->create_label("Lorem ipsum "
			"dolor sit amet\n"
			"consectetur "
			"adipisicing elit sed\n"
			"do eiusmod "
			"tempor incididunt ut\n"
			"labore et "
			"dolore magna\n"
			"aliqua")->show();
}

static void insert_list(const LIBCXX_NAMESPACE::w::container &c)
{
	LIBCXX_NAMESPACE::w::panelayoutmanager lm=c->get_layoutmanager();

	LIBCXX_NAMESPACE::w::panefactory f=lm->insert_panes(0);

	LIBCXX_NAMESPACE::w::new_listlayoutmanager nlm;

	f->configure_new_list(nlm);

	auto appearance=f->appearance->modify
		([]
		 (const auto &appearance)
		 {
			 appearance->size=20;
		 });

	f->appearance=appearance;

	f->create_focusable_container
		([]
		 (const auto &container)
		 {
			 LIBCXX_NAMESPACE::w::listlayoutmanager lm=
				 container->get_layoutmanager();

			 lm->append_items({
					 "Lorem ipsum",
					 "dolor sit amet",
					 "consectetur",
					 "adipisicing elit sed",
					 "do eiusmod",
					 "tempor incididunt ut",
					 "labore et",
					 "dolore magna",
					 "aliqua"});
		 },
		 nlm)->show();
}

static LIBCXX_NAMESPACE::w::scrollbar_visibility
get_scrollbar_visibility(const LIBCXX_NAMESPACE::w::container &container)
{
	LIBCXX_NAMESPACE::w::standard_comboboxlayoutmanager lm=
		container->get_layoutmanager();

	auto selected=lm->selected();

	size_t i=selected ? *selected:0;

	static const LIBCXX_NAMESPACE::w::scrollbar_visibility values[]={
		LIBCXX_NAMESPACE::w::scrollbar_visibility::never,
		LIBCXX_NAMESPACE::w::scrollbar_visibility::always,
		LIBCXX_NAMESPACE::w::scrollbar_visibility::automatic,
		LIBCXX_NAMESPACE::w::scrollbar_visibility::automatic_reserved
	};

	return values[i];
}

static void create_main_window(const LIBCXX_NAMESPACE::w::main_window &mw,
			       const testpaneoptions &options,
			       const LIBCXX_NAMESPACE::w::screen_positions
			       &pos)
{
	LIBCXX_NAMESPACE::w::new_panelayoutmanager npl{{20, 100, 200}};

	npl.restore(pos, "main");
	if (options.horizontal->value)
		npl.horizontal();

	LIBCXX_NAMESPACE::w::gridlayoutmanager layout=mw->get_layoutmanager();
	auto factory=layout->append_row();

	auto pane=factory->colspan(2).create_focusable_container
		([&]
		 (const auto &pane_container) {
			 if (npl.restored_sizes.empty())
				 return;
			 create_pane(pane_container->get_layoutmanager(), npl);
		}, npl);

	pane->show();

	factory=layout->append_row();

	factory->halign(LIBCXX_NAMESPACE::w::halign::right)
		.valign(LIBCXX_NAMESPACE::w::valign::bottom)
		.create_label("New elements'\nscrollbar visibility:")->show();

	auto scrollbar_visibility=
		factory->valign(LIBCXX_NAMESPACE::w::valign::bottom)
		.create_focusable_container
		([&]
		 (const auto &c)
		 {
			 LIBCXX_NAMESPACE::w::standard_comboboxlayoutmanager lm=
			 c->get_layoutmanager();

			 lm->append_items({ "Hide",
						 "Always",
						 "When needed",
						 "Reserve space"});

			 lm->selected(3, true);
		 },
		 LIBCXX_NAMESPACE::w::new_standard_comboboxlayoutmanager{}
		 );

	scrollbar_visibility->show();

	factory=layout->append_row();

	factory->halign(LIBCXX_NAMESPACE::w::halign::left);
	auto b=factory->create_button("Insert");

	b->show();
	b->on_activate([pane, scrollbar_visibility]
		       (THREAD_CALLBACK,
			const auto &trigger,
			const auto &busy) {
			       insert(pane, get_scrollbar_visibility
				      (scrollbar_visibility));
		       });

	factory->halign(LIBCXX_NAMESPACE::w::halign::right);
	b=factory->create_button("Append");

	b->show();
	b->on_activate([pane, scrollbar_visibility]
		       (THREAD_CALLBACK,
			const auto &trigger,
			const auto &busy) {
			       append(pane, get_scrollbar_visibility
				      (scrollbar_visibility));
		       });

	factory=layout->append_row();

	factory->halign(LIBCXX_NAMESPACE::w::halign::left);
	b=factory->create_button("Remove 1st pane");

	b->show();
	b->on_activate([pane]
		       (THREAD_CALLBACK,
			const auto &trigger,
			const auto &busy) {
			       remove_first(pane);
		       });

	factory->halign(LIBCXX_NAMESPACE::w::halign::right);
	b=factory->create_button("Remove last pane");

	b->show();
	b->on_activate([pane]
		       (THREAD_CALLBACK,
			const auto &trigger,
			const auto &busy) {
			       remove_last(pane);
		       });

	factory=layout->append_row();

	factory->halign(LIBCXX_NAMESPACE::w::halign::left);
	b=factory->create_button("Replace 1st pane");

	b->show();
	b->on_activate([pane, scrollbar_visibility]
		       (THREAD_CALLBACK,
			const auto &trigger,
			const auto &busy) {
			       replace_first(pane, get_scrollbar_visibility
					     (scrollbar_visibility));
		       });

	factory->halign(LIBCXX_NAMESPACE::w::halign::right);
	b=factory->create_button("Remove all panes");

	b->show();
	b->on_activate([pane]
		       (THREAD_CALLBACK,
			const auto &trigger,
			const auto &busy) {
			       	LIBCXX_NAMESPACE::w::panelayoutmanager
					lm=pane->get_layoutmanager();

				lm->remove_all_panes();
		       });

	factory=layout->append_row();

	factory->halign(LIBCXX_NAMESPACE::w::halign::left);
	b=factory->create_button("Insert list");

	b->show();
	b->on_activate([pane]
		       (THREAD_CALLBACK,
			const auto &trigger,
			const auto &busy) {
			       insert_list(pane);
		       });

	factory->halign(LIBCXX_NAMESPACE::w::halign::right);
	factory->create_canvas()->show();
}

static inline
void initialize_adjustable_pane(const LIBCXX_NAMESPACE::w::panelayoutmanager
				&plm)
{
	struct mondata {
		std::string process;
		int cpu;
		int ram;
		int diskio;
		int netio;
	};

	static const mondata processes[]=
		{
		 {"Compiler", 40, 12, 3, 0},
		 {"Idle", 35, 0, 0, 0},
		 {"Updater", 12, 4, 2, 2},
		 {"Editor", 4, 7, 0, 0},
		 {"Torrent Downloader", 5, 6, 4, 4},
		 {"Backup", 4, 5, 5, 0},
		};

	LIBCXX_NAMESPACE::w::panefactory f=plm->append_panes();

	LIBCXX_NAMESPACE::w::new_tablelayoutmanager ntlm
		{
		 []
		 (const auto &f, size_t i)
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
		 }
		};
	ntlm.unlimited_table_width();
	ntlm.adjustable_column_widths=true;
	ntlm.columns=5;
	ntlm.requested_col_widths={{0, 100}};

	ntlm.col_alignments=
		{
		 {0, LIBCXX_NAMESPACE::w::halign::center},
		 {1, LIBCXX_NAMESPACE::w::halign::right},
		 {2, LIBCXX_NAMESPACE::w::halign::right},
		 {3, LIBCXX_NAMESPACE::w::halign::right},
		 {4, LIBCXX_NAMESPACE::w::halign::right},
		};

	ntlm.column_borders=
		{
		 {1, "thin_0%"},
		 {2, "thin_dashed_0%"},
		 {3, "thin_dashed_0%"},
		 {4, "thin_dashed_0%"},
		};

	for (size_t i=0; i<2; ++i)
	{
		f->configure_new_list(ntlm);

		auto appearance=f->appearance->modify
			([]
			 (const auto &appearance)
			 {
				 appearance->size=50;
			 });

		f->appearance=appearance;

		f->create_focusable_container
			([]
			 (const auto &table_container)
			 {
				 LIBCXX_NAMESPACE::w::tablelayoutmanager tlm=
					 table_container
					 ->get_layoutmanager();

				 std::vector<LIBCXX_NAMESPACE::w
					     ::list_item_param> items;

				 items.reserve((std::end(processes)-
						std::begin(processes)) * 5);

				 for (const auto &p:processes)
				 {
					 std::ostringstream cpu, ram,
						 diskio, netio;

					 cpu << p.cpu << "%";

					 ram << p.ram << "%";

					 diskio << p.diskio << " Mbps";

					 netio << p.netio << " Mbps";

					 std::string fields[5]=
						 {
						  p.process,
						  cpu.str(),
						  ram.str(),
						  diskio.str(),
						  netio.str()
						 };

					 items.insert(items.end(),
						      std::begin
						      (fields),
						      std::end(fields)
						      );
				 }
				 tlm->append_items(items);
			 }, ntlm)->show();
	}
}

static void create_adjustable_pane(const LIBCXX_NAMESPACE::w::main_window &mw,
				   const testpaneoptions &options,
				   const LIBCXX_NAMESPACE::w::screen_positions
				   &pos)
{
	LIBCXX_NAMESPACE::w::new_panelayoutmanager npl{{0, 100}};

	npl.restore(pos, "main");

	if (options.horizontal->value)
		npl.horizontal();

	LIBCXX_NAMESPACE::w::gridlayoutmanager layout=mw->get_layoutmanager();
	auto factory=layout->append_row();

	auto pane=factory->create_focusable_container
		([&]
		 (const auto &pane_container) {
			 if (npl.restored_sizes.empty())
				 return;

			 auto lm=pane_container->get_layoutmanager();

			 create_pane(lm, npl);
			 initialize_adjustable_pane(lm);
		 }, npl);

	pane->show();
}

void testpane(const testpaneoptions &options)
{
	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto configfile=
		LIBCXX_NAMESPACE::configdir("testpane@libcxx.com") + "/windows";

	auto pos=LIBCXX_NAMESPACE::w::screen_positions::create(configfile);

	LIBCXX_NAMESPACE::w::main_window_config config;

	config.restore(pos, "main");

	auto main_window=LIBCXX_NAMESPACE::w::main_window
		::create(config,
			 [&]
			 (const auto &mw)
			 {
				 if (options.adjustable->value)
				 {
					 create_adjustable_pane(mw, options,
								pos);
				 }
				 else
					 create_main_window(mw, options,
							    pos);
			 });

	main_window->set_window_title("Panes!");
	main_window->set_window_class("main", "testpane@examples.w.libcxx.com");

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

	main_window->show();

	LIBCXX_NAMESPACE::mpcobj<bool>::lock lock{close_flag->flag};
	lock.wait([&] { return *lock; });

	main_window->save(pos);
	pos->save(configfile);
}

int main(int argc, char **argv)
{
	try {
		testpaneoptions options;
		options.parse(argc, argv);

		my_app app{my_app::create()};

		testpane(options);
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
