/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "combobox/standard_comboboxlayoutmanager.H"
#include "listlayoutmanager/in_thread_new_cells_info.H"
#include "x/w/focusable_label.H"
#include "x/w/label.H"
#include "busy.H"
#include "messages.H"
#include <x/exception.H>
#include <x/sentry.H>
#include <x/visitor.H>
#include <x/algorithm.H>
#include <utility>
#include <courier-unicode.h>

LIBCXXW_NAMESPACE_START

[[noreturn]] static void nosuchitem(size_t i);

const_standard_combobox_lock
::const_standard_combobox_lock(const const_standard_comboboxlayoutmanager &ll)
	: const_list_lock{ll}, locked_layoutmanager{ll}
{
}

const_standard_combobox_lock::~const_standard_combobox_lock()=default;

std::vector<text_param> &const_standard_combobox_lock::text_items() const
{
	return locked_layoutmanager->impl->text_items_protected;
}

size_t const_standard_combobox_lock::size() const
{
	return text_items().size();
}

text_param const_standard_combobox_lock::item(size_t i) const
{
	auto &ti=text_items();

	if (ti.size() <= i)
		nosuchitem(i);

	return ti.at(i);
}

standard_combobox_lock
::standard_combobox_lock(const standard_comboboxlayoutmanager &ll)
	: const_standard_combobox_lock(ll), locked_layoutmanager(ll)
{
}

standard_combobox_lock::~standard_combobox_lock()=default;

// Take the passed in vector of list_item_params.
//
// Only text_params, and separators are acceptable. That's why this is the
// standard combo-box.

static std::vector<text_param> to_text_param(const std::vector<list_item_param>
					     &items)
{
	std::vector<text_param> ret;

	for (const list_item_param::variant_t &i:items)
		std::visit
			(visitor
			 {
			  [&](const text_param &t)
			  {
				  ret.push_back(t);
			  },
			  [&](const separator &s)
			  {
				  ret.push_back({});
			  },
			  [](const list_item_status_change_callback &)
			  {
			  },
			  [](const auto &s)
			  {
				  throw EXCEPTION(_("This combo-box cannot "
						    "contain this item."));
			  }
			 }, i);
	return ret;
}

new_items_ret standard_combobox_lock::append_items(const list_item_param &item)
{
	return append_items({item});
}

namespace {
#if 0
}
#endif

//! Convert new items for a standard combo-box into text_params.

//! This is inherited by one of the two inherited helper objects.
//! The subclass's constructor receives a vector of list_item_params by
//! value, and passes a reference to its copy of the vector to this
//! constructor.
//!
//! This constructor converts the vector to_text_param() as the first
//! order of business. Afterword, update_items_if_needed() in the vector.
//!
//! This completes new_text_param's construction. The subclass then proceeds
//! and makes arrangement to use the now potentially-updated vector to
//! create_cells().

struct new_text_params_wrapper {

	//! The text_param equivalent of new list_item_params

	const std::vector<text_param> new_text_params;

	new_text_params_wrapper(std::vector<list_item_param>
				&updated_items,
				standard_combobox_lock &lock)
		: new_text_params{to_text_param(updated_items)}
	{
		lock.locked_layoutmanager->impl->
			update_items_if_needed(updated_items);
	}

	~new_text_params_wrapper()=default;
};

//! Shared logic used by non-in_thread modifiers.

//! Non-IN_THREAD methods create this object. The updated_items
//! parameter is intentionally passed by value, in order to make a copy
//! of it, before constructing the new_text_params_wrapper superclass, first.
//!
//! new_text_params_wrapper is inherited first, and gets constructed first.
//! Afterwards, the in_thread_new_cells_infoObj gets constructed, passing
//! it the same vector to its constructor, in order to create_cells().
//!
//! This object then gets tossed over the fence, IN_THREAD. Once IN_THREAD,
//! lock() recovers the relevant key details.

