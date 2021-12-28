/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "font_picker/font_picker_impl.H"
#include "font_picker/font_picker_preview_impl.H"
#include "textlabel.H"
#include "label_element.H"
#include "generic_window_handler.H"
#include "screen_positions_impl.H"
#include "x/w/font_picker_config.H"
#include "x/w/font_picker_appearance.H"
#include "x/w/element_popup_appearance.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/shortcut.H"
#include "x/w/factory.H"
#include "x/w/input_field.H"
#include "x/w/impl/container.H"
#include "x/w/impl/richtext/richtext.H"
#include "x/w/metrics/axis.H"
#include "x/w/dialogfwd.H"
#include "x/w/font.H"
#include "x/w/synchronized_axis.H"
#include "x/w/standard_comboboxlayoutmanager.H"
#include "x/w/editable_comboboxlayoutmanager.H"
#include "x/w/impl/focus/focusable.H"
#include "x/w/impl/canvas.H"
#include "x/w/impl/nonrecursive_visibility.H"
#include "x/w/impl/always_visible.H"
#include "x/w/impl/always_visible_element.H"

#include "peephole/peepholed_attachedto_container_impl.H"
#include "peephole/peephole_layoutmanager_impl_scrollbars.H"
#include "peephole/peephole.H"
#include "peephole/peephole_impl_element.H"

#include "popup/popup_attachedto_element.H"
#include "messages.H"
#include "dialog.H"
#include "x/w/uielements.H"
#include "gridlayoutmanager.H"
#include <fontconfig/fontconfig.h>
#include <x/mpweakptr.H>
#include <x/xml/xpath.H>
#include <charconv>
LIBCXXW_NAMESPACE_START

font_picker_config_appearance::font_picker_config_appearance()
	: appearance{font_picker_appearance::base::theme()}
{
}

font_picker_config_appearance::~font_picker_config_appearance()=default;

font_picker_config_appearance
::font_picker_config_appearance(const font_picker_config_appearance &)=default;

font_picker_config_appearance &
font_picker_config_appearance::operator=(const font_picker_config_appearance &)
=default;

font_pickerObj::font_pickerObj(const ref<implObj> &impl,
			       const ref<containerObj::implObj> &c_impl,
			       const layout_impl &container_layoutmanager)
	: focusable_containerObj{c_impl, container_layoutmanager},
	  impl{impl}
{
}

font_pickerObj::~font_pickerObj()=default;

focusable_impl font_pickerObj::get_impl() const
{
	return impl->popup_button->get_impl();
}

void font_picker_config::restore(const const_screen_positions &pos,
				 const std::string_view &name_arg)
{
	name=name_arg;

	if (name.empty())
		return;

	try
	{
		auto lock=pos->impl->data->readlock();

		if (!lock->get_root())
			return;

		auto xpath=lock->get_xpath(saved_element_to_xpath("font",
								  name_arg));

		if (xpath->count() != 1)
			return;
		xpath->to_node();

		{
			auto font=lock->clone();

			auto xpath2=font->get_xpath("font");

			if (xpath2->count() == 1)
			{
				xpath2->to_node();

				initial_font=font->get_text();
			}
		}

		xpath=lock->get_xpath("recent");

		size_t n=xpath->count();
		most_recently_used.clear();
		most_recently_used.reserve(n);

		for (size_t i=1; i <= n; ++i)
		{
			xpath->to_node(i);

			auto value=lock->clone();

			auto xpath2=value->get_xpath("family");

			font_picker_group_id mru;

			if (xpath2->count() == 1)
			{
				xpath2->to_node();

				mru.family=value->get_text();

				value=lock->clone();

				xpath2=value->get_xpath("foundry");

				if (xpath2->count() == 1)
				{
					xpath2->to_node();
					mru.foundry=value->get_text();
				}
			}
			most_recently_used.push_back(mru);
		}
		return;
	} catch (const exception &e)
	{
		auto ee=EXCEPTION( "Error restoring font \""
				   << name << "\": " << e );

		ee->caught();
	}

	initial_font.reset();
	most_recently_used.clear();
}

//////////////////////////////////////////////////////////////////////////

namespace {
#if 0
}
#endif


//! Custom placeholder implementation object

//! Overrides the standard text label implementation object. The text label's
//! metrics are overridden, and are forced to be the same as the combo-box's
//! label's size, in the popup.
//!
//! And as long as we have to override everything, here, might as well
//! take care of the initial visible with always_visible_elementObj.
//!
//! This is also a convenient place to store the font picker's name,
//! and implement save().

