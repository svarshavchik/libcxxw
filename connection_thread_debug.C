/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "connection_thread_debug.H"
#include <unordered_set>
#include <iostream>
#include <string_view>
#include <cstring>
#include <charconv>
#include <unistd.h>
#include <x/mpobj.H>
#include <x/singleton.H>
#include <sys/time.h>

LIBCXXW_NAMESPACE_START

#define TRACK_OBJECTS 1
#define TRACK_TOTALS 1

#if CONNECTION_THREAD_DEBUG

static size_t counter;
const char *action;

#if TRACK_OBJECTS

typedef mpobj<std::unordered_set<void *>> action_for_t;

struct connection_debug_pointersObj : virtual public x::obj {

	action_for_t action_for;
};

static x::singleton<connection_debug_pointersObj> connection_debug_data;

#endif

#if TRACK_TOTALS
static uint64_t total_read, total_written;
#endif

static struct timeval timestamp;
static size_t wait_counter;

void connection_thread_action(const char *p)
{
#define APPEND_S(s) do {						\
		const char *p=(s);					\
		ssize_t n=strlen(p);					\
									\
		if ((e-b) >= n)						\
			b=std::copy(p, p+n, b);				\
	} while (0)

#define APPEND_N(n) do {					\
		auto ret=std::to_chars(b, e, (n));		\
								\
		if (ret.ec == std::errc{})			\
			b=ret.ptr;				\
	} while(0)

	if (p != action)
	{
#if TRACK_OBJECTS
		auto connection_debug=connection_debug_data.get();

		auto &action_for=connection_debug->action_for;
#endif
		if (action)
		{
			struct timeval start_timestamp=timestamp;

			gettimeofday(&timestamp, nullptr);

			char buffer[256];

			auto b=buffer;
			auto e=buffer+sizeof(buffer);

			APPEND_S(action);
			APPEND_S(": ");
			APPEND_N(counter);
			APPEND_S(" times");

#if TRACK_OBJECTS
			size_t n=action_for_t::lock{action_for}->size();

			if (n)
			{
				APPEND_S(" (");
				APPEND_N(n);
				APPEND_S(" objects)");
			}
#endif

			start_timestamp.tv_sec=
				timestamp.tv_sec-start_timestamp.tv_sec;

			auto end_usec=timestamp.tv_usec;

			if (end_usec < start_timestamp.tv_usec)
			{
				--start_timestamp.tv_sec;
				end_usec += 1000000;
			}

			start_timestamp.tv_usec=
				end_usec-start_timestamp.tv_usec;

			APPEND_S(" (");
			APPEND_N(start_timestamp.tv_sec);

			APPEND_S(".");

			{
				char buffer[20];

				char *pp=buffer+10;

				{
					char *b=pp;
					char *e=buffer+19;

					APPEND_N(start_timestamp.tv_usec);
					*b=0;

					while (b-pp < 6)
					{
						*--pp='0';
					}
				}

				APPEND_S(pp);
			}

			APPEND_S(" sec)\n");

			if (write(1, buffer, b-buffer) < 0)
				;

		}
		else
		{
			gettimeofday(&timestamp, nullptr);
		}
		action=p;
		counter=0;

		if (p)
		{
			char buffer[256];

			auto b=buffer;
			auto e=buffer+sizeof(buffer);

			APPEND_S(action);
			APPEND_S(": started\n");

			if (write(1, buffer, b-buffer) < 0)
				;
		}
#if TRACK_OBJECTS
		{
			action_for_t::lock{action_for}->clear();
		}
#endif
	}
	++counter;
}

void connection_thread_action_for(const char *p, void *ptr)
{
	connection_thread_action(p);
#if TRACK_OBJECTS
	auto connection_debug=connection_debug_data.get();

	auto &action_for=connection_debug->action_for;

	action_for_t::lock{action_for}->insert(ptr);
#endif
}

void connection_thread_poll_start(xcb_connection_t *c)
{
	connection_thread_action(0);

	char buffer[256];

	auto b=buffer;
	auto e=buffer+sizeof(buffer);

	APPEND_S("--- wait: ");
	++wait_counter;
	APPEND_N(wait_counter);

#if TRACK_TOTALS
	APPEND_S(" (");

	auto r=xcb_total_read(c);
	auto w=xcb_total_written(c);

	APPEND_N(r-total_read);
	APPEND_S(" read, ");
	APPEND_N(w-total_written);
	APPEND_S(" written)");

	total_read=r;
	total_written=w;
#endif

	APPEND_S("\n");
	if (write(1, buffer, b-buffer) < 0)
		;


	connection_thread_action("poll");
}

void connection_thread_poll_end()
{
	connection_thread_action(0);
	if (write(1, "---\n", 4) < 0)
		;
}
#endif

LIBCXXW_NAMESPACE_END