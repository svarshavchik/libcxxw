/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "label.H"
#include "element_screen.H"
#include "screen.H"
#include "richtext/richtext.H"
#include "richtext/richtextmeta.H"
#include "background_color.H"
#include "messages.H"
#include "x/w/factory.H"

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
	auto l=label::create(ref<labelObj::implObj>
			     ::create(container_impl, text, alignment,
				      widthmm));

	created(l);

	return l;
}

labelObj::implObj::implObj(const ref<containerObj::implObj> &container,
			   const text_param &text,
			   halign alignment,
			   double initial_width)
	: implObj(container, text, alignment, initial_width,
		  container->get_element_impl())
{
}

labelObj::implObj::implObj(const ref<containerObj::implObj> &container,
			   const text_param &text,
			   halign alignment,
			   double initial_width,
			   elementObj::implObj &container_element)
	: implObj(container, alignment, initial_width,
		  container_element.convert
		  ({container_element.create_background_color
				  ("label_foreground_color",
				   rgb(rgb::maximum/10,
				       rgb::maximum/10,
				       rgb::maximum/10)),
				  container_element.create_theme_font
				  (container->label_theme_font())},
			  text),
		  "label@libcxx")
{
}

labelObj::implObj::implObj(const ref<containerObj::implObj> &container,
			   halign alignment,
			   double initial_width,
			   const richtextstring &string,
			   const char *element_id)
	: child_elementObj(container, metrics::horizvert_axi(),
			   element_id),
	  word_wrap_widthmm_thread_only(initial_width),
	  text(richtext::create(string, alignment, 0))
{
	if (initial_width < 0)
		throw EXCEPTION(_("Label width cannot be negative"));
}

labelObj::implObj::~implObj()=default;

void labelObj::implObj::compute_preferred_width(IN_THREAD_ONLY)
{
	auto screen=get_screen()->impl;

	preferred_width=word_wrap_widthmm(IN_THREAD) == 0 ?
		(dim_t)0: screen->compute_width
		(current_theme_t::lock{screen->current_theme},
		 word_wrap_widthmm(IN_THREAD));
}

void labelObj::implObj::initialize(IN_THREAD_ONLY)
{
	child_elementObj::initialize(IN_THREAD);

	compute_preferred_width(IN_THREAD);

	rewrap(IN_THREAD);

	// We can now compute and set our initial metrics.

	auto metrics=calculate_current_metrics(IN_THREAD);

	auto current_metrics=get_horizvert(IN_THREAD);

	current_metrics->horiz=metrics.first;
	current_metrics->vert=metrics.second;
}

void labelObj::implObj::theme_updated(IN_THREAD_ONLY)
{
	text->theme_updated(IN_THREAD);
	compute_preferred_width(IN_THREAD);
	rewrap(IN_THREAD);
	recalculate(IN_THREAD);

	child_elementObj::theme_updated(IN_THREAD);
}

void labelObj::implObj::process_updated_position(IN_THREAD_ONLY)
{
	child_elementObj::process_updated_position(IN_THREAD);

	rewrap_due_to_updated_position(IN_THREAD);
}

void labelObj::implObj::rewrap_due_to_updated_position(IN_THREAD_ONLY)
{
	if (word_wrap_widthmm(IN_THREAD) == 0)
		return; // Not word wrapping.

	// If the width matches the rich text's current position, nothing
	// must've changed.

	if (text->word_wrap_width(IN_THREAD) ==
	    data(IN_THREAD).current_position.width)
		return;

	// After rewrapping the text to the new width, we should reverse-
	// engineer word_wrap_width, in millimeters, so if the theme gets
	// updated we'll try our best to scale our current size accordingly.

	text->rewrap(IN_THREAD, data(IN_THREAD).current_position.width);

	auto screen=get_screen()->impl;

	word_wrap_widthmm(IN_THREAD)=
		screen->compute_widthmm
		(current_theme_t::lock{screen->current_theme},
		 text->word_wrap_width(IN_THREAD));

	// And we should now update our metrics, accordingly.
	recalculate(IN_THREAD);
}

bool labelObj::implObj::rewrap(IN_THREAD_ONLY)
{
	if (word_wrap_widthmm(IN_THREAD) == 0)
		return false;

	return text->rewrap(IN_THREAD, data(IN_THREAD).current_position.width);
}

void labelObj::implObj::do_draw(IN_THREAD_ONLY,
				const draw_info &di,
				const rectangle_set &areas)
{
	text->do_draw(IN_THREAD, *this, di, true, areas);
}

void labelObj::implObj::recalculate(IN_THREAD_ONLY)
{
	auto metrics=calculate_current_metrics(IN_THREAD);

	get_horizvert(IN_THREAD)->set_element_metrics
		(IN_THREAD,
		 metrics.first,
		 metrics.second);
}

std::pair<metrics::axis, metrics::axis>
labelObj::implObj::calculate_current_metrics(IN_THREAD_ONLY)
{
	return text->get_metrics(IN_THREAD, preferred_width);
}

LIBCXXW_NAMESPACE_END