class LIBCXX_HIDDEN current_font_placeholderObj :
	public always_visible_elementObj<label_elementObj<child_elementObj>> {

	typedef always_visible_elementObj<
		label_elementObj<child_elementObj>> superclass_t;

 public:
	mpobj<std::pair<metrics::axis, metrics::axis>> overridden_metrics;

	const std::string name;
	const font_pickerObj::implObj::current_state state;

	current_font_placeholderObj(const container_impl &parent_container,
				    textlabel_config &label_config,
				    const font_picker_config &config,
				    const font_pickerObj::implObj
				    ::current_state &state)
		: superclass_t{parent_container, "", label_config},
		name{config.name}, state{state}
	{
	}

	~current_font_placeholderObj()=default;

	void save(ONLY IN_THREAD, const screen_positions &) override;

 protected:
	//! Override calculate_current_metrics

	std::pair<metrics::axis, metrics::axis>
		calculate_current_metrics() override
	{
		return overridden_metrics.get();
	}
};

void current_font_placeholderObj::save(ONLY IN_THREAD,
				       const screen_positions &pos)
{
	superclass_t::save(IN_THREAD, pos);

	if (name.empty())
		return;

	std::vector<std::string> hierarchy;

	get_window_handler().window_id_hierarchy(hierarchy);

	auto writelock=pos->impl->create_writelock_for_saving(
		hierarchy, libcxx_uri, "font", name);

	writelock->create_child()->element({"font"})
		->text(static_cast<std::string>
		       (state->official_font.get().official_font))
		->parent()->parent();

	mpobj<std::vector<font_picker_group_id>>::lock
		lock{state->validated_most_recently_used};

	for (const auto &font:*lock)
	{
		writelock->create_child()->element({"recent"})
			->element({"family"})->text(font.family)
			->parent()->parent()
			->element({"foundry"})->text(font.foundry)
			->parent()->parent()->parent();
	}
}

//! Helper object for creating the font picker.

//! The create_elements() method returns the standard_dialog_elements_t
//! parameter that gets passed to initialize_theme_picker(), to populate
//! the contents of the font picker.

struct LIBCXX_HIDDEN font_picker_init_helper
	: public font_picker_popup_fieldsptr {

	ptr<current_font_placeholderObj> current_font_shown_impl;
	labelptr current_font_shown;
	standard_dialog_elements_t create_elements(const font_picker_config &);
};

#if 0
{
#endif
}

std::tuple<container, font_picker_preview>
create_font_picker_preview(const factory &f, const font_picker_config &config)
{
	// The font preview element is a peephole
	// with a text label inside it.

	// First, create the implementation object
	// for a container with a grid layout manager.
	// The grid contains a peephole, and its
	// scrollbars.
	//
	// Need to use the nonrecursive_visibilityObj
	// mixin, to protect the show_all() on the
	// entire popup container from messing with
	// the scrollbars.
	//
	// We'll use always_visibleObj with the
	// peephole and the label elements, to make
	// them visible, and the scrollbars will be
	// their supporting code's responsibility.

	auto preview_container_impl=
		ref<nonrecursive_visibilityObj<
			container_elementObj<
				child_elementObj>>>
		::create(f->get_container_impl());

	peephole_style pstyle{peephole_algorithm::automatic,
			      peephole_algorithm::automatic,
			      halign::center,
			      valign::middle};

	// Create a container for the peephole.
	//
	// The peephole container has fixed width
	// and height, for which we conveniently
	// borrow the canvas implementation object.

	auto preview_peep_container_impl=
		ref<always_visibleObj<
			peephole_impl_elementObj<
				container_elementObj<
					canvasObj::implObj>>>>
		::create
		(preview_container_impl,
		 canvas_init_params{
			config.appearance->preview_width,
				config.appearance->preview_height,
				"font_picker_preview@libcxx.com"
					});

	// The implementation object for the element
	// in the peephole.

	label_config font_picker_label_config;

	font_picker_label_config.alignment=halign::center;

	textlabel_config internal_config{font_picker_label_config};

	auto label_impl=
		ref<font_picker_previewObj::implObj>
		::create(preview_peep_container_impl, internal_config);

	auto new_preview_label=
		font_picker_preview::create(label_impl);

	// We can now construct the peephole+scrollbars
	// layout manager.
	const auto &[layout_impl, glm_impl, grid]=
		create_peephole_with_scrollbars
		([&]
		 (const auto &info, const auto &scrollbars)
		 {
			 return ref<peepholelayoutmanagerObj::implObj
				    ::scrollbarsObj>
				 ::create(info, scrollbars,
					  preview_peep_container_impl,
					  new_preview_label);
		 },
		 [&]
		 (const ref<peepholelayoutmanagerObj::implObj> &peephole_lm)
		 -> peephole_element_factory_ret_t
		 {
			 // The container public object, for the
			 // peephole.

			 auto preview_peep_container=
				 peephole::create
				 (preview_peep_container_impl,
				  peephole_lm);


			 return {
				 preview_peep_container,
				 preview_peep_container,
				 config.appearance->preview_border,
				 std::nullopt,
				 {},
			 };

		 },
		 create_peephole_gridlayoutmanager,
		 {
		  preview_container_impl,
		  std::nullopt,
		  pstyle,
		  scrollbar_visibility::automatic_reserved,
		  scrollbar_visibility::automatic_reserved,
		  config.appearance->preview_horizontal_scrollbar,
		  config.appearance->preview_vertical_scrollbar,
		 });

	// And, finally, we can create the container
	// that represents this preview element.
	//
	// This should really be a focusable_container
	// but we will never need to use it, so don't
	// even bother.
	auto c=container::create(preview_container_impl,
				 glm_impl);

	f->created_internally(c);

	return {c, new_preview_label};
}

