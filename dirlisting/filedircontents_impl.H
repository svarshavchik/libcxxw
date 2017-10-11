/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef filedircontents_impl_h
#define filedircontents_impl_h

#include "dirlisting/filedircontents.H"
#include "dirlisting/filedir_file.H"
#include <x/threadmsgdispatcherobj.H>
#include <x/dir.H>
#include <x/logger.H>

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

class LIBCXX_HIDDEN filedircontentsObj::implObj
	: public threadmsgdispatcherObj {

	//! Logger
	LOG_CLASS_SCOPE;

	struct current_chunkObj;

	//! Internal chunk of stuff.
	typedef ref<current_chunkObj> current_chunk_t;

 public:
	//! Constructor
	implObj(const std::string &directory,
		const filedir_callback_t &callback);

	//! Destructor
	~implObj();

	//! Directory being monitored.
	const std::string directory;

	//! Callback where changes are being reported.
	const filedir_callback_t callback;

	//! Run the execution thread for monitoring the contents of the directory

	void run(ptr<obj> &mcguffin);

#include "dirlisting/filedircontents_impl.msgs.H"

 private:

	//! Initial contents of the directory being synced

	//! On the executing thread's stack.
	dir::base::iterator *initial_b;
	dir::base::iterator *initial_e;

};

LIBCXXW_NAMESPACE_END

#endif
