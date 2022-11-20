#include "libcxxw_config.h"
#include <x/property_properties.H>
#include <x/exception.H>
#include <x/property_properties.H>

#include "x/w/focus.H"
#include <vector>
#include <tuple>

LIBCXXW_NAMESPACE_START

#define ONLY int

struct elementObj {

	struct implObj;
};

class child_elementObj;

LIBCXXW_NAMESPACE_END

typedef LIBCXX_NAMESPACE::ref<LIBCXX_NAMESPACE::w::elementObj::implObj
			      > element_impl;

std::vector<std::tuple<int, LIBCXX_NAMESPACE::w::focus_change>> results;

struct LIBCXX_NAMESPACE::w::elementObj::implObj :
	virtual public LIBCXX_NAMESPACE::obj {

	int id;

	bool original_focus;
	bool new_focus;

	LIBCXX_NAMESPACE::w::elementObj::implObj &container_element_impl()
	{ return *this; }

	implObj(int id) : id(id) {}

	typedef void (elementObj::implObj::*focus_reporter_t)
		(ONLY IN_THREAD, focus_change, const callback_trigger_t &)
		;

	void request_focus(ONLY IN_THREAD,
			   const ptr<elementObj::implObj> &,
			   focus_reporter_t focus_reporter,
			   const callback_trigger_t &trigger);

	virtual void requested_focus_to(ONLY IN_THREAD,
					const ptr<elementObj::implObj> &);
	virtual void requested_focus_from(ONLY IN_THREAD);

	void leaving_focus(ONLY IN_THREAD,
			   const ptr<elementObj::implObj> &leaving_for,
			   focus_reporter_t focus_reporter,
			   const callback_trigger_t &trigger);

	virtual void do_leaving_focus(ONLY IN_THREAD,
				      LIBCXX_NAMESPACE::w::focus_change &event,
				      const element_impl &,
				      const ptr<elementObj::implObj> &leaving_for,
				      focus_reporter_t focus_reporter,
				      const callback_trigger_t &trigger);

	void entering_focus(ONLY IN_THREAD,
			    const ptr<elementObj::implObj> &focus_from,
			    focus_reporter_t focus_reporter,
			    const callback_trigger_t &trigger);

	virtual void do_entering_focus(ONLY IN_THREAD,
				       LIBCXX_NAMESPACE::w::focus_change event,
				       const element_impl &focus_to,
				       const ptr<elementObj::implObj> &focus_from,
				       focus_reporter_t focus_reporter,
				       const callback_trigger_t &trigger);

	void focus_event(ONLY IN_THREAD,
			 LIBCXX_NAMESPACE::w::focus_change event,
			 const callback_trigger_t &trigger)
	{
		results.push_back({id, event});
	}

	virtual void focus_movement_complete(ONLY IN_THREAD,
					     bool,
					     focus_reporter_t,
					     const callback_trigger_t &trigger);
};

struct LIBCXX_NAMESPACE::w::child_elementObj
	: public LIBCXX_NAMESPACE::w::elementObj::implObj {

	LIBCXX_NAMESPACE::w::elementObj::implObj *child_container;

	child_elementObj(int id,
			 LIBCXX_NAMESPACE::w::elementObj::implObj *child_container)
		: elementObj::implObj(id), child_container(child_container) {}

	void requested_focus_to(ONLY IN_THREAD,
				const ptr<elementObj::implObj> &)
		override;

	void requested_focus_from(ONLY IN_THREAD) override;

	void do_leaving_focus(ONLY IN_THREAD,
			      LIBCXX_NAMESPACE::w::focus_change &event,
			      const element_impl &,
			      const ptr<elementObj::implObj> &leaving_for,
			      focus_reporter_t focus_reporter,
			      const callback_trigger_t &trigger)
		override;

	void do_entering_focus(ONLY IN_THREAD,
			       LIBCXX_NAMESPACE::w::focus_change event,
			       const element_impl &focus_to,
			       const ptr<elementObj::implObj> &focus_from,
			       focus_reporter_t focus_reporter,
			       const callback_trigger_t &trigger)
		override;

	void focus_movement_complete(ONLY IN_THREAD,
				     bool,
				     focus_reporter_t,
				     const callback_trigger_t &trigger)
		override;
};

#define x_w_impl_child_element_h
#define x_w_impl_container_H
#include "focus/element_focusable.C"

using namespace LIBCXX_NAMESPACE;
using namespace LIBCXX_NAMESPACE::w;

//
//                  0
//                  |
//                  1
//                  |
//                  2
//               +--+--+
//               |     |
//               3     5
//               |     |
//               4     6

