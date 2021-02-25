#include "libcxxw_config.h"
#include "x/w/impl/richtext/richtext.H"
#include "richtext/richtextparagraph.H"
#include "richtext/fragment_list.H"
#include "richtext/paragraph_list.H"
#include "x/w/richtext/richtextiterator.H"
#include "richtext/richtext_insert.H"
#include "x/w/impl/background_color.H"
#include "screen.H"
#include "main_window.H"
#include "main_window_handler.H"
#include "assert_or_throw.H"
#include <x/options.H>
#include <iostream>
#include <algorithm>

using namespace LIBCXX_NAMESPACE::w;
using namespace unicode::literals;

void testrichtext(const current_fontcollection &font1,
		  const current_fontcollection &font2,
		  const main_window &w)
{
	auto IN_THREAD=w->get_screen()->impl->thread;

	auto black=create_new_background_color(w->get_screen(),
					       w->elementObj::impl
					       ->get_window_handler()
					       .drawable_pictformat,
					       "0%");

	richtextstring ustring{
		U"Hello world\nHello\nworld\n",
		{
			{0, {black, font1}},
			{6, {black, font2}},
			{8, {black, font2}},
			{18, {black, font1}},
			{20, {black, font2}}
		}};

	auto richtext=richtext::create(std::move(ustring), richtext_options{});
	auto impl=richtext->debug_get_impl(IN_THREAD);

	if (impl->paragraphs.size() != 3)
		throw EXCEPTION("Did not get 3 paragraphs");

	if (impl->num_chars != ustring.size())
		throw EXCEPTION("Rich text size wrong");

	std::cout << "Width: " << impl->real_width
		  << ", Height: " << impl->above_baseline << "+"
		  << impl->below_baseline << std::endl;

	for (size_t i=0, s=impl->paragraphs.size(); i<s; ++i)
	{
		auto paragraph=*impl->paragraphs.get_paragraph(i);

		std::cout << "Paragraph: Width: " << paragraph->width
			  << ", Height: " << paragraph->above_baseline << "+"
			  << paragraph->below_baseline << std::endl;
	}

	for (size_t i=0, s=impl->paragraphs.size(); i<s; ++i)
	{
		auto paragraph=*impl->paragraphs.get_paragraph(i);

		if (paragraph->fragments.size() != 1)
			throw EXCEPTION("Somehow we ended up with multiple my_fragments");
	}

	impl->rewrap(1);

	std::vector<size_t> n_fragments;

	for (size_t i=0, s=impl->paragraphs.size(); i<s; ++i)
	{
		auto paragraph=*impl->paragraphs.get_paragraph(i);

		n_fragments.push_back(paragraph->fragments.size());
	}

	impl->unwrap();

	for (size_t i=0, s=impl->paragraphs.size(); i<s; ++i)
	{
		auto paragraph=*impl->paragraphs.get_paragraph(i);

		n_fragments.push_back(paragraph->fragments.size());
	}

	std::vector<size_t> expected={2, 1, 1, 1, 1, 1};

	if (n_fragments != expected)
		throw EXCEPTION("Unexpected wrap/unwrap result");
}

