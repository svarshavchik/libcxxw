/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "peephole/peephole_layoutmanager_impl_scrollbars.H"
#include "peephole/peephole_impl_element.H"
#include "x/w/font_arg.H"
#include "x/w/focusable_container.H"
#include "x/w/impl/container_element.H"
#include "x/w/impl/always_visible_element.H"
#include "x/w/impl/child_element.H"
#include "x/w/impl/theme_font_element.H"
#include "x/w/impl/focus/focusable.H"
#include "capturefactory.H"

LIBCXXW_NAMESPACE_START

peepholelayoutmanagerObj::peepholelayoutmanagerObj(const ref<implObj> &impl)
	: layoutmanagerObj{impl}, impl{impl}
{
}

peepholelayoutmanagerObj::~peepholelayoutmanagerObj()=default;

element peepholelayoutmanagerObj::get() const
{
	return impl->element_in_peephole->get_peepholed_element();
}

//////////////////////////////////////////////////////////////////////////////

new_scrollable_peepholelayoutmanager
::new_scrollable_peepholelayoutmanager(const functionref<void (const factory &)>
				       &peepholed_factory)
	: peepholed_factory{peepholed_factory},
	  peephole_border{"peephole_border"}
{
}

new_scrollable_peepholelayoutmanager::~new_scrollable_peepholelayoutmanager()=default;

new_peepholelayoutmanager
::new_peepholelayoutmanager(const functionref<void (const factory &)>
			    &peepholed_factory)
	: peepholed_factory{peepholed_factory}
{
}

new_peepholelayoutmanager::~new_peepholelayoutmanager()=default;

namespace {
#if 0
}
#endif

class custom_peepholed_elementObj : public peepholedObj {

	const element peepholed_element;

	const ref<theme_fontObj> reference_font;

public:

	custom_peepholed_elementObj(const element &peepholed_element,
				    const ref<theme_fontObj> &reference_font)
		: peepholed_element{peepholed_element},
		  reference_font{reference_font}
	{
	}

	element get_peepholed_element() override
	{
		return peepholed_element;
	}

	dim_t horizontal_increment(ONLY IN_THREAD) const override
	{
		return reference_font->font_nominal_width(IN_THREAD);
	}

	dim_t vertical_increment(ONLY IN_THREAD) const override
	{
		return reference_font->font_height(IN_THREAD);
	}

	size_t peepholed_rows(ONLY IN_THREAD) const override
	{
		return 0;
	}
};

class peephole_focusable_containerObj : public focusable_containerObj {

public:
	const focusable horizontal_scrollbar;

	const focusable vertical_scrollbar;

	const layout_impl peephole_layout_impl;

	peephole_focusable_containerObj(const container_impl &impl,
					const layout_impl &container_layout,
					const layout_impl &peephole_layout_impl,
					const scrollbar &horizontal_scrollbar,
					const scrollbar &vertical_scrollbar)
		: focusable_containerObj{impl, container_layout},
		  horizontal_scrollbar{horizontal_scrollbar},
		  vertical_scrollbar{vertical_scrollbar},
		  peephole_layout_impl{peephole_layout_impl}
	{
	}

	~peephole_focusable_containerObj()=default;

	ref<focusableObj::implObj> get_impl() const override
	{
		return horizontal_scrollbar->get_impl();
	}

	void do_get_impl(const function<internal_focusable_cb> &cb)
		const override
	{
		process_focusable_impls_from_focusables
			(cb,
			 {
			  horizontal_scrollbar,
			  vertical_scrollbar
			 });
	}

	layout_impl get_layout_impl() const override
	{
		return peephole_layout_impl;
	}
};

struct new_peephole_info {

	const element element_in_peephole;

	const ref<custom_peepholed_elementObj> peepholed_in_peephole;

	new_peephole_info(const container_impl &peephole_impl,
			  const ref<theme_fontObj> &reference_font,
			  const functionref<void (const factory &)
			  > &peepholed_factory)
		: element_in_peephole
		{({
			auto f=capturefactory::create(peephole_impl);

			peepholed_factory(f);

			f->get();
				})},
		  peepholed_in_peephole{ref<custom_peepholed_elementObj>
					::create(element_in_peephole,
						 reference_font)}
	{
	}

	~new_peephole_info()=default;
};

#if 0
{
#endif
}

focusable_container
new_scrollable_peepholelayoutmanager::create(const container_impl &parent) const
{
	auto grid_container_impl=
		ref<container_elementObj<child_elementObj>>::create(parent);

	auto peephole_impl=ref<theme_font_elementObj<
		always_visible_elementObj<
			peephole_impl_elementObj<
				container_elementObj
				<child_elementObj>>>>
			       >::create(theme_font{"label"},
					 grid_container_impl);

	new_peephole_info peephole_info{peephole_impl, peephole_impl,
					peepholed_factory};

	scrollbarptr horizontal_scrollbar;
	scrollbarptr vertical_scrollbar;

	const auto &[layout_impl, glm_impl, grid]=
		create_peephole_with_scrollbars
		([&]
		 (const auto &info, const auto &scrollbars)
		 {
			 horizontal_scrollbar=scrollbars.horizontal_scrollbar;
			 vertical_scrollbar=scrollbars.vertical_scrollbar;

			 return ref<peepholelayoutmanagerObj::implObj
				    ::scrollbarsObj>
				 ::create(info, scrollbars,
					  peephole_impl,
					  peephole_info.peepholed_in_peephole);
		 },
		 [&, this]
		 (const ref<peepholelayoutmanagerObj::implObj> &peephole_lm)
		 -> peephole_element_factory_ret_t
		 {
			 auto peephole_container=
				 peephole::create(peephole_impl, peephole_lm);

			 return {peephole_container,
				 peephole_container,
				 peephole_border,
				 peephole_border,
				 {},
				 left_padding,
				 right_padding,
				 top_padding,
				 bottom_padding,
			 };
		 },
		 create_peephole_gridlayoutmanager,
		 {
		  grid_container_impl,
		  std::nullopt,
		  *this,
		  this->horizontal_scrollbar,
		  this->vertical_scrollbar
		 });

	return ref<peephole_focusable_containerObj>
		::create(grid_container_impl,
			 glm_impl,
			 layout_impl,
			 horizontal_scrollbar,
			 vertical_scrollbar);
}

layout_impl new_peepholelayoutmanager::create(const container_impl &) const
{
	throw; // Not used
}

container new_peepholelayoutmanager::create(const container_impl &parent,
					    const function<void(const
								container &)>
					    &creator) const
{
	auto peephole_impl=ref<theme_font_elementObj<
		peephole_impl_elementObj<
			container_elementObj
			<child_elementObj>>>
			       >::create(theme_font{"label"}, parent);

	new_peephole_info peephole_info{peephole_impl, peephole_impl,
					peepholed_factory};

	auto peephole_layout_impl=
		ref<peepholelayoutmanagerObj::implObj>
		::create(peephole_impl, *this,
			 peephole_info.peepholed_in_peephole);

	auto c=peephole::create(peephole_impl, peephole_layout_impl);

	creator(c);

	return c;
}

LIBCXXW_NAMESPACE_END
