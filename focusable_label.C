/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/focusable_label.H"
#include "x/w/factoryobj.H"
#include "x/w/label.H"
#include "x/w/borderlayoutmanager.H"
#include "x/w/impl/focus/standard_focusframecontainer_element_impl.H"
#include "focus/focusframelayoutimpl.H"
#include "x/w/impl/focus/focusable.H"
#include "textlabel.H"
#include "focusable_label_impl.H"

LIBCXXW_NAMESPACE_START

focusable_label_config::~focusable_label_config()=default;

focusable_label factoryObj
::create_focusable_label(const text_param &text,
			 const label_hotspots_t &hotspots)
{
	return create_focusable_label(text, hotspots, {});
}

focusable_label factoryObj
::create_focusable_label(const text_param &text,
			 const label_hotspots_t &hotspots,
			 const focusable_label_config &config)
{
	auto ff=create_nonrecursive_visibility_focusframe_impl
		(get_container_impl(),
		 config.focus_border, 0, 0);

	textlabel_config internal_config{config};

	internal_config.initial_hotspots.emplace(hotspots);

	auto focusable_label_impl=ref<focusable_labelObj::implObj>
		::create(ff, text, internal_config);

	auto l=label::create(focusable_label_impl, focusable_label_impl);

	auto ffl=ref<focusframelayoutimplObj>::create(ff, ff, l);

	l->show();

	auto fl=focusable_label::create(ff, ffl,
					focusable_label_impl,
					focusable_label_impl);

	fl->label_for(fl);
	created(fl);
	return fl;
}

focusable_labelObj
::focusable_labelObj(const container_impl &container_impl,
		     const layout_impl &container_layout_impl,
		     const ref<textlabelObj::implObj> &label_impl,
		     const focusable_impl &focusable_impl)
	: containerObj{container_impl, container_layout_impl},
	  textlabelObj{label_impl},
	  focusableObj::ownerObj{focusable_impl}
{
}

focusable_labelObj::~focusable_labelObj()=default;

focusable_impl focusable_labelObj::get_impl() const
{
	return focusableObj::ownerObj::get_impl();
}

ref<elementObj::implObj> focusable_labelObj
::get_minimum_override_element_impl()
{
	return borderlayout()->get()->impl;
}

LIBCXXW_NAMESPACE_END
