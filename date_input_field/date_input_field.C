/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/focus/focusable.H"
#include "date_input_field/date_input_field_impl.H"
#include "date_input_field/date_input_field_handler.H"
#include "date_input_field/date_input_field_calendar.H"
#include "popup/popup_attachedto_info.H"
#include "popup/popup_handler.H"
#include "popup/popup_impl.H"
#include "popup/popup.H"
#include "popup_imagebutton.H"
#include "peephole/peephole_toplevel.H"
#include "peephole/peepholed_toplevel.H"
#include "peephole/peepholed_attachedto_container_impl.H"
#include "x/w/impl/focus/focusable.H"
#include "gridlayoutmanager.H"
#include "x/w/input_field.H"
#include "x/w/input_field_lock.H"
#include "x/w/text_param_literals.H"
#include "x/w/stop_message.H"
#include "x/w/main_window.H"
#include "x/w/date_input_field_appearance.H"
#include "x/w/input_field_appearance.H"
#include "x/w/input_field_filter.H"
#include "run_as.H"
#include "messages.H"

#include <x/functional.H>
#include <x/weakcapture.H>
#include <x/mpweakptr.H>
#include <x/strtok.H>
#include <x/strftime.H>
#include <x/visitor.H>
#include <courier-unicode.h>
#include <algorithm>

LIBCXXW_NAMESPACE_START

date_input_fieldObj
::date_input_fieldObj(const ref<implObj> &impl,
		      const layout_impl &my_layout_impl)
	: focusable_containerObj{impl->handler, my_layout_impl},
	  impl{impl}
{
}

date_input_fieldObj::~date_input_fieldObj()=default;

date_input_field_config::date_input_field_config()
	: appearance{date_input_field_appearance::base::theme()},
	  invalid_input{_("Invalid date")}
{
}
date_input_field_config::~date_input_field_config()=default;

date_input_field_config
::date_input_field_config(const date_input_field_config &)=default;

date_input_field_config &
date_input_field_config::operator=(const date_input_field_config &)=default;

focusable_impl date_input_fieldObj::get_impl() const
{
	return impl->calendar_container->text_input_field->get_impl();
}

void date_input_fieldObj::do_get_impl(const function<internal_focusable_cb> &cb)
	const
{
	// Combine the two internal focusables, pretend that we are one
	// focusable object.

	std::vector<focusable> v{impl->calendar_container->text_input_field,
			impl->popup_imagebutton};

	process_focusable_impls_from_focusables(cb, v);
}

void date_input_fieldObj::set(const std::optional<ymd> &d)
{
	// The function that sets the input field is actually in the
	// container object, which also sets the input field after picking
	// a day from the calendar. Hijack it.

	auto c=impl->calendar_container;

	c->in_thread([=]
		     (ONLY IN_THREAD)
		     {
			     c->set(IN_THREAD, d, {});
		     });

}

std::optional<ymd> date_input_fieldObj::get() const
{
	return impl->calendar_container->get();
}

void date_input_fieldObj::on_change(const date_input_field_callback_t &cb)
{
	impl->calendar_container->on_change(cb);
}

/////////////////////////////////////////////////////////////////////////////

date_input_field factoryObj::create_date_input_field()
{
	return create_date_input_field({});
}

