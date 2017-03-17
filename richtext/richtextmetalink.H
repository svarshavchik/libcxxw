/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef richtextmetalink_H
#define richtextmetalink_H

#include "richtext/richtextmetalinkfwd.H"
#include "richtext/richtextmetalinkobj.H"
#include "richtext/richtextmeta.H"

namespace LIBCXX_NAMESPACE {

	namespace w {
#if 0
	};
};
#endif

//! Base class for a \ref richtextmetalink "metadata link".

class richtextmetalinkBase {

public:

	//! Typedef alias
	typedef richtextmetalinkObj::event_t event_t;

	//! Implement a simple text link callback, that invokes a functor

	//! richtextmetalink's create() returns this.
	//!
	//! \internal
	template<typename override_text_markup_func_t,
		 typename event_func_t>
	class LIBCXX_HIDDEN default_link_implObj : public richtextmetalinkObj {

	public:
		//! The override_text_marup functor.
		override_text_markup_func_t override_text_markup_func;

		//! The event functor.
		event_func_t event_func;

		//! Constructor

		template<typename f1_t, typename f2_t>
			default_link_implObj(f1_t &&f1, f2_t &&f2)
			: override_text_markup_func(std::forward<f1_t>(f1)),
			event_func(std::forward<f2_t>(f2))
		{
		}

		//! Destructor
		~default_link_implObj()=default;

		//! Implement override_text_markup()

		void override_text_markup(richtextmeta &m) const override
		{
			override_text_markup_func(m);
		}

		//! Invoke the functor
		void event(enum event_t e) const override
		{
			event_func(e);
		}
	};

	//! Default create() implementation.

	//! \internal
	template<typename ptrrefType> class objfactory {
	public:

		//! Default create() implementation.

		template<typename f1_t, typename f2_t>
		static inline ptrrefType
		create(f1_t &&f1, f2_t &&f2)
		{
			return ref<default_link_implObj
				   <typename std::decay<f1_t>::type,
				    typename std::decay<f2_t>::type>>
				::create(std::forward<f1_t>(f1),
					 std::forward<f2_t>(f2));
		}
	};
};

LIBCXXW_NAMESPACE_END

#endif
