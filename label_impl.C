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
#include "x/w/factory.H"

LIBCXXW_NAMESPACE_START

label factoryObj::create_label(const text_param &text,
			       halign alignment)
{
	auto l=label::create(ref<labelObj::implObj>
			     ::create(container_impl, text, alignment));

	created(l);

	return l;
}

// Helper function invoked from the constructor to construct the richtext
// member object.

static inline richtext create_rich_text(const auto &container, const auto &text,
					halign alignment)
{
	auto &element_impl=container->get_element_impl();

	auto background_color=
		element_impl.create_background_color
		("label_foreground_color",
		 rgb(rgb::maximum/10,
		     rgb::maximum/10,
		     rgb::maximum/10));

	auto font=element_impl.create_theme_font("label");

	auto string=element_impl.convert({ background_color, font },
					 text);

	return richtext::create(string, alignment, 0);
}

labelObj::implObj::implObj(const ref<containerObj::implObj> &container,
			   const text_param &text,
			   halign alignment)
	: child_elementObj(container, metrics::horizvert_axi(),
			   "label@libcxx"),
	  text(create_rich_text(container, text, alignment))
{
}

labelObj::implObj::~implObj()=default;

void labelObj::implObj::initialize(IN_THREAD_ONLY)
{
	child_elementObj::initialize(IN_THREAD);

	// We can now compute and set our initial metrics.

	auto metrics=text->get_metrics(IN_THREAD);

	auto current_metrics=get_horizvert(IN_THREAD);

	current_metrics->horiz=metrics.first;
	current_metrics->vert=metrics.second;
}

void labelObj::implObj::theme_updated(IN_THREAD_ONLY)
{
	text->theme_updated(IN_THREAD);
	recalculate(IN_THREAD);

	child_elementObj::theme_updated(IN_THREAD);
}

void labelObj::implObj::do_draw(IN_THREAD_ONLY,
				const draw_info &di,
				const rectangle_set &areas)
{
	text->do_draw(IN_THREAD, *this, di, true);
}

void labelObj::implObj::recalculate(IN_THREAD_ONLY)
{
	auto metrics=text->get_metrics(IN_THREAD);

	get_horizvert(IN_THREAD)->set_element_metrics
		(IN_THREAD,
		 metrics.first,
		 metrics.second);
}

LIBCXXW_NAMESPACE_END