standard_dialog_elements_t font_picker_init_helper
::create_elements(const font_picker_config &config)
{
	auto same_width=synchronized_axis::create();

	return {
		{"ok", dialog_ok_button(config.ok_label,
					ok_button, '\n')},
		{"cancel", dialog_cancel_button(config.cancel_label,
						cancel_button,
						'\e')},
		{"font-preview", [&, this](const auto &f)
			{
				const auto &[c, label] =
					create_font_picker_preview(f, config);
				preview_label=label;
			}},
	};
}

text_param font_picker_config_settings::default_ok_label()
{
	return _("Ok");
}

text_param font_picker_config_settings::default_cancel_label()
{
	return _("Cancel");
}

font_picker factoryObj::create_font_picker()
{
	return create_font_picker({});
}

font_picker factoryObj::create_font_picker(const font_picker_config &config)
{
	peepholed_attachedto_container_implptr popup_container_impl;

	font_picker_init_helper helper;

	auto initial_state=font_pickerObj::implObj::current_state::create();


	//! Weakly-captured font picker implementation object.

	//! A bunch of callbacks need it. We'll weakly-capture it once, and
	//! recycle this in every callback.

	auto wimpl=mpweakptr<ptr<font_pickerObj::implObj>>::create();

	auto [real_impl, popup_imagebutton, glm, font_picker_popup]
		=create_popup_attachedto_element
		(get_container_impl(),
		 config.appearance->attached_popup_appearance,

		 [&](const container_impl &parent,
		     const child_element_init_params &params)
		 {
			 auto impl=peepholed_attachedto_container_impl
			 ::create(parent, params);

			 popup_container_impl=impl;

			 return impl;
		 },

		 [&](const popup_attachedto_info &info,
		     const ref<gridlayoutmanagerObj::implObj> &lm_impl)
		 {
			 auto glm=lm_impl->create_gridlayoutmanager();

			 uielements tmpl{helper.create_elements(config)};

			 tmpl.create_string_validated_input_field<unsigned>(
				 "font-size-combobox",
				 [selection_required=config.selection_required,
				  wimpl]
				 (ONLY IN_THREAD,
				  const std::string &value,
				  std::optional<unsigned> &parsed_value,
				  const auto &my_field,
				  const auto &trigger)
				 {
					 auto font_picker_impl=wimpl->getptr();

					 if (!font_picker_impl)
						 return;

					 auto &font_size_error=font_picker_impl
						 ->popup_fields
						 .font_size_error;

					 if (selection_required &&
					     (!parsed_value ||
					      *parsed_value == 0))
					 {
						 font_size_error->update(
							 value.empty() ?
							 _("Font size required")
							 :
							 _("Invalid size")
						 );
						 parsed_value.reset();
						 return;
					 }

					 if (!parsed_value)
					 {
						 if (value.empty())
						 {
							 parsed_value=0;
							 return;
						 }

						 font_size_error->update(
							 _("Invalid size"));
						 parsed_value.reset();
						 return;
					 }

					 if (*parsed_value > 999)
					 {
						 font_size_error->update(
							 _("Maximum point size"
							   " is 999"));
						 parsed_value.reset();
						 return;
					 }

					 font_size_error->update(" ");
				 },
				 []
				 (unsigned v) -> std::string
				 {
					 if (v == 0)
						 return "";

					 char buffer[8];

					 auto res=std::to_chars(
						 buffer,
						 buffer+sizeof(buffer),
						 v);

					 if (res.ec != std::errc{})
						 // Shouldn't happen.
					 {
						 return "";
					 }

					 return {buffer, res.ptr};
				 },
				 std::nullopt,
				 [wimpl]
				 (ONLY IN_THREAD,
				  const std::optional<unsigned> &v)
				 {
					 if (!v)
						 return;

					 auto impl=wimpl->getptr();

					 if (!impl)
						 return;

					 impl->update_font_size(IN_THREAD, *v);
				 }
			 );

			 glm->generate("font-picker-popup", tmpl);

			 helper.font_family=
				 tmpl.get_element("font-family-combobox");
			 helper.font_size=
				 tmpl.get_element("font-size-combobox");
			 helper.font_size_validated=
				 tmpl.get_validated_input_field<unsigned>(
					 "font-size-combobox"
				 );
			 helper.font_weight=
				 tmpl.get_element("font-weight-combobox");
			 helper.font_slant=
				 tmpl.get_element("font-slant-combobox");
			 helper.font_width=
				 tmpl.get_element("font-width-combobox");

			 helper.font_size_error=
				 tmpl.get_element("font-size-error");
			 return peepholed_attachedto_container
			 ::create(info, popup_container_impl, lm_impl);
		 },
		 [&](const auto &f)
		 {
			 label_config l_config;

			 l_config.alignment=halign::center;

			 textlabel_config internal_config{l_config};

			 auto label_impl=ref<current_font_placeholderObj>
				 ::create(f->get_container_impl(),
					  internal_config,
					  config, initial_state);

			 auto l=label::create(label_impl, label_impl);

			 helper.current_font_shown_impl=label_impl;
			 helper.current_font_shown=l;
			 f->created_internally(l);
		 });

	font_picker_impl_init_params init_params{
		popup_imagebutton,
		helper.current_font_shown,
		helper,
		config,
		initial_state};

	auto font_picker_impl=ref<font_pickerObj::implObj>::create(init_params);

	auto fp=font_picker::create(font_picker_impl, real_impl, glm->impl);

	popup_imagebutton->
		in_thread([font_picker_impl]
			  (ONLY IN_THREAD)
			  {
				  font_picker_impl->compute_new_preview
					  (IN_THREAD);
			  });

	wimpl->setptr(font_picker_impl);

	// Call update_font_family() when the font family combo-box value
	// changes.

	standard_comboboxlayoutmanager ff_lm=
		font_picker_impl->popup_fields.font_family->get_layoutmanager();

	ff_lm->on_selection_changed
		([wimpl]
		 (ONLY IN_THREAD, const auto &info)
		 {
			 // We autoselect() the font family combo-box,
			 // ignore this.
			 if (info.list_item_status_info.trigger.index()==0)
				 return;

			 if (!info.list_item_status_info.selected)
				 return;

			 auto impl=wimpl->getptr();

			 if (!impl)
				 return;

			 impl->update_font_family
				 (IN_THREAD,
				  info.list_item_status_info.layout_manager);
		 });

	// Update the metrics for the currently selected font to
	// be the same as the current selection label element's, from the
	// font family combo-box.
	ff_lm->custom_comboboxlayoutmanagerObj::current_selection()->on_metrics_update
		([current_selection_placeholder=
		  ref<current_font_placeholderObj>
		  {helper.current_font_shown_impl}]
		 (ONLY IN_THREAD,
		  const metrics::axis &h,
		  const metrics::axis &v)
		 {
			 current_selection_placeholder
				 ->text->minimum_width_override(IN_THREAD,
								h.minimum());

			 current_selection_placeholder->overridden_metrics=
				 std::make_pair(h, v);

			 current_selection_placeholder->get_horizvert(IN_THREAD)
				 ->set_element_metrics(IN_THREAD, h, v);
		 });

	// Call update_font_size() when the font size value changes

	// Validator for the font size editable combo-box.

	// Need the initial point size shown?
	if (config.selection_required || config.initial_font)
		helper.font_size_validated->set(
			initial_state->official_font.get().saved_font_size
		);

	font_picker_impl->finish_initialization(fp);

	// Install selection_changed() callback for all options

	for (const auto &f:font_picker_int_option_infos)
	{
		standard_comboboxlayoutmanager{
			(font_picker_impl->popup_fields.*(f.value_combobox))
				->get_layoutmanager()}
		->on_selection_changed
			  ([wimpl, fptr=&f]
			   (ONLY IN_THREAD, const auto &info)
			   {
				   if (!info.list_item_status_info.selected)
					   return;

				   // Ignore the blowback from autoselect(),
				   // etc...
				   if (info.list_item_status_info.trigger
				       .index()==0)
					   return;

				   auto impl=wimpl->getptr();

				   if (!impl)
					   return;

				   impl->updated_font_option(IN_THREAD,
							     *fptr,
							     info.lm);
		   });
	}
	/////////////////////////////////////////////////////////////////////

	// If the popup gets closed for any reason, invoke popup_closed().
	font_picker_popup->on_state_update
		([wimpl]
		 (ONLY IN_THREAD,
		  const element_state &new_state,
		  const busy &mcguffin)
		 {
			 if (new_state.state_update !=
			     new_state.before_hiding)
				 return;

			 auto impl=wimpl->getptr();

			 if (impl)
				 impl->popup_closed(IN_THREAD);
		 });

	// The cancel button closes the popup.
	font_picker_impl->popup_fields.cancel_button->on_activate
		([popup=make_weak_capture(font_picker_popup)]
		 (ONLY IN_THREAD,
		  const auto &trigger,
		  const auto &busy)
		 {
			 auto got=popup.get();

			 if (!got)
				 return;

			 auto &[popup]=*got;

			 popup->elementObj::impl->request_visibility(IN_THREAD,
								     false);
		 });

	//! The ok button calls set_official_font(), then closes the popup.
	font_picker_impl->popup_fields.ok_button->on_activate
		([wimpl, popup=make_weak_capture(font_picker_popup)]
		 (ONLY IN_THREAD,
		  const auto &trigger,
		  const auto &busy)
		 {
			 auto got=popup.get();

			 if (!got)
				 return;

			 auto &[popup]=*got;

			 auto impl=wimpl->getptr();

			 popup->elementObj::impl->request_visibility(IN_THREAD,
								     false);
			 if (impl)
				 impl->set_official_font(IN_THREAD, trigger);
		 });

	created(fp);

	fp->label_for(fp);
	return fp;
}

