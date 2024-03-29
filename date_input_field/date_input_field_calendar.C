/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "date_input_field/date_input_field_calendar.H"
#include "peephole/peepholed_attachedto_container_impl.H"
#include "popup/popup_attachedto_info.H"
#include "x/w/impl/container.H"
#include "x/w/image_button_appearance.H"
#include "x/w/date_input_field.H"
#include "x/w/date_input_field_config.H"
#include "x/w/date_input_field_appearance.H"
#include "x/w/gridfactory.H"
#include "x/w/container.H"
#include "x/w/label.H"
#include "x/w/focusable_label.H"
#include "x/w/text_param.H"
#include "x/w/text_param_literals.H"
#include "x/w/input_field.H"
#include "x/w/callback_trigger.H"
#include "generic_window_handler.H"
#include "gridlayoutmanager.H"
#include "grid_map_info.H"
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
			       const const_date_input_field_appearance
			       &appearance,
			       const ref<implObj> &impl,
			       const layout_impl &lm_impl,
			       const ymd &current_ym,
			       const input_field &text_input_field)
	: peepholed_attachedto_containerObj{attachedto_info, impl, lm_impl},
	  appearance{appearance},
	  text_input_field{text_input_field},
	  current_ym{current_ym}
{
}

date_input_field_calendarObj::~date_input_field_calendarObj()=default;

class LIBCXX_HIDDEN date_input_field_calendar_helperObj;

// The day of the month buttons invoke picked(). Use this helper class to
// avoid the overhead of separately weakly-capturing the object weakly for
// every day button. Just one of these gets created, and referenced by each
// day.

class date_input_field_calendar_helperObj : virtual public obj {

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
				auto me=link->weak_me.getptr();

				if (me && e==focus_change::gained)
				{
					ret(me->appearance->day_highlight_fg);
					ret(me->appearance->day_highlight_bg);
				}

				ret(unicode::literals::LRO);
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

			text_hotspot link{
				[day_str, current_date, weak_me]
				(ONLY IN_THREAD,
				 const text_event_t &event)
				{
					return do_hotspot(IN_THREAD,
							  day_str,
							  current_date,
							  event,
							  weak_me);
				}
			};

			day_label("link"_hotspot);
			day_label(unicode::literals::LRO);
			day_label(day_str);
			day_label(end_hotspot{});

			if (current_date.get_day() ==
			    current_date.get_last_day_of_month())
				reached_end_of_month=true;
			else
				++current_date;

			l->update(day_label, {
					{"link", link},
				});
			l->show();
		}
	}
}

static text_param get_month_label(const date_input_field_calendarObj &me,
				  const const_locale &e,
				  const ymd &new_ym)
{
	// Format the "Month YYYY" label.

	auto s=new_ym.format_date(U"%B %Y", e);

	return {me.appearance->month_font, me.appearance->month_color, s};
}

void date_input_field_calendarObj
::constructor(const popup_attachedto_info &attachedto_info,
	      const const_date_input_field_appearance &appearance,
	      const ref<implObj> &impl,
	      const layout_impl &lm_impl,
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

			 auto i_appearance=appearance->previous_year_appearance;

			 create_image_button_info scroll_button_info
				 {f->get_container_impl(), true, i_appearance,
				  true};

			 auto b=create_image_button
				 (scroll_button_info,
				  [&, this]
				  (const auto &parent)
				  {
					  auto b=scroll_imagebutton_specific_height
						  (parent,
						   i_appearance->images,
						   this->appearance
						   ->yscroll_height);

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
				  [](const auto &){});

			 f->created_internally(b);

			 scroll_button_info.parent_container_impl=
				 f->get_container_impl();
			 i_appearance=appearance->previous_month_appearance;
			 b=create_image_button
				 (scroll_button_info,
				  [&, this]
				  (const auto &parent)
				  {
					  auto b=scroll_imagebutton_specific_height
						  (parent,
						   i_appearance->images,
						   this->appearance
						   ->mscroll_height);
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
				  [](const auto &){});

			 f->created_internally(b);

			 f->create_label(get_month_label(*this, e, current_ym))
				 ->show();

			 scroll_button_info.parent_container_impl=
				 f->get_container_impl();
			 i_appearance=appearance->next_month_appearance;

			 b=create_image_button
				 (scroll_button_info,
				  [&, this]
				  (const auto &parent)
				  {
					  auto b=scroll_imagebutton_specific_height
						  (parent,
						   i_appearance->images,
						   this->appearance
						   ->mscroll_height);
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
				  [](const auto &){});

			 f->created_internally(b);

			 scroll_button_info.parent_container_impl=
				 f->get_container_impl();
			 i_appearance=appearance->next_year_appearance;
			 b=create_image_button
				 (scroll_button_info,
				  [&, this]
				  (const auto &parent)
				  {
					  auto b=scroll_imagebutton_specific_height
						  (parent,
						   i_appearance->images,
						   this->appearance
						   ->yscroll_height);
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
				  [](const auto &){});

			 f->created_internally(b);
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
					 ({appearance->day_of_week_font,
					   appearance->day_of_week_font_color,
					   day_of_week})->show();
				 ++sunday;
			 }

			 text_param day_label;
			 focusable_label_config config;

			 // Zap the focus borders around the focusable label,
			 // and create empty focusable labels. calendar_grid()
			 // will fill them in, in just a moment.
			 config.focus_border=focus_border_appearance::base
				 ::none_theme();

			 for (int row=0; row<6; ++row)
			 {
				 f=lm->append_row();

				 for (int col=0; col<7; col++)
				 {
					 f->halign(halign::center)
						 .create_focusable_label({},
									 {});

				 }
			 }

			 calendar_grid(lm, e, current_ym, ref(this));
		 },
		 new_gridlayoutmanager{})->show();
}

void date_input_field_calendarObj::on_change(const date_input_field_callback_t
					     &cb)
{
	in_thread([me=ref(this), cb]
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

void date_input_field_calendarObj
::report_new_date(ONLY IN_THREAD,
		  const std::optional<ymd> &d,
		  const callback_trigger_t &trigger)
{
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
	auto mon_yyyy=get_month_label(*this, e, *lock);

	impl->invoke_layoutmanager
		([&, this]
		 (const ref<gridlayoutmanagerObj::implObj> &my_lm)
		 {
			 grid_map_t::lock grid_lock{my_lm->grid_map};
			 container c=(*grid_lock)->get(0, 0);

			 // Row 0: container with the scroll buttons and
			 // the current month+year.

			 c->impl->invoke_layoutmanager
				 ([&]
				  (const ref<gridlayoutmanagerObj::implObj>
				   &row0_lm)
				  {
					  label l=row0_lm->lock_and_get(0, 2);

					  l->update(mon_yyyy);
				  });

			 // Row 1: Calendar grid.

			 c=(*grid_lock)->get(1, 0);

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
	elementObj::impl->get_window_handler().request_visibility(IN_THREAD,
								  false);
}

LIBCXXW_NAMESPACE_END
