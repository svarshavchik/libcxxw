/*
** Copyright 2018 Double Precision, Inc.
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

class debug {

	mpobj<bool>::lock lock;

	static mpobj<bool> debug_dump;

public:

	debug() : lock{debug_dump} {}

	~debug()
	{
		std::cout << std::endl;
	}

	template<typename T>
	const debug &operator<<(T && t) const
	{
		std::cout << std::forward<T>(t);
		return *this;
	}
};

mpobj<bool> debug::debug_dump{false};

void wait_for_search(int n)
{
	debug{} << "MAIN: WAIT_FOR_SEARCH " << n << " STARTED";
	mpcobj<int>::lock lock{search_counter};

	while (*lock < n)
		lock.wait();

	debug{} << "MAIN: WAIT_FOR_SEARCH " << n << " FINISHED";
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

				 debug{} << "THREAD: SEARCH "
					   << n << " STARTING";
			 }

			 {
				 mpcobj<int>::lock
					 lock{wait_until_search_counter};

				 while (*lock < n)
					 lock.wait();
			 }

			 if (info.search_string == U"EXCEPT")
			 {
				 debug{} << "THREAD: EXCEPTION THROWN"
					  ;
				 throw EXCEPTION("Exception");
			 }
			 if (info.search_string == U"WAIT4ABORT")
			 {
				 mpcobj<bool>::lock lock{flag->flag};

				 while (!*lock)
					 lock.wait();
			 }
			 debug{} << "THREAD: SEARCH "
				   << n << " PROCEEDING: "
				   << std::string{info.search_string.begin(),
						  info.search_string.end()}
				  ;

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
		mpobj<std::u32string>::lock lock{search_string};

		debug{} << "MAIN: SEARCH " << std::string{lock->begin(),
				lock->end()};
		return *lock;
	}

	typedef mpcobj<std::optional<std::tuple<search_thread_info,
						search_thread_results>>
		       > results_t;

	results_t results;

	std::vector<std::u32string> final_results;

	void search_executed(const search_thread_info &info,
			     const search_thread_results &mcguffin) override
	{
		results_t::lock lock{results};

		debug{} << "THREAD: SEARCH_EXECUTED, PLACING RESULTS";
		while (*lock)
			lock.wait();

		*lock=std::tuple{info, mcguffin};

		debug{} << "THREAD: RESULTS PLACED";
		lock.notify_all();
	}

	void search_results(ONLY IN_THREAD,
			    const std::vector<std::u32string>
			    &search_result_text,
			    const std::vector<list_item_param>
			    &search_result_items) override
	{
		{
			debug d;

			d << "MAIN: RECEIVED " << search_result_text.size()
			  << " RESULTS\n";
			for (const auto &s:search_result_text)
				d << "    "
				  << std::string{s.begin(), s.end()} << "\n";
		}

		final_results.insert(final_results.end(),
				     search_result_text.begin(),
				     search_result_text.end());

		if (search_result_text ==
		    std::vector<std::u32string>{U"EXCEPT2"})
			throw EXCEPTION("SEARCH");
	}

	std::vector<std::string> wait_for_n_search_results(size_t n)
	{
		while (final_results.size() < n)
		{
			auto [info, mcguffin]=
				({
					results_t::lock lock{results};

					debug{} << "MAIN: WAITING FOR " << n
						  << " RESULTS, "
						"I CURRENTLY HAVE "
						  << final_results.size()
						 ;

					while (!*lock)
						lock.wait();

					auto [info, mcguffin]=**lock;

					lock->reset();

					lock.notify_all();

					debug{} << "MAIN: A RESULT WAS COMPLETED"
						 ;

					std::tuple{info, mcguffin};
				});

			search_completed(0, info, mcguffin);
		}
		std::vector<std::string> s;

		for (const auto &us:final_results)
			s.emplace_back(us.begin(), us.end());

		return s;
	}

	mpobj<std::string> caught_exception;

	void search_exception_message(const exception &e) override
	{
		std::ostringstream o;

		o << e;

		caught_exception=std::string{"CAUGHT: "}+o.str();

		debug{} << "CAUGHT: " << caught_exception.get();
	}

	void search_stop_message(const text_param &t) override
	{
	}
};

void test1()
{
	debug{} << "test1";

	{
		auto s=ref<my_search_threadObj>::create();

		s->search("one");

		wait_for_search(1);

		s->search("two");
		s->search("three");
		search_proceed(2);

		if (s->wait_for_n_search_results(1) !=
		    std::vector<std::string>{"three"})
			throw EXCEPTION("test1 failed");

	}
	debug{} << "Waiting for thread to stop";
	mpcobj<int>::lock lock{thread_stopped};

	while (*lock < 1)
		lock.wait();
}

void test2()
{
	debug{} << "test2";

	{
		auto s=ref<my_search_threadObj>::create();

		s->search("WAIT4ABORT");

		wait_for_search(1);

		s->search_abort(0);

		s->search("one");
		s->search("two");
		search_proceed(2);

		if (s->wait_for_n_search_results(1) !=
		    std::vector<std::string>{"two"})
			throw EXCEPTION("test2 failed");

	}
	debug{} << "Waiting for thread to stop";
	mpcobj<int>::lock lock{thread_stopped};

	while (*lock < 1)
		lock.wait();
}

void test3()
{
	debug{} << "test3";

	{
		auto s=ref<my_search_threadObj>::create();

		s->search("one");

		wait_for_search(1);

		s->search_thread_request_stop(0);

		search_proceed(1);

		debug{} << "Waiting for thread to stop";
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
	debug{} << "Waiting for thread to stop";
	mpcobj<int>::lock lock{thread_stopped};

	while (*lock < 2)
		lock.wait();

	if (thread_started.get() != 2)
		throw EXCEPTION("test3: started " << thread_started.get());
}

void test4()
{
	debug{} << "test4";

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
	debug{} << "Waiting for thread to stop";
	mpcobj<int>::lock lock{thread_stopped};

	while (*lock < 1)
		lock.wait();

	if (thread_started.get() != 1)
		throw EXCEPTION("test4: started " << thread_started.get());
}

void test5()
{
	debug{} << "test5";

	{
		auto s=ref<my_search_threadObj>::create();

		s->search("EXCEPT");

		wait_for_search(1);

		s->search("two");

		search_proceed(2);

		if (s->wait_for_n_search_results(1) !=
		    std::vector<std::string>{"two"})
			throw EXCEPTION("test5 did not get its results");

		if (s->caught_exception.get() != "CAUGHT: Exception")
			throw EXCEPTION("test5 did not catch an exception");

	}
	debug{} << "Waiting for thread to stop";
	mpcobj<int>::lock lock{thread_stopped};

	while (*lock < 1)
		lock.wait();

	if (thread_started.get() != 1)
		throw EXCEPTION("test5: started " << thread_started.get());
}

void test6()
{
	debug{} << "test6";

	{
		auto s=ref<my_search_threadObj>::create();

		s->search("EXCEPT2");
		wait_for_search(1);
		search_proceed(1);

		s->wait_for_n_search_results(1);

		s->search("two");

		search_proceed(2);

		if (s->wait_for_n_search_results(2) !=
		    std::vector<std::string>{"EXCEPT2", "two"})
			throw EXCEPTION("test6 did not get its results");

		if (s->caught_exception.get() != "CAUGHT: SEARCH")
			throw EXCEPTION("test6 did not catch an exception");

	}
	debug{} << "Waiting for thread to stop";
	mpcobj<int>::lock lock{thread_stopped};

	while (*lock < 1)
		lock.wait();

	if (thread_started.get() != 1)
		throw EXCEPTION("test6: started " << thread_started.get());
}

void test7()
{
	debug{} << "test7";

	{
		destroy_callback::base::guard guard;

		auto s=ref<my_search_threadObj>::create();

		guard(s);

		s->search("EXCEPT");
		wait_for_search(1);

		s->search("two");
	}

	search_proceed(1);

	debug{} << "Waiting for thread to stop";
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