void font_pickerObj::on_font_update(const functionref<font_picker_callback_t>
				    &cb)
{
	in_thread([cb, impl=impl]
		  (ONLY IN_THREAD)
		  {
			  impl->callback(IN_THREAD)=cb;

			  auto official_font_value=impl->state
				  ->official_font.get();

			  impl->invoke_callback(IN_THREAD,
						official_font_value,
						initial{});
		  });
}

font font_pickerObj::current_font() const
{
	auto official_font_value=impl->state->official_font.get();

	impl->adjust_font_for_callback(official_font_value);

	return official_font_value.official_font;
}

void font_pickerObj::current_font(const font &f)
{
	in_thread([me=ref(this), f]
		  (ONLY IN_THREAD)
		  {
			  me->current_font(IN_THREAD, f);
		  });
}

void font_pickerObj::current_font(ONLY IN_THREAD, const font &f)
{
	impl->set_font(IN_THREAD, f);
}

void font_pickerObj::font_pickerObj
::most_recently_used(const std::vector<font_picker_group_id> &mru)
{
	in_thread([mru, impl=impl]
		  (ONLY IN_THREAD)
		  {
			  impl->most_recently_used(IN_THREAD, mru);
		  });
}

void font_pickerObj::font_pickerObj
::most_recently_used(ONLY IN_THREAD,
		     const std::vector<font_picker_group_id> &mru)
{
	impl->most_recently_used(IN_THREAD, mru);
}

std::vector<font_picker_group_id> font_pickerObj::font_pickerObj
::most_recently_used() const
{
	return impl->state->validated_most_recently_used.get();
}

LIBCXXW_NAMESPACE_END