static const element_impl root=element_impl::create(0);
static const element_impl first=ref<child_elementObj>::create(1, &*root);
static const element_impl second=ref<child_elementObj>::create(2, &*first);
static const element_impl third=ref<child_elementObj>::create(3, &*second);
static const element_impl fourth=ref<child_elementObj>::create(4, &*third);
static const element_impl fifth=ref<child_elementObj>::create(5, &*second);
static const element_impl sixth=ref<child_elementObj>::create(6, &*fifth);

static const struct {
	const char *testname;
	ptr<elementObj::implObj> from, to;
	std::vector<std::tuple<int, focus_change>> expected_results;
} tests[]={
	{
		"lost",
		second,
		ptr<elementObj::implObj>(),
		{
			{2, focus_change::lost},
			{1, focus_change::child_lost},
			{0, focus_change::child_lost},
			{2, focus_change::focus_movement_complete},
			{1, focus_change::focus_movement_complete},
			{0, focus_change::focus_movement_complete},
		},
	},
	{
		"gained",
		ptr<elementObj::implObj>(),
		second,
		{
			{0, focus_change::child_gained},
			{1, focus_change::child_gained},
			{2, focus_change::gained},
			{2, focus_change::focus_movement_complete},
			{1, focus_change::focus_movement_complete},
			{0, focus_change::focus_movement_complete},
		},
	},

	// We receive focus gained events from new elements before the
	// focus lost elements for the old elements.
	//
	// This way a container can monitor the events of its child elements
	// and know that a child element lost its focus because a new child
	// element already received it. The list layout manager depends on
	// this behavior for optimal performance.
	//
	// Only a focus lost event without a focus gained event indicates
	// a true loss of focusage.
	{
		"shallow transit",
		fifth,
		third,
		{
			{5, focus_change::lost},
			{2, focus_change::child_moved},
			{1, focus_change::child_moved},
			{0, focus_change::child_moved},
			{0, focus_change::child_moved},
			{1, focus_change::child_moved},
			{2, focus_change::child_moved},
			{3, focus_change::gained},
			{3, focus_change::focus_movement_complete},
			{5, focus_change::focus_movement_complete},
			{2, focus_change::focus_movement_complete},
			{1, focus_change::focus_movement_complete},
			{0, focus_change::focus_movement_complete},
		},
	},
	{
		"deep transit",
		fourth,
		sixth,
		{
			{4, focus_change::lost},
			{3, focus_change::child_lost},
			{2, focus_change::child_moved},
			{1, focus_change::child_moved},
			{0, focus_change::child_moved},
			{0, focus_change::child_moved},
			{1, focus_change::child_moved},
			{2, focus_change::child_moved},
			{5, focus_change::child_gained},
			{6, focus_change::gained},
			{6, focus_change::focus_movement_complete},
			{5, focus_change::focus_movement_complete},
			{4, focus_change::focus_movement_complete},
			{3, focus_change::focus_movement_complete},
			{2, focus_change::focus_movement_complete},
			{1, focus_change::focus_movement_complete},
			{0, focus_change::focus_movement_complete},
		},
	},

	{
		"to parent",
		fifth,
		first,
		{
			{5, focus_change::lost},
			{2, focus_change::child_lost},
			{1, focus_change::gained_from_child},
			{0, focus_change::child_moved},
			{0, focus_change::child_moved},
			{1, focus_change::gained},
			{5, focus_change::focus_movement_complete},
			{2, focus_change::focus_movement_complete},
			{1, focus_change::focus_movement_complete},
			{0, focus_change::focus_movement_complete},
		},
	},
	{
		"to child",
		first,
		fifth,
		{
			{1, focus_change::lost},
			{0, focus_change::child_moved},
			{0, focus_change::child_moved},
			{1, focus_change::lost_to_child},
			{2, focus_change::child_gained},
			{5, focus_change::gained},
			{5, focus_change::focus_movement_complete},
			{2, focus_change::focus_movement_complete},
			{1, focus_change::focus_movement_complete},
			{0, focus_change::focus_movement_complete},
		},
	},
};

void testfocusable()
{
	for (const auto &t:tests)
	{
		results.clear();

		if (!t.to)
		{
			// This is equivalent to elementObj::implObj::lose_focus().
			t.from->requested_focus_from(0);
			t.from->leaving_focus(0, ptr<obj>(),
					      &elementObj::implObj::focus_event,
					      {});
			t.from->focus_movement_complete(0, false,
							&elementObj::implObj::focus_event,
							{});
		}
		else
		{
			t.to->request_focus(0, t.from,
					    &elementObj::implObj::focus_event,
					    {});
		}

		if (results != t.expected_results)
			throw EXCEPTION("Test \"" << t.testname
					<< "\" failed");
	}
}


int main()
{
	x::property::load_property("x::w::themes", "./themes", true, false);
	try {
		LIBCXX_NAMESPACE::property
			::load_property(LIBCXX_NAMESPACE_STR "::themes",
					"themes", true, true);
		testfocusable();
	} catch (const exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
