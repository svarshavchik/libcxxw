#include "libcxxw_config.h"
#include <x/exception.H>

#include "x/w/focus.H"
#include <vector>
#include <tuple>

LIBCXXW_NAMESPACE_START

#define IN_THREAD_ONLY int thread_

#define IN_THREAD thread_

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

	LIBCXX_NAMESPACE::w::elementObj::implObj &get_element_impl()
	{ return *this; }

	implObj(int id) : id(id) {}

	typedef void (elementObj::implObj::*focus_reporter_t)
		(IN_THREAD_ONLY, focus_change)
		;

	void request_focus(IN_THREAD_ONLY,
			   const ptr<elementObj::implObj> &,
			   focus_reporter_t focus_reporter);

	virtual void requested_focus_to(IN_THREAD_ONLY,
					const ptr<elementObj::implObj> &);
	virtual void requested_focus_from(IN_THREAD_ONLY);

	void leaving_focus(IN_THREAD_ONLY,
			   const ptr<elementObj::implObj> &leaving_for,
			   focus_reporter_t focus_reporter);

	virtual void do_leaving_focus(IN_THREAD_ONLY,
				      LIBCXX_NAMESPACE::w::focus_change &event,
				      const element_impl &,
				      const ptr<elementObj::implObj> &leaving_for,
				      focus_reporter_t focus_reporter);

	void entering_focus(IN_THREAD_ONLY,
			    const ptr<elementObj::implObj> &focus_from,
			    focus_reporter_t focus_reporter);

	virtual void do_entering_focus(IN_THREAD_ONLY,
				       LIBCXX_NAMESPACE::w::focus_change event,
				       const element_impl &focus_to,
				       const ptr<elementObj::implObj> &focus_from,
				       focus_reporter_t focus_reporter);

	void focus_event(IN_THREAD_ONLY,
			 LIBCXX_NAMESPACE::w::focus_change event)
	{
		results.push_back({id, event});
	}

	virtual void focus_movement_complete(IN_THREAD_ONLY,
					     bool,
					     focus_reporter_t);
};

struct LIBCXX_NAMESPACE::w::child_elementObj
	: public LIBCXX_NAMESPACE::w::elementObj::implObj {

	LIBCXX_NAMESPACE::w::elementObj::implObj *child_container;

	child_elementObj(int id,
			 LIBCXX_NAMESPACE::w::elementObj::implObj *child_container)
		: elementObj::implObj(id), child_container(child_container) {}

	void requested_focus_to(IN_THREAD_ONLY,
				const ptr<elementObj::implObj> &)
		override;

	void requested_focus_from(IN_THREAD_ONLY) override;

	void do_leaving_focus(IN_THREAD_ONLY,
			      LIBCXX_NAMESPACE::w::focus_change &event,
			      const element_impl &,
			      const ptr<elementObj::implObj> &leaving_for,
			      focus_reporter_t focus_reporter)
		override;

	void do_entering_focus(IN_THREAD_ONLY,
			       LIBCXX_NAMESPACE::w::focus_change event,
			       const element_impl &focus_to,
			       const ptr<elementObj::implObj> &focus_from,
			       focus_reporter_t focus_reporter)
		override;

	void focus_movement_complete(IN_THREAD_ONLY,
				     bool,
				     focus_reporter_t) override;
};

#define child_element_h
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
					      &elementObj::implObj::focus_event);
			t.from->focus_movement_complete(0, false,
							&elementObj::implObj::focus_event);
		}
		else
		{
			t.to->request_focus(0, t.from,
					    &elementObj::implObj::focus_event);
		}

		if (results != t.expected_results)
			throw EXCEPTION("Test \"" << t.testname
					<< "\" failed");
	}
}


int main()
{
	try {
		testfocusable();
	} catch (const exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
