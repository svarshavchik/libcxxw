/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "date_input_field_calendar.H"
#include "peephole/peepholed_fontelement.H"
#include "peephole/peepholed_toplevel_element.H"
#include "popup/popup_attachedto_info.H"
#include "x/w/impl/container.H"
#include "x/w/date_input_field.H"
#include "x/w/gridfactory.H"
#include "x/w/container.H"
#include "x/w/label.H"
#include "x/w/focusable_label.H"
#include "x/w/text_param_literals.H"
#include "x/w/text_hotspot.H"
#include "x/w/input_field.H"
#include "x/w/callback_trigger.H"
#include "generic_window_handler.H"
#include "gridlayoutmanager.H"
#include "image_button.H"
#include "image_button_internal_impl.H"
#include "catch_exceptions.H"
#include "run_as.H"

#include <x/visitor.H>
#include <x/locale.H>
#include <x/weakcapture.H>
#include <courier-unicode.h>

LIBCXXW_NAMESPACE_START

date_input_field_calendarObj
::date_input_field_calendarObj(const popup_attachedto_info &attachedto_info,
			       const ref<implObj> &impl,
			       const ref<layoutmanagerObj::implObj> &lm_impl,
			       const ymd &current_ym,
			       const input_field &text_input_field)
	: superclass_t{impl, impl, lm_impl},
	  attachedto_info{attachedto_info},
	  text_input_field{text_input_field},
	  current_ym{current_ym}
{
}

date_input_field_calendarObj::~date_input_field_calendarObj()=default;

void date_input_field_calendarObj
::recalculate_peepholed_metrics(ONLY IN_THREAD,	const screen &s)
{
	max_width_value=attachedto_info->max_peephole_width(IN_THREAD, s);
	max_height_value=attachedto_info->max_peephole_height(IN_THREAD, s);
}

dim_t date_input_field_calendarObj::max_width(ONLY IN_THREAD) const
{
	return max_width_value;
}

dim_t date_input_field_calendarObj::max_height(ONLY IN_THREAD) const
{
	return max_height_value;
}


// The day of the month buttons invoke picked(). Use this helper class to
// avoid the overhead of separately weakly-capturing the object weakly for
// every day button. Just one of these gets created, and referenced by each
// day.

class LIBCXX_HIDDEN date_input_field_calendar_helperObj : virtual public obj {

 public:

	weakptr<ptr<date_input_field_calendarObj>> weak_me;

	date_input_field_calendar_helperObj
		(const ref<date_input_field_calendarObj> &me)
		: weak_me{me}
	{
	}
};

// Each day is a focusable label, do some work to highlight it, and do
// something when it gets selected.

static inline text_param do_hotspot(ONLY IN_THREAD,
				    const std::string &label,
				    const ymd &date,
				    const text_event_t &event,
				    const ref<
				    date_input_field_calendar_helperObj> &link)
{
	text_param ret;

	std::visit(visitor {
			[&](focus_change e)
			{
				ret("dateedit_day"_theme_font);

				if (e==focus_change::gained)
				{
					ret("dateedit_day_highlight_fg"_color);
					ret("dateedit_day_highlight_bg"_color);
				}
				else
				{
					ret("dateedit_day"_color);
				}

				ret(label);
			},
			[&](const button_event *b)
			{
				if (b->button != 1)
					return;

				auto me=link->weak_me.getptr();

				if (!me)
					return;

				if (!me->elementObj::impl->activate_for(*b))
					return;

				me->picked(IN_THREAD, date, b);
			},
			[&](const key_event *k)
			{
				auto me=link->weak_me.getptr();

				if (!me)
					return;

				me->picked(IN_THREAD, date, k);
			}},
		event);

	return ret;
}

// Rebuild the days of the month grid.

