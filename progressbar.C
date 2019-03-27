/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "progressbar_impl.H"
#include "progressbar_slider.H"
#include "x/w/progressbar_appearance.H"
#include "x/w/impl/layoutmanager.H"
#include "gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/impl/element.H"
#include "run_as.H"
#include "catch_exceptions.H"

LIBCXXW_NAMESPACE_START

progressbar_config::progressbar_config()
	: appearance{progressbar_appearance::base::theme()}
{
}

progressbar_config::~progressbar_config()=default;

progressbar_config::progressbar_config(const progressbar_config &)=default;

progressbar_config &progressbar_config::operator=(const progressbar_config &)
=default;

progressbarObj::progressbarObj(const ref<implObj> &impl,
			       const layout_impl &container_layout_impl)
	: containerObj{impl->handler, container_layout_impl}, impl{impl}
{
}

progressbarObj::~progressbarObj()=default;

layout_impl progressbarObj::get_layout_impl() const
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
::do_create_progressbar(const function<void (const progressbar &)> &creator)
{
	return do_create_progressbar(creator, {});
}

progressbar factoryObj
::do_create_progressbar(const function<void (const progressbar &)> &creator,
			const create_progressbar_args_t &args)
{
	std::optional<progressbar_config> default_config;
	std::optional<new_gridlayoutmanager> default_layoutmanager;

	const progressbar_config &config=
		optional_arg_or<progressbar_config>
		(args, default_config);
	const new_layoutmanager &layout_manager=
		optional_arg_or<new_layoutmanager>(args, default_layoutmanager);

	// Creating element:
	//
	// progressbar_handler -> progressbar_lm -> pb

	auto progressbar_handler=ref<progressbarObj::handlerObj>
		::create(get_container_impl(), config.appearance);

	ref<gridlayoutmanagerObj::implObj> progressbar_lm=
		new_gridlayoutmanager{}.create(progressbar_handler);

	// Inserting the slider into the progress bar.

	auto glm=progressbar_lm->create_gridlayoutmanager();

	auto f=glm->append_row();

	f->border(config.appearance->border);
	f->halign(halign::fill);
	f->padding(0);

	// The slider in the progress bar.

	auto slider=progressbar_slider::create(progressbar_handler,
					       config.appearance);

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
