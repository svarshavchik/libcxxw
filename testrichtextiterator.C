#include "libcxxw_config.h"
#include "richtext/richtext.H"
#include "richtext/richtextiterator.H"
#include "richtext/richtextiterator.H"
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

void testrichtext1(const main_window &w,
		   const current_fontcollection &font1,
		   const current_fontcollection &font2)
{
	auto IN_THREAD=w->get_screen()->impl->thread;

	auto black=w->get_screen()->impl->create_background_color("0%");

	richtextstring ustring{
		U"Helloworld ",
		{
			{0, {black, font1}},
		}};

	auto richtext=richtext::create(ustring, halign::left, 0);

	auto b=richtext->at(0);
	auto e=richtext->end();

	assert_or_throw(b->at(IN_THREAD).character == 'H',
			"pos(0) is not character 'H'.");
	assert_or_throw(e->at(IN_THREAD).character == ' ',
			"end() is not space.");
	assert_or_throw(b->debug_get_location()->get_offset() == 0,
			"pos(0) offset is not 0");
	assert_or_throw(e->debug_get_location()->get_offset() == 10,
			"end() offset is not 10");


	b->swap(e);
	assert_or_throw(e->debug_get_location()->get_offset() == 0,
			"pos(0) offset is not 0 after swap");
	assert_or_throw(b->debug_get_location()->get_offset() == 10,
			"end() offset is not 10 after swap");
	b->swap(e);

	auto b_horiz_pos=b->horiz_pos(IN_THREAD);
	auto e_horiz_pos=e->horiz_pos(IN_THREAD);

	b->next(IN_THREAD);
	e->prev(IN_THREAD);

	assert_or_throw(b->debug_get_location()->get_offset() == 1,
			"pos(0)+1 offset is not 1");
	assert_or_throw(e->debug_get_location()->get_offset() == 9,
			"end() offset is not 9");

	assert_or_throw(b->horiz_pos(IN_THREAD) != b_horiz_pos,
			"pos(0)+1 horiz pos did not change");
	assert_or_throw(e->horiz_pos(IN_THREAD) != e_horiz_pos,
			"end() horiz pos did not change");

	b->prev(IN_THREAD);
	e->next(IN_THREAD);

	assert_or_throw(b->debug_get_location()->get_offset() == 0,
			"pos(0) offset is not 0");
	assert_or_throw(e->debug_get_location()->get_offset() == 10,
			"end() offset is not 10");
	assert_or_throw(b->horiz_pos(IN_THREAD) == b_horiz_pos,
			"pos(0) horiz pos different");
	assert_or_throw(e->horiz_pos(IN_THREAD) == e_horiz_pos,
			"end() horiz pos different");
	b->next(IN_THREAD);
	b->next(IN_THREAD);
	b->next(IN_THREAD);
	b->next(IN_THREAD);
	b->next(IN_THREAD);
	assert_or_throw(b->at(IN_THREAD).character == 'w',
			"pos(5) is not character 'w'.");

	auto o=b->insert(IN_THREAD, {
			U"\n",
			{
				{0, {black, font2}}
			}
		});

	assert_or_throw(o->debug_get_location()->my_fragment->my_paragraph
			->num_chars == 6,
			"Number of characters in 1st paragraph is not 12.");

	assert_or_throw(b->debug_get_location()->my_fragment->my_paragraph
			->num_chars == 6,
			"Number of characters in 2nd paragraph is not 12.");

	assert_or_throw(b->at(IN_THREAD).character == 'w',
			"pos(6) is not character 'w' after insert");
	assert_or_throw(b->debug_get_location()->get_offset() == 0,
			"pos(6) offset is not 0");

	assert_or_throw(o->debug_get_location()->get_offset() == 5,
			"pos(5) offset is not 5");
	assert_or_throw(o->at(IN_THREAD).character == '\n',
			"pos(5) is not character '\\n' after insert");

	auto meta=o->debug_get_location()->my_fragment->string.get_meta();

	decltype(meta) expected{
		{0, {black, font1}},
		{5, {black, font2}}
	};

	assert_or_throw(meta == expected,
			"First fragment metadata is not correct");

	meta=b->debug_get_location()->my_fragment->string.get_meta();

	expected={
		{0, {black, font1}},
	};

	assert_or_throw(meta == expected,
			"Second fragment metadata is not correct");

	assert_or_throw(e->debug_get_location()->get_offset() == 5,
			"end() offset is not 5 after insert");

	{
		auto ee=e->clone();

		ee->next(IN_THREAD);

		assert_or_throw(e->debug_get_location()->get_offset() == 5,
				"offset changed after next() on end cursor");
	}
	e->prev(IN_THREAD);
	e->prev(IN_THREAD);
	e->prev(IN_THREAD);
	e->prev(IN_THREAD);
	e->prev(IN_THREAD);

	assert_or_throw(e->debug_get_location()->get_offset() == 0,
			"end()-5 offset is not 0");
	assert_or_throw(e->horiz_pos(IN_THREAD) == 0,
			"end()-5 horiz pos is not 0");

	e->prev(IN_THREAD);
	assert_or_throw(!e->debug_get_location()->my_fragment->prev_fragment(),
		"Cursor not on first fragment");

	assert_or_throw(e->debug_get_location()->get_offset() == 5,
		"Cursor did not move to previous fragment");

	o->next(IN_THREAD);
	assert_or_throw(!o->debug_get_location()->my_fragment->next_fragment(),
		"Cursor not on last fragment");
	assert_or_throw(o->debug_get_location()->get_offset() == 0,
		"Cursor did not move to next fragment");
}

