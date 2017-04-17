/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "xim/ximencoding.H"
#include <x/iconviofilter.H>
#include <x/namespace.h>

#include <cstring>
#include <set>
#include <string>
#include <sstream>
#include <courier-unicode.h>

#define f_left 1
#define f_right 2

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::ximencodingObj);

LIBCXXW_NAMESPACE_START

ximencodingObj::ximencodingObj(const char *charsetArg)
	: charset(charsetArg)
{
}

ximencodingObj::~ximencodingObj()=default;

//////////////////////////////////////////////////////////////////////////////
//
// COMPOUND_TEXT nonsense
//
// http://www.x.org/releases/X11R7.7/doc/xorg-docs/ctext/ctext.html
//
// Use iconv with iso-2022 to handle this
//
// http://en.wikipedia.org/wiki/ISO/IEC_2022

class LIBCXX_HIDDEN ximencoding_ctObj : public ximencodingObj {

 public:

	struct ct_charset_info {
		const char *charset;	// Character set for iconv
		int flags;		// left/right group (lower/upper range)
		const char *esc_seq;    // ESC seq in compound text
		const char *switch_to;	// Prepend to text sent to iconv.
	};

	class ct_cmp_by_esc {
	public:

		bool operator()(const ct_charset_info &a,
				const ct_charset_info &b)
		{
			return strcmp(a.esc_seq, b.esc_seq) < 0;
		}
	};

	std::set<ct_charset_info, ct_cmp_by_esc> ct_escape_codes;

	ximencoding_ctObj();

	~ximencoding_ctObj();

	std::u32string to_ustring(const std::string &compound_text) override;
};

ximencoding_ctObj::ximencoding_ctObj()
	: ximencodingObj("COMPOUND_TEXT"),
	  ct_escape_codes
	  ({
		  { "ISO8859-1", f_left,  "(B", 0},  //  US-ASCII
		  { "ISO8859-1", f_right, "-A", 0},  //  ISO-8859-1
		  { "ISO8859-2", f_right, "-B", 0},  //  ISO-8859-2
		  { "ISO8859-3", f_right, "-C", 0},  //  ISO-8859-3
		  { "ISO8859-4", f_right, "-D", 0},  //  ISO-8859-4
		  { "ISO8859-5", f_right, "-L", 0},  //  ISO-8859-5
		  { "ISO8859-6", f_right, "-G", 0},  //  ISO-8859-6
		  { "ISO8859-7", f_right, "-F", 0},  //  ISO-8859-7
		  { "ISO8859-8", f_right, "-H", 0},  //  ISO-8859-8
		  { "ISO8859-9", f_right, "-M", 0},  //  ISO-8859-9
		  { "ISO8859-10", f_right,"-V", 0},  //  ISO-8859-10
		  { "ISO8859-11", f_right,"-T", 0},  //  ISO-8859-11
		  { "ISO8859-13", f_right,"-Y", 0},  //  ISO-8859-13
		  { "ISO8859-14", f_right,"-_", 0},  //  ISO-8859-14
		  { "ISO8859-15", f_right,"-b", 0},  //  ISO-8859-15
		  { "ISO8859-16", f_right,"-f", 0},  //  ISO-8859-16
		  { "ISO2022-JP-2", f_left,
				  "(J", "\e(J"},  //  JISX0201
		  { "ISO2022-JP-2", f_right,
				  ")I", "\e(J"},  //  JISX0201

		  { "ISO-2022-JP-2", f_left,
				  "$(A", "\e$A"}, //  GB_2312-80
		  { "ISO-2022-JP-2", f_right,
				  "$)A", "\e$A"}, //  GB_2312-80

		  { "ISO-2022-JP-2", f_left,
				  "$(B", "\e$B"}, //  JIS_X0208-1983/1990
		  { "ISO-2022-JP-2", f_right,
				  "$)B", "\e$B"}, //  JIS_X0208-1983/1990
		  { "ISO-2022-JP-2", f_left,
				  "$(D", "\e$(D"},//  JIS_X0212-1990
		  { "ISO-2022-JP-2", f_right,
				  "$)D", "\e$(D"},//  JIS_X0212-1990
		  { "ISO-2022-KR", f_left,
				  "$(C", "\e$)C\x0e"},//  KS_C_5601-1987
		  { "ISO-2022-KR", f_right,
				  "$)C", "\e$)C"},//  KS_C_5601-1987

		  { "ISO-2022-CN-EXT", f_left,
				  "$(G", "\e$)G\x0e"},//  CNS 11643-1992 plane 1
		  { "ISO-2022-CN-EXT", f_right,
				  "$)G", "\e$)G"},    //  CNS 11643-1992 plane 1
		  { "ISO-2022-CN-EXT", f_left,
				  "$(H", "\e$*H"},    //  CNS 11643-1992 plane 2
		  { "ISO-2022-CN-EXT", f_right,
				  "$)H", "\e$*H"},    //  CNS 11643-1992 plane 2
		  { "ISO-2022-CN-EXT", f_left,
				  "$(I", "\e$+I\eo"}, //  CNS 11643-1992 plane 3
		  { "ISO-2022-CN-EXT", f_right,
				  "$)I", "\e$+I\e|"}, //  CNS 11643-1992 plane 3
		  { "ISO-2022-CN-EXT", f_left,
				  "$(J", "\e$+J\eo"}, //  CNS 11643-1992 plane 4
		  { "ISO-2022-CN-EXT", f_right,
				  "$)J", "\e$+J\e|"}, //  CNS 11643-1992 plane 4
		  { "ISO-2022-CN-EXT", f_left,
				  "$(K",  "\e$+K\eo"},//  CNS 11643-1992 plane 5
		  { "ISO-2022-CN-EXT", f_right,
				  "$)K", "\e$+K\e|"}, //  CNS 11643-1992 plane 5
		  { "ISO-2022-CN-EXT", f_left,
				  "$(L",  "\e$+L\eo"},//  CNS 11643-1992 plane 6
		  { "ISO-2022-CN-EXT", f_right,
				  "$)L", "\e$+L\e|"}, //  CNS 11643-1992 plane 6
		  { "ISO-2022-CN-EXT", f_left,
				  "$(M",  "\e$+M\eo"},//  CNS 11643-1992 plane 7
		  { "ISO-2022-CN-EXT", f_right,
				  "$)M", "\e$+M\e|"}, //  CNS 11643-1992 plane 7

		  { "UTF-8",     f_left|f_right,   "%G" , 0},       //  UTF-8

		  { "BIG5HKSCS", f_left | f_right, "%/2", 0},
		  { "GBK",       f_left | f_right, "%/2", 0}
	  })
{
}