class noninthread_update_helperObj : public new_text_params_wrapper,
				     public in_thread_new_cells_infoObj {

public:

	//! The layout manager for this.

	const ref<standard_comboboxlayoutmanagerObj::implObj> impl;

	//! Constructor
	noninthread_update_helperObj(standard_combobox_lock &lock,
				     std::vector<list_item_param>
				     updated_items,
				     new_items_ret &ret)
		: new_text_params_wrapper{updated_items, lock},
		  in_thread_new_cells_infoObj{lock.locked_layoutmanager,
					      updated_items, ret},
		  impl{lock.locked_layoutmanager->impl}
	{
	}

	//! Destructor
	~noninthread_update_helperObj()=default;

	//! What we need to restart things, now IN_THREAD.

	struct relocked {
		//! The IN_THREAD lock on the combo-box.
		standard_combobox_lock lock;

		//! The implementation objects, whose methods we invoke.
		ref<list_elementObj::implObj> list_impl;
	};

	//! Restore relocked environment, now IN_THREAD.

	//! The IN_THREAD code now wants to relock the combo-box, and get
	//! the impl object.
	auto lock() const
	{
		standard_comboboxlayoutmanager lm{impl->create_public_object()};

		return relocked{ lm, lm->listlayoutmanagerObj::impl
				->list_element_singleton->impl };
	}
};

typedef ref<noninthread_update_helperObj> noninthread_update_helper;

//! Alternative helper for IN_THREAD methods.

//! Inherit from new_text_params_wrapper, also intentionally making a copy
//! of the updated_items vector, passing it to new_text_params_wrapper's
//! constructor.
//!
//! Once that's done, we call create_cells() ourselves.

struct inthread_update_helper : new_text_params_wrapper {

	//! What the list implementation object's methods need.

	new_cells_info info;

	//! The implementation object.
	const ref<list_elementObj::implObj> list_impl;

	//! Constructor
	inthread_update_helper(standard_combobox_lock &lock,
			       std::vector<list_item_param> updated_items,
			       new_items_ret &ret)
		: new_text_params_wrapper{updated_items, lock},
		  info{ret},
		  list_impl{lock.locked_layoutmanager
			    ->listlayoutmanagerObj::impl
			    ->list_element_singleton->impl}
	{
		list_impl->list_style.create_cells(updated_items,
						   lock.locked_layoutmanager
						   ->listlayoutmanagerObj::impl,
						   info);
	}

	//! Destructor.
	~inthread_update_helper()=default;
};

//! Shared logic used when appending, appending, or replacing items.

//! The modifiers all end up, sooner or later, with a standard_combobox_lock,
//! IN_THREAD, and new list items as a vector of text_params. All of these
//! exist somewhere, and we capture refs to them.

struct update_helper {

	//! Existing text_params in the combo-box, that we retrieve.

	std::vector<text_param> &existing_text_params;

	//! New list items.
	const std::vector<text_param> &new_text_params;

	//! Our lock.
	standard_combobox_lock &lock;

	//! Constructor.
	update_helper(standard_combobox_lock &lock,
		      const std::vector<text_param> &new_text_params)
		: existing_text_params{lock.text_items()},
		  new_text_params{new_text_params},
		  lock{lock}
	{
	}
};

//! Shared logic used to append_rows().

//! The non-IN_THREAD and IN_THREAD overloads set things up differently, but
//! use this shared logic.
//!
//! start() gets called immediately after this is constructed.
//! finish() gets called after append_rows() returns.
//!
//! start() appends the text_params version of new combo-box items.
//!
//! If the destructor sees started=true, it unwinds what start() did.
//!
//! We hold a combo-box lock, ensuring no other changes to its contents.

struct append_helper : update_helper {

	using update_helper::update_helper;

	bool started=false;

	//! Start appending. We append the text_params to existing_text_params.

	void start()
	{
		existing_text_params.insert(existing_text_params.end(),
					    new_text_params.begin(),
					    new_text_params.end());
		started=true;
	}

	//! We called append_items().

	void finish()
	{
		started=false;
	}

	//! Unwind the append, if an exception was thrown.

	~append_helper()
	{
		if (started)
		{
			auto e=existing_text_params.end();

			existing_text_params.erase(e-new_text_params.size(),
						   e);
		}
	}
};

//! Shared logic used to insert_rows().

