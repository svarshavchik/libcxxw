#include "x/w/types.H"

#define x_w_impl_richtext_richtextmeta_H
#define x_w_impl_fonts_freetypefontfwd_H
#define x_w_impl_fonts_freetypefont_H

LIBCXXW_NAMESPACE_START

class richtextmeta {

public:

	char c;
	bool rl=false;

	static constexpr dim_t ascender=8;
	static constexpr dim_t descender=4;

	bool operator==(const richtextmeta &o) const
	{
		return c == o.c && rl == o.rl;
	}

	bool operator!=(const richtextmeta &o) const { return !operator==(o); }

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
				      char32_t unprintable_char) const
	{
		while (b != e)
		{
		 lambda(16, 16, prev_char == ' ' ? -10:
			-(int)(prev_char & 15), 0);

		 prev_char = *b;

		 ++b;
		}
	}
};

typedef richtextmeta freetypefont;

LIBCXXW_NAMESPACE_END

#include "richtext/richtextstring.C"
#include "richtext/richtextstring2.C"
#include "richtext/richtextfragment.C"
#include "richtext/richtextparagraph.C"
#include "richtext/richtext_impl.C"
#include "richtext/paragraph_list.C"
#include "richtext/fragment_list.C"
#include "richtext/richtext_linebreak_info.C"
#include "richtext/richtexthorizinfo.C"
#include "richtext/richtextcursorlocation.C"
#include "metrics_axis.C"
#include "assert_or_throw.C"

LIBCXXW_NAMESPACE_START

const richtextstring::resolved_fonts_t &richtextstring::resolve_fonts() const
{
	resolved_fonts=get_meta();

	return resolved_fonts;
}

LIBCXXW_NAMESPACE_END
