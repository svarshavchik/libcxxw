#include "libcxxw_config.h"
#include "richtext/richtext.H"
#include "richtext/richtextparagraph.H"
#include "richtext/richtextmetalink.H"
#include "richtext/richtextmetalinkcollection.H"
#include "richtext/fragment_list.H"
#include "richtext/paragraph_list.H"
#include "background_color.H"
#include "screen.H"
#include "main_window.H"
#include "main_window_handler.H"
#include "assert_or_throw.H"
#include <x/options.H>
#include <iostream>

using namespace LIBCXX_NAMESPACE::w;

void testrichtext(const current_fontcollection &font1,
		  const current_fontcollection &font2,
		  const main_window &w)
{
	auto thread_=w->get_screen()->impl->thread;

	auto black=w->get_screen()->impl->create_background_color("0%",
								  {0,0,0});

	richtextstring ustring{
		U"Hello world\nHello\nworld\n",
		{
			{0, {black, font1}},
			{6, {black, font2}},
			{8, {black, font2}},
			{18, {black, font1}},
			{20, {black, font2}}
		}};

	auto richtext=richtext::create(ustring, halign::left, 0);
	auto impl=richtext->debug_get_impl(IN_THREAD);

	if (impl->paragraphs.size() != 3)
		throw EXCEPTION("Did not get 3 paragraphs");

	if (impl->num_chars != ustring.get_string().size())
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

	impl->rewrap(IN_THREAD, 1);

	std::vector<size_t> n_fragments;

	for (size_t i=0, s=impl->paragraphs.size(); i<s; ++i)
	{
		auto paragraph=*impl->paragraphs.get_paragraph(i);

		n_fragments.push_back(paragraph->fragments.size());
	}

	impl->unwrap(IN_THREAD);

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
	auto thread_=w->get_screen()->impl->thread;

	auto black=w->get_screen()->impl->create_background_color("0%",
								  {0,0,0});

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
		auto richtext=richtext::create(ustring, halign::left, 0);
		auto impl=richtext->debug_get_impl(IN_THREAD);

		assert_or_throw(impl->num_chars == ustring.get_string().size(),
				"num_chars in rich text is not right after set()");

		assert_or_throw(impl->paragraphs.size() == 1,
				"rich text was not 1 paragraph");
		assert_or_throw((*impl->paragraphs.get_paragraph(0))
				->num_chars == ustring.get_string().size(),
				"num_chars in paragraph is not right after set()");

		const auto &paragraph=*impl->paragraphs.get_paragraph(0);

		if (paragraph->fragments.size() != 1)
			throw EXCEPTION("Did not get 1 my_fragments");

		LIBCXX_NAMESPACE::w::paragraph_list my_paragraphs{*impl};
		LIBCXX_NAMESPACE::w::fragment_list
			my_fragments{IN_THREAD,
				my_paragraphs,
				*paragraph};

		auto fragment=*paragraph->fragments.get_iter(0);

		auto control_text=fragment->string;
		auto control_breaks=fragment->breaks;

		auto control_horiz=fragment->horiz_info;

		auto new_fragment=fragment->split(IN_THREAD, my_fragments,
						  test.split_pos);

		fragment=*paragraph->fragments.get_iter(0);

		assert_or_throw(impl->num_chars == ustring.get_string().size(),
				"num_chars in rich text should not have changed");
		assert_or_throw(impl->paragraphs.size() == 1,
				"rich text is not still 1 paragraph");
		assert_or_throw((*impl->paragraphs.get_paragraph(0))
				->num_chars == ustring.get_string().size(),
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

		fragment->merge(IN_THREAD, my_fragments);

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

class linkObj : public richtextmetalinkObj {

public:

	linkObj()=default;
	~linkObj()=default;
	void override_text_markup(richtextmeta &markup) const override {}

	void event(enum event_t) const override {}
};

typedef LIBCXX_NAMESPACE::ref<linkObj> my_link;

void testlink(const current_fontcollection &font1,
	      const current_fontcollection &font2,
	      const main_window &w)
{
	auto black=w->get_screen()->impl->create_background_color("0%",
								  {0,0,0});

	richtextstring ustring{
		U"AAAAAAAAAAAAAAA",
		{
			{0, {black, font1}},
			{10, {black, font2}},
		}};

	auto f=richtextmetalinkcollection::create();

	f->emplace_back(0, 5, my_link::create());

	f->emplace_back(8, 4, my_link::create());

	f->emplace_back(13, 2, my_link::create());

	f->apply(ustring);

	std::ostringstream o;

	for (const auto &e: ustring.get_meta())
	{
		o << e.first << ' ' << (e.second.getfont() == font1 ? "1":"2")
		  << ' ' << e.second.link.null() << ';';
	}

	if (o.str() !=
	    "0 1 0;"
	    "5 1 1;"
	    "8 1 0;"
	    "10 2 0;"
	    "12 2 1;"
	    "13 2 0;")
		throw EXCEPTION("textlink() failed: " << o.str());
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

		auto font1=mw->impl->handler->create_theme_font("serif");
		auto font2=mw->impl->handler->create_theme_font("sans serif");

		testrichtext(font1, font2, mw);
		testsplit(font1, font2, mw);
		testlink(font1, font2, mw);
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return 0;
}
