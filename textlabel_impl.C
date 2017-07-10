/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "textlabel.H"
#include "label_element.H"
#include "element_screen.H"
#include "generic_window_handler.H"
#include "screen.H"
#include "connection_thread.H"
#include "richtext/richtext.H"
#include "richtext/richtextmeta.H"
#include "richtext/richtext_draw_info.H"
#include "background_color.H"
#include "messages.H"
#include "defaulttheme.H"
#include "x/w/factory.H"
#include "x/w/label.H"

LIBCXXW_NAMESPACE_START

label factoryObj::create_label(const text_param &text,
			       halign alignment)
{
	return create_label(text, 0, alignment);
}

label factoryObj::create_label(const text_param &text,
			       double widthmm,
			       halign alignment)
{
	auto label_impl=ref<label_elementObj<child_elementObj>>
		::create(container_impl, text, alignment, widthmm);

	auto l=label::create(label_impl, label_impl);

	created(l);

	return l;
}

textlabelObj::implObj::implObj(const text_param &text,
			       halign alignment,
			       double initial_width,
			       elementObj::implObj &element_impl)
	: implObj(text,
		  {element_impl.create_background_color
				  ("label_foreground_color"),
				  element_impl.create_theme_font
				  (element_impl.label_theme_font())},
		  alignment,
		  initial_width,
		  element_impl)
{
}

textlabelObj::implObj::implObj(const text_param &text,
			       const richtextmeta &default_meta,
			       halign alignment,
			       double initial_width,
			       elementObj::implObj &element_impl)
	: implObj(alignment, initial_width,
		  element_impl.create_richtextstring
		  (default_meta, text),
		  default_meta)
{
}

textlabelObj::implObj::implObj(halign alignment,
			       double initial_width,
			       const richtextstring &string,
			       const richtextmeta &default_meta)
	: word_wrap_widthmm_thread_only(initial_width),
	  text(richtext::create(string, alignment, 0)),
	  default_meta(default_meta)
{
	if (initial_width < 0)
		throw EXCEPTION(_("Label width cannot be negative"));
}

textlabelObj::implObj::~implObj()=default;

void textlabelObj::implObj::update(const text_param &string)
{
	get_label_element_impl().
		get_window_handler().screenref->impl->thread->run_as
		(RUN_AS,
		 [me=ref<implObj>(this), string]
		 (IN_THREAD_ONLY)
		 {
			 me->update(IN_THREAD, string);
		 });
}

void textlabelObj::implObj::update(IN_THREAD_ONLY, const text_param &string)
{
	text->set(IN_THREAD,
		  get_label_element_impl()
		  .create_richtextstring(default_meta, string));
	updated(IN_THREAD);
	get_label_element_impl().schedule_redraw(IN_THREAD);
}

void textlabelObj::implObj::compute_preferred_width(IN_THREAD_ONLY)
{
	auto screen=get_label_element_impl()
		.get_screen()->impl;
	preferred_width=0;
	if (word_wrap_widthmm(IN_THREAD) == 0)
		return;

	preferred_width=(*current_theme_t::lock{screen->current_theme})
		->compute_width(word_wrap_widthmm(IN_THREAD));

	rewrap(IN_THREAD);

	preferred_width=text->get_width(IN_THREAD);
}

void textlabelObj::implObj::initialize(IN_THREAD_ONLY)
{
	compute_preferred_width(IN_THREAD);

	updated(IN_THREAD);
}

void textlabelObj::implObj::updated(IN_THREAD_ONLY)
{
	// We can now compute and set our initial metrics.

	auto metrics=calculate_current_metrics(IN_THREAD);

	get_label_element_impl().get_horizvert(IN_THREAD)
		->set_element_metrics(IN_THREAD,
				      metrics.first, metrics.second);
}

void textlabelObj::implObj::theme_updated(IN_THREAD_ONLY,
				      const defaulttheme &new_theme)
{
	text->theme_updated(IN_THREAD, new_theme);
	compute_preferred_width(IN_THREAD);
	recalculate(IN_THREAD);
}

void textlabelObj::implObj::process_updated_position(IN_THREAD_ONLY)
{
	rewrap_due_to_updated_position(IN_THREAD);
}

void textlabelObj::implObj::set_inherited_visibility(IN_THREAD_ONLY,
						 inherited_visibility_info
						 &visibility_info)
{
	// The label's metrics depend on the label's visibility. While the
	// label is hidden its metrics are fixed. Once it is shown if it's
	// a wrappable label its metrics will now reflect that.

	rewrap_due_to_updated_position(IN_THREAD);
}

void textlabelObj::implObj::rewrap_due_to_updated_position(IN_THREAD_ONLY)
{
	if (word_wrap_widthmm(IN_THREAD) == 0)
		return; // Not word wrapping.

	auto &element_impl=get_label_element_impl();
	element_impl.initialize_if_needed(IN_THREAD); // Just make sure

	// If we are not visible, just update the metrics.
	//
	// Do not rewrap the label according to the hidden display element's
	// size. Top level containers are created with minimum size. This
	// results in the layout manager initially attempting to resize its
	// elements to their minimum size. Once all the metrics are set,
	// the containers will keep recalculating themselves until they
	// reach their preferred size. Recalculating the label based on its
	// current width changes the label's metrics. This causes needless
	// recalculation churn. Bypass it, and call recalculate().

	if (!element_impl.data(IN_THREAD).inherited_visibility)
	{
		recalculate(IN_THREAD);
		return;
	}

	// If the width matches the rich text's current position, nothing
	// must've changed.

	if (text->word_wrap_width(IN_THREAD) ==
	    element_impl.data(IN_THREAD).current_position.width)
	{
		recalculate(IN_THREAD);
		return;
	}
	// After rewrapping the text to the new width, we should reverse-
	// engineer word_wrap_width, in millimeters, so if the theme gets
	// updated we'll try our best to scale our current size accordingly.

	text->rewrap(IN_THREAD,
		     element_impl.data(IN_THREAD).current_position.width);

	auto screen=element_impl.get_screen()->impl;

	word_wrap_widthmm(IN_THREAD)=
		screen->compute_widthmm
		(current_theme_t::lock{screen->current_theme},
		 text->word_wrap_width(IN_THREAD));

	// And we should now update our metrics, accordingly.
	recalculate(IN_THREAD);
}

bool textlabelObj::implObj::rewrap(IN_THREAD_ONLY)
{
	if (word_wrap_widthmm(IN_THREAD) == 0)
		return false;

	return text->rewrap(IN_THREAD, preferred_width);
}

void textlabelObj::implObj::do_draw(IN_THREAD_ONLY,
				const draw_info &di,
				const rectangle_set &areas)
{
	text->full_redraw(IN_THREAD, get_label_element_impl(), {}, di, areas);
}

void textlabelObj::implObj::recalculate(IN_THREAD_ONLY)
{
	auto metrics=calculate_current_metrics(IN_THREAD);

	get_label_element_impl()
		.get_horizvert(IN_THREAD)->set_element_metrics
		(IN_THREAD,
		 metrics.first,
		 metrics.second);
}

std::pair<metrics::axis, metrics::axis>
textlabelObj::implObj::calculate_current_metrics(IN_THREAD_ONLY)
{
	return text->get_metrics(IN_THREAD, preferred_width,
				 get_label_element_impl()
				 .data(IN_THREAD).inherited_visibility);
}

LIBCXXW_NAMESPACE_END