//! The non-IN_THREAD and IN_THREAD overloads set things up differently, but
//! use this shared logic.
//!
//! start() gets called immediately after this is constructed.
//! finish() gets called after insert_rows() returns.
//!
//! start() inserts the text_params version of new combo-box items.
//!
//! If the destructor sees started=true, it unwinds what start() did.
//!
//! We hold a combo-box lock, ensuring no other changes to its contents.

struct insert_helper : update_helper {

	using update_helper::update_helper;

	bool started=false;

	size_t i;

	//! Start the insert.

	void start(size_t i)
	{
		this->i=i;

		if (existing_text_params.size() < i)
			nosuchitem(i);

		existing_text_params.insert(existing_text_params.begin()+i,
					    new_text_params.begin(),
					    new_text_params.end());
		started=true;
	}

	// We called insert_items()

	void finish()
	{
		started=false;
	}

	// Unwind the insert, if an exception was thrown.

	~insert_helper()
	{
		if (started)
		{
			auto iter=existing_text_params.begin()+i;

			existing_text_params.erase(iter,
						   iter+new_text_params.size());
		}
	}
};

//! Shared logic used to replace_rows().

//! The non-IN_THREAD and IN_THREAD overloads set things up differently, but
//! use this shared logic.
//!
//! start() gets called immediately this is constructed. finish() gets called
//! after replace_rows() returns.
//!
//! finish() replaces the existing text_params, now that the underlying
//! combo-box items were successfully replaced.
//!
//! We hold a combo-box lock, ensuring no other changes to its contents.

struct replace_helper : update_helper {

	using update_helper::update_helper;

	size_t i;

	//! Start replacing the items.

	void start(size_t i)
	{
		this->i=i;
		auto n=existing_text_params.size();

		if (n < i)
			nosuchitem(i);

		if (n-i < new_text_params.size())
			nosuchitem(n);
	}

	// All the items were replaced.

	void finish()
	{
		std::copy(new_text_params.begin(),
			  new_text_params.end(),
			  existing_text_params.begin()+i);
	}
};

//! Shared logic used to remove_items().

//! Although the non-IN_THREAD version does not need to implement remove_items()
//! itself, and delegates to the IN_THREAD version, for consistency's sake
//! we take the same approach as with other changes to the combo-box, and
//! use a helper object.
//!
//! removed() gets called if remove_items() succesfully returns.
//!
//! finish() replaces the existing text_params, now that the underlying
//! combo-box items were successfully replaced.
//!
//! We hold a combo-box lock, ensuring no other changes to its contents.

struct remove_helper {

	//! Lock.
	standard_combobox_lock &lock;

	//! Starting position
	const size_t i;

	//! Number of items
	const size_t n;

	//! Existing text_params in the combo-box.
	std::vector<text_param> &existing_text_params;

	//! Sanity check the removal range.
	remove_helper(standard_combobox_lock &lock,
		      size_t i,
		      size_t n)
		: lock{lock}, i{i}, n{n},
		  existing_text_params{lock.text_items()}
	{

		size_t s=existing_text_params.size();

		if (s < i)
			nosuchitem(i);

		auto m=s-i;

		if (m < n)
			nosuchitem(s);
	}

	//! Removal complete.
	void removed()
	{
		auto iter=existing_text_params.begin()+i;
		existing_text_params.erase(iter, iter+n);
	}
};

//! Shared logic used to replace_all_rows().

//! The non-IN_THREAD and IN_THREAD overloads set things up differently, but
//! use this shared logic.
//!
//! start() gets called immediately after this is constructed.
//! finish() gets called
//! after replace_all_rows() returns.
//!
//! finish() replaces the existing text_params, now that the underlying
//! combo-box items were successfully replaced.
//!
//! The destructor makes sure that the job was finish()ed. If not, the combo-box
//! is in an indeterminant state, so we recover by removing everything from the
//! underlying list, and the text_params version of it.
//!
//! We hold a combo-box lock, ensuring no other changes to its contents.

struct replace_all_helper : public update_helper {

	bool started=false;

	ONLY IN_THREAD;

	replace_all_helper(ONLY IN_THREAD,
			   standard_combobox_lock &lock,
			   const std::vector<text_param> &new_text_params)
		: update_helper{lock, new_text_params},
		  IN_THREAD{IN_THREAD}
	{
	}

