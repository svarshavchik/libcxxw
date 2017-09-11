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
#include "x/w/text_hotspot.H"
#include "run_as.H"

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
	auto l=create_label_internal(text, widthmm, alignment, container_impl);

	created(l);

	return l;
}

label factoryObj::create_label_internal(const text_param &text,
					double widthmm,
					halign alignment,
					const ref<containerObj::implObj>
					&container_impl)
{
	auto label_impl=ref<label_elementObj<child_elementObj>>
		::create(container_impl, text, alignment, widthmm, false);

	return label::create(label_impl, label_impl);
}

textlabelObj::implObj::implObj(const text_param &text,
			       halign alignment,
			       double initial_width,
			       bool allow_links,
			       elementObj::implObj &element_impl)
	: implObj(text,
		  {element_impl.create_background_color
				  ("label_foreground_color"),
				  element_impl.create_theme_font
				  (element_impl.label_theme_font())},
		  alignment,
		  initial_width,
		  allow_links,
		  element_impl)
{
}

static textlabelObj::implObj::hotspot_info_t
create_hotspot_info(richtextstring &s, const richtext &t)
{
	textlabelObj::implObj::hotspot_info_t info;

	const auto &m=s.get_meta();

	auto b=m.begin(), e=m.end();

	while (b != e)
	{
		if (!b->second.link)
		{
			++b;
			continue;
		}

		auto p=b;

		while (b->second.link == p->second.link)
		{
			if (++b == e)
				throw EXCEPTION("Internal error: cannot find end of link");
		}

		info.insert({p->second.link, {t->at(p->first),
						t->at(b->first)}});
	}
	return info;
}

textlabelObj::implObj::implObj(const text_param &text,
			       const richtextmeta &default_meta,
			       halign alignment,
			       double initial_width,
			       bool allow_links,
			       elementObj::implObj &element_impl)
	: implObj(alignment, initial_width,
		  element_impl.create_richtextstring
		  (default_meta, text, allow_links),
		  default_meta, allow_links)
{
}

textlabelObj::implObj::implObj(halign alignment,
			       double initial_width,
			       richtextstring &&string,
			       const richtextmeta &default_meta,
			       bool allow_links)
	: implObj(alignment, initial_width, std::move(string),
		  richtext::create(string, alignment, 0),
		  default_meta, allow_links)
{
}

textlabelObj::implObj::implObj(halign alignment,
			       double initial_width,
			       richtextstring &&string,
			       const richtext &text,
			       const richtextmeta &default_meta,
			       bool allow_links)
	: word_wrap_widthmm_thread_only(initial_width),
	  hotspot_info_thread_only(create_hotspot_info(string, text)),
	  text(text),
	  default_meta(default_meta),
	  allow_links(allow_links)
{
	if (std::isnan(initial_width))
		initial_width=0;

	if (initial_width < 0)
		throw EXCEPTION(_("Label width cannot be negative"));
}

textlabelObj::implObj::~implObj()=default;

void textlabelObj::implObj::update(const text_param &string)
{
	get_label_element_impl().THREAD->run_as
		([me=ref<implObj>(this), string]
		 (IN_THREAD_ONLY)
		 {
			 me->update(IN_THREAD, string);
		 });
}

void textlabelObj::implObj::update(IN_THREAD_ONLY, const text_param &string)
{
	auto s=get_label_element_impl()
		.create_richtextstring(default_meta, string, allow_links);
	text->set(IN_THREAD, s);

	hotspot_info(IN_THREAD)=create_hotspot_info(s, text);
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
