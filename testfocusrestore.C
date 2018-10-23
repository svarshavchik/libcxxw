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

	auto file_menu=mbf->add([]
				(const auto &f)
				{
					f->create_label("File");
				},
				[&]
				(const auto &f)
				{
					f->append_items
						({
						  [w=make_weak_capture(mw)]
						  (ONLY IN_THREAD,
						   const auto &status_info)
						  {
							  auto got=w.get();
							  if (!got)
								  return;

							  auto &[w]=*got;

							  w->cut_or_copy_selection(cut_or_copy_op::cut);
						  },
						  "Cut",
						  [w=make_weak_capture(mw)]
						  (ONLY IN_THREAD,
						   const auto &status_info)
						  {
							  auto got=w.get();
							  if (!got)
								  return;

							  auto &[w]=*got;

							  w->cut_or_copy_selection(cut_or_copy_op::copy);
						  },
						  "Copy",
						  [w=make_weak_capture(mw)]
						  (ONLY IN_THREAD,
						   const auto &status_info)
						  {
							  auto got=w.get();
							  if (!got)
								  return;

							  auto &[w]=*got;

							  w->receive_selection();
						  },
						  "Paste"
						});
				});

	file_menu->on_popup_state_update
		([file_menu=make_weak_capture(file_menu)]
		 (ONLY IN_THREAD,
		  const element_state &es,
		  const busy &mcguffin)
		 {
			 if (es.state_update != es.before_showing)
				 return;

			 auto got=file_menu.get();

			 if (!got)
				 return;

			 auto & [file_menu] = *got;

			 listlayoutmanager lm=file_menu->get_layoutmanager();

			 bool has_cut_or_copy=
				 file_menu->cut_or_copy_selection
				 (cut_or_copy_op::available);

			 lm->enabled(0, has_cut_or_copy);
			 lm->enabled(1, has_cut_or_copy);

			 lm->enabled(2, file_menu->selection_has_owner());
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
