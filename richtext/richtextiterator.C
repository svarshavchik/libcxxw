/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "x/w/richtext/richtextiterator.H"
#include "x/w/impl/richtext/richtext.H"
#include "richtext/richtext_insert.H"
#include "richtext/richtextcursorlocation.H"
#include "richtext/richtext_impl.H"
#include "messages.H"
#include <utility>
#include <x/sentry.H>

LIBCXXW_NAMESPACE_START

/////////////////////////////////////////////////////////////////////////////
//
// The actual  of the iterator

richtextiteratorObj::richtextiteratorObj(const richtext &my_richtext,
					 const richtextcursorlocation
					 &my_location,
					 richtextfragmentObj *my_fragment,
					 size_t offset,
					 new_location location_option)
	: richtextcursorlocationownerObj{my_location, my_fragment,
	offset, location_option},
	  my_richtext(my_richtext)
{
}

richtextiteratorObj::richtextiteratorObj(const richtextiteratorObj &clone)
	: richtextcursorlocationownerObj{clone},
	  my_richtext(clone.my_richtext)
{
}

richtextiteratorObj::~richtextiteratorObj()=default;

richtextiterator richtextiteratorObj::begin() const
{
	return my_richtext->begin();
}


richtextiterator richtextiteratorObj::end() const
{
	return my_richtext->end();
}

richtextiterator richtextiteratorObj::pos(size_t offset) const
{
	return my_richtext->at(offset, new_location::bidi);
}

richtextiterator richtextiteratorObj::clone() const
{
	return my_richtext->read_only_lock
		([&, this]
		 (const auto &ignore)
		 {
			 return richtextiterator::create(*this);
		 });
}

void richtextiteratorObj::swap(const richtextiterator &other)
{
	assert_or_throw(my_richtext == other->my_richtext,
			"Iterators to two different objects.");

	my_richtext->read_only_lock([&, this]
				    (const auto &ignore)
				    {
					    std::swap(my_location,
						      other->my_location);
				    });
}

std::ptrdiff_t richtextiteratorObj::compare(const const_richtextiterator &other)
	const
{
	assert_or_throw(my_richtext == other->my_richtext,
			"Iterators to two different objects.");

	return my_richtext->read_only_lock
		([&, this]
		 (const auto &ignore)
		 {
			 return my_location->compare(*other->my_location);
		 });
}

bool richtextiteratorObj::left(ONLY IN_THREAD)
{
	return my_richtext->thread_lock
		(IN_THREAD,
		 [&, this]
		 (ONLY IN_THREAD, const auto &ignore)
		 {
			 return my_location->left(IN_THREAD);
		 });
}

bool richtextiteratorObj::right(ONLY IN_THREAD)
{
	return my_richtext->thread_lock
		(IN_THREAD,
		 [&, this]
		 (ONLY IN_THREAD, const auto &ignore)
		 {
			 return my_location->right(IN_THREAD);
		 });
}

void richtextiteratorObj::move(ONLY IN_THREAD, ssize_t howmuch)
{
	my_richtext->thread_lock
		(IN_THREAD,
		 [&, this]
		 (ONLY IN_THREAD, const auto &ignore)
		 {
			 my_location->move(IN_THREAD, howmuch);
		 });
}

void richtextiteratorObj::move_for_delete(ONLY IN_THREAD)
{
	my_richtext->thread_lock
		(IN_THREAD,
		 [&, this]
		 (ONLY IN_THREAD, const auto &ignore)
		 {
			 my_location->move_for_delete(IN_THREAD);
		 });
}

void richtextiteratorObj::start_of_line()
{
	my_richtext->read_only_lock
		([&, this]
		 (const auto &ignore)
		 {
			 my_location->start_of_line();
		 });
}

void richtextiteratorObj::up(ONLY IN_THREAD)
{
	my_richtext->thread_lock(IN_THREAD,
				 [this]
				 (ONLY IN_THREAD, const auto &lock)
				 {
					 my_location->up(IN_THREAD);
				 });
}

void richtextiteratorObj::down(ONLY IN_THREAD)
{
	my_richtext->thread_lock(IN_THREAD,
				 [this]
				 (ONLY IN_THREAD, const auto &lock)
				 {
					 my_location->down(IN_THREAD);
				 });
}

void richtextiteratorObj::page_up(ONLY IN_THREAD, dim_t height)
{
	my_richtext->thread_lock(IN_THREAD,
				 [height, this]
				 (ONLY IN_THREAD, const auto &lock)
				 {
					 my_location->page_up(IN_THREAD,
							      height);
				 });
}

void richtextiteratorObj::page_down(ONLY IN_THREAD, dim_t height)
{
	my_richtext->thread_lock(IN_THREAD,
				 [height, this]
				 (ONLY IN_THREAD, const auto &lock)
				 {
					 my_location->page_down(IN_THREAD,
								height);
				 });
}

