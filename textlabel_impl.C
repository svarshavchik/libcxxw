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
#include "focus/focusable.H"
#include "background_color.H"
#include "messages.H"
#include "defaulttheme.H"
#include "x/w/factory.H"
#include "x/w/label.H"
#include "x/w/text_hotspot.H"
#include "x/w/key_event.H"
#include "x/w/motion_event.H"
#include "x/w/button_event.H"
#include "run_as.H"
#include <X11/keysym.h>

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
		::create(get_container_impl(), text, alignment, widthmm, false);

	auto l=label::create(label_impl, label_impl);

	created(l);

	return l;
}

textlabelObj::implObj::implObj(const text_param &text,
			       halign alignment,
			       double initial_width,
			       bool allow_links,
			       elementObj::implObj &element_impl)
	: implObj(text,
		  {element_impl.create_background_color
				  (element_impl.label_theme_color()),
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

	size_t counter=0;

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
						t->at(b->first), counter++}});
	}
	return info;
}

// After create_hotspot_info() comes rebuild_ordered_hotspots.

static auto rebuild_ordered_hotspots(const auto &hotspot_info)
{
	std::unordered_map<size_t, text_hotspot> m;

	for (const auto &h:hotspot_info)
		m.insert({h.second.n, h.first});
	return m;
}

// Order hotspots by their appearance order.
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
			       bool allow_links_param)
	: word_wrap_widthmm_thread_only(initial_width),
	  hotspot_info_thread_only(create_hotspot_info(string, text)),
	  ordered_hotspots(rebuild_ordered_hotspots(hotspot_info_thread_only)),
	  text(text),
	  hotspot_cursor(allow_links_param ? (richtextiteratorptr)text->begin()
			 : richtextiteratorptr{}),
	  default_meta(default_meta),
	  allow_links(allow_links_param)
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
	get_label_element_impl().initialize_if_needed(IN_THREAD);
	auto s=get_label_element_impl()
		.create_richtextstring(default_meta, string, allow_links);
	text->set(IN_THREAD, s);

	hotspot_info(IN_THREAD)=create_hotspot_info(s, text);
	ordered_hotspots=rebuild_ordered_hotspots(hotspot_info(IN_THREAD));
	hotspot_highlighted(IN_THREAD)=nullptr;
	updated(IN_THREAD);
	get_label_element_impl().schedule_redraw(IN_THREAD);
}

void textlabelObj::implObj::compute_preferred_width(IN_THREAD_ONLY)
{
	auto screen=get_label_element_impl().get_screen()->impl;

	preferred_width=screen->current_theme.get()
		->compute_width(word_wrap_widthmm(IN_THREAD));
}

void textlabelObj::implObj::initialize(IN_THREAD_ONLY)
{
	auto screen=get_label_element_impl().get_screen()->impl;
	auto current_theme=screen->current_theme.get();
	text->theme_updated(IN_THREAD, current_theme);

	compute_preferred_width(IN_THREAD);

	updated(IN_THREAD);
}

void textlabelObj::implObj::updated(IN_THREAD_ONLY)
{
	rewrap_due_to_updated_position(IN_THREAD);

	// We can now compute and set our initial metrics.

	recalculate(IN_THREAD);
}

void textlabelObj::implObj::theme_updated(IN_THREAD_ONLY,
				      const defaulttheme &new_theme)
{
	text->theme_updated(IN_THREAD, new_theme);
	compute_preferred_width(IN_THREAD);
	updated(IN_THREAD);
}

void textlabelObj::implObj::position_set(IN_THREAD_ONLY)
{
	if (position_set_flag)
		return;

	// We avoided expensive rewrapping until the container sets our
	// initial position, at which point we can wrap everything to our
	// actual width.

	position_set_flag=true;
	if (preferred_width == 0)
		return;

	rewrap_due_to_updated_position(IN_THREAD);
}

void textlabelObj::implObj::process_updated_position(IN_THREAD_ONLY)
{
	rewrap_due_to_updated_position(IN_THREAD);
}

void textlabelObj::implObj::rewrap_due_to_updated_position(IN_THREAD_ONLY)
{
	if (preferred_width == 0)
	{
		text->rewrap(IN_THREAD, 0);
		recalculate(IN_THREAD);
		return; // Not word wrapping.
	}

	auto &element_impl=get_label_element_impl();
	element_impl.initialize_if_needed(IN_THREAD); // Just make sure

	auto rewrap_to=element_impl.data(IN_THREAD).current_position.width;

	if (!position_set_flag)
		rewrap_to=preferred_width;

	text->rewrap(IN_THREAD, rewrap_to);

	// And we should now update our metrics, accordingly.
	recalculate(IN_THREAD);
}

void textlabelObj::implObj::do_draw(IN_THREAD_ONLY,
				const draw_info &di,
				const rectangle_set &areas)
{
#ifdef TEST_TEXTLABEL_DRAW
	TEST_TEXTLABEL_DRAW();
#endif
	text->full_redraw(IN_THREAD, get_label_element_impl(), {}, di, areas);
}

void textlabelObj::implObj::recalculate(IN_THREAD_ONLY)
{
	auto metrics=calculate_current_metrics(IN_THREAD);

	if (!position_set_flag)
	{
		metrics.first={metrics.first.preferred(),
			       metrics.first.preferred(),
			       metrics.first.preferred()};
	}

	get_label_element_impl()
		.get_horizvert(IN_THREAD)->set_element_metrics
		(IN_THREAD,
		 metrics.first,
		 metrics.second);
}

