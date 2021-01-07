#include "x/w/types.H"

#define x_w_impl_richtext_richtextmeta_H
#define x_w_impl_richtext_richtextmeta_H_defined
#define x_w_impl_fonts_freetypefontfwd_H
#define x_w_impl_fonts_freetypefont_H
#define x_w_text_hotspot_H
#define x_w_text_hotspotfwd_H
#define connection_thread_h

LIBCXXW_NAMESPACE_START

typedef int text_hotspot;
typedef int text_hotspotptr;

class richtextmeta {

public:

	char c;
	bool rl=false;

	text_hotspotptr link=0;

	static constexpr dim_t ascender=8;
	static constexpr dim_t descender=4;

	bool operator==(const richtextmeta &o) const
	{
		return c == o.c && rl == o.rl && link == o.link;
	}

	richtextmeta *operator->()
	{
		return this;
	}

	const richtextmeta *operator->() const
	{
		return this;
	}

	template<typename iter_type>
	void load_glyphs(iter_type b, iter_type e,
			 char32_t unprintable_char) const
	{
	}

	template<typename iter_type, typename lambda_type>
	void glyphs_size_and_kernings(iter_type b,
				      iter_type e,
				      lambda_type &&lambda,
				      char32_t prev_char,
				      char32_t unprintable_char) const;
};

typedef richtextmeta freetypefont;

LIBCXXW_NAMESPACE_END

bool skip_hotspot_sanity_check=false;

#define TO_CANONICAL_ORDER_HOOK() if (skip_hotspot_sanity_check) return

#include "richtext/richtextstring.C"
#include "richtext/richtextstring2.C"
#include "richtext/richtextiterator.C"
#include "richtext/richtextfragment.C"
#include "richtext/richtextparagraph.C"
#include "richtext/richtext.C"
#include "richtext/richtext_impl.C"
#include "richtext/paragraph_list.C"
#include "richtext/fragment_list.C"
#include "richtext/richtext_linebreak_info.C"
#include "richtext/richtexthorizinfo.C"
#include "richtext/richtextcursorlocation.C"
#include "richtext/richtextcursorlocationowner.C"
#include "richtext/richtext_range.C"

static LIBCXX_NAMESPACE::w::richtextstring inserted_canonical_string;

#define CFFIT_CONSTRUCTOR_DEBUG inserted_canonical_string={}
#define CFFIT_NEXT_DEBUG inserted_canonical_string += current_string

#include "richtext/richtext_insert.C"
#include "metrics_axis.C"
#include "assert_or_throw.C"

LIBCXXW_NAMESPACE_START

template<typename iter_type, typename lambda_type>
void richtextmeta::glyphs_size_and_kernings(iter_type b,
					    iter_type e,
					    lambda_type &&lambda,
					    char32_t prev_char,
					    char32_t unprintable_char) const
{
	while (b != e)
	{
		if (*b == richtextstring::hotspot_marker)
		{
			lambda(0, 0, 0, 0);
			++b;
			continue;
		}
		lambda(16, 16, prev_char == ' ' ? -10:
		       -(int)(prev_char & 15), 0);

		prev_char = *b;

		++b;
	}
}

const richtextstring::resolved_fonts_t &richtextstring::resolve_fonts() const
{
	resolved_fonts=get_meta();

	return resolved_fonts;
}

struct connection_threadObj : virtual public obj {};
LIBCXXW_NAMESPACE_END