	//! Starting the replace
	void start()
	{
		started=true;
	}

	//! Ended the replace. Update the text param representation.
	void finish()
	{
		existing_text_params=new_text_params;
		started=false;
	}

	//! Destructor

	//! If an exception gets thrown in a middle of replace, the
	//! combo-box is in an indeterminant state. Clean it up by
	//! removing everything.

	~replace_all_helper()
	{
		if (started)
		{
			existing_text_params.clear();
			lock.locked_layoutmanager
				->superclass_t::replace_all_items
				(IN_THREAD, std::vector<list_item_param>{});
		}
	}

};

#if 0
{
#endif
}

new_items_ret standard_combobox_lock
::append_items(const std::vector<list_item_param> &items)
{
	new_items_ret ret;

	auto helper=noninthread_update_helper::create(*this, items, ret);

	locked_layoutmanager->impl->run_as
		([helper]
		 (ONLY IN_THREAD)
		 {
			 auto [lock, list_impl]=helper->lock();

			 append_helper doit{lock,
					    helper->new_text_params};

			 doit.start();
			 list_impl->append_rows(IN_THREAD,
						lock.locked_layoutmanager,
						helper->info);

			 doit.finish();
		 });

	return ret;
}

new_items_ret standard_combobox_lock
::append_items(ONLY IN_THREAD,
	       const std::vector<list_item_param> &items)
{
	new_items_ret ret;

	inthread_update_helper helper{*this, items, ret};

	append_helper doit{*this, helper.new_text_params};

	doit.start();

	helper.list_impl->append_rows(IN_THREAD,
				      locked_layoutmanager,
				      helper.info);
	doit.finish();

	return ret;

}

new_items_ret standard_combobox_lock::insert_items(size_t i,
						   const list_item_param &item)
{
	return insert_items(i, {item});
}

new_items_ret standard_combobox_lock
::insert_items(size_t i,
	       const std::vector<list_item_param> &items)
{
	new_items_ret ret;

	auto helper=noninthread_update_helper::create(*this, items, ret);

	locked_layoutmanager->impl->run_as
		([helper, i]
		 (ONLY IN_THREAD)
		 {
			 auto [lock, list_impl]=helper->lock();

			 insert_helper doit{lock, helper->new_text_params};

			 doit.start(i);
			 list_impl->insert_rows(IN_THREAD,
						lock.locked_layoutmanager,
						i,
						helper->info);
			 doit.finish();
		 });

	return ret;
}

new_items_ret standard_combobox_lock
::insert_items(ONLY IN_THREAD,
	       size_t i,
	       const std::vector<list_item_param> &items)
{
	new_items_ret ret;

	inthread_update_helper helper{*this, items, ret};

	insert_helper doit{*this, helper.new_text_params};

	doit.start(i);

	helper.list_impl->insert_rows(IN_THREAD,
				      locked_layoutmanager,
				      i,
				      helper.info);
	doit.finish();

	return ret;
}

new_items_ret standard_combobox_lock::replace_items(size_t i,
						    const list_item_param &item)
{
	return replace_items(i, {item});
}

new_items_ret standard_combobox_lock
::replace_items(size_t i,
		const std::vector<list_item_param> &items)
{
	new_items_ret ret;

	auto helper=noninthread_update_helper::create(*this, items, ret);

	locked_layoutmanager->impl->run_as
		([helper, i]
		 (ONLY IN_THREAD)
		 {
			 auto [lock, list_impl]=helper->lock();

			 replace_helper doit{lock, helper->new_text_params};

			 doit.start(i);
			 list_impl->replace_rows(IN_THREAD,
						 lock.locked_layoutmanager,
						 i,
						 helper->info);
			 doit.finish();
		 });

	return ret;
}

new_items_ret standard_combobox_lock
::replace_items(ONLY IN_THREAD,
		size_t i,
		const std::vector<list_item_param> &items)
{
	new_items_ret ret;

	inthread_update_helper helper{*this, items, ret};

	replace_helper doit{*this, helper.new_text_params};

	doit.start(i);

	helper.list_impl->replace_rows(IN_THREAD,
				       locked_layoutmanager,
				       i,
				       helper.info);
	doit.finish();

	return ret;
}

