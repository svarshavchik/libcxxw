/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "editor_container.H"
#include "editor.H"
#include "editor_impl.H"
#include "input_field.H"
#include "focus/focusframecontainer_element.H"
#include "background_color.H"
#include "nonrecursive_visibility.H"
#include "generic_window_handler.H"
#include "connection_thread.H"
#include "xid_t.H"
#include "x/w/input_field.H"
#include "x/w/new_layoutmanager.H"
#include "x/w/input_field_config.H"
#include "x/w/text_param.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include <courier-unicode.h>

LIBCXXW_NAMESPACE_START

input_fieldObj::input_fieldObj(const ref<implObj> &impl)
	: containerObj(impl, new_gridlayoutmanager()),
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

static focusframecontainer get_focusframecontainer(const input_fieldObj *field)
{
	const_gridlayoutmanager glm=field->get_layoutmanager();

	container inner_container=glm->get(0, 0);

	glm=inner_container->get_layoutmanager();

	return glm->get(0, 0);
}

static editor_container get_editor_container(const input_fieldObj *field)
{
	return get_focusframecontainer(field)->get_focusable();
}

static editor get_internal_editor(const input_fieldObj *field)
{
	return get_editor_container(field)->editor_element;
}

typedef nonrecursive_visibilityObj<focusframecontainer_elementObj<
					   container_elementObj<
						   child_elementObj>>
				   > focusframe_impl_t;

static inline void create_focusframe_with_editor(const auto &factory,
						 const text_param &text,
						 const input_field_config
						 &config)
{
	// Create the focusframe implementation object, first.

	auto focusframecontainer_impl=ref<focusframe_impl_t>
		::create(factory->container_impl,
			 metrics::horizvert_axi(),
			 "focusframe@libcxx",
			 factory->container_impl->get_element_impl()
			 .create_background_color
			 ("textedit_background_color",
			  {rgb::maximum, rgb::maximum,
					  rgb::maximum})
			 );

	// Now that the focusframe implementation object exists we can
	// crate the editor_container element.

	auto editor_container=create_editor_container(focusframecontainer_impl,
						      text,
						      config);

	// We can now create the focusframecontainer public object, now that
	// the implementation object, and the focusable object (the
	// editor_element in the editor_container) exist.

	auto ff=focusframecontainer
		::create(focusframecontainer_impl,
			 editor_container->editor_element->impl,
			 "inputfocusoff_border",
			 "inputfocuson_border");

	// We still need to:
	//
	// Explicitly show() the editor_container and the focusframe, since
	// the input_field has nonrecursive_visibility.
	//
	// Officially place the editor_container in focusframe's layout
	// manager, it was created_internally().
	//
	// Officially place the focusframe inside the input field's grid
	// layout manager, it was created_internally().

	gridlayoutmanager l=ff->get_layoutmanager();

	editor_container->show();
	l->append_row()->padding(.2).created_internally(editor_container);
	ff->show();

	factory->padding(0).created_internally(ff);
}

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

	auto impl=ref<input_fieldObj::implObj>
		::create(container_impl);
	auto input_field=input_field::create(impl);

	gridlayoutmanager grid=input_field->get_layoutmanager();

	auto new_container=grid->append_row()
		->padding(0).border("textedit_border")
		.create_container
		([&]
		 (const auto &container)
		 {
			 // Ok, now create a focus frame in here.

			 gridlayoutmanager grid=
				 container->get_layoutmanager();

			 create_focusframe_with_editor(grid->append_row(),
						       text,
						       config);
		 },
		 new_gridlayoutmanager());

	created(input_field);
	return input_field;
}

ref<focusableImplObj> input_fieldObj::get_impl() const
{
	return get_focusframecontainer(this)->get_impl();
}

std::u32string input_fieldObj::get_unicode() const
{
	return get_internal_editor(this)->impl->get();
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
	auto impl=get_internal_editor(this)->impl;

	impl->get_window_handler().thread()->run_as
		(RUN_AS,
		 [str=std::u32string{str}, impl]
		 (IN_THREAD_ONLY)
		 {
			 impl->set(IN_THREAD, str);
		 });
}

LIBCXXW_NAMESPACE_END
