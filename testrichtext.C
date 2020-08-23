#include "libcxxw_config.h"
#include "x/w/impl/richtext/richtext.H"
#include "richtext/richtextparagraph.H"
#include "richtext/fragment_list.H"
#include "richtext/paragraph_list.H"
#include "x/w/impl/background_color.H"
#include "screen.H"
#include "main_window.H"
#include "main_window_handler.H"
#include "assert_or_throw.H"
#include <x/options.H>
#include <iostream>
#include <algorithm>

using namespace LIBCXX_NAMESPACE::w;

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

	auto richtext=richtext::create(std::move(ustring), halign::left, 0);
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
					       halign::left, 0);
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

		auto new_fragment=fragment->split(my_fragments,
						  test.split_pos);

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

		fragment->merge(my_fragments);

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
	richtextmeta meta2 {black, font1};
	richtextmeta meta3 {black, font2};
	meta2.rl=true;
	meta3.rl=true;

	auto richtext=richtext::create(richtextstring{
		U"Lorem IpsumDolor Sit Amet",
		{
			{0, meta1},
			{11, meta2},
			{17, meta3},
			{20, meta1},
		}}, halign::left, 0);
	auto impl=richtext->debug_get_impl(IN_THREAD);

	if (impl->paragraphs.size() != 1)
		throw EXCEPTION("Did not get 1 paragraph");

	auto p=impl->paragraphs.get_paragraph(0);

	if ((*p)->fragments.size() != 1)
		throw EXCEPTION("Somehow we ended up with multiple fragments");

	auto f=(*p)->get_fragment(0);

	if (f->string.get_string() != U"Lorem IpsumtiS roloD Amet")

		throw EXCEPTION("Rendering order not set");

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

int main(int argc, char **argv)
{
	try {
		LIBCXX_NAMESPACE::property
			::load_property(LIBCXX_NAMESPACE_STR "::w::themes",
					"themes", true, true);

		LIBCXX_NAMESPACE::locale::base::environment()->global();

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
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return 0;
}