std::pair<metrics::axis, metrics::axis>
textlabelObj::implObj::calculate_current_metrics(IN_THREAD_ONLY)
{
	return text->get_metrics(IN_THREAD, preferred_width);
}

bool textlabelObj::implObj::process_button_event(IN_THREAD_ONLY,
						 const button_event &be,
						 xcb_timestamp_t timestamp)
{
	if (!hotspot_highlighted(IN_THREAD))
		return false;
	link_update(IN_THREAD, hotspot_highlighted(IN_THREAD), &be);
	return true;
}

bool textlabelObj::implObj::process_key_event(IN_THREAD_ONLY,
					      const key_event &ke)
{
	if (hotspot_info(IN_THREAD).empty())
		return false;

	text_hotspotptr next_link;

	if (next_key_pressed(ke))
	{
		if (hotspot_highlighted(IN_THREAD))
		{
			auto iter=hotspot_info(IN_THREAD)
				.find(hotspot_highlighted(IN_THREAD));

			if (iter == hotspot_info(IN_THREAD).end())
			{
				const auto &logger=elementObj::implObj::logger;
				LOG_ERROR("Internal error: cannot locate hotspot");
			}
			else
			{
				auto iter2=
					ordered_hotspots.find(iter->second.n+1);
				if (iter2 != ordered_hotspots.end())
					next_link=iter2->second;
			}
		}
		else
		{
			auto iter2=ordered_hotspots.find(0);
			if (iter2 != ordered_hotspots.end())
				next_link=iter2->second;
		}
	}
	else if (prev_key_pressed(ke))
	{
		if (hotspot_highlighted(IN_THREAD))
		{
			auto iter=hotspot_info(IN_THREAD)
				.find(hotspot_highlighted(IN_THREAD));

			if (iter == hotspot_info(IN_THREAD).end())
			{
				const auto &logger=elementObj::implObj::logger;
				LOG_ERROR("Internal error: cannot locate hotspot");
			}
			else if (iter->second.n)
			{
				auto iter2=
					ordered_hotspots.find(iter->second.n-1);
				if (iter2 != ordered_hotspots.end())
					next_link=iter2->second;
			}
		}
		else
		{
			auto iter2=ordered_hotspots
				.find(hotspot_info(IN_THREAD).size()-1);
			if (iter2 != ordered_hotspots.end())
				next_link=iter2->second;
		}
	}
	else if (select_key_pressed(ke))
	{
		if (hotspot_highlighted(IN_THREAD))
		{
			link_update(IN_THREAD, hotspot_highlighted(IN_THREAD),
				    &ke);
			return true;
		}
	}
	else return false;

	hotspot_unhighlight(IN_THREAD);

	if (!next_link)
		return false;

	hotspot_highlighted(IN_THREAD)=next_link;
	link_update(IN_THREAD, next_link, focus_change::gained);
	return true;
}

void textlabelObj::implObj::report_motion_event(IN_THREAD_ONLY,
						const motion_event &me)
{
	if (hotspot_info(IN_THREAD).empty() || !hotspot_cursor)
		return; // Shortcut

	bool flag=hotspot_cursor->moveto(IN_THREAD, me.x, me.y);

	auto link=hotspot_cursor->at(IN_THREAD).link;

	if (!link || !flag)
	{
		hotspot_unhighlight(IN_THREAD);
		return;
	}

	if (link == hotspot_highlighted(IN_THREAD))
		return;

	hotspot_unhighlight(IN_THREAD);
	hotspot_highlighted(IN_THREAD)=link;

	link_update(IN_THREAD, link, focus_change::gained);
}

void textlabelObj::implObj::pointer_focus(IN_THREAD_ONLY)
{
	if (!get_label_element_impl().current_pointer_focus(IN_THREAD))
		hotspot_unhighlight(IN_THREAD);
}

void textlabelObj::implObj::hotspot_unhighlight(IN_THREAD_ONLY)
{
	if (!hotspot_highlighted(IN_THREAD))
		return;

	text_hotspot old_link=hotspot_highlighted(IN_THREAD);
	hotspot_highlighted(IN_THREAD)=nullptr;

	link_update(IN_THREAD, old_link, focus_change::lost);
}

void textlabelObj::implObj::link_update(IN_THREAD_ONLY,
					const text_hotspot &link,
					const text_event_t &event_type)
{
	auto replacement_text=link->event(event_type);

	if (replacement_text.string.empty())
		return;

	// Hotspot callback provided replacement text.

	auto &e=get_label_element_impl();

	auto iter=hotspot_info(IN_THREAD).find(link);

	if (iter == hotspot_info(IN_THREAD).end())
	{
		const auto &logger=e.logger;
		LOG_ERROR("Internal error: cannot locate hotspot");
		return;
	}

	replacement_text.hotspots.clear(); // Too bad, so sad.
	replacement_text.hotspots.insert({0, link});

	iter->second.link_start->replace(IN_THREAD, iter->second.link_end,
					 e.create_richtextstring
					 (default_meta, replacement_text,
					  true));
	updated(IN_THREAD);

	text->redraw_whatsneeded(IN_THREAD, e,
				 {},
				 e.get_draw_info(IN_THREAD));
}

LIBCXXW_NAMESPACE_END
