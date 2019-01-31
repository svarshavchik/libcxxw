/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef catch_exceptions_H
#define catch_exceptions_H

#include <x/exception.H>
#include <x/logger.H>
#include "x/w/text_param.H"

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

#define REPORT_EXCEPTIONS(window) catch (const exception &caught_e)	\
	{								\
		(window)->exception_message(caught_e);			\
	}								\
	catch (const std::exception &caught_e)				\
	{								\
		(window)->exception_message(EXCEPTION(caught_e.what())); \
	}								\
	catch(...)							\
	{								\
		(window)->stop_message("Unknown exception thrown.");	\
	}

#define REPORT_EXCEPTIONS_WITH_CONFIG(window,config)			\
	catch (const exception &caught_e)				\
	{								\
		(window)->exception_message(caught_e,(config));		\
	}								\
	catch (const std::exception &caught_e)				\
	{								\
		(window)->exception_message(EXCEPTION(caught_e.what()),	\
					    (config));			\
	}								\
	catch(...)							\
	{								\
		(window)->stop_message("Unknown exception thrown.", (config)); \
	}
#endif
