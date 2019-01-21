/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"

#include <x/exception.H>
#include <x/destroy_callback.H>

#include "x/w/main_window.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/factory.H"
#include "x/w/label.H"
#include "x/w/input_field.H"
#include "x/w/button.H"
#include "x/w/menubarlayoutmanager.H"
#include "x/w/menubarfactory.H"
#include "x/w/menu.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/copy_cut_paste_menu_items.H"
#include <x/weakcapture.H>

using namespace LIBCXX_NAMESPACE;
using namespace LIBCXX_NAMESPACE::w;

class close_flagObj : public obj {

public:
	mpcobj<bool> flag;

	close_flagObj() : flag{false} {}
	~close_flagObj()=default;

	void close()
	{
		mpcobj<bool>::lock lock{flag};

		*lock=true;
		lock.notify_all();
	}
};

typedef ref<close_flagObj> close_flag_ref;

void create_mainwindow(const main_window &mw)
{
	auto mblm=mw->get_menubarlayoutmanager();
	auto mbf=mblm->append_menus();

	copy_cut_paste_menu_itemsptr ccp;

	auto file_menu=mbf->add([]
				(const auto &f)
				{
					f->create_label("File");
				},
				[&]
				(const auto &f)
				{
					ccp=f->append_copy_cut_paste(mw);
				});

	file_menu->on_popup_state_update
		([ccp=copy_cut_paste_menu_items{ccp}]
		 (ONLY IN_THREAD,
		  const element_state &es,
		  const busy &mcguffin)
		 {
			 if (es.state_update != es.before_showing)
				 return;

			 ccp->update(0);
		 });
	mw->get_menubar()->show();
	gridlayoutmanager layout=mw->get_layoutmanager();

	layout->row_alignment(0, valign::middle);

	gridfactory f=layout->append_row();

	f->create_label("Input Field:");

	input_field_config config{30};

	f->create_input_field("", config);

	f->create_normal_button_with_label("Ok");
}

void testfocusrestore()
{
	destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto mw=main_window::create
		([&]
		 (const auto &mw)
		 {
			 create_mainwindow(mw);
		 });

	mw->on_disconnect([]
			  {
				  _exit(1);
			  });

	guard(mw->connection_mcguffin());

	mw->on_delete
		([close_flag]
		 (ONLY IN_THREAD,
		  const busy &ignore)
		 {
			 close_flag->close();
		 });

	mw->show_all();

	mpcobj<bool>::lock lock{close_flag->flag};

	lock.wait([&] { return *lock; });

}

int main(int argc, char **argv)
{
	try {
		testfocusrestore();
	} catch (const exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
