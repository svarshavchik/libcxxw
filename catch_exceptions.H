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

#define REPORT_EXCEPTIONS(window) catch (const exception &e)		\
	{								\
		(window)->exception_message(e);				\
	}								\
	catch (const std::exception &e)					\
	{								\
		(window)->exception_message(EXCEPTION(e.what()));	\
	}								\
	catch(...)							\
	{								\
		(window)->error_message("Unknown exception thrown.");	\
	}

#define REPORT_EXCEPTIONS_WITH_CONFIG(window,config)			\
	catch (const exception &e)					\
	{								\
		(window)->exception_message(e,config);			\
	}								\
	catch (const std::exception &e)					\
	{								\
		(window)->exception_message(EXCEPTION(e.what()), config); \
	}								\
	catch(...)							\
	{								\
		(window)->error_message("Unknown exception thrown.", config); \
	}
#endif
