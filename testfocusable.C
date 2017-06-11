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

std::vector<std::tuple<int, int, LIBCXX_NAMESPACE::w::focus_change>> results;

struct LIBCXX_NAMESPACE::w::elementObj::implObj :
	virtual public LIBCXX_NAMESPACE::obj {

	int id;

	bool original_focus;
	bool new_focus;

	LIBCXX_NAMESPACE::w::elementObj::implObj &get_element_impl()
	{ return *this; }

	implObj(int id) : id(id) {}

	typedef void (elementObj::implObj::*focus_reporter_t)
		(IN_THREAD_ONLY, focus_change, const ref<elementObj::implObj> &)
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
			 LIBCXX_NAMESPACE::w::focus_change event,
			 const element_impl &ptr)
	{
		results.push_back({ptr->id, id, event});
	}
};

struct LIBCXX_NAMESPACE::w::child_elementObj
	: public LIBCXX_NAMESPACE::w::elementObj::implObj {

	LIBCXX_NAMESPACE::w::elementObj::implObj *container;

	child_elementObj(int id,
			 LIBCXX_NAMESPACE::w::elementObj::implObj *container)
		: elementObj::implObj(id), container(container) {}

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
};

#define child_element_h
#include "focus/element_focusable.C"

using namespace LIBCXX_NAMESPACE;
using namespace LIBCXX_NAMESPACE::w;

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
	std::vector<std::tuple<int, int, focus_change>> expected_results;
} tests[]={
	{
		"lost",
		second,
		ptr<elementObj::implObj>(),
		{
			{2, 2, focus_change::lost},
			{2, 1, focus_change::child_lost},
			{2, 0, focus_change::child_lost},
		},
	},
	{
		"gained",
		ptr<elementObj::implObj>(),
		second,
		{
			{2, 0, focus_change::child_gained},
			{2, 1, focus_change::child_gained},
			{2, 2, focus_change::gained},
		},
	},
	{
		"shallow transit",
		fifth,
		third,
		{
			{5, 5, focus_change::lost},
			{5, 2, focus_change::child_moved_from},
			{5, 1, focus_change::child_moved_from},
			{5, 0, focus_change::child_moved_from},
			{3, 0, focus_change::child_moved_to},
			{3, 1, focus_change::child_moved_to},
			{3, 2, focus_change::child_moved_to},
			{3, 3, focus_change::gained},
		},
	},
	{
		"deep transit",
		fourth,
		sixth,
		{
			{4, 4, focus_change::lost},
			{4, 3, focus_change::child_lost},
			{4, 2, focus_change::child_moved_from},
			{4, 1, focus_change::child_moved_from},
			{4, 0, focus_change::child_moved_from},
			{6, 0, focus_change::child_moved_to},
			{6, 1, focus_change::child_moved_to},
			{6, 2, focus_change::child_moved_to},
			{6, 5, focus_change::child_gained},
			{6, 6, focus_change::gained},
		},
	},

	{
		"to parent",
		fifth,
		first,
		{
			{5, 5, focus_change::lost},
			{5, 2, focus_change::child_lost},
			{5, 1, focus_change::gained_from_child},
			{5, 0, focus_change::child_moved_from},
			{1, 0, focus_change::child_moved_to},
			{1, 1, focus_change::gained},
		},
	},

	{
		"to child",
		first,
		fifth,
		{
			{1, 1, focus_change::lost},
			{1, 0, focus_change::child_moved_from},
			{5, 0, focus_change::child_moved_to},
			{5, 1, focus_change::lost_to_child},
			{5, 2, focus_change::child_gained},
			{5, 5, focus_change::gained},
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
			t.from->requested_focus_from(0);
			t.from->leaving_focus(0, ptr<obj>(),
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