void validate_richtext(ONLY IN_THREAD,
		       const richtext &rtext,
		       const std::string &s,
		       const std::string &what)
{
	std::cout << "validate_richtext: " << what << std::endl;

	auto text=rtext->thread_lock(IN_THREAD,
				     []
				     (ONLY IN_THREAD, auto &lock)
				     {
					     return *lock;
				     });
	std::string v;

	assert_or_throw(text->num_chars == s.size(),
			"invalid overall text num_chars value");

	size_t expected_row=0;
	size_t total_numchars_par=0;
	size_t expected_y_position=0;

	size_t total_fragment_number=0;
	size_t my_paragraph_number=0;

	size_t n_paragraphs=text->paragraphs.size();

	for (size_t i=0; i<n_paragraphs; ++i)
	{
		const auto &paragraph=*(text->paragraphs.get_paragraph(i));

		assert_or_throw(paragraph->my_richtext,
				"paragraph's my_richtext is null");

		assert_or_throw(paragraph->first_char_n == total_numchars_par,
				"paragraph's first_char_n is wrong");

		total_numchars_par += paragraph->num_chars;

		assert_or_throw(paragraph->my_paragraph_number ==
				my_paragraph_number++,
				"wrong my_paragraph_number");

		assert_or_throw(paragraph->my_richtext,
				"my_richtext is null");
		assert_or_throw(paragraph->first_fragment_y_position ==
				expected_y_position,
				"bad first_fragment_y_position");
		assert_or_throw(paragraph->first_fragment_n == expected_row,
				"bad first_fragment_n value");
		assert_or_throw(paragraph->n_fragments_in_paragraph ==
				paragraph->fragments.size(),
				"bad n_fragments_in_paragraph value");

		expected_row += paragraph->fragments.size();

		assert_or_throw(paragraph->below_baseline
				+ paragraph->above_baseline != 0,
				"paragraph height is 0");

		expected_y_position=paragraph->next_paragraph_y_position();

		size_t n=0;

		size_t n_fragments=paragraph->fragments.size();
		size_t my_fragment_number=0;
		for (auto b=paragraph->fragments.get_iter(0);
		     n_fragments; --n_fragments, ++b)
		{
			auto &fragment=*b;

			assert_or_throw(fragment->my_paragraph,
					"fragment's my_paragraph is null");
			assert_or_throw(fragment->my_fragment_number ==
					my_fragment_number++,
					"fragment's my_fragment_number"
					" is wrong");

			assert_or_throw(fragment->index() ==
					total_fragment_number++,
					"fragment's index() doesn't work"
					);

			assert_or_throw(fragment->first_char_n == n,
					"fragment's first_char_n is wrong");
			v.insert(v.end(), fragment->string.get_string().begin(),
				 fragment->string.get_string().end());

			assert_or_throw(fragment->height() > 0,
					"fragment height is 0");

			n += fragment->string.size();
		}

		assert_or_throw(paragraph->num_chars == n,
				"bad paragraph num_chars value");
	}

	assert_or_throw(total_numchars_par == v.size(),
			"sum of paragraph num_chars does not match"
			" total text size");

	std::string ss=
		"expected result of \""
		+ s
		+ "\", got \""
		+ v + "\"";

	std::replace(ss.begin(), ss.end(), '\n', '<');

	assert_or_throw(v == s, ss.c_str());
}

