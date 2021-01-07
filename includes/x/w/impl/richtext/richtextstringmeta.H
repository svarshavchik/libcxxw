/*
** Copyright 2020-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifdef x_w_impl_richtext_richtextmeta_H_defined
#ifdef x_w_impl_richtext_richtextstring_H_defined

template<typename string_type>
richtextstring::richtextstring(string_type &&str,
			       const std::unordered_map<size_t,
			       richtextmeta> &meta)
	: string{std::forward<string_type>(str)},
	  meta{meta.begin(), meta.end()}
{
	finish_from_string();
}

#endif
#endif