ximencoding_ctObj::~ximencoding_ctObj()=default;

// Convert "compound text" to UTF-8.

std::u32string ximencoding_ctObj::to_ustring(const std::string &compound_text)
{
	std::u32string str;
	std::string left_charset="ISO-8859-1";
	std::string right_charset=left_charset;
	const char  *left_switch_to=0;
	const char  *right_switch_to=0;

	LOG_DEBUG("Converting compound text:" << ({
				std::ostringstream o;

				for (unsigned char c:compound_text)
				{
					o << ' ' << std::setw(2) << std::hex
					  << std::setfill('0')
					  << (int) c;
				}
				o.str();
			}));

	auto b=compound_text.begin();
	auto e=compound_text.end();

	while (b != e)
	{
		auto p=std::find(b, e, '\e');

		// Convert text until the next escape sequence.

		{
			while (p != b)
			{
				auto c= *b;

				auto switch_to=(c & 0x80)
					? right_switch_to:left_switch_to;

				std::ostringstream o;

				if (switch_to)
					o << switch_to;

				while (p != b && ((*b ^ c) & 0x80) == 0)
				{
					o << *b;
					++b;
				}

				str += unicode::iconvert::tou
					::convert(o.str(),
						  (c & 0x80)
						  ? right_charset:left_charset)
					.first;
			}
		}

		if (p == e)
			continue;

		// Look up the escape sequence.

		for (b=++p; b != e && ((*b & 0xF0) == 0x20); ++b)
			;

		if (b == e)
			continue;

		{
			std::string esc_seq(p, ++b);
			ct_charset_info dummy;

			dummy.esc_seq=esc_seq.c_str();

			auto p=ct_escape_codes.find(dummy);

			if (p == ct_escape_codes.end())
			{
				left_charset="ISO-8859-1";
				right_charset=left_charset;
			}
			else
			{
				if (p->flags & f_left)
				{
					left_charset=p->charset;
					left_switch_to=p->switch_to;
				}
				if (p->flags & f_right)
				{
					right_charset=p->charset;
					right_switch_to=p->switch_to;
				}
			}
		}
	}

	return str;
}

//////////////////////////////////////////////////////////////////////////////
//
// Everything else uses iconv.

class LIBCXX_HIDDEN ximencoding_iconvObj : public ximencodingObj {

 public:

	ximencoding_iconvObj(const char *charset) : ximencodingObj(charset)
	{
	}

	~ximencoding_iconvObj()=default;

	std::u32string to_ustring(const std::string &text) override
	{
		return unicode::iconvert::tou::convert(text, charset).first;
	}
};

///////////////////////////////////////////////////////////////////////////////

ximencoding ximencodingBase::create(const char *charset)
{
	if (strcmp(charset, "COMPOUND_TEXT") == 0)
		return ref<ximencoding_ctObj>::create();

	return ref<ximencoding_iconvObj>::create(charset);
}

LIBCXXW_NAMESPACE_END