void testrichtext2(const main_window &w,
		   const current_fontcollection &font1)
{
	auto IN_THREAD=w->get_screen()->impl->thread;

	auto black=w->get_screen()->impl->create_background_color("0%");

	const struct {
		const char *orig;
		size_t insert_pos;
		const char *insert_string;
	} tests[] = {
		{
			"Helloworld",
			5,
			"a",
		},
		{
			"Helloworld",
			5,
			"\n",
		},
		{
			"Helloworld",
			5,
			"\n\nb\n\n",
		},
		{
			"world",
			0,
			"Hello\n",
		},
		{
			"Hello ",
			5,
			"world",
		},
		{
			"Hello",
			5,
			"world\n",
		},
		{
			"He\nl\nlo",
			3,
			"world",
		},
		{
			"He\nl\nlo",
			3,
			"\nworld\n",
		},
		{
			"a\nb\nc\nd\ne\nf\n",
			2,
			"\n",
		},
	};

	int testnum=0;

	for (const auto &test:tests)
	{
		std::string test_name=({
				std::ostringstream o;

				o << "test " << ++testnum << ": ";
				o.str();
			});

		std::string orig(test.orig);
		orig += ' ';

		richtextstring ustring{
			std::u32string{orig.begin(), orig.end()},
			{
				{0, {black, font1}}
			}};

		auto text=richtext::create(ustring, halign::left, 0);

		validate_richtext(IN_THREAD, text, orig,
				  test_name + "before insert: ");

		std::string insert_string(test.insert_string);

		ustring={
			std::u32string{insert_string.begin(),
				       insert_string.end()},
			{
				{0, {black, font1}}
			}};

		auto b=text->at(test.insert_pos);

		assert_or_throw(b->at(IN_THREAD).character ==
				(char32_t)orig[test.insert_pos],
				"before insert, unexpected cursor position's"
				" value");

		auto o=b->insert(IN_THREAD, ustring);

		orig.insert(orig.begin()+test.insert_pos,
			    insert_string.begin(),
			    insert_string.end());

		validate_richtext(IN_THREAD, text, orig,
				  test_name + "after insert: ");
	}
}


