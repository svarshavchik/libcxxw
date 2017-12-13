/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/screen.H"
#include "x/w/main_window.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/label.H"
#include "x/w/standard_comboboxlayoutmanager.H"
#include "x/w/focusable_container.H"
#include "x/w/screen.H"
#include "x/w/connection.H"
#include "x/w/canvas.H"
#include "x/w/scrollbar.H"

#include "configfile.H"

#include <x/logger.H>
#include <x/destroy_callback.H>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <iterator>

LOG_FUNC_SCOPE_DECL("cxxwtheme", cxxwLog);

#define SCALE_INC 25

using namespace LIBCXX_NAMESPACE;

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

// object that stores the currently shown theme name and scale.

class theme_infoObj : virtual public obj {

public:

	std::string name;
	int scale;

	theme_infoObj(const w::connection &conn)
	{
		std::tie(name, scale)=conn->current_theme();
	}

	~theme_infoObj()=default;
};

static void create_main_window(const w::main_window &mw)
{
	w::gridlayoutmanager glm=mw->get_layoutmanager();

	glm->row_alignment(0, w::valign::middle);

	auto f=glm->append_row();

	f->create_label("Theme: ");

	auto conn=mw->get_screen()->get_connection();

	auto theme_info=ref<theme_infoObj>::create(conn);

	auto available_themes=w::connection::base::available_themes();

	size_t i=std::find_if(available_themes.begin(),
			      available_themes.end(),
			      [&]
			      (const auto &t)
			      {
				      return t.identifier == theme_info->name;
			      })-available_themes.begin();

	std::vector<std::string> themeids;

	themeids.reserve(available_themes.size());

	std::transform(available_themes.begin(),
		       available_themes.end(),
		       std::back_insert_iterator{themeids},
		       []
		       (const auto &available_theme)
		       {
			       return available_theme.identifier;
		       });

	w::new_standard_comboboxlayoutmanager
		themes_combobox{
		[themeids](const auto &info)
		{
			if (!info.list_item_status_info.selected)
				return;

			// info.list_item_status_info.item_number
		}};

	f->create_focusable_container
		([&]
		 (const auto &new_container)
		 {
			 w::standard_comboboxlayoutmanager
				 lm=new_container->get_layoutmanager();

			 std::vector<w::list_item_param> descriptions;

			 descriptions.reserve(available_themes.size());

			 std::transform(available_themes.begin(),
					available_themes.end(),
					std::back_insert_iterator{descriptions},
					[]
					(const auto &available_theme)
					{
						return available_theme
							.description;
					});
			 lm->append_items(descriptions);
			 lm->autoselect(i);
		 },
		 themes_combobox);

	// Sanity check.

	if (theme_info->scale < SCALE_MIN)
		theme_info->scale=SCALE_MIN;

	if (theme_info->scale > SCALE_MAX)
		theme_info->scale=SCALE_MAX;

	theme_info->scale -= (theme_info->scale % SCALE_INC);

	std::ostringstream initial_scale;

	initial_scale << theme_info->scale << "%";

	f->left_padding(4).create_label("Scale:");

	auto scale_label=f->create_label(initial_scale.str());

	f->create_canvas();

	f=glm->append_row();

	f->colspan(3).create_canvas(); // "Theme:" <combobox> Scale:

	w::scrollbar_config config{(SCALE_MAX-SCALE_MIN)/SCALE_INC+1};

	config.value=(theme_info->scale-SCALE_MIN)/SCALE_INC;

	auto scale_scrollbar=
		f->colspan(2).create_horizontal_scrollbar(config, 100);

	scale_scrollbar->on_update
		([scale_label, theme_info, conn]
		 (const auto &info)
		 {
			 if (std::holds_alternative<w::initial>(info.trigger))
				 return;

			 std::ostringstream initial_scale;

			 int v=info.dragged_value * SCALE_INC
				 + SCALE_MIN;

			 initial_scale << v << "%";

			 scale_label->update(initial_scale.str());

			 if (info.value != info.dragged_value)
				 return;

			 theme_info->scale=v;

			 conn->set_theme(theme_info->name,
					 theme_info->scale,
					 true);
		 });
}

void cxxwtheme()
{
	destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=w::main_window
		::create([]
			 (const auto &main_window)
			 {
				 create_main_window(main_window);
			 });

	main_window->set_window_title("Set LibCXXW theme");
	main_window->set_window_class("main", "cxxwtheme@w.libcxx.com");

	guard(main_window->connection_mcguffin());

	main_window->on_disconnect([]
				   {
					   exit(1);
				   });

	main_window->on_delete
		([close_flag]
		 (const auto &ignore)
		 {
			 close_flag->close();
		 });

	main_window->show_all();

	mpcobj<bool>::lock lock{close_flag->flag};
	lock.wait([&] { return *lock; });
}

int main(int argc, char **argv)
{
	LOG_FUNC_SCOPE(cxxwLog);


	try {
		cxxwtheme();
	} catch (const exception &e)
	{
		e->caught();
	}
	return 0;
}