void testsplit(const current_fontcollection &font1,
	       const current_fontcollection &font2,
	       const main_window &w)
{
	auto IN_THREAD=w->get_screen()->impl->thread;

	auto black=create_new_background_color(w->get_screen(),
					       w->elementObj::impl
					       ->get_window_handler()
					       .drawable_pictformat, "0%");

	richtextstring ustring{
		U"A B C D E",
		{
			{0, {black, font1}},
			{4, {black, font2}},
			{6, {black, font1}}
		}};

	static struct {
		size_t split_pos;
		const char *results;
	} tests[]={
		{
			8,
			"0=1 4=2 6=1"
			"|"
			"0=1",
		},
		{
			6,
			"0=1 4=2"
			"|"
			"0=1",
		},
		{
			4,
			"0=1"
			"|"
			"0=2 2=1",
		},
		{
			2,
			"0=1"
			"|"
			"0=1 2=2 4=1",
		},
	};

	for (const auto &test:tests)
	{
		auto richtext=richtext::create(std::move(ustring),
					       richtext_options{});
		auto impl=richtext->debug_get_impl(IN_THREAD);

		assert_or_throw(impl->num_chars == ustring.size(),
				"num_chars in rich text is not right after set()");

		assert_or_throw(impl->paragraphs.size() == 1,
				"rich text was not 1 paragraph");
		assert_or_throw((*impl->paragraphs.get_paragraph(0))
				->num_chars == ustring.size(),
				"num_chars in paragraph is not right after set()");

		const auto &paragraph=*impl->paragraphs.get_paragraph(0);

		if (paragraph->fragments.size() != 1)
			throw EXCEPTION("Did not get 1 my_fragments");

		LIBCXX_NAMESPACE::w::paragraph_list my_paragraphs{*impl};
		LIBCXX_NAMESPACE::w::fragment_list
			my_fragments{my_paragraphs,
				     *paragraph};

		auto fragment=*paragraph->fragments.get_iter(0);

		auto control_text=fragment->string;
		auto control_breaks=fragment->breaks;

		auto control_horiz=fragment->horiz_info;

		richtext_insert_results results;

		fragment->split(my_fragments, test.split_pos,
				fragment->split_lr, false, results);

		auto new_fragment=x::ref{fragment->next_fragment()};

		fragment=*paragraph->fragments.get_iter(0);

		assert_or_throw(impl->num_chars == ustring.size(),
				"num_chars in rich text should not have changed");
		assert_or_throw(impl->paragraphs.size() == 1,
				"rich text is not still 1 paragraph");
		assert_or_throw((*impl->paragraphs.get_paragraph(0))
				->num_chars == ustring.size(),
				"num_chars in paragraph is not right after split()");

		std::ostringstream o;

		const char *p="";
		for (const auto &m:fragment->string.get_meta())
		{
			o << p << m.first
			  << "=" << (m.second.getfont() == font1 ? "1":"2");
			p=" ";
		}
		p="|";
		for (const auto &m:new_fragment->string.get_meta())
		{
			o << p << m.first
			  << "=" << (m.second.getfont() == font1 ? "1":"2");
			p=" ";
		}
		std::string s=o.str();

		if (s != test.results)
		{
			throw EXCEPTION(std::string("Expected: ")
					+ test.results + ", got: " + s);
		}

		richtext_insert_results ignored;
		fragment->merge(my_fragments, fragment->merge_bidi,
				ignored);

		if (control_text.get_string() != fragment->string.get_string()
		    ||
		    control_text.get_meta() != fragment->string.get_meta())
			throw EXCEPTION(std::string("Text was different after split/merge: ") + test.results);

		if (control_breaks != fragment->breaks)
			throw EXCEPTION(std::string("Breaks was different after split/merge: ") + test.results);

		if (control_horiz != fragment->horiz_info)
			throw EXCEPTION(std::string("Widths was different after split/merge: ") + test.results);
	}
}

