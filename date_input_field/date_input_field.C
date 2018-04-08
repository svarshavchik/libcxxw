/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "focus/focusable.H"
#include "date_input_field/date_input_field_impl.H"
#include "date_input_field/date_input_field_handler.H"
#include "date_input_field/date_input_field_calendar.H"
#include "popup/popup_attachedto_info.H"
#include "popup/popup_attachedto_handler.H"
#include "popup/popup_attachedto_handler_element.H"
#include "popup/popup_impl.H"
#include "popup/popup.H"
#include "popup_imagebutton.H"
#include "peephole/peephole_toplevel.H"
#include "peephole/peepholed_toplevel.H"
#include "focus/focusable.H"
#include "gridlayoutmanager.H"
#include "x/w/input_field.H"
#include "x/w/text_param_literals.H"
#include "x/w/stop_message.H"
#include "x/w/main_window.H"
#include "run_as.H"
#include "messages.H"

#include <x/functional.H>
#include <x/weakcapture.H>
#include <x/strtok.H>

LIBCXXW_NAMESPACE_START

date_input_fieldObj
::date_input_fieldObj(const ref<implObj> &impl,
		      const ref<layoutmanagerObj::implObj> &layout_impl)
	: focusable_containerObj{impl->handler, layout_impl},
	  impl{impl}
{
}

date_input_fieldObj::~date_input_fieldObj()=default;

date_input_field_config::~date_input_field_config()=default;

text_param date_input_field_config::default_invalid_input() noexcept
{
	return _("Invalid date");
}

ref<focusableObj::implObj> date_input_fieldObj::get_impl() const
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

	c->impl->get_window_handler().thread()->run_as
		([=]
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
					      attached_to::submenu_next);

	// Our container implementation object, for the input field and
	// the popup button.
	auto handler=ref<date_input_fieldObj::handlerObj>
		::create(parent_container);

	// We use the grid layout manager, of course.

	auto glm_impl=ref<gridlayoutmanagerObj::implObj>::create(handler);

	auto glm=glm_impl->create_gridlayoutmanager();

	// Create the input field for the date.

	// We use the grid layout manager to draw the border around it
	// (config.border)...
	auto f=glm->append_row();
	f->padding(0);
	f->border(config.border);

	input_field_config input_conf{11};

	input_conf.autoselect=true;
	input_conf.maximum_size=10;

	// ... and remove the border provided by the input field
	input_conf.border="empty";

	input_conf.background_color=config.background_color;

	auto text_input_field=f->create_input_field({"dateedit"_theme_font,
				"dateedit_foreground_color"_color},
		input_conf);

	text_input_field->show();

	auto parent_handler=ref(&parent_container->get_window_handler());

	auto attachedto_handler=
		ref<popup_attachedto_handlerObj>::create
		(popup_attachedto_handler_args{
			&shared_handler_dataObj::opening_exclusive_popup,
			&shared_handler_dataObj::closing_exclusive_popup,
			"date_input",
			parent_handler,
			attachedto_info,
			parent_container->container_element_impl()
				.nesting_level+2});

	attachedto_handler->set_window_type("popup_menu,dropdown_menu");

	auto popup_impl=ref<popupObj::implObj>::create(attachedto_handler,
						       parent_handler);

	peephole_style popup_peephole_style{halign::fill};

	date_input_field_calendarptr calendar_containerptr;

	auto popup_lm=create_peephole_toplevel
		(attachedto_handler,
		 config.popup_border,
		 config.popup_background_color,
		 config.popup_background_color,
		 popup_peephole_style,
		 [&]
		 (const ref<containerObj::implObj> &parent)
		 {
			 child_element_init_params init_params;

			 init_params.background_color=
			 config.popup_background_color;

			 auto container_impl=ref<date_input_field_calendarObj
			 ::implObj>::create
			 (theme_font{
				 parent->container_element_impl()
					 .label_theme_font()},
			  parent, init_params);

			 auto glm_impl=ref<gridlayoutmanagerObj::implObj>
			 ::create(container_impl);

			 auto glm=glm_impl->create_gridlayoutmanager();

			 auto container=date_input_field_calendar
			 ::create(attachedto_info,
				  container_impl,
				  glm_impl,
				  ymd{},
				  text_input_field);

			 calendar_containerptr=container;

			 return container;
		 });

	// Validate the contents of the date input field.

	text_input_field->set_validator
		([cc=make_weak_capture(calendar_containerptr),
		  invalid_input_error_message=config.invalid_input]
		 (ONLY IN_THREAD,
		  const std::u32string &d,
		  text_param &error_message,
		  const callback_trigger_t &trigger) -> std::optional<ymd>
		 {
			 std::optional<ymd> parsed_date;

			 // try_parse() what was entered.

			 if (!d.empty())
			 {
				 parsed_date=ymd::parser{}.try_parse(d);

				 if (!parsed_date)
					 error_message=
						 invalid_input_error_message;
			 }

			 auto got=cc.get();

			 if (!got)
				 return parsed_date;

			 auto &[cc]=*got;

			 cc->report_new_date(IN_THREAD, parsed_date, trigger);

			 // If a valid date was entered, move the calendar
			 // popup to its month.

			 if (parsed_date)
				 cc->update_month(*parsed_date);

			 // And report the new date to the callback.

			 cc->report_new_date(IN_THREAD, parsed_date, trigger);

			 return parsed_date;
		 },
		 []
		 (const auto &new_date)
		 {
			 // Canonically format the date.

			 return new_date.format_date(U"%x");
		 });

	auto date_picker_popup=popup::create(popup_impl, popup_lm->impl);

	auto popup_imagebutton=create_popup_imagebutton
		(f,
		 [&]
		 (const border_arg &focusoff_border,
		  const border_arg &focuson_border,
		  const ref<containerObj::implObj> &parent_container,
		  const child_element_init_params &init_params)
		 {
			 auto ff=ref<popup_attachedto_handler_elementObj
				     <popup_imagebutton_focusframe_implObj>>
				 ::create(attachedto_handler,
					  focusoff_border,
					  focuson_border,
					  parent_container,
					  init_params);

			 return ff;
		 },

		 date_picker_popup->elementObj::impl,
		 {
			 config.border,
				 config.background_color,
				 "scroll-right1",
				 "scroll-right2",
				 config.focusoff_border,
				 config.focuson_border
		 });

	auto impl=ref<date_input_fieldObj::implObj>
		::create(handler,
			 date_picker_popup,
			 calendar_containerptr,
			 popup_imagebutton);

	auto dif=date_input_field::create(impl, glm_impl);


	created(dif);
	return dif;
}

LIBCXXW_NAMESPACE_END