static void calendar_grid(const gridlayoutmanager &glm,
			  const const_locale &e,
			  const ymd &new_ym,
			  const ref<date_input_field_calendarObj> &me)
{
	auto weak_me=ref<date_input_field_calendar_helperObj>::create(me);

	// Start drawing the calendar starting with the 1st of the month.

	auto current_date=ymd(new_ym.get_year(),
			      new_ym.get_month(), 1);

	// We have six rows to rebuild.

	// Row 0 is the heading row. Leave it alone.

	bool reached_end_of_month=false;

	for (int row=1; row <= 6; row++)
	{
		for (int day_of_week=0; day_of_week<7; ++day_of_week)
		{
			focusable_label l=glm->get(row, day_of_week);

			text_param day_label;

			day_label("dateedit_day"_theme_font);
			day_label("dateedit_day"_color);

			// current_date starts on the first of the month.
			//
			// This hides the cells when the 1st of the
			// month starts in the middle of the week, and after
			// we reach the end of the month.

			if (reached_end_of_month ||
			    current_date.get_day_of_week() != day_of_week)
			{
				l->hide();
				continue;
			}

			std::ostringstream o;

			if (current_date.get_day() < 10)
				o << ' ';
			o << current_date.get_day();

			auto day_str=o.str();

			auto link=text_hotspot::create
				([day_str, current_date, weak_me]
				 (ONLY IN_THREAD,
				  const text_event_t &event)
				 {
					 return do_hotspot(IN_THREAD,
							   day_str,
							   current_date,
							   event,
							   weak_me);
				 });

			day_label(link);
			day_label(day_str);
			day_label(nullptr);

			if (current_date.get_day() ==
			    current_date.get_last_day_of_month())
				reached_end_of_month=true;
			else
				++current_date;

			l->update(day_label);
			l->show();
		}
	}
}

static text_param get_month_label(const const_locale &e,
				  const ymd &new_ym)
{
	// Format the "Month YYYY" label.

	auto s=new_ym.format_date(U"%B %Y", e);

	return {"dateedit_popup_month"_theme_font, "dateedit_popup_month"_color,
			s};
}

