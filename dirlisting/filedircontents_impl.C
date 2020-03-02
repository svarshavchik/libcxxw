/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "dirlisting/filedircontents_impl.H"
#include "catch_exceptions.H"
#include <x/inotify.H>
#include <x/sysexception.H>
#include <x/property_value.H>
#include <x/weakcapture.H>

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::filedircontentsObj::implObj);

LIBCXXW_NAMESPACE_START

static property::value<size_t>
update_chunksize(LIBCXX_NAMESPACE_STR "::w::filedialog::chunksize",
		 50);

filedircontentsObj::implObj::implObj(const std::string &directory,
				     const filedir_callback_t &callback)
	: directory(directory), callback(callback)
{
}

filedircontentsObj::implObj::~implObj()=default;

// Inotify watcher lambda captures this object by value.

// When it's full, the main execution thread ships it on its merry way, and
// then creates a new object here.

struct filedircontentsObj::implObj::current_chunkObj : virtual public obj {

	filedir_file files;

	current_chunkObj(const filedir_file &files) : files(files) {}
};

void filedircontentsObj::implObj::run(ptr<obj> &mcguffin)
{
	msgqueue_auto q{this, eventfd::create()};

	auto fd=q->get_eventfd();

	fd->nonblock(true);

	auto i=inotify::create();

	i->nonblock(true);

	// Collect inotify events into this buffer

	auto c=current_chunk_t::create(filedir_file::create());

	auto w=i->create(directory,
			 inotify_create | inotify_delete | inotify_moved_from
			 | inotify_moved_to,
			 [c]
			 (uint32_t mask, uint32_t cookie, const char *name)
			 {
				 if (mask & (inotify_create|inotify_moved_to))
				 {
					 c->files->files.emplace_back(name,
								      false);
				 }

				 if (mask & (inotify_delete|inotify_moved_from))
				 {
					 c->files->files.emplace_back(name,
								      true);
				 }
			 });

	struct pollfd pfd[2];

	pfd[0].fd=fd->get_fd();
	pfd[0].events=POLLIN;
	pfd[1].fd=i->get_fd();
	pfd[1].events=POLLIN;


	try {
		initial_b=initial_e=nullptr;

		auto initial_dir_contents=dir::create(directory);

		auto b=initial_dir_contents->begin();
		auto e=initial_dir_contents->end();

		initial_b=&b;
		initial_e=&e;

		mcguffin=nullptr;

		dispatch_next_chunk();

		// Until the whole thing goes out the door, only monitor
		// the event file descriptor

		while (initial_b)
		{
			if (!q->empty())
			{
				q.event();
				continue;
			}

			if (poll(pfd, 1, -1) < 0)
				throw SYSEXCEPTION("poll() failed");
		}
	} catch (const stopexception &e)
	{
		return;
	} CATCH_EXCEPTIONS;

	// All that's left to do is monitor what inotify is doing, going
	// forward, until this thread gets stopped.

	while (1)
	{
		size_t n;

		do
		{
			n=c->files->files.size();
			i->read();
		} while (c->files->files.size() != n);

		if (n)
		{
			try {
				callback(c->files);
			} CATCH_EXCEPTIONS;

			c->files=filedir_file::create();
			c->files->files.reserve(n);
		}

		if (!q->empty())
		{
			q.event();
			continue;
		}

		if (poll(pfd, 2, -1) < 0)
			throw SYSEXCEPTION("poll() failed");

		if (pfd[0].revents & POLLIN)
			 // Clear the eventfd, q.event() will take of this.
			fd->event();
	}
}

void filedircontentsObj::implObj::dispatch_next_chunk()
{
	if (!initial_b) return; // Shouldn't get here.

	if (*initial_b == *initial_e)
	{
		initial_b=initial_e=nullptr;
		return; // Done.
	}

	auto n=update_chunksize.get();
	auto c=current_chunk_t::create(filedir_file::create());
	c->files->files.reserve(n);

	for (auto i=n*0; i<n; ++i)
	{
		if (*initial_b == *initial_e)
			break;
		c->files->files.emplace_back((*initial_b)->first, false);

		++*initial_b;
	}

	// Attach a destructor callback to this object to invoke next_chunk()
	// again, whenever the callback is finished doing what it needs to
	// do with the object.

	c->files->ondestroy([impl=make_weak_capture(ref(this))]
			    {
				    auto got=impl.get();

				    if (got)
				    {
					    auto &[impl]=*got;

					    impl->next_chunk();
				    }
			    });

	try {
		callback(c->files);
	} CATCH_EXCEPTIONS;
}

LIBCXXW_NAMESPACE_END