void standard_combobox_lock::remove_item(size_t i)
{
	remove_items(i, 1);
}

void standard_combobox_lock::remove_items(size_t i, size_t n)
{
	// For consistency:

	new_items_ret ret;

	auto helper=noninthread_update_helper
		::create(*this, std::vector<list_item_param>{}, ret);

	locked_layoutmanager->impl->run_as
		([helper, i, n]
		 (ONLY IN_THREAD)
		 {
			 auto [lock, list_impl]=helper->lock();

			 lock.remove_items(IN_THREAD, i, n);
		 });
}

void standard_combobox_lock::remove_item(ONLY IN_THREAD, size_t i)
{
	remove_items(IN_THREAD, i, 1);
}

void standard_combobox_lock::remove_items(ONLY IN_THREAD, size_t i, size_t n)
{
	remove_helper doit{*this, i, n};

	locked_layoutmanager->superclass_t::remove_items(IN_THREAD, i, n);
	doit.removed();
}

new_items_ret standard_combobox_lock
::replace_all_items(const std::vector<list_item_param> &items)
{
	new_items_ret ret;

	auto helper=noninthread_update_helper::create(*this, items, ret);

	locked_layoutmanager->impl->run_as
		([helper]
		 (ONLY IN_THREAD)
		 {
			 auto [lock, list_impl]=helper->lock();

			 replace_all_helper doit{IN_THREAD,
						 lock,
						 helper->new_text_params};

			 doit.start();
			 list_impl->replace_all_rows(IN_THREAD,
						     lock.locked_layoutmanager,
						     helper->info);
			 doit.finish();
		 });

	return ret;
}

new_items_ret standard_combobox_lock
::replace_all_items(ONLY IN_THREAD,
		    const std::vector<list_item_param> &items)
{
	new_items_ret ret;

	inthread_update_helper helper{*this, items, ret};

	replace_all_helper doit{IN_THREAD, *this, helper.new_text_params};


	doit.start();
	locked_layoutmanager->listlayoutmanagerObj::impl
		->list_element_singleton->impl
		->replace_all_rows(IN_THREAD,
				   locked_layoutmanager,
				   helper.info);
	doit.finish();

	return ret;
}

void standard_combobox_lock::resort_items(const std::vector<size_t> &indexes)
{
	// For consistency:

	new_items_ret ret;

	auto helper=noninthread_update_helper
		::create(*this, std::vector<list_item_param>{}, ret);

	locked_layoutmanager->impl->run_as
		([helper, indexes]
		 (ONLY IN_THREAD)
		 {
			 auto [lock, list_impl]=helper->lock();

			 lock.resort_items(IN_THREAD, indexes);
		 });
}

void standard_combobox_lock::resort_items(ONLY IN_THREAD,
					  const std::vector<size_t> &indexes)
{
	// Try to do the right thing when an exception gets thrown.

	auto s=make_sentry
		([&, this]
		 {
			 this->locked_layoutmanager
				 ->superclass_t::replace_all_items(IN_THREAD,
								   std::vector<list_item_param>{});
			 text_items().clear();
		 });

	auto cpy=indexes;

	s.guard();
	locked_layoutmanager->superclass_t::resort_items(IN_THREAD, indexes);

	auto &ti=text_items();

	sort_by(cpy,
		[&]
		(size_t a, size_t b)
		{
			std::swap(ti.at(a), ti.at(b));
		});

	s.unguard();
}

bool standard_combobox_lock::search(size_t starting_index,
				    const std::u32string &text,
				    size_t &found,
				    bool shortest_match) const
{
	auto &ti=text_items();

	size_t n=ti.size();

	size_t search_size=text.size();

	bool was_found=false;
	size_t was_found_size=0;

	for (size_t i=0; i<n; ++i)
	{
		size_t j=(i+starting_index) % n;

		const auto &string=ti.at(j).string;

		if (string.size() < search_size)
			continue;

		size_t l;

		for (l=0; l<search_size; ++l)
			if (unicode_lc(string[l]) != unicode_lc(text[l]))
				break;

		if (l == search_size && locked_layoutmanager->enabled(i))
		{
			if (!was_found || was_found_size > string.size())
			{
				found=j;
				was_found=true;
				was_found_size=string.size();
				if (!shortest_match)
					return true;
				// else keep searching.
			}
		}
	}

	return was_found;
}

