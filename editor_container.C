/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "editor_container.H"
#include "editor_container_impl.H"
#include "editor.H"
#include "peephole/peephole.H"
#include "peephole/peephole_impl.H"
#include "peephole/peephole_layoutmanager_impl.H"
#include "container_element.H"
#include "fonts/current_fontcollection.H"
#include "x/w/factory.H"
#include "scrollbar/scrollbar.H"

LIBCXXW_NAMESPACE_START

std::tuple<editor_container, scrollbar, scrollbar>
create_editor_container(const ref<containerObj::implObj> &parent_container,
			const ref<containerObj::implObj> &scrollbars_container,
			const text_param &initial_contents,
			const input_field_config &config)
{
	// First, the implementation object for the editor container.

	auto impl=ref<editor_containerObj::implObj>
		::create(parent_container,
			 metrics::horizvert_axi(),
			 "textedit-peephole@libcxx");

	// Create the editor element.
	auto editor=create_editor(impl, initial_contents, config);

	// We can now create our layout manager, and give it the created
	// editor.

	auto scrollbars=create_peephole_scrollbars(scrollbars_container);

	auto layout_impl=ref<editor_containerObj::layoutmanager_implObj>
		::create(impl, editor,
			 scrollbars,
			 scrollbar_visibility::never,
			 config.vertical_scrollbar);

	layout_impl->initialize_scrollbars();

	// In order to properly initialize the editor element, the layout
	// manager needs_recalculation(). Arrange to invoke it indirectly
	// by constructing the layout manager public object, which will
	// take care of calling needs_recalculation().

	auto public_layout=layout_impl->create_public_object();

	// We'll make the editor element visible.
	//
	// The implementation object overrides request_visibility_recursive().

	editor->show();

	// Ok, we can now create the container.
	return {editor_container::create(editor, impl, layout_impl),
			scrollbars.horizontal_scrollbar,
			scrollbars.vertical_scrollbar};
}

editor_containerObj::editor_containerObj(const editor &editor_element,
					 const ref<implObj> &impl,
					 const ref<layoutmanager_implObj>
					 &layout_impl)
	: peepholeObj(impl, layout_impl),
	  editor_element(editor_element)
{
}

editor_containerObj::~editor_containerObj()=default;

LIBCXXW_NAMESPACE_END
