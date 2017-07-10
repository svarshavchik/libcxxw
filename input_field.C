/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "editor.H"
#include "editor_impl.H"
#include "input_field.H"
#include "scrollbar/scrollbar.H"
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
		::create(container_impl);

	editorptr created_editor;

	auto elements=create_peepholed_focusable_with_frame
		("textedit_border",
		 "inputfocusoff_border",
		 "inputfocuson_border",
		 .2,
		 container_impl->get_element_impl()
		 .create_background_color
		 ("textedit_background_color"),
		 impl_mixin,
		 peephole_style(),
		 make_function<make_peepholed_func_t>
		 ([&]
		  (const auto &parent_container_impl)
		{
			auto peephole_impl=ref<editor_peephole_implObj>
			::create(parent_container_impl);

			auto e=create_editor(peephole_impl,
					     text, config);

			created_editor=e;

			// We'll make the editor element visible.

			e->show();

			return std::make_tuple(peephole_impl, e, e, e->impl);
		}),
		 scrollbar_visibility::never,
		 config.vertical_scrollbar);

	// TODO: structured bindings
	auto &peephole_info=std::get<0>(elements);
	auto &lm=std::get<1>(elements);

	auto impl=ref<input_fieldObj::implObj>
		::create(impl_mixin,
			 created_editor);

	auto input_field=input_field::create(impl,
					     peephole_info,
					     lm->impl);

	created(input_field);
	return input_field;
}

std::u32string input_fieldObj::get_unicode() const
{
	return impl->editor_element->impl->get();
}

std::string input_fieldObj::get() const
{
	return unicode::iconvert::fromu::convert(get_unicode(),
						 unicode::utf_8).first;
}

void input_fieldObj::set(const std::experimental::string_view &str)
{
	set(unicode::iconvert::tou::convert(std::string{str},
					    unicode::utf_8).first);
}

void input_fieldObj::set(const std::experimental::u32string_view &str)
{
	auto editor_impl=impl->editor_element->impl;

	editor_impl->get_window_handler().thread()->run_as
		([str=std::u32string{str}, editor_impl]
		 (IN_THREAD_ONLY)
		 {
			 editor_impl->set(IN_THREAD, str);
		 });
}

LIBCXXW_NAMESPACE_END