//////////////////////////////////////////////////////////////////////////


standard_comboboxlayoutmanagerObj
::standard_comboboxlayoutmanagerObj(const ref<implObj> &impl,
				    const ref<listlayoutmanagerObj::implObj>
				    &list_layout_impl)
	: custom_comboboxlayoutmanagerObj(impl, list_layout_impl),
	  impl(impl)
{
}

standard_comboboxlayoutmanagerObj::~standard_comboboxlayoutmanagerObj()=default;

static void nosuchitem(size_t i)
{
	throw EXCEPTION(gettextmsg(_("Item %1% does not exist."), i));
}

new_items_ret standard_comboboxlayoutmanagerObj
::append_items(const std::vector<list_item_param> &items)
{
	standard_combobox_lock lock{standard_comboboxlayoutmanager(this)};

	return lock.append_items(items);
}

new_items_ret standard_comboboxlayoutmanagerObj
::append_items(ONLY IN_THREAD, const std::vector<list_item_param> &items)
{
	standard_combobox_lock lock{standard_comboboxlayoutmanager(this)};

	return lock.append_items(IN_THREAD, items);
}

new_items_ret standard_comboboxlayoutmanagerObj
::insert_items(size_t i,
	       const std::vector<list_item_param> &items)
{
	standard_combobox_lock lock{standard_comboboxlayoutmanager(this)};

	return lock.insert_items(i, items);
}

new_items_ret standard_comboboxlayoutmanagerObj
::insert_items(ONLY IN_THREAD, size_t i,
	       const std::vector<list_item_param> &items)
{
	standard_combobox_lock lock{standard_comboboxlayoutmanager(this)};

	return lock.insert_items(IN_THREAD, i, items);
}

new_items_ret standard_comboboxlayoutmanagerObj
::replace_items(size_t i,
		const std::vector<list_item_param> &items)
{
	standard_combobox_lock lock{standard_comboboxlayoutmanager(this)};

	return lock.replace_items(i, items);
}

new_items_ret standard_comboboxlayoutmanagerObj
::replace_items(ONLY IN_THREAD, size_t i,
		const std::vector<list_item_param> &items)
{
	standard_combobox_lock lock{standard_comboboxlayoutmanager(this)};

	return lock.replace_items(IN_THREAD, i, items);
}

void standard_comboboxlayoutmanagerObj::remove_items(size_t item_number,
						     size_t n_items)
{
	standard_combobox_lock lock{ref{this}};

	lock.remove_items(item_number, n_items);
}

void standard_comboboxlayoutmanagerObj::remove_items(ONLY IN_THREAD,
						     size_t i,
						     size_t n_items)
{
	standard_combobox_lock lock{ref{this}};

	lock.remove_items(IN_THREAD, i, n_items);
}

text_param standard_comboboxlayoutmanagerObj::item(size_t i) const
{
	const_standard_combobox_lock
		lock{const_standard_comboboxlayoutmanager(this)};

	return lock.item(i);
}

new_items_ret standard_comboboxlayoutmanagerObj
::replace_all_items(const std::vector<list_item_param> &items)
{
	standard_combobox_lock lock{standard_comboboxlayoutmanager(this)};

	return lock.replace_all_items(items);
}

new_items_ret standard_comboboxlayoutmanagerObj
::replace_all_items(ONLY IN_THREAD,
		    const std::vector<list_item_param> &items)
{
	standard_combobox_lock lock{standard_comboboxlayoutmanager(this)};

	return lock.replace_all_items(IN_THREAD, items);
}

void standard_comboboxlayoutmanagerObj
::resort_items(ONLY IN_THREAD, const std::vector<size_t> &indexes)
{
	standard_combobox_lock lock{standard_comboboxlayoutmanager(this)};

	lock.resort_items(IN_THREAD, indexes);
}

//////////////////////////////////////////////////////////////////////////////

// The standard combo-box uses a focusable_label to represent the current
// selection.

