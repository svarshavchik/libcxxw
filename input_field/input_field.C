/*
** Copyright 2017-2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "editor.H"
#include "editor_impl.H"
#include "input_field/input_field.H"
#include "input_field/input_field_search.H"
#include "input_field/input_field_search_popup_handler.H"
#include "input_field/editor_search_impl.H"
#include "x/w/impl/background_color.H"
#include "x/w/impl/container_element.H"
#include "x/w/impl/nonrecursive_visibility.H"
#include "generic_window_handler.H"
#include "connection_thread.H"
#include "peephole/peephole.H"
#include "peephole/peepholed.H"
#include "peephole/peephole_layoutmanager_impl.H"
#include "combobox/custom_comboboxlayoutmanager.H"
#include "peepholed_toplevel_listcontainer/create_popup.H"
#include "editor_peephole_impl.H"
#include "button.H"
#include "grid_map_info.H"
#include "xid_t.H"
#include "x/w/input_field.H"
#include "x/w/input_field_config.H"
#include "x/w/text_param.H"
#include "x/w/scrollbar.H"
#include "x/w/button.H"
#include "x/w/image.H"
#include "gridlayoutmanager.H"
#include "x/w/factory.H"
#include "messages.H"
#include <courier-unicode.h>

LIBCXXW_NAMESPACE_START

input_fieldObj::input_fieldObj(const ref<implObj> &impl,
			       const ref<peepholed_focusableObj::implObj>
			       &peephole_impl,
			       const layout_impl &container_layout_impl)
	: peepholed_focusableObj{peephole_impl, container_layout_impl},
	  impl{impl}
{
}

input_fieldObj::~input_fieldObj()=default;

void input_field_config
::set_spin_control_factories(const functionref<void(const factory &)> &first,
			     const functionref<void(const factory &)> &second)
{
	spin_control_factories.emplace(first, second);
}

void input_field_config
::set_default_spin_control_factories()
{
	set_spin_control_factories
		([](const auto &factory) {
			factory->create_image("spin-decrement");
		},[](const auto &factory) {
			factory->create_image("spin-increment");
		});
}

input_field_config::~input_field_config()=default;

/////////////////////////////////////////////////////////////////////
//
// The input field is a container based on the grid layout manager.
//
// Element (0, 0) in the input field is a nested grid layout manager.
//
// The nested grid layout manager's sole element has the textedit_border.
// The sole purpose of the nested grid layout manager is to have this
// non-collapsible border.
//
// The element in the text edit border is a focus frame that contains
// the editor_container element.


input_field factoryObj::create_input_field(const text_param &text)
{
	return create_input_field(text, input_field_config());
}

// Create the container that will have the editor implementation object's
// peephole.

static inline input_fieldObj::implObj::impl_mixin
create_input_field_impl_mixin(const container_impl &parent,
			      const input_field_config &config,
			      ptr<input_field_searchObj> &search_container)
{
	// If there's no search callback, this is just the impl_mixin.

	if (!config.input_field_search_callback)
		return input_fieldObj::implObj::impl_mixin::create(parent);

	if (config.password_char)
		throw EXCEPTION(_("Input field with search callbacks cannot be passwords"));

	if (config.rows > 1)
		throw EXCEPTION(_("Input field with search callbacks must have one row"));

	// Otherwise, we need to do more work, such as creating the popup.
	//
	// We are going to borrow most of the combo-box popup's code for this.

	new_listlayoutmanager style=combobox_new_listlayoutmanager(true);

	create_peepholed_toplevel_listcontainer_popup_args
		popup_args=combobox_listcontainer_popup_args(parent, style, 1);

	popup_args.popup_peephole_style.width_algorithm=dim_axis_arg{};
	popup_args.popup_peephole_style.width_truncate=true;

	// The search popup should be as tall as it wants to be.
	popup_args.popup_peephole_style.height_algorithm=
		peephole_algorithm::stretch_peephole;

	custom_combobox_popup_containerptr popup_containerptr;

	auto [combobox_popup, popup_handler]=
		create_peepholed_toplevel_listcontainer_popup
		(popup_args,
		 [&]
		 (const auto &peephole_container,
		  const popup_attachedto_info &attachedto_info)
		 {
			 return combobox_create_list(peephole_container,
						     attachedto_info,
						     style,
						     popup_containerptr);
		 },
		 [&]
		 (const peepholed_toplevel_listcontainer_handler_args &args)
		 {
			 return ref<input_field_search_popup_handlerObj>
				 ::create(args);
		 }
		 );

	auto search=ref<input_field_searchObj>::create(combobox_popup,
						       popup_handler,
						       parent);
	search_container=search;

	return search;
}

input_field
factoryObj::create_input_field(const text_param &text,
			       const input_field_config &config)
{
	// First, create the input field object. The input field object is
	// basically a grid container.
	//
	// If, however, a search_callback was specified, a subclass gets
	// created that implements the search functionality.
	ptr<input_field_searchObj> search_container;

	auto impl_mixin=
		create_input_field_impl_mixin(get_container_impl(), config,
					      search_container);

	editorptr created_editor;

	peephole_style input_field_peephole_style{halign::fill, valign::fill};

	auto [peephole_info, lm]=create_peepholed_focusable_with_frame
		({config.border,
		  config.focusoff_border,
		  config.focuson_border,
		  .2,
		  config.background_color,
		  impl_mixin,
		  input_field_peephole_style,
		  scrollbar_visibility::never,
		  config.oneline() ? scrollbar_visibility::never
		  : config.vertical_scrollbar
		},
			[&]
			(const auto &parent_container_impl,
			 const auto &peephole_container_impl)
		   {
			   auto peephole_impl=ref<editor_peephole_implObj>
				   ::create(parent_container_impl);

			   // The peephole contains the real editor element.

			   // If an optional search callback was specified,
			   // we will create an editor_search_implObj subclass.

			   editorObj::implObj::init_args args
				   {
				    peephole_impl, text, config};
			   auto e_impl=
				   search_container ?
				   ref<editorObj::implObj>
				   {
				    ref<editor_search_implObj>::create
				    (args, search_container)
				   } : ref<editorObj::implObj>::create(args);

			   auto e=editor::create(e_impl);

			   created_editor=e;

			   // We'll make the editor element visible.

			   e->show();

			   return std::make_tuple(peephole_impl, e, e, e->impl);
		   });

	auto impl=ref<input_fieldObj::implObj>
		::create(impl_mixin,
			 created_editor);


	if (config.spin_control_factories)
	{
		// Append spinner elements. In row 0 element #0 is the
		// input field, element #1 is the vertical scrollbar (usually
		// hidden). This will create elements #2 and #3, which
		// do_get_impl() checks for, below.

#define HAS_SPIN_CONTROLS() ((*grid_lock)->cols(0) >= 4)
#define SPIN_CONTROL_A() ((*grid_lock)->get(0,2))
#define SPIN_CONTROL_B() ((*grid_lock)->get(0,3))

		auto f=lm->append_columns(0);
		f->padding(0);
		// The unused horizontal scrollbar is on the 2nd row.
		f->rowspan(2);
		f->valign(valign::fill);

		child_element_init_params init_params;

		init_params.background_color=x::w::rgb{x::w::rgb::maximum,
						       0, 0};

		do_create_button_with_explicit_borders
			(*f, "empty", "empty",
			 config.border, config.border,
			 "button_spinner_normal_color",
			 "button_spinner_selected_color",
			 "button_spinner_active_color",
			 make_function<factoryObj::factory_creator_t>
			 ([c=std::get<0>(*config.spin_control_factories)]
			  (const auto &f)
			  {
				  c(f);
			  }),
			 {},
			 init_params)->show_all();

		f->padding(0);
		f->rowspan(2);
		f->valign(valign::fill);

		do_create_button_with_explicit_borders
			(*f, "empty", config.border,
			 config.border, config.border,
			 "button_spinner_normal_color",
			 "button_spinner_selected_color",
			 "button_spinner_active_color",
			 make_function<factoryObj::factory_creator_t>
			 ([c=std::get<1>(*config.spin_control_factories)]
			  (const auto &f)
			  {
				  c(f);
			  }),
			 {},
			 init_params)->show_all();
	}

	auto input_field=input_field::create(impl,
					     peephole_info,
					     lm->impl);

	created(input_field);
	return input_field;
}

void input_fieldObj::do_get_impl(const function<internal_focusable_cb> &cb)
	const
{
	containerObj::impl->invoke_layoutmanager
		([&, this]
		 (const ref<gridlayoutmanagerObj::implObj> &impl)
		 {
			 grid_map_t::lock grid_lock{impl->grid_map};

			 if (!HAS_SPIN_CONTROLS())
			 {
				 peepholed_focusableObj::do_get_impl(cb);
				 return;
			 }

			 // Additinal spinner elements, created above.

			 focusable a=SPIN_CONTROL_A(),
				 b=SPIN_CONTROL_B();

			 // Recursively invoke do_get_impl from:
			 //
			 // our superclass, a, and b
			 //
			 // Helper macros invokes each one, executing "code".

#define GRAB_FOCUSABLE_GROUPS_FROM(source,name,code)			\
			 source(make_function<internal_focusable_cb>	\
				 ([&]					\
				 (const auto &name)			\
			 {						\
				 do {					\
					 code				\
						 } while (0);		\
			 }));

			 // And, once all three are grabbed, we combined
			 // their focusables, and invoke the real callback.

#define DO_WITH_GRABBED_FOCUSABLES()					\
			 std::vector<focusable_impl> impls;	\
									\
			 impls.reserve(group1.internal_impl_count+	\
				       group2.internal_impl_count+	\
				       group3.internal_impl_count);	\
									\
			 impls.insert(impls.end(),			\
				      group1.impls,			\
				      group1.impls+			\
				      group1.internal_impl_count);	\
									\
			 impls.insert(impls.end(),			\
				      group2.impls,			\
				      group2.impls+			\
				      group2.internal_impl_count);	\
									\
			 impls.insert(impls.end(),			\
				      group3.impls,			\
				      group3.impls+			\
				      group3.internal_impl_count);	\
									\
			 internal_focusable_group combined{		\
				 impls.size(),				\
					 &*impls.begin()		\
					 };				\
									\
			 cb(combined);

			 // And now, put the jigsaw puzzle together.

			 GRAB_FOCUSABLE_GROUPS_FROM
				 (peepholed_focusableObj::do_get_impl,
				  group1,
				  GRAB_FOCUSABLE_GROUPS_FROM
				  (a->do_get_impl, group2,
				   GRAB_FOCUSABLE_GROUPS_FROM
				   (b->do_get_impl, group3,
				    DO_WITH_GRABBED_FOCUSABLES())))
				 });

}

void input_fieldObj::on_spin(const hotspot_callback_t &a_cb,
			     const hotspot_callback_t &b_cb)
{
	containerObj::impl->invoke_layoutmanager
		([&]
		 (const ref<gridlayoutmanagerObj::implObj> &impl)
		 {
			 grid_map_t::lock grid_lock{impl->grid_map};

			 if (!HAS_SPIN_CONTROLS())
			 {
				 throw EXCEPTION(_("Input field does not have spin controls."));
			 }

			 button a=SPIN_CONTROL_A(),
				 b=SPIN_CONTROL_B();

			 a->on_activate(a_cb);
			 b->on_activate(b_cb);
		 });
}


void input_fieldObj::set(const std::string_view &str)
{
	set(unicode::iconvert::tou::convert(std::string{str},
					    unicode_locale_chset()).first);
}

void input_fieldObj::set(const std::u32string_view &str)
{
	auto editor_impl=impl->editor_element->impl;

	editor_impl->get_window_handler().thread()->run_as
		([str=std::u32string{str}, editor_impl]
		 (ONLY IN_THREAD)
		 {
			 editor_impl->set(IN_THREAD, str);
		 });
}

void input_fieldObj::on_change(const functionref<
			       void(THREAD_CALLBACK,
				    const input_change_info_t &)> &callback)
{
	auto editor_impl=impl->editor_element->impl;

	editor_impl->get_window_handler().thread()->run_as
		([callback, editor_impl]
		 (ONLY IN_THREAD)
		 {
			 editor_impl->on_change(IN_THREAD)=callback;
		 });
}


void input_fieldObj::on_autocomplete(const functionref<bool
				     (THREAD_CALLBACK,
				      input_autocomplete_info_t &)>
				     &callback)
{
	auto editor_impl=impl->editor_element->impl;

	editor_impl->get_window_handler().thread()->run_as
		([callback, editor_impl]
		 (ONLY IN_THREAD)
		 {
			 editor_impl->on_autocomplete(IN_THREAD)=callback;
		 });
}

void input_fieldObj::on_validate(const
				 functionref<input_field_validation_callback_t
				 > &callback)
{
	auto editor_impl=impl->editor_element->impl;

	editor_impl->get_window_handler().thread()->run_as
		([callback, editor_impl]
		 (ONLY IN_THREAD)
		 {
			 editor_impl->validation_callback(IN_THREAD)=callback;
		 });
}

ref<elementObj::implObj> input_fieldObj::get_minimum_override_element_impl()
{
	return impl->editor_element->impl;
}

LIBCXXW_NAMESPACE_END