date_input_field factoryObj
::create_date_input_field(const date_input_field_config &config)
{
	auto parent_container=get_container_impl();

	auto attachedto_info=
		popup_attachedto_info::create(rectangle{},
					      attached_to::right_or_left);

	// Our container implementation object, for the input field and
	// the popup button.
	auto handler=ref<date_input_fieldObj::handlerObj>
		::create(parent_container);

	// We use the grid layout manager, of course.

	ref<gridlayoutmanagerObj::implObj> glm_impl=
		new_gridlayoutmanager{}.create(handler);

	auto glm=glm_impl->create_gridlayoutmanager();

	// Create the input field for the date.

	// We use the grid layout manager to draw the border around it
	// (config.border)...
	auto f=glm->append_row();
	f->padding(0);
	f->border(config.appearance->border);

	// Investigate the current locale date format:
	auto preferred=strftime::upreferred();

	// Now format today's date, later we'll replace all digits by
	// 'Y','m', or 'd', by parsing "preferred" ourselves. This results
	// in something like "mm/dd/YYYY" or "YYYY.mm.dd".

	auto date_format=ymd{}.format_date(preferred);

	input_field_config input_conf{date_format.size()+1};

	input_conf.appearance=config.appearance->input_appearance;

	input_conf.autoselect=true;
	input_conf.maximum_size=date_format.size();
	input_conf.direction=bidi::left_to_right;

	text_param initial;

	auto ccweak=mpweakptr<date_input_field_calendarptr>::create();

	// Validate the contents of the date input field.

	// on_validate() uses std::optional to indicate whether the input
	// passed validation, or not. If nullopt, validation failed.
	//
	// In order to allow empty input, our validated data type
	// is std::optional<ymd>, so if no date is entered it is nullopt.

	typedef std::optional<ymd> validated_type_t;

	// And the validator validates std::optional<validated_type_t>

	auto contents=create_validated_input_field_contents(
		[ccweak,
		 invalid_input_error_message=config.invalid_input]
		(ONLY IN_THREAD,
		 const std::u32string &d,
		 input_lock &lock,
		 const callback_trigger_t &trigger)
		-> std::optional<validated_type_t>
		{
			std::optional<validated_type_t> ret;

			// try_parse() what was entered.

			bool error=false;

			if (d.empty())
			{
				ret.emplace(std::nullopt);
			}
			else
			{
				ret=ymd::parser{}.try_parse(d);

				if (!ret.value())
				{
					error=true;
					lock.stop_message(
						invalid_input_error_message
					);
				}
			}

			auto &parsed_date=ret.value();

			auto cc=ccweak->getptr();

			if (!cc)
				return ret;

			cc->report_new_date(IN_THREAD, parsed_date, trigger);

			// If a valid date was entered, move the calendar
			// popup to its month.

			if (parsed_date)
				cc->update_month(*parsed_date);

			// And report the new date to the callback.

			cc->report_new_date(IN_THREAD, parsed_date, trigger);

			if (error)
				ret.reset();
			return ret;
		},
		[preferred]
		(const auto &new_date)
		{
			if (!new_date)
				return std::u32string{};

			// Canonically format the date.

			return new_date->format_date(preferred);
		});

	const auto &[text_input_field, validated_text_input_field] =
		f->create_input_field(
			contents,
			input_conf
		);

	text_input_field->show();

	auto parent_handler=ref(&parent_container->get_window_handler());

	auto attachedto_handler=
		ref<popupObj::handlerObj>::create
		(popup_handler_args{exclusive_popup_type,
				    "date_input",
				    parent_handler,
				    attachedto_info,
				    config.appearance->toplevel_appearance,
				    parent_container->container_element_impl()
				    .nesting_level+2,
				    "popup_menu,dropdown_menu",
				    "",
		});

	auto popup_impl=ref<popupObj::implObj>::create(attachedto_handler,
						       parent_handler);

	peephole_style popup_peephole_style{peephole_algorithm::automatic,
					    peephole_algorithm::automatic,
					    halign::fill};

	date_input_field_calendarptr calendar_containerptr;

	auto popup_lm=create_peephole_toplevel
		(attachedto_handler,
		 config.appearance->popup_border,
		 config.appearance->popup_background_color,
		 config.appearance->popup_background_color,
		 popup_peephole_style,

		 // Borrow input field's scrollbars for the popup's
		 // scrollbars
		 config.appearance->input_appearance->horizontal_scrollbar,
		 config.appearance->input_appearance->vertical_scrollbar,

		 [&]
		 (const container_impl &parent)
		 {
			 child_element_init_params init_params;

			 init_params.background_color=
			 config.appearance->popup_background_color;

			 auto container_impl=ref<date_input_field_calendarObj
						 ::implObj>::create
				 (parent, init_params);

			 ref<gridlayoutmanagerObj::implObj> glm_impl=
				 new_gridlayoutmanager{}.create(container_impl);

			 auto glm=glm_impl->create_gridlayoutmanager();

			 auto container=date_input_field_calendar
				 ::create(attachedto_info,
					  config.appearance,
					  container_impl,
					  glm_impl,
					  ymd{},
					  text_input_field);

			 calendar_containerptr=container;

			 ccweak->setptr(container);

			 return container;
		 });

	// Ok, time to fullfill our earlier promise by figuring out
	// the date format.

	{
		auto b=date_format.begin(), e=date_format.end();

		int Y=0,m=0,d=0;

		for (auto c:preferred)
		{
			switch (c) {
			case 'Y':
				++Y;
				break;
			case 'm':
				++m;
				break;
			case 'd':
				++d;
				break;
			default:
				continue;
			}

			// No consecutive punctuation between values.
			int nondigit=0;

			// Find where this number starts in the sample date.
			while (b != e && !unicode_isdigit(*b))
			{
				if (++nondigit > 1)
				{
					date_format.clear();
					break;
				}

				++b;
			}
			// And replace all digits by the corresponding
			// character.

			while (b != e && unicode_isdigit(*b))
			{
				*b=c;
				++b;
			}
		}

		// Sanity check, part 1.

		if (Y != 1 && m != 1 && d != 1)
			date_format.clear();
	}

	// Input field filter only if it passes the sanity check.

	if (std::count(date_format.begin(), date_format.end(), 'Y') == 4 &&
	    std::count(date_format.begin(), date_format.end(), 'm') == 2 &&
	    std::count(date_format.begin(), date_format.end(), 'd') == 2)
		text_input_field->on_filter
			([date_format, preferred,
			  me=make_weak_capture(text_input_field)]
			 (ONLY IN_THREAD,
			  const input_field_filter_info &info)
			 {
				 auto got=me.get();

				 if (!got)
					 return;

				 auto & [text_input_field]=*got;

				 size_t starting_pos=info.starting_pos->pos();
				 size_t n_delete=info.ending_pos->pos()
					 - starting_pos;
				 auto &str=info.new_contents;
				 const size_t size=info.size;

#define CURRENT_INPUT_FIELD_CONTENTS()					\
				 (input_lock{text_input_field}.get_unicode())

#include "date_input_field/date_input_field_filter.H"

				 // TODO: spurious gcc warning
				 new_string.insert(new_string.begin(),
						   unicode::literals::LRO[0]);

				 info.update(info.starting_pos
					     ->pos(starting_pos),
					     info.starting_pos
					     ->pos(starting_pos+n_delete),
					     new_string);
			 });

	auto date_picker_popup=popup::create(popup_impl, popup_lm->impl,
					     popup_lm->impl);

	auto popup_imagebutton=create_standard_popup_imagebutton
		(f, date_picker_popup,
		 {
		  config.appearance->border,
		  config.appearance->input_appearance->background_color,
		  config.appearance->popup_button_image1,
		  config.appearance->popup_button_image2,
		  config.appearance->popup_button_focus_border,
		 });

	auto impl=ref<date_input_fieldObj::implObj>
		::create(handler,
			 calendar_containerptr,
			 popup_imagebutton);

	auto dif=date_input_field::create(impl, glm_impl);


	created(dif);
	return dif;
}

LIBCXXW_NAMESPACE_END