void date_input_field_calendarObj
::constructor(const popup_attachedto_info &attachedto_info,
	      const ref<implObj> &impl,
	      const ref<layoutmanagerObj::implObj> &lm_impl,
	      const ymd &current_ym,
	      const input_field &text_input_field)
{
	auto e=locale::base::global();

	gridlayoutmanager lm=get_layoutmanager();

	lm->col_alignment(0, halign::fill);

	auto f=lm->append_row();

	// Row 0 is the strip with the month's name, and the +/- month/year
	// buttons.
	//
	// Use create_image_button() to create each button, weakly capturing
	// myself, installing callbacks to invoke myself->prev/next_year/mon().

	f->create_container
		([&, this]
		 (const auto &row0)
		 {
			 gridlayoutmanager row0_lm=row0->get_layoutmanager();

			 // Everything aligned to the bottom
			 row0_lm->row_alignment(0, valign::middle);

			 // All extra space to the month+year label, centered.

			 row0_lm->requested_col_width(2, 100);
			 row0_lm->col_alignment(2, halign::center);

			 auto f=row0_lm->append_row();

			 create_image_button
				 (true,
				  [this]
				  (const auto &parent)
				  {
					  auto b=scroll_imagebutton_specific_height
						  (parent,
						   "scroll-left1",
						   "scroll-left2",
						   "dateedit_popup_yscroll_height");

					  b->on_activate
						  ([me=make_weak_capture(ref(this))]
						   (ONLY IN_THREAD,
						    const auto &trigger,
						    const auto &busy)
						   {
							   auto got=me.get();

							   if (!got)
								   return;

							   auto &[r]=*got;
							   r->prev_year();
						   });
					  return b;
				  },
				  *f,
				  valign::bottom,
				  [](const auto &){})->show();

			 create_image_button
				 (true,
				  [this]
				  (const auto &parent)
				  {
					  auto b=scroll_imagebutton_specific_height
						  (parent,
						   "scroll-left1",
						   "scroll-left2",
						   "dateedit_popup_mscroll_height");
					  b->on_activate
						  ([me=make_weak_capture(ref(this))]
						   (ONLY IN_THREAD,
						    const auto &trigger,
						    const auto &busy)
						   {
							   auto got=me.get();

							   if (!got)
								   return;

							   auto &[r]=*got;

							   r->prev_mon();
						   });

					  return b;
				  },
				  *f,
				  valign::bottom,
				  [](const auto &){})->show();

			 f->create_label(get_month_label(e, current_ym))
				 ->show();

			 create_image_button
				 (true,
				  [this]
				  (const auto &parent)
				  {
					  auto b=scroll_imagebutton_specific_height
						  (parent,
						   "scroll-right1",
						   "scroll-right2",
						   "dateedit_popup_mscroll_height");
					  b->on_activate
						  ([me=make_weak_capture(ref(this))]
						   (ONLY IN_THREAD,
						    const auto &trigger,
						    const auto &busy)
						   {
							   auto got=me.get();

							   if (!got)
								   return;

							   auto &[r]=*got;

							   r->next_mon();
						   });

					  return b;
				  },
				  *f,
				  valign::bottom,
				  [](const auto &){})->show();

			 create_image_button
				 (true,
				  [this]
				  (const auto &parent)
				  {
					  auto b=scroll_imagebutton_specific_height
						  (parent,
						   "scroll-right1",
						   "scroll-right2",
						   "dateedit_popup_yscroll_height");
					  b->on_activate
						  ([me=make_weak_capture(ref(this))]
						   (ONLY IN_THREAD,
						    const auto &trigger,
						    const auto &busy)
						   {
							   auto got=me.get();

							   if (!got)
								   return;

							   auto &[r]=*got;

							   r->next_year();
						   });

					  return b;
				  },
				  *f,
				  valign::bottom,
				  [](const auto &){})->show();
		 },
		 new_gridlayoutmanager{})->show();

	f=lm->append_row();

	// update_month will populate the calendar grid container.

	f->create_container
		([&]
		 (const auto &row1)
		 {
			 gridlayoutmanager lm=row1->get_layoutmanager();

			 // Find a sunday, somewhere around here, then
			 // use format_date() to grab the locale-specific
			 // days of week abbreviations.

			 ymd sunday=current_ym +
				 ((7-current_ym.get_day_of_week()) % 7);

			 auto f=lm->append_row();

			 // Column headings.

			 for (int i=0; i<7; ++i)
			 {
				 auto day_of_week=sunday.format_date(U"%a", e);

				 f->halign(halign::center).create_label
					 ({"dateedit_day_of_week"_theme_font,
						 "dateedit_day_of_week"_color,
						 day_of_week})
					 ->show();
				 ++sunday;
			 }

			 text_param day_label;
			 focusable_label_config config;

			 // Zap the focus borders around the focusable label,
			 // and create empty focusable labels. calendar_grid()
			 // will fill them in, in just a moment.

			 config.off_border=border_infomm{};
			 config.on_border=border_infomm{};

			 for (int row=0; row<6; ++row)
			 {
				 f=lm->append_row();

				 for (int col=0; col<7; col++)
				 {
					 f->halign(halign::center)
						 .create_focusable_label({});

				 }
			 }

			 calendar_grid(lm, e, current_ym, ref(this));
		 },
		 new_gridlayoutmanager{})->show();
}

void date_input_field_calendarObj::on_change(const date_input_field_callback_t
					     &cb)
{
	impl->get_window_handler().thread()->run_as
		([me=ref(this), cb]
		 (ONLY IN_THREAD)
		 {
			 most_recent_date_t::lock lock{me->most_recent_date};

			 lock->callback=cb;

			 try {
				 cb(IN_THREAD, lock->date_value, initial{});
			 } REPORT_EXCEPTIONS(&me->impl
					     ->container_element_impl());
		 });
}