static const standard_combobox_selection_changed_t &
standard_combobox_selection_changed_noop()
{
	static const standard_combobox_selection_changed_t config=
		[]
		(ONLY IN_THREAD,
		 const standard_combobox_selection_changed_info_t &)
		{
		};

	return config;
}

static const custom_combobox_selection_search_t &
standard_combobox_selection_search()
{
	static const custom_combobox_selection_search_t config=
		// Install callback to search items using whatever was typed into the
		// current selection display element.
		//
		// NOTE: the custom combobox will want to know if we did
		// something, so we must use IN_THREAD to invoke
		// list layout manager methods.
		[]
		(ONLY IN_THREAD,
		 const auto &search_info)
		{
			standard_comboboxlayoutmanager lm=search_info.lm;

			if (search_info.text.size() == 0)
			{
				if (!search_info.selection_required)
					lm->unselect(IN_THREAD);
				return;
			}

			size_t found;

			standard_combobox_lock lock{lm};

			if (lock.search(search_info.starting_index,
					search_info.text,
					found, false))
			{
				if (!lm->selected(found))
					lm->autoselect(IN_THREAD, found,
						       search_info.trigger);
			}
		};

	return config;
}

new_standard_comboboxlayoutmanager::new_standard_comboboxlayoutmanager()
	: new_standard_comboboxlayoutmanager{
	standard_combobox_selection_changed_noop()
		}
{
}

focusable new_standard_comboboxlayoutmanager
::selection_factory(const factory &f) const
{
	return f->create_focusable_label("");
}

new_standard_comboboxlayoutmanager
::new_standard_comboboxlayoutmanager(const standard_combobox_selection_changed_t
				     &selection_changed)
	: selection_changed{selection_changed}
{
	selection_search=standard_combobox_selection_search();
}

new_standard_comboboxlayoutmanager
::~new_standard_comboboxlayoutmanager()=default;

static custom_combobox_selection_changed_t standard_selection_changed=
	[]
	(ONLY IN_THREAD, const auto &info)
	{
		standard_comboboxlayoutmanager lm=info.lm;
		x::w::focusable_label current_selection=info.current_selection;

		standard_combobox_lock lock{lm};

		if (info.list_item_status_info.selected)
		{
			current_selection->update
				(lock.item(info.list_item_status_info
					   .item_number));
		}
		else // Unselected.
		{
			current_selection->update("");
		}

		// The busy mcguffin in info is the busy
		// mcguffin for the popup window. The callback
		// would probably want to install the busy
		// mcguffin for the window that
		// contains the combo-box.
		busy_impl yes_i_am{*current_selection->elementObj::impl};

		lm->impl->selection_changed.get()
		(IN_THREAD, standard_combobox_selection_changed_info_t{
			lm,
			lock, info.list_item_status_info, yes_i_am});
	};

custom_combobox_selection_changed_t new_standard_comboboxlayoutmanager
::get_selection_changed() const
{
	return standard_selection_changed;
}

void standard_comboboxlayoutmanagerObj
::on_selection_changed(const standard_combobox_selection_changed_t &cb)
{
	if (!impl->selection_changed_enabled)
		throw EXCEPTION(_("This standard combo-box layoutmanager"
				  " function is disabled in editable"
				  " combo-boxes"));

	impl->selection_changed=cb;
}

void standard_comboboxlayoutmanagerObj
::on_selection_changed(ONLY IN_THREAD,
		       const standard_combobox_selection_changed_t &cb)
{
	on_selection_changed(cb);
}

ref<custom_comboboxlayoutmanagerObj::implObj>
new_standard_comboboxlayoutmanager
::create_impl(const create_impl_info &i) const
{
	return ref<standard_comboboxlayoutmanagerObj::implObj>
		::create(i.container_impl, *this,
			 selection_changed, true);
}

element standard_comboboxlayoutmanagerObj::current_selection()
{
	throw EXCEPTION(_("Standard combo-box current selection widgets are"
			  " not accessible"));
}

const_element standard_comboboxlayoutmanagerObj::current_selection() const
{
	throw EXCEPTION(_("Standard combo-box current selection widgets are"
			  " not accessible"));
}

LIBCXXW_NAMESPACE_END
