/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef catch_exceptions_H
#define catch_exceptions_H

#include <x/exception.H>
#include <x/logger.H>

#define CATCH_EXCEPTIONS catch (const exception &e)		\
	{							\
		e->caught();					\
	}							\
	catch (const std::exception &e)				\
	{							\
		EXCEPTION(e.what())->caught();			\
	}							\
	catch(...)						\
	{							\
		LOG_ERROR("Unknown exception thrown");		\
	}
#endif