void testresolvedfonts(const current_fontcollection &font1,
		       const current_fontcollection &font2,
		       const main_window &w)
{
	auto IN_THREAD=w->get_screen()->impl->thread;
	auto black=create_new_background_color(w->get_screen(),
					       w->elementObj::impl
					       ->get_window_handler()
					       .drawable_pictformat, "0%");

	richtextstring ustring{
		U"0123456789",
		{
			{0, {black, font1}},
			{5, {black, font2}},
		}};

	const auto &resolved_fonts=ustring.resolve_fonts();

	if (ustring.need_font_resolution())
		throw EXCEPTION("resolve_fonts() didn't cache");

	if (resolved_fonts.size() != 2)
		throw EXCEPTION("Unexpected return value from resolve_fonts");

	auto ffont1=resolved_fonts.at(0).second;
	auto ffont2=resolved_fonts.at(1).second;

	static const struct {
		size_t pos;
		size_t len;

		std::vector<std::pair<size_t, freetypefont>> results;
	} substr_tests[]={
		{0, 4, {
				{0, ffont1},
			}},
		{0, 5, {
				{0, ffont1},
			}},
		{0, 6, {
				{0, ffont1},
				{5, ffont2},
			}},
		{0, 7, {
				{0, ffont1},
				{5, ffont2},
			}},
		{4, 1, {
				{0, ffont1},
			}},
		{4, 2, {
				{0, ffont1},
				{1, ffont2},
			}},
		{4, 3, {
				{0, ffont1},
				{1, ffont2},
			}},
	};

	for (const auto &t:substr_tests)
	{
		richtextstring substr{ustring, t.pos, t.len};

		if (substr.need_font_resolution())
			throw EXCEPTION("substr() didn't produce cached results");

		if (substr.resolve_fonts()!=t.results)
			throw EXCEPTION("substr(" << t.pos << ", "
					<< t.len << ") did not recalculate"
					" resolved fonts correctly");
	}

	static const struct {
		size_t pos;

		std::vector<std::pair<size_t, freetypefont>> results;
	} insert_tests[]={
		{0, {
				{0, ffont1},
				{5, ffont2},
				{10, ffont1},
				{15, ffont2},
			}},
		{1, {
				{0, ffont1},
				{6, ffont2},
				{11, ffont1},
				{15, ffont2},
			}},
		{5, {
				{0, ffont1},
				{10, ffont2},
			}},
		{6, {
				{0, ffont1},
				{5, ffont2},
				{6, ffont1},
				{11, ffont2},
			}},
		{10, {
				{0, ffont1},
				{5, ffont2},
				{10, ffont1},
				{15, ffont2},
			}},
	};

	for (const auto &t:insert_tests)
	{
		richtextstring orig=ustring;

		orig.insert(t.pos, orig);

		if (orig.need_font_resolution())
			throw EXCEPTION("substr() didn't produce cached results");

		if (orig.resolve_fonts()!=t.results)
			throw EXCEPTION("insert(" << t.pos
					<< ") did not recalculate"
					" resolved fonts correctly");

		orig.erase(t.pos, ustring.size());

		if (orig.need_font_resolution())
			throw EXCEPTION("substr() didn't produce cached results");

		if (orig.resolve_fonts() != ustring.resolve_fonts())
			throw EXCEPTION("erase(" << t.pos
					<< ") did not recalculate"
					" resolved fonts correctly");
	}
}


void testlinebreaks(const current_fontcollection &font1,
		    const current_fontcollection &font2,
		    const main_window &w)
{
	auto IN_THREAD=w->get_screen()->impl->thread;
	auto black=create_new_background_color(w->get_screen(),
					       w->elementObj::impl
					       ->get_window_handler()
					       .drawable_pictformat, "0%");

	richtextmeta meta1 {black, font1};
	richtextmeta meta3 {black, font2};

	auto richtext=richtext::create(richtextstring{
			// U"Lorem IpsumtiS roloD Amet"
			//   lllllllllllrrrrrrrrrlllll

			U"Lorem Ipsum" + std::u32string{RLO}
			+ U"Dolor Sit" + std::u32string{PDF}
			+ U" Amet",
		{
			{0, meta1},
			{18, meta3},
			{22, meta1},
		}}, richtext_options{});
	auto impl=richtext->debug_get_impl(IN_THREAD);

	if (impl->paragraphs.size() != 1)
		throw EXCEPTION("Did not get 1 paragraph");

	auto p=impl->paragraphs.get_paragraph(0);

	if ((*p)->fragments.size() != 1)
		throw EXCEPTION("Somehow we ended up with multiple fragments");

	auto f=(*p)->get_fragment(0);

	std::vector<unicode_lb> expected_breaks=
		{
		 unicode_lb::none,	// L
		 unicode_lb::none,	// o
		 unicode_lb::none,	// r
		 unicode_lb::none,	// e
		 unicode_lb::none,	// m
		 unicode_lb::none,	//
		 unicode_lb::allowed,	// I
		 unicode_lb::none,	// p
		 unicode_lb::none,	// s
		 unicode_lb::none,	// u
		 unicode_lb::none,	// m
		 unicode_lb::allowed,	// t         <-- R-L start (can break)
		 unicode_lb::none,	// i
		 unicode_lb::none,	// S
		 unicode_lb::allowed,	//
		 unicode_lb::none,	// r
		 unicode_lb::none,	// o
		 unicode_lb::none,	// l
		 unicode_lb::none,	// o
		 unicode_lb::none,	// D
		 unicode_lb::allowed,	//           <-- L-R start (can break)
		 unicode_lb::allowed,	// A
		 unicode_lb::none,	// m
		 unicode_lb::none,	// e
		 unicode_lb::none,	// t
		};
	if (f->breaks != expected_breaks)
		throw EXCEPTION("Unexpected break result");
}