bool richtextiteratorObj::moveto(ONLY IN_THREAD, coord_t x, coord_t y)
{
	return my_richtext->thread_lock
		(IN_THREAD,
		 [&, this]
		 (ONLY IN_THREAD, const auto &lock)
		 {
			return my_location->moveto(IN_THREAD, x, y);
		 });
}

void richtextiteratorObj::end_of_line()
{
	my_richtext->read_only_lock
		([&, this]
		 (const auto &ignore)
		 {
			 my_location->end_of_line();
		 });
}

/////////////////////////////////////////////////////////////////////////////
//
// Framework for implementing insert().
//
// In order to avoid code duplication for supporting insert()ion of both
// std::u32string and richtextstring, we need to do type erasure several
// times.

// The last level of type-erased function takes a richtext_insert_base
// parameter, invokes insert_at_location, in the richtext object, and
// returns what it returns.
//
// insert_callback_func defines the signature for this.

typedef void insert_callback_func(const richtext_insert_base &);

// The next level of type-erase takes a richtextcursorlocation, and
// invokes a type-erased function which takes the insert_callback_func as
// a parameter. The two versions of insert() will construct a subclass
// of richtext_insert_base, for the std::u32string and richtextstring,
// whichever is the case, and then invoke the type-erased function.

struct LIBCXX_HIDDEN richtextiteratorObj::internal_insert {


	virtual void
		operator()( const richtextiterator &i,
			    const function< insert_callback_func > &) const=0;
};

// Implement type-erasure for internal_insert. The template parameter is
// either a std::u32string_view or a richtextstring. This is constructed directly
// by insert(), and captures a reference to one or the other string type.
//
// The implemented operator() has everything it needs to construct a
// richtext_insert<> subclass of richtext_insert_base, and invoke the
// type-erased function.

template<typename type>
class LIBCXX_HIDDEN richtextiteratorObj::internal_insert_impl
	: public internal_insert {

 public:
	type &s;
	const bool replacing_hotspots;

	internal_insert_impl(type &s,
			     bool replacing_hotspots)
		: s{s}, replacing_hotspots{replacing_hotspots} {}

	void operator()( const richtextiterator &i,
			 const function< insert_callback_func> &f)
		const override
	{
		f( richtext_insert<type>(i, replacing_hotspots, s) );
	}
};

// We have now reduced insert()s to constructing an internal_insert
// subclass, and invoking the private function.

richtextiterator
richtextiteratorObj::insert(ONLY IN_THREAD,
			    richtextstring &&new_string)
{
	return insert(IN_THREAD,
		      internal_insert_impl<richtextstring>{new_string, false});
}

richtextiterator richtextiteratorObj::insert(ONLY IN_THREAD,
					     const std::u32string_view
					     &new_string)
{
	return insert(IN_THREAD,
		      internal_insert_impl<const std::u32string_view>
		      {new_string, false});
}

void richtextiteratorObj::replace(ONLY IN_THREAD,
				  const const_richtextiterator &other,
				  richtextstring &&new_string,
				  bool replacing_hotspots) const
{
	replace(IN_THREAD,
		other,
		internal_insert_impl<richtextstring>{new_string,
			replacing_hotspots});
}

void richtextiteratorObj::replace(ONLY IN_THREAD,
				  const const_richtextiterator &other,
				  const std::u32string_view &new_string) const
{
	replace(IN_THREAD,
		other,
		internal_insert_impl<const std::u32string_view>
		{new_string, false});
}

// This handles the common insert() code. The type of the inserted string
// is effectively type-erased in the new_string.

struct LIBCXX_HIDDEN richtextiteratorObj::insert_lock {

	richtextObj::impl_t::lock &lock;
};

richtextiterator richtextiteratorObj::insert(ONLY IN_THREAD,
					     const internal_insert &new_string)
{
	return my_richtext->thread_lock
		(IN_THREAD,
		 [&, this]
		 (ONLY IN_THREAD, auto &lock)
		 {
			 insert_lock wrapper{lock};

			 return this->insert(IN_THREAD, wrapper, new_string);
		 });
}

richtextiterator richtextiteratorObj::insert(ONLY IN_THREAD,
					     struct insert_lock &wrapper,
					     const internal_insert &new_string)
{
	auto orig=richtextiterator::create(*this);

	// Clone the insert position, and tell the insert
	// code not to adjust it, temporarily.

	orig->my_location->do_not_adjust_in_insert=true;

	// We now type-erase the insert_callback_func
	// to complete the process of constructing the
	// appropriate insert_richtext subclass, and
	// invoking insert_at_location().

	new_string(orig,
		   make_function<insert_callback_func>
		   ([&, this]
		    (const richtext_insert_base &s) {
			   this->my_richtext->insert_at_location
				   (IN_THREAD, wrapper.lock, s);
		   }));

	orig->my_location->do_not_adjust_in_insert=false;

	return orig;
}

