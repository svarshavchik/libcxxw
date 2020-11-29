/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "richtext/richtext_insert.H"
#include "richtext/richtext_linebreak_info.H"

LIBCXXW_NAMESPACE_START

// Construct a richtextstring for the new paragraph fragment.
//
// Moves \n to the beginning of the string in right-to-left paragraphs

static inline auto create_new_fragment_text(richtextstring &string,
					    size_t i,
					    size_t j,
					    unicode_bidi_level_t
					    embedding_level)
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
		return richtextstring{string, i, j-i};

	richtextstring rl_str{string, j-1, 1}; // The newline

	if (j-i > 1)
	{
		rl_str += richtextstring{string, i, j-i-1};
	}

	return rl_str;
}

#if 0
// Take the new fragment text, created in richtextstring, and create a
// new fragment. We made a single pass to calculate all the linebreaks,
// so we pull out the linebreaks for that. Checks swap_newline, and makes
// sure we move the paragraph break too, in that case.

static inline auto
create_new_fragment_from_string(richtextstring &&string,
				bool &swap_newline,
				std::vector<unicode_lb> &breaks,
				size_t i,
				size_t j,
				unicode_bidi_level_t embedding_level)
{
	if (!swap_newline)
		return richtextfragmentptr::create(std::move(string),
						   std::vector<unicode_lb>{
							   breaks.begin()+i,
							   breaks.begin()+j
						   });

	std::vector<unicode_lb> rl_breaks;

	rl_breaks.reserve(j-i);
	rl_breaks.push_back(unicode_lb::none); // The newline

	rl_breaks.insert(rl_breaks.end(), breaks.begin()+i,
			 breaks.begin()+(j-1));

	// Figure out the linebreaking situation after the newline. If there's
	// right-to-left text there, it's none, if there's left-to-right text
	// there, it is allowed.

	if (j-i > 1)
	{
		rl_breaks.at(1)=
			string.meta_at(1).rl ? unicode_lb::none
			: unicode_lb::allowed;
	}
	return richtextfragmentptr::create(std::move(string),
					   std::move(rl_breaks));
}

static inline auto create_new_fragment(richtextstring &string,
				       std::vector<unicode_lb> &breaks,
				       size_t i,
				       size_t j,
				       unicode_bidi_level_t embedding_level)
{
	bool swap_newline;

	return create_new_fragment_from_string
		( create_new_fragment_text(string, i, j, embedding_level,
					   swap_newline),
		  swap_newline,
		  breaks,
		  i, j, embedding_level);
}
#endif

create_fragments_from_inserted_text
::create_fragments_from_inserted_text(richtextstring &string,
				      unicode_bidi_level_t embedding_level)
	: string{string}, embedding_level{embedding_level},
	  current_pos{0},
	  n{string.size()}
{
}

create_fragments_from_inserted_text::~create_fragments_from_inserted_text()
=default;

bool create_fragments_from_inserted_text::has_paragraph_break() const
{
	if (current_pos >= n)
		return false;

	size_t s=next_paragraph_start();

	return s > current_pos && string.get_string().at(s-1) == '\n';
}

size_t create_fragments_from_inserted_text::next_paragraph_start() const
{
	auto &s=string.get_string();

	auto e=&s[0]+n;

	auto p=std::find(&s[0]+current_pos, e, '\n');

	if (p != e)
		++p;

	return p-&s[0];
}

richtext_dir create_fragments_from_inserted_text::next_string_dir() const
{
	if (current_pos >= n)
	{
		return embedding_level == UNICODE_BIDI_LR
			? richtext_dir::lr : richtext_dir::rl;
	}

	auto &s=string.get_string();

	auto e=&s[0]+n;

	auto p=std::find(&s[0]+current_pos, e, '\n');

	return string.get_dir(current_pos, p-(&s[0]));
}

richtextstring create_fragments_from_inserted_text::next_string()
{
	// Each mandatory line break starts a new paragraph.
	// Put each paragraph into a single fragment.

	if (current_pos<n)
	{
		// Find the next mandatory line break.
		size_t j=next_paragraph_start();

		auto orig_pos=current_pos;

		current_pos=j;

		return create_new_fragment_text(string,
						orig_pos, j,
						embedding_level);
	}

	return {};
}


void richtext_insert_results::split(const richtextfragment &split_from,
				    size_t pos,
				    size_t count,
				    const richtextfragment &split_to)
{
}

void richtext_insert_results::merged(const richtextfragment &merged_from,
				     const richtextfragment &merged_to,
				     size_t merged_to_pos)
{
}

LIBCXXW_NAMESPACE_END