void testrlsplit(const current_fontcollection &font1,
		 const current_fontcollection &font2,
		 const main_window &w)
{
	richtext_insert_results ignored;

	auto IN_THREAD=w->get_screen()->impl->thread;
	auto black=create_new_background_color(w->get_screen(),
					       w->elementObj::impl
					       ->get_window_handler()
					       .drawable_pictformat, "0%");

	richtextmeta metarl{black, font1}, metalr=metarl;

	metarl.rl=true;
	richtext_options options;

	options.paragraph_embedding_level=UNICODE_BIDI_RL;

	auto richtext=richtext::create(richtextstring{
			std::u32string{RLO} + U"lorem\n" + PDF
			+ RLO + U"IPSUM" + PDF,
			{
				{0, metalr},
			}}, options);
	auto impl=richtext->debug_get_impl(IN_THREAD);

	if (impl->paragraphs.size() != 2)
		throw EXCEPTION("Did not get 2 paragraphs");

	auto p=impl->paragraphs.get_paragraph(0);

	if ((*p)->fragments.size() != 1)
		throw EXCEPTION("Somehow we ended up with multiple fragments");

	auto iter1=richtext->at(1, new_location::lr);
	auto iter2=richtext->at(6, new_location::lr);

	{
		auto f=(*p)->get_fragment(0);

		paragraph_list my_paragraphs{*impl};
		fragment_list my_fragments{my_paragraphs, **p};

		f->merge(my_fragments, f->merge_bidi, ignored);
	}

	if ((*p)->get_fragment(0)->string.get_string() !=
	    U"MUSPI\nmerol")
		throw EXCEPTION("testrlsplit: unexpected result of merge");

	if (iter1->pos() != 4 || iter2->pos() != 10 ||
	    iter1->at(IN_THREAD).character != U'm' ||
	    iter2->at(IN_THREAD).character != U'M')
		throw EXCEPTION("testrlsplit: unexpected locations"
				" after merge");

	richtext=richtext::create(richtextstring{
			// U"merol\nMUSPIDolor
			//   rrrrrr rrrrrlllll
			std::u32string{RLO} + U"lorem\n" + PDF
			+ LRI + U"Dolor" + RLI
			+ RLO + U"IPSUM" + PDF,
		{
			{0, metalr},
		}}, richtext_options{options});
	impl=richtext->debug_get_impl(IN_THREAD);

	if (impl->paragraphs.size() != 2)
		throw EXCEPTION("Did not get 2 paragraphs (2)");

	p=impl->paragraphs.get_paragraph(0);

	if ((*p)->fragments.size() != 1)
		throw EXCEPTION("Somehow we ended up with multiple fragments "
				"(2)");

	iter1=richtext->at(1, new_location::lr);
	iter2=richtext->at(11, new_location::lr);

	{
		auto f=(*p)->get_fragment(0);

		paragraph_list my_paragraphs{*impl};
		fragment_list my_fragments{my_paragraphs, **p};

		f->merge(my_fragments, f->merge_bidi, ignored);
	}

	if ((*p)->get_fragment(0)->string.get_string() !=
	    U"DolorMUSPI\nmerol")
		throw EXCEPTION("testrlsplit: unexpected result of merge(2)");

	if (iter1->pos() != 4 || iter2->pos() != 10 ||
	    iter1->at(IN_THREAD).character != U'm' ||
	    iter2->at(IN_THREAD).character != U'M')
		throw EXCEPTION("testrlsplit: unexpected locations"
				" after merge");

	if ((*p)->get_fragment(0)->string.get_meta() !=
	    richtextstring::meta_t{
		    {0, metalr},
		    {5, metarl}
	    })
	{
		throw EXCEPTION("testrlsplit: unexpected meta"
				" after merge");
	}

	richtext=richtext::create(richtextstring{
			std::u32string{RLO} + U"lorem IPSUM" + PDF,
			{
				{0, metalr},
			}}, richtext_options{});
	impl=richtext->debug_get_impl(IN_THREAD);

	if (impl->paragraphs.size() != 1)
		throw EXCEPTION("Did not get 1 paragraphs (splitrl)");

	p=impl->paragraphs.get_paragraph(0);

	if ((*p)->fragments.size() != 1)
		throw EXCEPTION("Somehow we ended up with multiple fragments "
				"(splitrl)");

	iter1=richtext->at(0, new_location::lr);
	iter2=richtext->at(6, new_location::lr);

	if (iter1->at(IN_THREAD).character != U'M' ||
	    iter2->at(IN_THREAD).character != U'm')
		throw EXCEPTION("testrlsplit: unexpected render_order (3)");

	{
		auto f=(*p)->get_fragment(0);

		paragraph_list my_paragraphs{*impl};
		fragment_list my_fragments{my_paragraphs, **p};

		f->split(my_fragments, 5, f->split_rl, false, ignored);
	}

	if ((*p)->fragments.size() != 2 ||
	    (*p)->get_fragment(0)->string.get_string() != U" merol" ||
	    (*p)->get_fragment(1)->string.get_string() != U"MUSPI")
		throw EXCEPTION("testrlsplit: unexpected result of splitrl");

	if (iter1->pos() != 6 || iter2->pos() != 1 ||
	    iter1->at(IN_THREAD).character != U'M' ||
	    iter2->at(IN_THREAD).character != U'm')
		throw EXCEPTION("testrlsplit: unexpected locations"
				" after splitrl");
}

