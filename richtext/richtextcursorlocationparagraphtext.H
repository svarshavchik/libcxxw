#ifdef richtextcursorlocation_H_defined
#ifdef richtextparagraph_H_defined
#ifdef richtext_impl_H_defined

inline void richtextcursorlocationObj::removed_from_fragment(size_t pos,
							     size_t nchars)
{
	assert_or_throw(my_fragment && my_fragment->my_paragraph &&
			my_fragment->my_paragraph->my_richtext,
			"Internal error in removed_from_fragment(): "
			"my_fragment is not initialized");

	if (position.offset < pos)
		return;

	if (position.offset < pos+nchars)
	{
		position.offset=pos;

		if (my_fragment->my_paragraph->my_richtext->rl())
			--position.offset;
	}
	else
		position.offset -= nchars;

	horiz_pos_no_longer_valid();
}

#endif
#endif
#endif
