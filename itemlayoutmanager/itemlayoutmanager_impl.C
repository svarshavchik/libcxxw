/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "itemlayoutmanager/itemlayoutmanager_impl.H"
#include "itemlayoutmanager/peepholed_item_container_impl.H"
#include "itemlayoutmanager/itembutton_impl.H"
#include "x/w/impl/container.H"
#include "x/w/impl/borderlayoutmanager.H"
#include "x/w/image_button.H"
#include "gridlayoutmanager.H"
#include "image_button.H"
#include "image_button_internal_impl.H"
#include "messages.H"
#include <x/weakcapture.H>

LIBCXXW_NAMESPACE_START

itemlayout_button_container
::itemlayout_button_container(const itemlayout_callback_t &callback)
	: callback{callback}
{
}

itemlayoutmanagerObj::implObj::implObj(const ref<peepholed_item_containerObj
				       ::implObj> &layout_container_impl,
				       const new_itemlayoutmanager &config)
	: layoutmanagerObj::implObj{layout_container_impl},
	  layout_container_impl{layout_container_impl},
	  appearance{config.appearance},
	  item_info{config.callback}
{
}

itemlayoutmanagerObj::implObj::~implObj()=default;

void itemlayoutmanagerObj::implObj::do_for_each_child(ONLY IN_THREAD,
						      const function<void
						      (const element &)> &cb)
{
	auto all_buttons=item_info_t::lock{item_info}->all_buttons;

	for (const auto &e:all_buttons)
		cb(e);
}

size_t itemlayoutmanagerObj::implObj::num_children(ONLY IN_THREAD)
{
	item_info_t::lock lock{item_info};

	return lock->all_buttons.size();
}

layoutmanager itemlayoutmanagerObj::implObj::create_public_object()
{
	return itemlayoutmanager::create(ref{this});
}

void itemlayoutmanagerObj::implObj::recalculate(ONLY IN_THREAD)
{
	process_updated_position(IN_THREAD,
				 get_element_impl()
				 .data(IN_THREAD).current_position);
}

void itemlayoutmanagerObj::implObj
::process_updated_position(ONLY IN_THREAD,
			   const rectangle &position)
{
	item_info_t::lock lock{item_info};

	// First, some paperwork:
	bool redraw_needed=!lock->removed_buttons.empty();

	for (const auto &b:lock->new_buttons)
	{
		b->elementObj::impl->initialize_if_needed(IN_THREAD);
		redraw_needed=true;
	}
	if (redraw_needed)
		get_element_impl().schedule_full_redraw(IN_THREAD);

	lock->removed_buttons.clear();
	lock->new_buttons.clear();

	auto layout_h_padding=layout_container_impl
		->themedim_element<itemlayout_h_padding>::pixels(IN_THREAD);
	auto layout_v_padding=layout_container_impl
		->themedim_element<itemlayout_v_padding>::pixels(IN_THREAD);

	dim_t minimum=0;
	coord_t ypos=0;

	for (auto p=lock->all_buttons.begin(), e=lock->all_buttons.end();
	     p != e;)
	{
		if (ypos > 0)
			ypos=dim_t::truncate(ypos + layout_v_padding);

		// Make a first pass, putting as many items as we can before
		// we bump into our width. We keep track of the tallest one.

		dim_t height=0;

		coord_t xpos=0;
		auto b=p;

		while (p != e)
		{
			auto e_impl=(*p)->elementObj::impl;
			auto e_impl_hv=e_impl->get_horizvert(IN_THREAD);

			auto w=e_impl_hv->horiz.preferred();

			if (w > minimum)
				minimum=w;
			auto x_end=w+xpos;

			if (xpos > 0 && dim_t::truncate(x_end) > position.width)
				break;

			auto e_h=e_impl_hv->vert.preferred();

			if (e_h > height)
				height=e_h;

			xpos=coord_t::truncate(x_end + layout_h_padding);
			++p;
		}

		// On the 2nd pass we actually position all items.
		xpos=0;

		for ( ; b != p; ++b)
		{
			auto e_impl=(*b)->elementObj::impl;
			auto e_impl_hv=e_impl->get_horizvert(IN_THREAD);

			e_impl->update_current_position
				(IN_THREAD,
				 {xpos,
				  coord_t::truncate
				  (ypos+
				   (height-e_impl_hv->vert.preferred())/2),
				  e_impl_hv->horiz.preferred(),
				  e_impl_hv->vert.preferred()});

			xpos=coord_t::truncate
				(e_impl_hv->horiz.preferred()+xpos+
				 layout_h_padding);
		}
		ypos=coord_t::truncate(ypos+height);
	}

	// We now know our own metrics.

	dim_t final_height{dim_t::truncate(ypos)};

	get_element_impl().get_horizvert(IN_THREAD)
		->set_element_metrics(IN_THREAD,
				      {minimum, minimum, dim_t::infinite()},
				      {final_height, final_height,
				       final_height});
}