void richtextiteratorObj::remove(ONLY IN_THREAD,
				 const const_richtextiterator &other) const
{
	assert_or_throw(my_richtext == other->my_richtext,
			"Iterators to two different objects.");

	my_richtext->thread_lock
		(IN_THREAD,
		 [&, this]
		 (ONLY IN_THREAD, auto &lock)
		 {
			 my_richtext->remove_at_location(IN_THREAD, lock,
							 my_location,
							 other->my_location);
		 });
}

void richtextiteratorObj::replace(ONLY IN_THREAD,
				  const const_richtextiterator &other,
				  const internal_insert &new_string) const
{
	auto c=compare(other);

	if (c == 0)
		throw EXCEPTION(_("Cannot replace empty text, sorry."));

	if (c > 0)
	{
		other->replace(IN_THREAD, const_richtextiterator(this),
			       new_string);
		return;
	}

	my_richtext->thread_lock
		(IN_THREAD,
		 [&, this]
		 (ONLY IN_THREAD, auto &lock)
		 {
			 insert_lock wrapper{lock};

			 this->replace(IN_THREAD, wrapper,
				       other, new_string);
		 });
}

void richtextiteratorObj::replace(ONLY IN_THREAD,
				  struct insert_lock &wrapper,
				  const const_richtextiterator &other,
				  const internal_insert &new_string) const
{
	auto orig=richtextiterator::create(*this);

	// Clone the end of the insert position, and tell the insert
	// code not to adjust it, temporarily.

	my_location->do_not_adjust_in_insert=true;

	auto sentry=make_sentry([my_location=this->my_location]
	{
		my_location->do_not_adjust_in_insert=false;
	});

	// We now type-erase the insert_callback_func
	// to complete the process of constructing the
	// appropriate insert_richtext subclass, and
	// invoking insert_at_location().

	new_string(orig,
		   make_function<insert_callback_func>
		   ([&, this]
		    (const richtext_insert_base &s) {
			   this->my_richtext->replace_at_location
				   (IN_THREAD, wrapper.lock, s,
				    orig->my_location, other->my_location);
		   }));
}

richtextiteratorObj::at_info richtextiteratorObj::at(ONLY IN_THREAD) const
{
	return my_richtext->thread_lock
		(IN_THREAD,
		 [this]
		 (ONLY IN_THREAD, const auto &lock)
		 {
			 auto f=my_location->my_fragment;
			 auto o=my_location->get_offset();

			 assert_or_throw(f && o < f->string.size(),
					 "Internal error: invalid fragment");

			 return at_info{
				 {
					 coord_t::truncate
						 (my_location->get_horiz_pos
						  (IN_THREAD)),
					 f->y_position(),
					 f->horiz_info.width(o),
					 f->height()
				 },
					 f->string.get_string().at(o),
					 f->string.meta_at(o).link
						 };
		 });
}

size_t richtextiteratorObj::pos() const
{
	return my_richtext->read_only_lock
		([this]
		 (const auto &lock)
		 {
			 return my_richtext->pos(lock, my_location);
		 });
}

richtextstring
richtextiteratorObj::get_richtextstring(const const_richtextiterator &other,
					const std::optional<bidi_format>
					&embedding)
	const
{
	assert_or_throw(my_richtext == other->my_richtext,
			"Iterators to two different objects.");

	return my_richtext->read_only_lock
		([&, this]
		 (auto &lock)
		{
			return my_richtext->get(lock, my_location,
						other->my_location,
						embedding);
		});
}

size_t richtextiteratorObj::count_richtextstring(const const_richtextiterator &other)
	const
{
	assert_or_throw(my_richtext == other->my_richtext,
			"Iterators to two different objects.");

	return my_richtext->read_only_lock
		([&, this]
		 (auto &lock)
		{
			return my_richtext->count(lock, my_location,
						  other->my_location);
		});
}

dim_t richtextiteratorObj::horiz_pos(ONLY IN_THREAD)
{
	return my_richtext->thread_lock
		(IN_THREAD,
		 [this]
		 (ONLY IN_THREAD, auto &lock)
		 {
			 return my_location->get_horiz_pos(IN_THREAD);
		 });
}

void richtextiteratorObj::set_cursor(ONLY IN_THREAD, bool cursor_on)
{
	my_richtext->thread_lock
		(IN_THREAD,
		 [cursor_on, this]
		 (ONLY IN_THREAD, auto &lock)
		 {
			 assert_or_throw(my_location->my_fragment,
					 "my_fragment cannot be null.");
			 my_location->cursor_on=cursor_on;
			 my_location->my_fragment->redraw_needed=true;
		 });
}

std::tuple<richtextiterator, richtextiterator>
richtextiteratorObj::select_word() const
{
	return my_richtext->read_only_lock
		([&, this]
		 (auto &lock)
		{
			return my_richtext->select_word(lock, my_location);
		});
}

LIBCXXW_NAMESPACE_END