void testrichtext3(const main_window &w,
		   const current_fontcollection &font1)
{
	auto IN_THREAD=w->get_screen()->impl->thread;

	auto black=w->get_screen()->impl->create_background_color("0%");

	const struct {
		size_t pos1, pos2;
	} tests[]={
		{0, 2},
		{2, 0},
		{8, 9},
		{9, 8},
		{8, 13},
		{13, 8},
		{8, 25},
		{25, 8},
		{1, 1},
		{40, 48},
		{48, 40},
		{49, 51},
		{51, 49},
		{47, 51},
		{51, 47},
		{46, 51},
		{51, 46},
		{8, 50}
	};

	for (const auto &test:tests)
	{
		std::cout << "remove test: "
			  << test.pos1 << "-" << test.pos2
			  << ":" << std::endl;

		std::string orig=
			"The quick\n"			// 10 chars
			"brown fox "			// 10 chars
			"jumped over\n"			// 12 chars
			"the lazy dog's\n"		// 15 chars
			"tail";				// 4 chars

		std::string test_string=orig + " ";

		richtextstring ustring{
			std::u32string{test_string.begin(), test_string.end()},
			{
				{0, {black, font1}}
			}};

		auto text=richtext::create(ustring, halign::left, 0);

		validate_richtext(IN_THREAD, text, test_string,
				  "initial rich text value: ");

		text->thread_lock
			(IN_THREAD,
			 []
			 (ONLY IN_THREAD, auto &lock)
			 {
				 assert_or_throw
					 ((*lock)->paragraphs.size() == 4,
					  "testrichtext2: "
					  "number of paragraphs is not"
					  " 4");

				 auto p=(*lock)->paragraphs.get_paragraph(1);

				 assert_or_throw((*p)->fragments.size() == 1,
						 "testrichtext2: "
						 "number of fragments is not 1")
					 ;

				 {
					 paragraph_list
						 my_paragraphs(**lock);
					 fragment_list
						 my_fragments(IN_THREAD,
							      my_paragraphs,
							      **p);

					 auto f=(*p)->fragments.get_iter(0);

					 (*f)->split(IN_THREAD,
						     my_fragments, 10);
					 my_fragments
						 .fragments_were_rewrapped();
				 }
			 });

		auto b=text->at(0), e=text->end();

		assert_or_throw(b->debug_get_location()->my_fragment
				->find_y_position( e->debug_get_location()
						   ->my_fragment
						   ->y_position())
				.first->index() ==
				e->debug_get_location()->my_fragment
				->index(),
				"find_y_position forward failed");

		assert_or_throw(e->debug_get_location()->my_fragment
				->find_y_position(0)
				.first->index() == 0,
				"find_y_position backward failed");

		auto pos1=text->at(test.pos1);
		auto pos2=text->at(test.pos2);

		auto pos_middle=text->at((test.pos1+test.pos2)/2);

		assert_or_throw(pos1->at(IN_THREAD).character ==
				(char32_t)test_string[test.pos1],
				"unexpected return from at(pos1)");
		assert_or_throw(pos2->at(IN_THREAD).character ==
				(char32_t)test_string[test.pos2],
				"unexpected return from at(pos2)");

		if (test.pos1 < test.pos2)
		{
			assert_or_throw(pos1->compare(pos2) < 0,
					"compare_position() did not "
					"return < 0");
		}
		else if (test.pos1 > test.pos2)
		{
			assert_or_throw(pos1->compare(pos2) > 0,
					"compare_position() did not "
					"return > 0");
		}
		else
		{
			assert_or_throw(pos1->compare(pos2) == 0,
					"compare_position() did not "
					"return 0");
		}


		pos1->remove(IN_THREAD, pos2);

		auto p=test.pos1 < test.pos2 ? test.pos1:test.pos2;

		if (test.pos1 < test.pos2)
		{
			test_string.erase(test_string.begin()+test.pos1,
				      test_string.begin()+test.pos2);
		} else if (test.pos1 > test.pos2)
		{
			test_string.erase(test_string.begin()+test.pos2,
				      test_string.begin()+test.pos1);
		}

		assert_or_throw(pos1->at(IN_THREAD).character ==
				(char32_t)test_string[p],
				"after remove() unexpected return from at(pos1)"
				);
		assert_or_throw(pos2->at(IN_THREAD).character ==
				(char32_t)test_string[p],
				"after remove() unexpected return from at(pos2)"
				);
		assert_or_throw(pos_middle->at(IN_THREAD).character ==
				(char32_t)test_string[p],
				"after remove() unexpected return from at(pos_middle)"
				);

		assert_or_throw(pos1->compare(pos2) == 0,
				"compare_position() did not "
				"return 0 after remove()");

		assert_or_throw(pos1->compare(pos_middle) == 0,
				"compare_position() for "
				"pos_middle did not "
				"return 0 after remove()");

		validate_richtext(IN_THREAD, text, test_string,
				  "validate_richtext");


		auto v2=text->begin()->get(text->end());

		std::u32string check{test_string.begin(),
				--test_string.end()};

		assert_or_throw(v2.get_string() == check,
				"get(begin - end) did not return"
				" expected results");

		v2=text->end()->get(text->begin());

		assert_or_throw(v2.get_string() == check,
				"get(begin - end) did not return"
				" expected results");
	}
}

void testrichtext4(const main_window &w,
		   const current_fontcollection &font1)
{
	auto IN_THREAD=w->get_screen()->impl->thread;

	auto black=w->get_screen()->impl->create_background_color("0%");

	std::string test_string="12345 67890 ABCDEF\n" // 19 chars
		"G\n"
		"H\n"
		"I\n"
		"J"

		" ";

	richtextstring ustring{
		std::u32string{test_string.begin(), test_string.end()},
		{
			{0, {black, font1}}
		}};

	auto richtext=richtext::create(ustring, halign::left, 0);

	richtext->thread_lock
		(IN_THREAD,
		 [&]
		 (ONLY IN_THREAD, auto &lock)
		 {
			 auto p=(*lock)->paragraphs.get_paragraph(0);

			 {
				 paragraph_list my_paragraphs(**lock);
				 fragment_list my_fragments{IN_THREAD,
						 my_paragraphs,
						 **p};

				 auto f=*(*p)->fragments.get_iter(0);

				 f->split(IN_THREAD, my_fragments, 12);
				 f->split(IN_THREAD, my_fragments, 6);
				 my_fragments.fragments_were_rewrapped();
			 }
		 });

	validate_richtext(IN_THREAD, richtext, test_string,
			  "testrichtext4: ");

	int n=test_string.size();

	for (int i=0; i<n; ++i)
	{
		for (int j=-1; j<n+2; ++j)
		{
			std::ostringstream o;

			o << "testrichtext4: initial position=" << i
			  << ", move by: " << (j-i);

			auto cursor=richtext->at(i);
			if (cursor->at(IN_THREAD).character !=
			    (char32_t)test_string[i])
				throw EXCEPTION(o.str() +
						": unexpected character under cursor");
			cursor->move(IN_THREAD, j-i);

			if (cursor->at(IN_THREAD).character != (char32_t)
			    (test_string[j < 0 ? 0: j >= n ? n-1:j]))
				throw EXCEPTION(o.str() +
						": unexpected character under cursor");
			cursor->next(IN_THREAD);
			cursor->prev(IN_THREAD);
		}
	}
}