void itemlayoutmanagerObj::implObj::insert(const itembutton &b, size_t i)
{
	item_info_t::lock lock{item_info};
	insert(b, i, lock);
}

void itemlayoutmanagerObj::implObj::insert(const itembutton &b, size_t i,
					   item_info_t::lock &lock)
{
	size_t n=lock->all_buttons.size();

	if (i == n)
	{
		// Equivalent to append
		append(b, lock);
		return;
	}

	if (i > n)
	{
		throw EXCEPTION(gettextmsg(_("Item #%1% does not exist"), i));
	}

	// Definitely inserting before an existing button.
	// Set the index, focus order, then insert the button.

	b->index(lock)=i;

	auto iter=lock->all_buttons.begin()+i;

	b->deletebutton->get_focus_before( (*iter)->deletebutton );

	lock->new_buttons.push_back(b);
	lock->all_buttons.insert(iter, b);

	// Now we can bump all following buttons' index.
	iter=lock->all_buttons.begin()+i;

	auto e=lock->all_buttons.end();

	while (++iter != e)
		++(*iter)->index(lock);

	initialize(b, lock);
}


void itemlayoutmanagerObj::implObj::append(const itembutton &b)
{
	item_info_t::lock lock{item_info};
	append(b, lock);
}

void itemlayoutmanagerObj::implObj::append(const itembutton &b,
					   item_info_t::lock &lock)
{
	b->index(lock)=lock->all_buttons.size(); // We now know what it is.

	b->deletebutton->get_focus_after
		(lock->all_buttons.empty() ?
		 layout_container_impl->horizontal_scrollbar :
		 focusable{lock->all_buttons.back()->deletebutton});

	lock->new_buttons.push_back(b);
	lock->all_buttons.push_back(b);

	initialize(b, lock);
}

void itemlayoutmanagerObj::implObj::initialize(const itembutton &b,
					       item_info_t::lock &lock)
{
	b->deletebutton->impl->button->impl->on_activate
		([layout_container_impl=this->layout_container_impl,
		  b=make_weak_capture(b)]
		 (ONLY IN_THREAD,
		  const auto &trigger,
		  const auto &busy_mcguffin)
		 {
			 auto got=b.get();

			 if (!got)
				 return;

			 const auto &[b]=*got;

			 layout_container_impl->invoke_layoutmanager
				 ([&]
				  (const auto &layout_impl)
				  {
					  itemlayoutmanager lm=layout_impl
						  ->create_public_object();

					  itemlayout_lock lock{lm};

					  lock->callback
						  (IN_THREAD,
						   b->index(lock),
						   lock,
						   trigger,
						   busy_mcguffin);
				  });
		 });
}

size_t itemlayoutmanagerObj::implObj::size()
{
	return item_info_t::lock{item_info}->all_buttons.size();
}

void itemlayoutmanagerObj::implObj::remove_item(size_t i)
{
	item_info_t::lock lock{item_info};

	if (i >= lock->all_buttons.size())
		throw EXCEPTION(gettextmsg(_("Item #%1% does not exist"), i));

	auto iter=lock->all_buttons.begin()+i;

	lock->removed_buttons.push_back(*iter);
	lock->all_buttons.erase(iter);

	// Keep track of the indexes.

	auto p=lock->all_buttons.begin()+i;
	auto e=lock->all_buttons.end();

	while (p != e)
	{
		(*p)->index(lock)=i;
		++i;
		++p;
	}
}

element itemlayoutmanagerObj::implObj::get_item(size_t i)
{
	item_info_t::lock lock{item_info};

	if (i >= lock->all_buttons.size())
		throw EXCEPTION(gettextmsg(_("Item #%1% does not exist"), i));

	elementptr e;

	lock->all_buttons[i]->impl->invoke_layoutmanager
		([&]
		 (const ref<borderlayoutmanagerObj::implObj> &blmi)
		 {
			 image_button i=blmi->get();

			 i->containerObj::impl->invoke_layoutmanager
				 ([&]
				  (const ref<gridlayoutmanagerObj
				   ::implObj> &i)
				  {
					  e=i->get(0, 1);
				  });

		 });
	return e;
}

focusable itemlayoutmanagerObj::implObj::get_main_focusable()
{
	item_info_t::lock lock{item_info};

	if (lock->all_buttons.empty())
		return layout_container_impl->horizontal_scrollbar;

	auto f=lock->all_buttons.front();

	return f;
}

std::vector<focusable> itemlayoutmanagerObj::implObj::get_all_focusables()
{
	std::vector<focusable> ret;

	item_info_t::lock lock{item_info};

	ret.reserve(lock->all_buttons.size()+2);

	ret.push_back(layout_container_impl->vertical_scrollbar);
	ret.push_back(layout_container_impl->horizontal_scrollbar);

	for (const auto &b:lock->all_buttons)
		ret.push_back(b->deletebutton);

	return ret;
}

LIBCXXW_NAMESPACE_END
