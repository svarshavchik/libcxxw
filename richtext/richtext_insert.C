/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "richtext/richtext_insert.H"
#include "richtext/richtext_linebreak_info.H"

LIBCXXW_NAMESPACE_START

static inline auto create_new_fragment(richtextstring &string,
				       std::vector<unicode_lb> &breaks,
				       size_t i,
				       size_t j,
				       unicode_bidi_level_t embedding_level)
{
	// Normally the \n ends the paragraph. If the paragraph
	// embedding level is right-to-left, \n belongs at the
	// beginning of the paragraph.

	bool swap_newline=false;

	if (embedding_level != UNICODE_BIDI_LR)
	{
		if (string.get_string().at(j-1) == '\n')
			swap_newline=true;
	}

	if (!swap_newline)
		return richtextfragmentptr::create(richtextstring{string,
								  i,
								  j-i},
			std::vector<unicode_lb>{
				breaks.begin()+i,
					breaks.begin()+j
					});

	richtextstring rl_str{string, j-1, 1}; // The newline

	std::vector<unicode_lb> rl_breaks;

	rl_breaks.reserve(j-i);
	rl_breaks.push_back(unicode_lb::none); // The newline

	if (j-i > 1)
	{
		rl_str += richtextstring{string, i, j-i-1};
	}

	rl_breaks.insert(rl_breaks.end(), breaks.begin()+i,
			 breaks.begin()+(j-1));

	// Figure out the linebreaking situation after the newline. If there's
	// right-to-left text there, it's none, if there's left-to-right text
	// there, it is allowed.

	if (j-i > 1)
	{
		rl_breaks.at(1)=
			rl_str.meta_at(1).rl ? unicode_lb::none
			: unicode_lb::allowed;
	}
	return richtextfragmentptr::create(std::move(rl_str),
					   std::move(rl_breaks));
}

create_fragments_from_inserted_text
::create_fragments_from_inserted_text(richtextstring &string,
				      unicode_bidi_level_t embedding_level)
	: string{string}, embedding_level{embedding_level},
	  current_pos{0},
	  n{string.size()}
{
	// Calculate mandatory line breaks.

	auto &s=string.get_string();

	breaks.resize(n, unicode_lb::none);

	if (!s.empty())
	{
		richtextstring *ptr= &string;

		richtext_linebreak_info(0, s.size(), &breaks[0],
					&ptr, 1);
	}

	// The first character, by definition, does not break. This
	// logic is also present in recalculate_linebreaks().
	if (!breaks.empty())
		breaks[0]=unicode_lb::none;

	// richtext_linebreak_info will miss the mark when there are
	// embedded newlines in rtol text. We'll fix this up here.

	auto b=s.begin();
	auto e=s.end();

	auto p=b;

	while (p != e)
	{
		auto q=std::find(p, e, '\n');

		if (q == e)
			break;

		breaks[q-b]=unicode_lb::none;
		if (++q != e)
			breaks[q-b]=unicode_lb::mandatory;
		p=q;
	}
}

create_fragments_from_inserted_text::~create_fragments_from_inserted_text()
=default;

richtextfragmentptr create_fragments_from_inserted_text::operator()()
{
	// Each mandatory line break starts a new paragraph.
	// Put each paragraph into a single fragment.

	if (current_pos<n)
	{
		// Find the next mandatory line break.
		size_t j=std::find(&breaks[0]+(current_pos+1), &breaks[0]+n,
				   unicode_lb::mandatory)-&breaks[0];

		auto orig_pos=current_pos;

		current_pos=j;

		return create_new_fragment(string,
					   breaks,
					   orig_pos, j,
					   embedding_level);
	}

	return {};
}

LIBCXXW_NAMESPACE_END
