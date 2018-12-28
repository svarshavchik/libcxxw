/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"

#include <iostream>
#include <x/mpobj.H>
#include <x/destroy_callback.H>

#include "x/w/connection_threadfwd.H"

#define ONLY int

LIBCXX_NAMESPACE::mpcobj<int> thread_started=0;

LIBCXX_NAMESPACE::mpcobj<int> thread_stopped=0;

#define LIBCXXW_DEBUG_THREAD_STARTED()					\
	do {								\
									\
		std::cout << "THREAD STARTED" << std::endl;		\
		LIBCXX_NAMESPACE::mpcobj<int>::lock lock{thread_started}; \
		++*lock; lock.notify_all();				\
	} while(0)

#define LIBCXXW_DEBUG_THREAD_STOPPED()					\
	do {								\
									\
		std::cout << "THREAD STOPPED" << std::endl;		\
		LIBCXX_NAMESPACE::mpcobj<int>::lock lock{thread_stopped}; \
		++*lock; lock.notify_all();				\
	} while(0)

#include "input_field/input_field_search_thread.C"


using namespace LIBCXX_NAMESPACE;
using namespace LIBCXX_NAMESPACE::w;

class LIBCXX_PUBLIC my_search_threadObj;

mpcobj<int> search_counter=0;
mpcobj<int> wait_until_search_counter=0;

void wait_for_search(int n)
{
	mpcobj<int>::lock lock{search_counter};

	while (*lock < n)
		lock.wait();
}

void search_proceed(int n)
{
	mpcobj<int>::lock lock{wait_until_search_counter};

	*lock=n;
	lock.notify_all();
}

class abortflagObj : virtual public obj {

public:

	mpcobj<bool> flag=false;
};

class my_search_threadObj : public input_field_search_threadObj {

public:
	my_search_threadObj()
		: input_field_search_threadObj
		{
		 []
		 (const auto &info)
		 {
			 int n;

			 auto flag=ref<abortflagObj>::create();

			 info.get_abort_mcguffin()->ondestroy
				 ([flag]
				  {
					  mpcobj<bool>::lock lock{flag->flag};

					  *lock=true;
					  lock.notify_all();
				  });

			 {
				 mpcobj<int>::lock lock{search_counter};

				 n=++*lock;
				 lock.notify_all();

				 std::cout << "SEARCH "
					   << n << " STARTING" << std::endl;
			 }

			 {
				 mpcobj<int>::lock
					 lock{wait_until_search_counter};

				 while (*lock < n)
					 lock.wait();
			 }

			 if (info.search_string == U"EXCEPT")
				 throw EXCEPTION("Exception");

			 if (info.search_string == U"WAIT4ABORT")
			 {
				 mpcobj<bool>::lock lock{flag->flag};

				 while (!*lock)
					 lock.wait();
			 }
			 std::cout << "SEARCH "
				   << n << " PROCEEDING" << std::endl;

			 info.results({info.search_string});
		 }
		}
	{
		thread_started=0;
		thread_stopped=0;
		search_counter=0;
		wait_until_search_counter=0;
	}

	~my_search_threadObj()=default;

	mpobj<std::u32string> search_string;

	void search(const std::string &s)
	{
		{
			mpobj<std::u32string>::lock lock{search_string};

			*lock={s.begin(), s.end()};
		}
		search_request(0);
	}

	std::u32string get_search_string(ONLY IN_THREAD) override
	{
		return *mpobj<std::u32string>::lock{search_string};
	}

	void search_executed(const search_thread_info &info,
			     const search_thread_results &mcguffin) override
	{
		search_completed(0, info, mcguffin);
	}

	mpcobj<std::vector<std::u32string>> results;

	void search_results(ONLY IN_THREAD,
			    const std::vector<std::u32string>
			    &search_result_text,
			    const std::vector<list_item_param>
			    &search_result_items) override
	{
		if (search_result_text ==
		    std::vector<std::u32string>{U"EXCEPT2"})
			throw EXCEPTION("SEARCH");

		mpcobj<std::vector<std::u32string>>::lock lock{results};

		lock->insert(lock->end(), search_result_text.begin(),
			     search_result_text.end());
		lock.notify_all();
	}

	std::vector<std::string> wait_for_n_search_results(size_t n)
	{
		mpcobj<std::vector<std::u32string>>::lock lock{results};

		while (lock->size() < n)
			lock.wait();

		std::vector<std::string> s;

		for (const auto &us:*lock)
		{
			s.emplace_back(us.begin(), us.end());
		}

		return s;
	}

	void search_exception_message(const exception &e) override
	{
		mpcobj<std::vector<std::u32string>>::lock lock{results};

		std::ostringstream o;

		o << e;

		std::u32string msg{U"CAUGHT: "};

		std::string s=o.str();
		msg.insert(msg.end(), s.begin(), s.end());

		lock->push_back(msg);
	}

	void search_stop_message(const text_param &t) override
	{
	}
};

