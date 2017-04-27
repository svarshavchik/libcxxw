/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "richtext/richtextiterator.H"
#include "richtext/richtext.H"
#include "richtext/richtext_insert.H"
#include "richtext/richtextcursorlocation.H"

#include <utility>
LIBCXXW_NAMESPACE_START

/////////////////////////////////////////////////////////////////////////////
//
// The actual  of the iterator

richtextiteratorObj::richtextiteratorObj(const richtext &my_richtext,
					 const richtextcursorlocation
					 &my_location,
					 richtextfragmentObj *my_fragment,
					 size_t offset)
	: my_richtext(my_richtext),
	  my_location(my_location)
{
	my_location->initialize(my_fragment, offset);
}

richtextiteratorObj::richtextiteratorObj(const richtextiteratorObj &clone)
	: my_richtext(clone.my_richtext),
	  my_location(richtextcursorlocation::create())
{
	my_location->initialize(clone.my_location);
}

richtextiteratorObj::~richtextiteratorObj() noexcept
{
	my_location->deinitialize();
}


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
	return my_richtext->at(offset);
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

void richtextiteratorObj::move(IN_THREAD_ONLY, ssize_t howmuch)
{
	my_richtext->thread_lock
		(IN_THREAD,
		 [&, this]
		 (IN_THREAD_ONLY, const auto &ignore)
		 {
			 my_location->move(IN_THREAD, howmuch);
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

void richtextiteratorObj::up(IN_THREAD_ONLY)
{
	my_richtext->thread_lock(IN_THREAD,
				 [this]
				 (IN_THREAD_ONLY, const auto &lock)
				 {
					 my_location->up(IN_THREAD);
				 });
}

void richtextiteratorObj::down(IN_THREAD_ONLY)
{
	my_richtext->thread_lock(IN_THREAD,
				 [this]
				 (IN_THREAD_ONLY, const auto &lock)
				 {
					 my_location->down(IN_THREAD);
				 });
}

void richtextiteratorObj::page_up(IN_THREAD_ONLY, dim_t height)
{
	my_richtext->thread_lock(IN_THREAD,
				 [height, this]
				 (IN_THREAD_ONLY, const auto &lock)
				 {
					 my_location->page_up(IN_THREAD,
							      height);
				 });
}

void richtextiteratorObj::page_down(IN_THREAD_ONLY, dim_t height)
{
	my_richtext->thread_lock(IN_THREAD,
				 [height, this]
				 (IN_THREAD_ONLY, const auto &lock)
				 {
					 my_location->page_down(IN_THREAD,
								height);
				 });
}

void richtextiteratorObj::moveto(IN_THREAD_ONLY, coord_t x, coord_t y)
{
	my_richtext->thread_lock(IN_THREAD,
				 [&, this]
				 (IN_THREAD_ONLY, const auto &lock)
				 {
					 my_location->moveto(IN_THREAD, x, y);
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

typedef size_t insert_callback_func(const richtext_insert_base &);

// The next level of type-erase takes a richtextcursorlocation, and
// invokes a type-erased function which takes the insert_callback_func as
// a parameter. The two versions of insert() will construct a subclass
// of richtext_insert_base, for the std::u32string and richtextstring,
// whichever is the case, and then invoke the type-erased function.

struct LIBCXX_HIDDEN richtextiteratorObj::internal_insert {


	virtual size_t
		operator()( const richtextcursorlocation &l,
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
	const type &s;

	internal_insert_impl(const type &s) : s{s} {}

	size_t operator()( const richtextcursorlocation &l,
			   const function< insert_callback_func> &f)
		const override
	{
		return f( richtext_insert<type>(l, s) );
	}
};

// We have now reduced insert()s to constructing an internal_insert
// subclass, and invoking the private function.

std::pair<richtextiterator, size_t>
richtextiteratorObj::insert(IN_THREAD_ONLY,
			    const richtextstring &new_string)
{
	return insert(IN_THREAD,
		      internal_insert_impl<richtextstring>{new_string});
}

std::pair<richtextiterator, size_t>
richtextiteratorObj::insert(IN_THREAD_ONLY,
			    const std::experimental::u32string_view &new_string)
{
	return insert(IN_THREAD,
		      internal_insert_impl<std::experimental::u32string_view>
		      {new_string});
}

// This handles the common insert() code. The type of the inserted string
// is effectively type-erased in the new_string.

std::pair<richtextiterator, size_t>
richtextiteratorObj::insert(IN_THREAD_ONLY,
			    const internal_insert &new_string)
{
	return my_richtext->thread_lock
		(IN_THREAD,
		 [&, this]
		 (IN_THREAD_ONLY, auto &lock)
		 {
			 auto orig=richtextiterator::create(*this);

			 // Clone the insert position, and tell the insert
			 // code not to adjust it, temporarily.

			 orig->my_location->do_not_adjust_in_insert=true;

			 // We now type-erase the insert_callback_func
			 // to complete the process of constructing the
			 // appropriate insert_richtext subclass, and
			 // invoking insert_at_location().

			 auto n=new_string
				 (orig->my_location,
				  make_function<insert_callback_func>
				  ([&, this]
				   (const richtext_insert_base &s) {
					  return this->my_richtext
					  ->insert_at_location
					  (IN_THREAD, lock, s);
				 }));

			 orig->my_location->do_not_adjust_in_insert=false;

			 return std::make_pair(orig, n);
		 });
}

void richtextiteratorObj::remove(IN_THREAD_ONLY,
				 const const_richtextiterator &other) const
{
	assert_or_throw(my_richtext == other->my_richtext,
			"Iterators to two different objects.");

	my_richtext->thread_lock
		(IN_THREAD,
		 [&, this]
		 (IN_THREAD_ONLY, auto &lock)
		 {
			 my_richtext->remove_at_location(IN_THREAD, lock,
							 my_location,
							 other->my_location);
		 });
}

richtextiteratorObj::at_info richtextiteratorObj::at(IN_THREAD_ONLY) const
{
	return my_richtext->thread_lock
		(IN_THREAD,
		 [this]
		 (IN_THREAD_ONLY, const auto &lock)
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
					 f->string.get_string().at(o)};
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

richtextstring richtextiteratorObj::get(const const_richtextiterator &other)
	const
{
	assert_or_throw(my_richtext == other->my_richtext,
			"Iterators to two different objects.");

	richtextstring ret;

	my_richtext->read_only_lock
		([&, this]
		 (auto &lock)
		 {
			 // Estimate how big ret will be. We can compute the
			 // number of characters exactly. Use the number of
			 // lines as the estimate for the number of metadata
			 // changes. richtextstring's insert() is optimized
			 // to avoid coalescing the metadata until it's
			 // needed. So it will end up inserting a metadata
			 // record for every line, initially.
			 //
			 // TODO: when we support rich text editing, we'll
			 // need to add some additional overhead, here.


			 auto pos1=my_richtext->pos(lock, my_location);
			 auto pos2=my_richtext->pos(lock, other->my_location);

			 if (pos1 > pos2)
				 std::swap(pos1, pos2);

			 auto index1=my_location->my_fragment->index();
			 auto index2=other->my_location->my_fragment->index();

			 if (index1 > index2)
				 std::swap(index1, index2);

			 ret.reserve(pos2-pos1+1, index2-index1+1);

			 // And now that the buffers are ready and waiting...
			 my_richtext->get(lock, ret, my_location,
					  other->my_location);
		 });

	return ret;
}

dim_t richtextiteratorObj::horiz_pos(IN_THREAD_ONLY)
{
	return my_richtext->thread_lock
		(IN_THREAD,
		 [this]
		 (IN_THREAD_ONLY, auto &lock)
		 {
			 return my_location->get_horiz_pos(IN_THREAD);
		 });
}

void richtextiteratorObj::set_cursor(IN_THREAD_ONLY, bool cursor_on)
{
	my_richtext->thread_lock
		(IN_THREAD,
		 [cursor_on, this]
		 (IN_THREAD_ONLY, auto &lock)
		 {
			 assert_or_throw(my_location->my_fragment,
					 "my_fragment cannot be null.");
			 my_location->cursor_on=cursor_on;
			 my_location->my_fragment->redraw_needed=true;
		 });
}

LIBCXXW_NAMESPACE_END