void date_input_field_calendarObj::set(ONLY IN_THREAD,
				       const std::optional<ymd> &d,
				       const callback_trigger_t &trigger)
{
	auto e=locale::base::global();

	std::u32string s;

	if (d)
		s=d->format_date(U"%x", e);

	text_input_field->set(s);

	report_new_date(IN_THREAD, d, trigger);
}

LOG_FUNC_SCOPE_DECL(LIBCXX_NAMESPACE::w::date_input_field, date_inputfieldLog);

void date_input_field_calendarObj
::report_new_date(ONLY IN_THREAD,
		  const std::optional<ymd> &d,
		  const callback_trigger_t &trigger)
{
	LOG_FUNC_SCOPE(date_inputfieldLog);

	most_recent_date_t::lock lock{most_recent_date};

	if (lock->date_value == d)
		return;

	lock->date_value=d;

	try {
		if (lock->callback)
			lock->callback(IN_THREAD, d, trigger);
	} REPORT_EXCEPTIONS(elementObj::impl);
}

std::optional<ymd> date_input_field_calendarObj::get() const
{
	auto e=locale::base::global();

	input_lock lock{text_input_field};

	return ymd::parser{e}.try_parse(lock.get_unicode());
}

void date_input_field_calendarObj::update_month(const ymd &new_date)
{
	mpobj<ymd>::lock lock{current_ym};

	if (lock->get_year() == new_date.get_year() &&
	    lock->get_month() == new_date.get_month())
		return;

	*lock=new_date;

	update_month(lock);
}

void date_input_field_calendarObj::update_month(mpobj<ymd>::lock &lock)
{
	// Rebuild the monthly calendar contents.

	auto e=locale::base::global();
	auto mon_yyyy=get_month_label(e, *lock);

	impl->invoke_layoutmanager
		([&, this]
		 (const ref<gridlayoutmanagerObj::implObj> &my_lm)
		 {
			 container c=my_lm->get(0, 0);

			 // Row 0: container with the scroll buttons and
			 // the current month+year.

			 c->impl->invoke_layoutmanager
				 ([&]
				  (const ref<gridlayoutmanagerObj::implObj>
				   &row0_lm)
				  {
					  label l=row0_lm->get(0, 2);

					  l->update(mon_yyyy);
				  });

			 // Row 1: Calendar grid.

			 c=my_lm->get(1, 0);

			 calendar_grid(c->get_layoutmanager(), e, *lock,
				       ref(this));
		 });
}

void date_input_field_calendarObj::prev_year()
{
	mpobj<ymd>::lock lock{current_ym};

	auto y=lock->get_year();

	*lock=ymd{--y, lock->get_month(), 1};
	update_month(lock);
}

void date_input_field_calendarObj::prev_mon()
{
	mpobj<ymd>::lock lock{current_ym};

	auto m=lock->get_month();
	auto y=lock->get_year();

	if (--m == 0)
	{
		m=12;
		--y;
	}

	*lock=ymd(y, m, 1);
	update_month(lock);
}

void date_input_field_calendarObj::next_year()
{
	mpobj<ymd>::lock lock{current_ym};

	auto y=lock->get_year();

	*lock=ymd{++y, lock->get_month(), 1};
	update_month(lock);
}

void date_input_field_calendarObj::next_mon()
{
	mpobj<ymd>::lock lock{current_ym};

	auto m=lock->get_month();
	auto y=lock->get_year();

	if (++m > 12)
	{
		m=1;
		++y;
	}

	*lock=ymd{y, m, 1};
	update_month(lock);
}

void date_input_field_calendarObj::picked(ONLY IN_THREAD,
					  const ymd &y,
					  const callback_trigger_t &trigger)
{
	set(IN_THREAD, y, trigger);
	text_input_field->request_focus();

	// Hide the popup.
	impl->get_window_handler().request_visibility(false);
}

LIBCXXW_NAMESPACE_END