void test1()
{
	std::cout << "test1" << std::endl;

	{
		auto s=ref<my_search_threadObj>::create();

		s->search("one");

		wait_for_search(1);

		s->search("two");
		s->search("three");
		search_proceed(1);

		wait_for_search(2);
		search_proceed(2);

		if (s->wait_for_n_search_results(2) !=
		    std::vector<std::string>{"one","three"})
			throw EXCEPTION("test1 failed");

	}
	std::cout << "Waiting for thread to stop" << std::endl;
	mpcobj<int>::lock lock{thread_stopped};

	while (*lock < 1)
		lock.wait();
}

void test2()
{
	std::cout << "test2" << std::endl;

	{
		auto s=ref<my_search_threadObj>::create();

		s->search("WAIT4ABORT");

		wait_for_search(1);

		s->search_abort(0);

		s->search("two");
		search_proceed(2);

		if (s->wait_for_n_search_results(1) !=
		    std::vector<std::string>{"two"})
			throw EXCEPTION("test2 failed");

	}
	std::cout << "Waiting for thread to stop" << std::endl;
	mpcobj<int>::lock lock{thread_stopped};

	while (*lock < 1)
		lock.wait();
}

void test3()
{
	std::cout << "test3" << std::endl;

	{
		auto s=ref<my_search_threadObj>::create();

		s->search("one");

		wait_for_search(1);

		s->search_thread_request_stop(0);

		search_proceed(1);

		std::cout << "Waiting for thread to stop" << std::endl;
		{
			mpcobj<int>::lock lock{thread_stopped};

			while (*lock < 1)
				lock.wait();
		}
		s->search("two");
		search_proceed(2);

		if (s->wait_for_n_search_results(1) !=
		    std::vector<std::string>{"two"})
			throw EXCEPTION("test3 failed");

	}
	std::cout << "Waiting for thread to stop" << std::endl;
	mpcobj<int>::lock lock{thread_stopped};

	while (*lock < 2)
		lock.wait();

	if (thread_started.get() != 2)
		throw EXCEPTION("test3: started " << thread_started.get());
}

void test4()
{
	std::cout << "test4" << std::endl;

	{
		auto s=ref<my_search_threadObj>::create();

		s->search("one");

		wait_for_search(1);

		s->search_thread_request_stop(0);

		s->search("two");

		search_proceed(2);

		if (s->wait_for_n_search_results(1) !=
		    std::vector<std::string>{"two"})
			throw EXCEPTION("test4 failed");

	}
	std::cout << "Waiting for thread to stop" << std::endl;
	mpcobj<int>::lock lock{thread_stopped};

	while (*lock < 1)
		lock.wait();

	if (thread_started.get() != 1)
		throw EXCEPTION("test4: started " << thread_started.get());
}

void test5()
{
	std::cout << "test5" << std::endl;

	{
		auto s=ref<my_search_threadObj>::create();

		s->search("EXCEPT");
		s->search("two");

		wait_for_search(1);

		search_proceed(2);

		if (s->wait_for_n_search_results(2) !=
		    std::vector<std::string>{"CAUGHT: Exception", "two"})
			throw EXCEPTION("test5 failed");

	}
	std::cout << "Waiting for thread to stop" << std::endl;
	mpcobj<int>::lock lock{thread_stopped};

	while (*lock < 1)
		lock.wait();

	if (thread_started.get() != 1)
		throw EXCEPTION("test5: started " << thread_started.get());
}

void test6()
{
	std::cout << "test6" << std::endl;

	{
		auto s=ref<my_search_threadObj>::create();

		s->search("EXCEPT2");
		s->search("two");

		wait_for_search(1);

		search_proceed(2);

		if (s->wait_for_n_search_results(2) !=
		    std::vector<std::string>{"CAUGHT: SEARCH", "two"})
			throw EXCEPTION("test6 failed");

	}
	std::cout << "Waiting for thread to stop" << std::endl;
	mpcobj<int>::lock lock{thread_stopped};

	while (*lock < 1)
		lock.wait();

	if (thread_started.get() != 1)
		throw EXCEPTION("test6: started " << thread_started.get());
}

void test7()
{
	std::cout << "test7" << std::endl;

	{
		destroy_callback::base::guard guard;

		auto s=ref<my_search_threadObj>::create();

		guard(s);

		s->search("EXCEPT");
		s->search("two");

		wait_for_search(1);
	}

	search_proceed(1);

	std::cout << "Waiting for thread to stop" << std::endl;
	mpcobj<int>::lock lock{thread_stopped};

	while (*lock < 1)
		lock.wait();

	if (thread_started.get() != 1)
		throw EXCEPTION("test7: started " << thread_started.get());
	if (search_counter.get() != 1)
		throw EXCEPTION("test7: searched " << search_counter.get());
}

int main(int argc, char **argv)
{
	try {
		alarm(60);
		test1();
		test2();
		test3();
		test4();
		test5();
		test6();
		test7();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