void testrichtext5(const main_window &w,
		   const current_fontcollection &font2)
{
	auto IN_THREAD=w->get_screen()->impl->thread;

	auto black=w->get_screen()->impl->create_background_color("0%");

	std::string test_string=
		"The quick brown fox jumped over the lazy dog's tail.";

	richtextstring ustring{
		std::u32string{test_string.begin(), test_string.end()},
		{
			{0, {black, font2}}
		}};


	auto richtext=richtext::create(ustring, halign::left, 0);

	auto text_width=richtext->thread_lock
		(IN_THREAD,
		 []
		 (ONLY IN_THREAD, auto &lock)
		 {
			 return (*lock)->width();
		 });

	richtext->rewrap(IN_THREAD, text_width / 3);

	static const char * const tests[]={
		"abra cadabra",
		"The quick brown fox jumped over the lazy dog's tail",
		"a\nb\nc\nd\ne\nf\ng\nh\n"
	};

	for (const auto &test:tests)
	{
		auto insert_pos=richtext->at(7);

		std::u32string ustring{test, test+strlen(test)};

		auto before_insert=insert_pos->insert(IN_THREAD, ustring);

		std::string shouldbe=test_string.substr(0, 7) + test
			+ test_string.substr(7);

		validate_richtext(IN_THREAD, richtext, shouldbe,
				  "testrichtext5, after insert");

		insert_pos->remove(IN_THREAD, before_insert);

		validate_richtext(IN_THREAD, richtext, test_string,
				  "testrichtext5, after remove");
	}
}

void testrichtext6(const main_window &w,
		   const current_fontcollection &font1,
		   const current_fontcollection &font2)
{
	auto IN_THREAD=w->get_screen()->impl->thread;

	auto black=w->get_screen()->impl->create_background_color("0%");

	richtextstring ustring{
		U"Hello---world ",
		{
			{0, {black, font1}},
		}};

	auto richtext=richtext::create(ustring, halign::left, 0);

	auto b=richtext->at(5);
	auto e=richtext->at(8);

	ustring={
		U" ",
		{
			{0, {black, font2}},
		}};

	e->replace(IN_THREAD, b, ustring);

	validate_richtext(IN_THREAD, richtext, "Hello world ", "replace()");

	if (b->at(IN_THREAD).character != ' ')
		throw EXCEPTION("Beginning iterator changed");
	if (e->at(IN_THREAD).character != 'w')
		throw EXCEPTION("Ending iterator changed");

	auto s=b->get(e);

	if (s.get_string() != U" " ||
	    s.get_meta() != std::vector<std::pair<size_t, richtextmeta>>{
		    {0, {black, font2}}
	    })
		throw EXCEPTION("Return value from replace()");

	s=b->begin()->get(b->end());

	if (s.get_string() != U"Hello world" ||
	    s.get_meta() != std::vector<std::pair<size_t, richtextmeta>>{
		    {0, {black, font1}},
		    {5, {black, font2}},
		    {6, {black, font1}}
	    })
		throw EXCEPTION("Entire text after replace()");

}

int main(int argc, char **argv)
{
	try {
		LIBCXX_NAMESPACE::property
			::load_property(LIBCXX_NAMESPACE_STR "::themes",
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
		auto font2=mw->impl->handler->create_theme_font("sans_serif");

		testrichtext1(mw, font1, font2);
		testrichtext2(mw, font1);
		testrichtext3(mw, font1);
		testrichtext4(mw, font1);
		testrichtext5(mw, font2);
		testrichtext6(mw, font1, font2);
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return 0;
}