void testrlmerge(const current_fontcollection &font1,
		 const current_fontcollection &font2,
		 const main_window &w)
{
	auto IN_THREAD=w->get_screen()->impl->thread;
	auto black=create_new_background_color(w->get_screen(),
					       w->elementObj::impl
					       ->get_window_handler()
					       .drawable_pictformat, "0%");

	richtext_insert_results ignored;
	richtextmeta lr{black, font1}, rl=lr;

	rl.rl=true;

	static const struct {
		richtextstring s;
		richtextstring result;
	} testcases[]={

		       // Test case 0
		       {
			{
			 U"lorem ipsum\ndolorsit amet",
			 {
			  {0, lr},
			 }
			},
			{
			 U"dolorsit amet\nlorem ipsum",
			 {
			  {0, lr},
			  {13, rl},
			  {14, lr},
			 }
			},
		       },

		       // Test case 1

		       {
			{
				// U"lorem ipsum\ndolorsit amet",
				//   rrrrrrrrrrrr rrrrrrrrrllll

				std::u32string{RLO} + U"muspi merol" + PDF +
				U"\n" +
				LRI + U"amet" + PDI +
				std::u32string{RLO} + U" tisrolod" + PDF,
			 {
			  {0, lr},
			 }
			},
			{
			 U"dolorsit amet\nlorem ipsum",
			 {
			  {0, rl},
			  {9, lr},
			  {13, rl},
			 }
			},
		       },

		       // Test case 2
		       {
			{
				// U"lorem ipsum\ndolorsit amet",
				//   llllllllllll rrrrrrrrrllll

				std::u32string{LRI} + U"lorem ipsum\n"
				U"amet" + PDI + RLO + U" tisrolod" + PDF,
			 {
			  {0, lr},
			 }
			},
			{
			 U"dolorsit amet\nlorem ipsum",
			 {
			  {0, rl},
			  {9, lr},
			  {13, rl},
			  {14, lr},
			 }
			},
		       },

	};

	size_t i=0;

	for (const auto &t:testcases)
	{
		auto copy=t.s;

		richtext_options options;
		options.paragraph_embedding_level=UNICODE_BIDI_RL;

		auto richtext=richtext::create(std::move(copy), options);

		auto impl=richtext->debug_get_impl(IN_THREAD);

		auto p=impl->paragraphs.get_paragraph(0);

		auto f=(*p)->get_fragment(0);

		{
			paragraph_list my_paragraphs{*impl};
			fragment_list my_fragments{my_paragraphs, **p};

			f->merge(my_fragments, f->merge_bidi, ignored);
		}

		if (impl->paragraphs.size() != 1)
		{
			throw EXCEPTION("testrlmerge: test case "
					<< i
					<< " failed, expected 1 paragraph");
		}

		if ((*impl->paragraphs.get_paragraph(0))->fragments.size() != 1)
		{
			throw EXCEPTION("testrlmerge: test case "
					<< i
					<< " failed, expected 1 fragment");
		}

		auto actual=impl->get_as_richtext();

		if (actual.get_string() != t.result.get_string())
		{
			throw EXCEPTION("testrlmerge: test case "
					<< i << " failed, different string");
		}

		if (actual.get_meta() != t.result.get_meta())
		{
			throw EXCEPTION("testrlmerge: test case "
					<< i << " failed, different meta");
		}
		++i;
	}
}

