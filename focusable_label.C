/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/focusable_label.H"
#include "x/w/factoryobj.H"
#include "x/w/label.H"
#include "focus/standard_focusframecontainer_element.H"
#include "focus/focusframelayoutimpl.H"
#include "focus/focusable.H"
#include "textlabel.H"
#include "focusable_label_impl.H"

LIBCXXW_NAMESPACE_START

focusable_label factoryObj
::create_focusable_label(const text_param &text, halign alignment)
{
	auto ff=create_standard_focusframe_container_element(container_impl);

	auto ffl=ref<focusframelayoutimplObj>
		::create(ff,
			 "inputfocusoff_border",
			 "inputfocuson_border");

	auto glm=ffl->create_gridlayoutmanager();

	auto gf=glm->append_row();

	auto focusable_label_impl=ref<focusable_labelObj::implObj>
		::create(ff, text, alignment);

	auto l=label::create(focusable_label_impl, focusable_label_impl);

	gf->padding(0);
	gf->created_internally(l);

	l->show();

	auto fl=focusable_label::create(ff, ffl,
					focusable_label_impl,
					focusable_label_impl);

	fl->label_for(fl);
	created(fl);
	return fl;
}

focusable_labelObj
::focusable_labelObj(const ref<containerObj::implObj> &container_impl,
		     const ref<layoutmanagerObj::implObj> &layout_impl,
		     const ref<textlabelObj::implObj> &label_impl,
		     const ref<focusableImplObj> &focusable_impl)
	: containerObj(container_impl, layout_impl),
	  textlabelObj(label_impl),
	  focusableObj::ownerObj(focusable_impl)
{
}

focusable_labelObj::~focusable_labelObj()=default;

ref<focusableImplObj> focusable_labelObj::get_impl() const
{
	return focusableObj::ownerObj::get_impl();
}

ref<elementObj::implObj> focusable_labelObj
::get_minimum_override_element_impl()
{
	gridlayoutmanager glm=get_layoutmanager();

	return glm->get(0, 0)->impl;
}

LIBCXXW_NAMESPACE_END