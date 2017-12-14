/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "editor.H"
#include "editor_impl.H"
#include "input_field.H"
#include "background_color.H"
#include "container_element.H"
#include "nonrecursive_visibility.H"
#include "generic_window_handler.H"
#include "connection_thread.H"
#include "peephole/peephole.H"
#include "peephole/peepholed.H"
#include "peephole/peephole_layoutmanager_impl.H"
#include "editor_peephole_impl.H"
#include "xid_t.H"
#include "x/w/input_field.H"
#include "x/w/input_field_config.H"
#include "x/w/text_param.H"
#include "x/w/scrollbar.H"
#include "gridlayoutmanager.H"
#include "x/w/factory.H"
#include <courier-unicode.h>

LIBCXXW_NAMESPACE_START

input_fieldObj::input_fieldObj(const ref<implObj> &impl,
			       const ref<peepholed_focusableObj::implObj>
			       &peephole_impl,
			       const ref<layoutmanagerObj::implObj>
			       &layout_impl)
	: peepholed_focusableObj(peephole_impl, layout_impl),
	  impl(impl)
{
}

input_fieldObj::~input_fieldObj()=default;

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

input_field
factoryObj::create_input_field(const text_param &text,
			       const input_field_config &config)
{
	// First, create the input field object. The input field object is
	// basically a grid container.

	auto impl_mixin=input_fieldObj::implObj::impl_mixin
		::create(get_container_impl());

	editorptr created_editor;

	peephole_style input_field_peephole_style{halign::fill};

	auto [peephole_info, lm]=create_peepholed_focusable_with_frame
		({"textedit_border",
				"inputfocusoff_border",
				"inputfocuson_border",
				.2,
				get_element_impl()
				.create_background_color
				("textedit_background_color"),
				impl_mixin,
				input_field_peephole_style,
				scrollbar_visibility::never,
				config.oneline() ? scrollbar_visibility::never
				: config.vertical_scrollbar},
		 [&]
		 (const auto &parent_container_impl)
		 {
			auto peephole_impl=ref<editor_peephole_implObj>
			::create(parent_container_impl);


			auto e_impl=ref<editorObj::implObj>
			::create(peephole_impl, text, config);

			auto e=editor::create(e_impl);

			created_editor=e;

			// We'll make the editor element visible.

			e->show();

			return std::make_tuple(peephole_impl, e, e, e->impl);
		 });

	auto impl=ref<input_fieldObj::implObj>
		::create(impl_mixin,
			 created_editor);

	auto input_field=input_field::create(impl,
					     peephole_info,
					     lm->impl);

	created(input_field);
	return input_field;
}

void input_fieldObj::set(const std::string_view &str)
{
	set(unicode::iconvert::tou::convert(std::string{str},
					    unicode::utf_8).first);
}

void input_fieldObj::set(const std::u32string_view &str)
{
	auto editor_impl=impl->editor_element->impl;

	editor_impl->get_window_handler().thread()->run_as
		([str=std::u32string{str}, editor_impl]
		 (IN_THREAD_ONLY)
		 {
			 editor_impl->set(IN_THREAD, str);
		 });
}

void input_fieldObj::on_change(const std::function<
			       void(const input_change_info_t &)> &callback)
{
	auto editor_impl=impl->editor_element->impl;

	editor_impl->get_window_handler().thread()->run_as
		([callback, editor_impl]
		 (IN_THREAD_ONLY)
		 {
			 editor_impl->on_change(IN_THREAD)=callback;
		 });
}


void input_fieldObj::on_autocomplete(const std::function<bool
				     (input_autocomplete_info_t &)>
				     &callback)
{
	auto editor_impl=impl->editor_element->impl;

	editor_impl->get_window_handler().thread()->run_as
		([callback, editor_impl]
		 (IN_THREAD_ONLY)
		 {
			 editor_impl->on_autocomplete(IN_THREAD)=callback;
		 });
}

ref<elementObj::implObj> input_fieldObj::get_minimum_override_element_impl()
{
	return impl->editor_element->impl;
}

LIBCXXW_NAMESPACE_END