void testunwrap(const current_fontcollection &font1,
		const current_fontcollection &font2,
		const main_window &w)
{
	auto IN_THREAD=w->get_screen()->impl->thread;
	auto black=create_new_background_color(w->get_screen(),
					       w->elementObj::impl
					       ->get_window_handler()
					       .drawable_pictformat, "0%");

	richtextmeta lr{black, font1}, rl=lr;

	rl.rl=true;

	static const struct {
		richtextstring s;

		unicode_bidi_level_t embedding_level;

		// Split the test string at the given positions. A vector
		// of <fragment #, position> tuples.

		std::vector< std::tuple<size_t, size_t,
					richtextfragmentObj::split_t>
			     > preliminary_split;

		// Expected contents of each fragment, after the splits.

		std::vector< std::tuple<size_t, const char32_t *>
			     > expected_fragments;

		const char32_t *unwrap_result;
	} testcases[]={
		       // Case 0
		       {
			{
				// U"lorem rolod muspisit amet",
				//   llllllrrrrrrrrrrrllllllll

				U"lorem "
				+ std::u32string{RLO}
				+ U"ipsum dolor" + PDF
				+ U"sit amet",
			 {
			  {0, lr},
			 }
			},

			UNICODE_BIDI_LR,


			{
			 {0, 17, // fragment 2: "sit amet"
			  richtextfragmentObj::split_lr},

			 {0, 6,  // fragment 1: "rolod muspi"
			  richtextfragmentObj::split_lr},

			 {1, 5,  // split off "rolod",
			  richtextfragmentObj::split_rl},
			},

			{
			 {1, U" muspi"},
			 {2, U"rolod"},
			},

			U"lorem rolod muspisit amet",
		       },
		       // Case 1
		       {
			{
				// U" merolipsum dolor tema tis\n",
				//   rrrrrrllllllllllllrrrrrrrrr

				std::u32string{RLO} + U"sit amet" + PDF +
				LRI + U"ipsum dolor " + PDI +
				RLO + U"lorem " + PDF + U"\n",
			 {
			  {0, lr},
			 }
			},

			UNICODE_BIDI_RL,

			{
			},

			{
			},

			U"\n merolipsum dolor tema tis",
		       },

		       // Case 2
		       {
			{
				// U"lorem ipsum tema tis rolod\n",
				//   llllllllllllrrrrrrrrrrrrrrr
				std::u32string{RLO} + U"dolor sit amet" + PDF
				+ LRI + U"lorem ipsum " + LRM + PDI + U"\n",
			 {
			  {0, lr},
			 }
			},

			UNICODE_BIDI_RL,

			{
			 {0, 17, richtextfragmentObj::split_rl},
			 {1, 1, richtextfragmentObj::split_rl},
			 {1, 6, richtextfragmentObj::split_lr},
			 {2, 6, richtextfragmentObj::split_lr},
			},

			{
			 {0, U" tis rolod"},  // rl
			 {1, U"lorem "},      // lr
			 {2, U"ipsum "},      // lr
			 {3, U"tema"},        // rl
			 {4, U"\n"},          // rl
			},

			U"\ntemalorem ipsum  tis rolod",
		       },
	};
	richtext_insert_results ignored;

	size_t i=0;

	for (const auto &t:testcases)
	{
		auto copy=t.s;

		richtext_options options;

		options.paragraph_embedding_level=t.embedding_level;
		auto richtext=richtext::create(std::move(copy), options);

		auto impl=richtext->debug_get_impl(IN_THREAD);

		auto p=impl->paragraphs.get_paragraph(0);

		{
			paragraph_list my_paragraphs{*impl};
			fragment_list my_fragments{my_paragraphs, **p};

			for (const auto &[fragment, offset, split_type]
				     : t.preliminary_split)
			{
				auto f=(*p)->get_fragment(fragment);
				f->split(my_fragments, offset, split_type,
					 false, ignored);
			}

			for (const auto &[fragment, expected]
				     : t.expected_fragments)
			{
				auto f=(*p)->get_fragment(fragment);

				if (f->string.get_string() != expected)
					throw EXCEPTION("testunwrap: case "
							<< i
							<< ": unexpected "
							"fragment "
							<< fragment);
			}
		}

		impl->unwrap();
		std::u32string s;

		for (size_t i=0, n=impl->paragraphs.size(); i<n; ++i)
		{
			s += (*impl->paragraphs.get_paragraph(i))
				->get_fragment(0)->string.get_string();
		}

		if (s != t.unwrap_result)
			throw EXCEPTION("testunwrap: case "
					<< i
					<< ": unwrap failed");

		++i;
	}
}

int main(int argc, char **argv)
{
	try {
		LIBCXX_NAMESPACE::property
			::load_property(LIBCXX_NAMESPACE_STR "::w::themes",
					"themes", true, true);

		auto options=LIBCXX_NAMESPACE::option::list::create();

		options->addDefaultOptions();

		auto parser=LIBCXX_NAMESPACE::option::parser::create();

		parser->setOptions(options);

		int flag=parser->parseArgv(argc, argv);

		if (flag == 0)
			flag=parser->validate();

		if (flag == LIBCXX_NAMESPACE::option::parser::base::err_builtin)
			exit(0);

		auto mw=main_window::create([&]
					    (const auto &ignore)
					    {
					    });

		auto font1=mw->impl->handler->create_current_fontcollection
			(theme_font{"serif"});
		auto font2=mw->impl->handler->create_current_fontcollection
			(theme_font{"sans_serif"});

		testresolvedfonts(font1, font2, mw);
		testrichtext(font1, font2, mw);
		testsplit(font1, font2, mw);
		testlinebreaks(font1, font2, mw);
		testrlsplit(font1, font2, mw);
		testrlmerge(font1, font2, mw);
		testunwrap(font1, font2, mw);
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return 0;
}
