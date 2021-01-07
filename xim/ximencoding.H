/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef ximencoding_H
#define ximencoding_H

#include <x/ref.H>
#include <x/obj.H>
#include <x/logger.H>
#include <string>
#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

//! X Input Method character set decoder.

class LIBCXX_HIDDEN ximencodingObj : virtual public obj {

 protected:
	LOG_CLASS_SCOPE;
 public:

	const char * const charset;
	ximencodingObj(const char *charsetArg);
	~ximencodingObj();

	virtual std::u32string to_ustring(const std::string &text)=0;
};

class LIBCXX_HIDDEN ximencodingBase;

//! Decode X Input Method text decoder

//! Converts text received from an X Input Method server into unicode.
//! After negotiating a character set with the X Input Method server, this
//! object handles decoding of text from the input method.

typedef ref<ximencodingObj, ximencodingBase> ximencoding;

//! Base class for X Input Method text decoders

//! Refer to this class as INSERT_LIBX_NAMESPACE::w::ximencoding::base

class LIBCXX_HIDDEN ximencodingBase {

 public:
	//! Creator
	static ximencoding create(const char *charset);

	//! Object factory

	template<typename ref_type>
	class objfactory {
	public:

		//! Creator
		static inline ref_type create(const char *charset)
		{
			return ximencodingBase::create(charset);
		}
	};
};

LIBCXXW_NAMESPACE_END

#endif
