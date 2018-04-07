/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "progressbar_impl.H"
#include "progressbar_slider.H"
#include "layoutmanager.H"
#include "gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/impl/element.H"
#include "run_as.H"
#include "catch_exceptions.H"

LIBCXXW_NAMESPACE_START

progressbarObj::progressbarObj(const ref<implObj> &impl,
			       const ref<layoutmanagerObj::implObj>
			       &layout_impl)
	: containerObj(impl->handler, layout_impl), impl{impl}
{
}

progressbarObj::~progressbarObj()=default;

ref<layoutmanagerObj::implObj> progressbarObj::get_layout_impl() const
{
	// This is the inner container, and this returns its layout manager,
	// rather than our boring one.

	return impl->contents->get_layout_impl();
}

void progressbarObj::update(size_t value, size_t maximum_value)
{
	update(value, maximum_value, [](THREAD_CALLBACK){});
}

void progressbarObj::update(size_t value, size_t maximum_value,
			    const functionref<void (THREAD_CALLBACK)>
			    &closure)
{
	elementObj::impl->THREAD->run_as
		([=, me=ref(this)]
		 (ONLY IN_THREAD)
		 {
			 try {
				 closure(IN_THREAD);
			 } REPORT_EXCEPTIONS(me->elementObj::impl);

			 me->impl->slider->value(IN_THREAD)=value;
			 me->impl->slider->maximum_value(IN_THREAD)=
				 maximum_value;
			 me->impl->slider->update(IN_THREAD);
		 });
}

//////////////////////////////////////////////////////////////////////////

progressbar factoryObj
::do_create_progressbar(const function<void (const progressbar &)> &creator,
			const progressbar_config &config)
{
	return do_create_progressbar(creator, config,
				     new_gridlayoutmanager{});
}

progressbar factoryObj
::do_create_progressbar(const function<void (const progressbar &)> &creator,
			const progressbar_config &config,
			const new_layoutmanager &layout_manager)
{
	// Creating element:
	//
	// progressbar_handler -> progressbar_lm -> pb

	auto progressbar_handler=ref<progressbarObj::handlerObj>
		::create(get_container_impl(), config);

	auto progressbar_lm=
		ref<gridlayoutmanagerObj::implObj>::create(progressbar_handler);


	// Inserting the slider into the progress bar.

	auto glm=progressbar_lm->create_gridlayoutmanager();

	auto f=glm->append_row();

	f->border(config.border);
	f->halign(halign::fill);
	f->padding(0);

	// The slider in the progress bar.

	auto slider=progressbar_slider::create(progressbar_handler, config);

	// Use the passed-in layout manager to create the layout manager
	// for slider.

	auto inner_lm=layout_manager.create(slider);

	auto contents=container::create(slider, inner_lm);

	// Construct the rest of the progressbar.

	auto impl=ref<progressbarObj::implObj>
		::create(progressbar_handler, contents, slider);

	auto pb=progressbar::create(impl,  progressbar_lm);

	creator(pb);

	f->created_internally(contents);

	created_internally(pb);

	return pb;
}

LIBCXXW_NAMESPACE_END
