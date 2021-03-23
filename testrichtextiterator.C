#include "libcxxw_config.h"
#include "mockrichtext.H"
#include "x/w/impl/richtext/richtext.H"
#include "x/w/richtext/richtextiterator.H"
#include "x/w/richtext/richtextiterator.H"
#include "richtext/fragment_list.H"
#include "richtext/paragraph_list.H"
#include "richtext/richtext_insert.H"
#include "x/w/impl/background_color.H"
#include "screen.H"
#include "main_window.H"
#include "main_window_handler.H"
#include "assert_or_throw.H"
#include <x/options.H>
#include <iostream>
#include <algorithm>
#include <variant>
#include <functional>
#include <utility>
#include <courier-unicode.h>

using namespace LIBCXX_NAMESPACE::w;
using namespace unicode::literals;

void testrichtext1(ONLY IN_THREAD)
{
	richtextstring ustring{
		U"Helloworld ",
		{
			{0, {'1'}},
		}};

	auto richtext=richtext::create(std::move(ustring), richtext_options{});

	auto b=richtext->at(0, new_location::lr);
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

	b->right(IN_THREAD);
	e->left(IN_THREAD);

	assert_or_throw(b->debug_get_location()->get_offset() == 1,
			"pos(0)+1 offset is not 1");
	assert_or_throw(e->debug_get_location()->get_offset() == 9,
			"end() offset is not 9");

	assert_or_throw(b->horiz_pos(IN_THREAD) != b_horiz_pos,
			"pos(0)+1 horiz pos did not change");
	assert_or_throw(e->horiz_pos(IN_THREAD) != e_horiz_pos,
			"end() horiz pos did not change");

	b->left(IN_THREAD);
	e->right(IN_THREAD);

	assert_or_throw(b->debug_get_location()->get_offset() == 0,
			"pos(0) offset is not 0");
	assert_or_throw(e->debug_get_location()->get_offset() == 10,
			"end() offset is not 10");
	assert_or_throw(b->horiz_pos(IN_THREAD) == b_horiz_pos,
			"pos(0) horiz pos different");
	assert_or_throw(e->horiz_pos(IN_THREAD) == e_horiz_pos,
			"end() horiz pos different");
	b->right(IN_THREAD);
	b->right(IN_THREAD);
	b->right(IN_THREAD);
	b->right(IN_THREAD);
	b->right(IN_THREAD);
	assert_or_throw(b->at(IN_THREAD).character == 'w',
			"pos(5) is not character 'w'.");

	auto bm1=b->clone();
	auto bp1=b->clone();
	bm1->left(IN_THREAD);
	bp1->right(IN_THREAD);
	auto o=b->insert(IN_THREAD, {
			U"\n",
			{
				{0, {'2'}}
			}
		});

	assert_or_throw(bm1->pos() == 4 &&
			o->pos() == 5 &&
			b->pos() == 6 &&
			bp1->pos() == 7,
			"Unexpected iterator positions");
	assert_or_throw(o->debug_get_location()->my_fragment->my_paragraph
			->num_chars == 6,
			"Number of characters in 1st paragraph is not 6.");

	assert_or_throw(b->debug_get_location()->my_fragment->my_paragraph
			->num_chars == 6,
			"Number of characters in 2nd paragraph is not 6.");

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
		{0, {'1'}},
		{5, {'2'}}
	};

	assert_or_throw(meta == expected,
			"First fragment metadata is not correct");

	meta=b->debug_get_location()->my_fragment->string.get_meta();

	expected={
		{0, {'1'}},
	};

	assert_or_throw(meta == expected,
			"Second fragment metadata is not correct");

	assert_or_throw(e->debug_get_location()->get_offset() == 5,
			"end() offset is not 5 after insert");

	{
		auto ee=e->clone();

		ee->right(IN_THREAD);

		assert_or_throw(e->debug_get_location()->get_offset() == 5,
				"offset changed after right() on end cursor");
	}
	e->left(IN_THREAD);
	e->left(IN_THREAD);
	e->left(IN_THREAD);
	e->left(IN_THREAD);
	e->left(IN_THREAD);

	assert_or_throw(e->debug_get_location()->get_offset() == 0,
			"end()-5 offset is not 0");
	assert_or_throw(e->horiz_pos(IN_THREAD) == 0,
			"end()-5 horiz pos is not 0");

	e->left(IN_THREAD);
	assert_or_throw(!e->debug_get_location()->my_fragment->prev_fragment(),
		"Cursor not on first fragment");

	assert_or_throw(e->debug_get_location()->get_offset() == 5,
		"Cursor did not move to previous fragment");

	o->right(IN_THREAD);
	assert_or_throw(!o->debug_get_location()->my_fragment->next_fragment(),
		"Cursor not on last fragment");
	assert_or_throw(o->debug_get_location()->get_offset() == 0,
		"Cursor did not move to next fragment");
}

// For validation purposes, richtext's hotspot collection get decoded into
// a vector containing:
//
// 1) hotspot number,
// 2) hotspot's first and last fragments' index().
// 3) A vector of start/end tuples, the location of the hotspot in the
//    fragment from the first to the last one.

typedef std::vector<std::tuple<size_t,
			       size_t, size_t,
			       std::vector<std::tuple<size_t, size_t>>
			       >> decoded_hotspot_info_t;

static std::string dump_hotspot_info(const decoded_hotspot_info_t &info)
{
	std::ostringstream o;

	for (const auto &[number, first, last, ranges]:info)
	{
		o << "hotspot " << number << ": fragments "
		  << first << "-" << last << "\n";

		for (const auto &[start, end]:ranges)
		{
			o << "    " << start << "-" << end << "\n";
		}
	}

	return o.str();
}

auto validate_richtext_structure(ONLY IN_THREAD,
				 const richtext &rtext,
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

	decoded_hotspot_info_t decoded_hotspot_info;

	for (auto &hotspot_info:text->hotspot_collection)
	{
		auto &[begin_fragment, end_fragment] = hotspot_info.second;

		decoded_hotspot_info.emplace_back
			(hotspot_info.first,
			 begin_fragment->index(),
			 end_fragment->index(),
			 std::vector<std::tuple<size_t, size_t>>{});

		auto &v=std::get<3>(decoded_hotspot_info.back());

		assert_or_throw(begin_fragment->index()<=end_fragment->index(),
				"hotspot begin after its end");

		auto p=begin_fragment;

		while (1)
		{
			auto iter=
				p->hotspot_collection.find(hotspot_info.first);

			assert_or_throw(iter != p
					->hotspot_collection.end(),
					"did not find expected hotspot");
			auto &[start, end]=iter->second;

			v.emplace_back(start, end);

			assert_or_throw(start <= end &&	end <= p->string.size(),
					"invalid hotspot fragment location");

			auto &m=p->string.get_meta();
			auto mb=m.begin(), me=m.end();

			while (mb != me)
			{
				if (mb->first == start)
					break;
				++mb;
			}

			assert_or_throw(mb != me &&
					mb->first == start,
					"did not find hotspot start in fragment"
					);
			while (mb != me)
			{
				if (mb->first == end)
					break;
				++mb;
			}

			assert_or_throw((mb != me ? mb->first
					 : p->string.size())
					== end,
					"did not find hotspot end in fragment");

			if (p == end_fragment)
				break;

			auto n=p->next_fragment();

			assert_or_throw(n,
					"did not find last hotspot fragment");

			p=richtextfragment{n};
		}
	}
	std::sort(decoded_hotspot_info.begin(), decoded_hotspot_info.end());
	return std::tuple{v, text->num_chars, decoded_hotspot_info};
}

void validate_richtext(ONLY IN_THREAD,
		       const richtext &rtext,
		       const std::string &s,
		       const std::string &what)
{
	auto [v, num_chars, hotspot_collection]=
		validate_richtext_structure(IN_THREAD, rtext, what);
	assert_or_throw(num_chars == s.size(),
			"invalid overall text num_chars value");

	std::string ss=
		"expected result of \""
		+ s
		+ "\", got \""
		+ v + "\"";

	std::replace(ss.begin(), ss.end(), '\n', '<');

	assert_or_throw(v == s, ss.c_str());
}

void testrichtext2(ONLY IN_THREAD)
{
	const struct {
		const char *orig;
		size_t insert_pos;
		const char *insert_string;
	} tests[] = {
		// Test 1
		{
			"Helloworld",
			5,
			"a",
		},
		// Test 2
		{
			"Helloworld",
			5,
			"\n",
		},
		// Test 3
		{
			"Helloworld",
			5,
			"\n\nb\n\n",
		},
		// Test 4
		{
			"world",
			0,
			"Hello\n",
		},
		// Test 5
		{
			"world",
			0,
			"Hello",
		},
		// Test 6
		{
			"Hello ",
			5,
			"world",
		},
		// Test 7
		{
			"Hello",
			5,
			"world\n",
		},
		// Test 8
		{
			"He\nl\nlo",
			3,
			"world",
		},
		// Test 9
		{
			"He\nl\nlo",
			3,
			"\nworld\n",
		},
		// Test 10
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
				{0, {'1'}}
			}};

		auto text=richtext::create(std::move(ustring), richtext_options{});

		validate_richtext(IN_THREAD, text, orig,
				  test_name + "before insert: ");

		std::string insert_string(test.insert_string);

		ustring={
			std::u32string{insert_string.begin(),
				insert_string.end()},
			{
				{0, {'1'}}
			}};

		auto b=text->at(test.insert_pos, new_location::lr);

		assert_or_throw(b->at(IN_THREAD).character ==
				(char32_t)orig[test.insert_pos],
				"before insert, unexpected cursor position's"
				" value");

		auto bm1=b->clone();
		auto bp1=b->clone();
		bm1->left(IN_THREAD);
		bp1->right(IN_THREAD);

		auto o=b->insert(IN_THREAD, std::move(ustring));

		if (test.insert_pos > 0)
			assert_or_throw(bm1->pos() == test.insert_pos-1,
					"unexpected preceding iterator position"
					);
		assert_or_throw(b->pos() == test.insert_pos+
				insert_string.size(),
				"unexpected current iterator position");

		if (test.insert_pos < std::string{test.orig}.size())
		{
			assert_or_throw(bp1->pos() == test.insert_pos+1+
					insert_string.size(),
					"unexpected following iterator"
					" position");
		}
		orig.insert(orig.begin()+test.insert_pos,
			    insert_string.begin(),
			    insert_string.end());

		validate_richtext(IN_THREAD, text, orig,
				  test_name + "after insert: ");
	}
}


void testrichtext3(ONLY IN_THREAD)
{
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
				{0, {'1'}}
			}};

		auto text=richtext::create(std::move(ustring), richtext_options{});

		validate_richtext(IN_THREAD, text, test_string,
				  "initial rich text value: ");

		text->thread_lock
			(IN_THREAD,
			 []
			 (ONLY IN_THREAD, auto &lock)
			 {
				 richtext_insert_results ignored;

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
						 my_fragments(my_paragraphs,
							      **p);

					 auto f=(*p)->fragments.get_iter(0);

					 (*f)->split(my_fragments, 10,
						     (*f)->split_lr, false,
						     ignored);
					 my_fragments
						 .fragments_were_rewrapped();
				 }
			 });

		auto b=text->at(0, new_location::lr), e=text->end();

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

		auto pos1=text->at(test.pos1, new_location::lr);
		auto pos2=text->at(test.pos2, new_location::lr);

		auto pos_middle=text->at((test.pos1+test.pos2)/2, new_location::lr);

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


		auto v2=text->begin()->get_richtextstring(text->end());

		std::u32string check{test_string.begin(),
				--test_string.end()};

		assert_or_throw(v2.get_string() == check,
				"get(begin - end) did not return"
				" expected results");

		v2=text->end()->get_richtextstring(text->begin());

		assert_or_throw(v2.get_string() == check,
				"get(begin - end) did not return"
				" expected results");
	}
}

void testrichtext4(ONLY IN_THREAD)
{
	richtext_insert_results ignored;

	std::string test_string="12345 67890 ABCDEF\n" // 19 chars
		"G\n"
		"H\n"
		"I\n"
		"J"

		" ";

	richtextstring ustring{
		std::u32string{test_string.begin(), test_string.end()},
		{
			{0, {'1'}}
		}};

	auto richtext=richtext::create(std::move(ustring), richtext_options{});

	richtext->thread_lock
		(IN_THREAD,
		 [&]
		 (ONLY IN_THREAD, auto &lock)
		 {
			 auto p=(*lock)->paragraphs.get_paragraph(0);

			 {
				 paragraph_list my_paragraphs(**lock);
				 fragment_list my_fragments{my_paragraphs,
							    **p};

				 auto f=*(*p)->fragments.get_iter(0);

				 f->split(my_fragments, 12, f->split_lr, false,
					  ignored);
				 f->split(my_fragments, 6, f->split_lr, false,
					  ignored);
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

			auto cursor=richtext->at(i, new_location::lr);
			if (cursor->at(IN_THREAD).character !=
			    (char32_t)test_string[i])
				throw EXCEPTION(o.str() +
						": unexpected character under cursor");
			cursor->move(IN_THREAD, j-i);

			if (cursor->at(IN_THREAD).character != (char32_t)
			    (test_string[j < 0 ? 0: j >= n ? n-1:j]))
				throw EXCEPTION(o.str() +
						": unexpected character under cursor");
			cursor->right(IN_THREAD);
			cursor->left(IN_THREAD);
		}
	}
}

void testrichtext5(ONLY IN_THREAD)
{
	std::string test_string=
		"The quick brown fox jumped over the lazy dog's tail.";

	richtextstring ustring{
		std::u32string{test_string.begin(), test_string.end()},
		{
			{0, {'2'}}
		}};


	auto richtext=richtext::create(std::move(ustring), richtext_options{});

	auto text_width=richtext->thread_lock
		(IN_THREAD,
		 []
		 (ONLY IN_THREAD, auto &lock)
		 {
			 return (*lock)->width();
		 });

	richtext->rewrap(text_width / 3);

	static const char * const tests[]={
		"abra cadabra",
		"The quick brown fox jumped over the lazy dog's tail",
		"a\nb\nc\nd\ne\nf\ng\nh\n"
	};

	for (const auto &test:tests)
	{
		auto insert_pos=richtext->at(7, new_location::lr);

		std::u32string ustring{test, test+strlen(test)};

		auto before_insert=insert_pos->insert(IN_THREAD, ustring);

		assert_or_throw(insert_pos->pos() == 7 + ustring.size(),
				"insert() did not update the insert pos");
		std::string shouldbe=test_string.substr(0, 7) + test
			+ test_string.substr(7);

		validate_richtext(IN_THREAD, richtext, shouldbe,
				  "testrichtext5, after insert");

		insert_pos->remove(IN_THREAD, before_insert);

		validate_richtext(IN_THREAD, richtext, test_string,
				  "testrichtext5, after remove");
	}
}

void testrichtext6(ONLY IN_THREAD)
{
	richtextstring ustring{
		U"Hello---world ",
		{
			{0, {'1'}},
		}};

	auto richtext=richtext::create(std::move(ustring), richtext_options{});

	auto b=richtext->at(5, new_location::lr);
	auto e=richtext->at(8, new_location::lr);

	ustring={
		U" ",
		{
			{0, {'2'}},
		}};

	e->replace(IN_THREAD, b, std::move(ustring), false);

	validate_richtext(IN_THREAD, richtext, "Hello world ", "replace()");

	if (b->at(IN_THREAD).character != ' ')
		throw EXCEPTION("Beginning iterator changed");
	if (e->at(IN_THREAD).character != 'w')
		throw EXCEPTION("Ending iterator changed");

	auto s=b->get_richtextstring(e);

	if (s.get_string() != U" " ||
	    s.get_meta() != std::vector<std::pair<size_t, richtextmeta>>{
		    {0, {'2'}}
	    })
		throw EXCEPTION("Return value from replace()");

	s=b->begin()->get_richtextstring(b->end());

	if (s.get_string() != U"Hello world" ||
	    s.get_meta() != std::vector<std::pair<size_t, richtextmeta>>{
		    {0, {'1'}},
		    {5, {'2'}},
		    {6, {'1'}}
	    })
		throw EXCEPTION("Entire text after replace()");

}

void testrichtext7(ONLY IN_THREAD)
{
	richtextmeta metalr{'1'};

	richtextstring ustring{
		std::u32string{RLO} +
		U"Hello\nworld\nrolem\nipsum" + PDF,
		{
			{0, metalr},
		}};

	// Right to left tests.
	richtext_options options;
	options.paragraph_embedding_level=UNICODE_BIDI_RL;
	options.alignment=halign::left;
	options.unprintable_char=' ';
	auto richtext=richtext::create(std::move(ustring), options);

	auto b=richtext->begin();
	auto e=richtext->end();

	if (b->pos() != 0 || e->pos() != 22 || b->at(IN_THREAD).character != 'H'
	    || e->at(IN_THREAD).character != 'm')
		throw EXCEPTION("testrichtext7: test 1 failed");

	b->move(IN_THREAD, 1);
	e->move(IN_THREAD, -1);
	if (b->pos() != 1 || e->pos() != 21 || b->at(IN_THREAD).character != 'e'
	    || e->at(IN_THREAD).character != 'u')
		throw EXCEPTION("testrichtext7: test 2 failed");

	if (b->get_richtextstring(e, bidi_format::none) !=
	    richtextstring{
		    U"ello\nworld\nrolem\nips",
		    {
			    {0, metalr},
		    }})
	{
		throw EXCEPTION("testrichtext7: test 13 failed");
	}
	b->move(IN_THREAD, 10);
	if (b->pos() != 11 || b->at(IN_THREAD).character != '\n')
		throw EXCEPTION("testrichtext7: test 3 failed");
	b->move(IN_THREAD, -10);
	if (b->pos() != 1 || b->at(IN_THREAD).character != 'e')
		throw EXCEPTION("testrichtext7: test 4 failed");
	b->start_of_line();
	if (b->pos() != 5 || b->at(IN_THREAD).character != '\n')
		throw EXCEPTION("testrichtext7: test 5 failed");
	b->down(IN_THREAD);
	if (b->pos() != 11 || b->at(IN_THREAD).character != '\n')
		throw EXCEPTION("testrichtext7: test 6 failed");
	b->up(IN_THREAD);
	if (b->pos() != 5 || b->at(IN_THREAD).character != '\n')
		throw EXCEPTION("testrichtext7: test 7 failed");
	b=b->pos(1);
	if (b->at(IN_THREAD).character != 'e')
		throw EXCEPTION("testrichtext7: test 8 failed");
	b=b->pos(11);
	if (b->at(IN_THREAD).character != '\n')
		throw EXCEPTION("testrichtext7: test 9 failed");

	b=b->pos(1);
	if (b->at(IN_THREAD).character != 'e')
		throw EXCEPTION("testrichtext7: test 10 failed");

	b->move(IN_THREAD, 12);
	if (b->at(IN_THREAD).character != 'o')
		throw EXCEPTION("testrichtext7: test 11 failed");
	b->move(IN_THREAD, -12);
	if (b->at(IN_THREAD).character != 'e')
		throw EXCEPTION("testrichtext7: test 12 failed");
}

struct printable_char {
	char32_t c;
};

std::ostream &operator<<(std::ostream &o, printable_char c)
{
	if (c.c == '\n')
		o << "\\n";
	else
		o << (char)c.c;

	return o;
}

std::string format(const std::vector<std::string> &l)
{
	std::ostringstream o;

	for (auto &s:l)
	{
		o << "\"";

		for (auto c:s)
		{
			if (c == '\n')
			{
				o << "\\n";
			}
			else if (c == richtextstring::hotspot_marker)
			{
				o.put('|');
			}
			else o.put(c);
		}
		o << "\",\n";
	}
	return o.str();
}

template<typename F> static void extract(ONLY IN_THREAD,
					 const richtext &richtext, F &&f)
{
	richtext->thread_lock
		(IN_THREAD,
		 [&]
		 (ONLY IN_THREAD, auto &lock)
		 {
			 (*lock)->paragraphs.for_paragraphs
				 (0,
				  [&]
				  (auto p)
				  {
					  p->fragments.for_fragments
						  (std::forward<F>(f));
					  return true;
				  });
		 });
}

static void dump(ONLY IN_THREAD,
		 const richtext &richtext)
{
	std::vector<std::string> actual;

	extract(IN_THREAD, richtext, [&](const auto &f)
	{
		auto &s=f->string.get_string();

		actual.push_back(unicode::iconvert::convert(s, unicode::utf_8));
	});

	std::cout << format(actual);
}

void testrichtext8(ONLY IN_THREAD)
{
	richtextmeta metalr{'0'};

	static const struct {
		std::u32string base_string;
		dim_t wrap_width;
		unicode_bidi_level_t level;
		size_t insert_pos;
		std::u32string insert_string;
		std::vector<std::string> expected;
		size_t expected_orig;
		size_t expected_insert;
	} tests[] = {
		// Test 1
		{
			// U"olleH dlrow melor muspi\n",
			std::u32string{RLO} +
			U"ipsum rolem world Hello\n" + PDF,
			120,
			UNICODE_BIDI_RL,
			0,
			// U" tema tis",
			std::u32string{RLO} +
			U"sit amet " + PDF,
			{
				" tema tis",
				" melor muspi",
				"\nolleH dlrow",
			},
			0,
			9,
		},
		// Test 2
		{
			// U"olleH dlrow melor muspi\n",
			std::u32string{RLO} +
			U"ipsum rolem world Hello\n" + PDF,
			120,
			UNICODE_BIDI_RL,
			0,
			// U"tema tis\n111 222 333",
			std::u32string{RLO} +
			U"sit amet\n333 222 111",
			{
				"\ntema tis",
				" 222 333",
				" muspi111",
				" melor",
				"\nolleH dlrow",
			},
			0,
			20,
		},
		// Test 3
		{
			// U"olleH dlrow melor muspi\n",
			std::u32string{RLO} +
			U"ipsum rolem world Hello\n" + PDF,
			120,
			UNICODE_BIDI_RL,
			0,
			std::u32string{RLO} +
			U"sit amet\n333 222 111\n",
			{
				"\ntema tis",
				" 222 333",
				"\n111",
				" melor muspi",
				"\nolleH dlrow",
			},
			0,
			21,
		},
		// Test 4
		{
			// U"olleH dlrow melor muspi\n",
			std::u32string{RLO} +
			U"ipsum rolem world Hello\n" + PDF,
			120,
			UNICODE_BIDI_RL,
			0,
			// U"111 227\n333\n444"
			//   llllrrrr rrrr rrr

			std::u32string{RLO} + U"722" + PDF +
			LRI + U"111 " + LRM + PDI +
			U"\n333\n444",
			{
				"227",
				"\n111 ",
				"\n333",
				" muspi444",
				" melor",
				"\nolleH dlrow",
			},
			0,
			15,
		},
		// Test 5
		{
			U"Lorem Ipsum Dolor Sit Amet\n",
			120,
			UNICODE_BIDI_LR,
			11,
			std::u32string{RLO} + U"4321" + PDF,
			{
				"Lorem Ipsum ",
				"1234Dolor ",
				"Sit Amet\n",
			},
			15,
			11,
		},
		// Test 6
		{
			U"Lorem Ipsum Dolor Sit Amet\n",
			120,
			UNICODE_BIDI_LR,
			11,
			// U"1234 56 78 90\nAB CD\nEF GH",
			std::u32string{RLO} +
			U"09 87 65 4321\nDC BA\nHG FE" + PDF,
			{
				"Lorem Ipsum ",
				" 56 78 90",
				"1234\n",
				"AB CD\n",
				"EF GHDolor ",
				"Sit Amet\n",
			},
			20,
			37,
		},
		// Test 7
		{
			U"Lorem Ipsum Dolor\n",
			120,
			UNICODE_BIDI_LR,
			11,
			// U"1234 56 78 90\n",

			std::u32string{RLO} +
			U"09 87 65 4321" + PDF + U"\n",
			{
				"Lorem Ipsum ",
				" 56 78 90",
				"1234\n",
				"Dolor\n",
			},
			20,
			26,
		},
		// Test 8
		{
			U"Lorem Ipsum Dolor\n",
			120,
			UNICODE_BIDI_LR,
			5,
			std::u32string{RLO} +
			U"sit amet\n333 222 111" + PDF,
			{
				"Lorem ",
				"tema tis\n",
				" 222 333",
				"111Ipsum ",
				"Dolor\n",
			},
			6,
			26,
		},

		// Test 9
		{
			U"Lorem Ipsum Dolor\n",
			120,
			UNICODE_BIDI_LR,
			5,
			std::u32string{RLO} + U"sit amet\n" + PDF,
			{
				"Lorem ",
				"tema tis\n",
				"Ipsum Dolor\n",
			},
			6,
			15,
		},
		// Test 10
		{
			U"Lorem Ipsum Dolor\n",
			120,
			UNICODE_BIDI_LR,
			5,
			std::u32string{RLO} +
			U"333 222 111\nsit amet\n",
			{
				"Lorem ",
				" 222 333",
				"111\n",
				"tema tis\n",
				"Ipsum Dolor\n",
			},
			13,
			27,
		},
		// Test 11
		{
			std::u32string{RLO} +
			U"ipsum rolem world Hello\n" + PDF,
			120,
			UNICODE_BIDI_RL,
			0,
			std::u32string{RLO} +
			U"sit amet \n" + PDF,
			{
				"\n tema tis",
				" melor muspi",
				"\nolleH dlrow",
			},
			0,
			10,
		},
		// Test 12
		{
			U"Lorem IpsumDolor",
			120,
			UNICODE_BIDI_LR,
			5,
			std::u32string{RLO} + U"amet" + PDF,
			{
				"Lorem tema",
				"IpsumDolor",
			},
			9,
			5,
		},
		// Test 13
		{
			U"Lorem\nIpsum",
			120,
			UNICODE_BIDI_LR,
			5,
			std::u32string{RLO} + U"amet" + PDF,
			{
				"Loremtema\n",
				"Ipsum",
			},
			5,
			9,
		},
		// Test 14
		{
			std::u32string{RLO} +
			U"Lorem Ipsum Dolor sit amet" + PDF,
			120,
			UNICODE_BIDI_RL,
			0,
			std::u32string{RLO}+U"amet" + PDF,
			{
				" meroLtema",
				" muspI",
				" tis roloD",
				"tema"
			},
			0,
			4,
		},
		// Test 15
		{
			std::u32string{RLO} + U"Lorem\nmuspI" + PDF,
			120,
			UNICODE_BIDI_RL,
			5,
			std::u32string{RLO}+U"amet" + PDF,
			{
				"\ntemameroL",
				"Ipsum"
			},
			5,
			9,
		},
		// Test 16
		{
			std::u32string{RLO} + U"Lorem\nmuspI" + PDF,
			120,
			UNICODE_BIDI_RL,
			5,
			U"tematema",
			{
				"meroL",
				"\ntematema",
				"Ipsum",
			},
			5,
			13,
		},
		// Test 17
		{
			std::u32string{RLO} +
			U"Lorem Ipsum Dolor sit amet" + PDF,
			120,
			UNICODE_BIDI_RL,
			0,
			std::u32string{RLO} + U"3 2 1" + PDF,
			{
				" meroL1 2 3",
				" muspI",
				" tis roloD",
				"tema"
			},
			0,
			5,
		},
		// Test 18
		{
			std::u32string{RLO} +
			U"Lorem Ipsum Dolor sit amet" + PDF,
			120,
			UNICODE_BIDI_RL,
			0,
			std::u32string{RLO} +
			U"3 2 1\n" + PDF,
			{
				"\n1 2 3",
				" muspI meroL",
				" tis roloD",
				"tema"
			},
			0,
			6,
		},

		// Test 19
		{
			std::u32string{RLO} +
			U"Lorem Ipsum Dolor sit amet" + PDF,
			120,
			UNICODE_BIDI_RL,
			0,
			std::u32string{RLO} +
			U"3 2 1\n6 5 4",
			{
				"\n1 2 3",
				" meroL4 5 6",
				" muspI",
				" tis roloD",
				"tema"
			},
			0,
			11,
		},

		// Test 20
		{
			std::u32string{RLO} +
			U"Lorem Ipsum Dolor sit amet" + PDF,
			120,
			UNICODE_BIDI_RL,
			6,
			std::u32string{RLO} + U"3 2 " + PDF,
			{
				" 2 3 meroL",
				" muspI",
				" tis roloD",
				"tema"
			},
			6,
			10,
		},

		// Test 21
		{
			std::u32string{RLO} +
			U"Lorem Ipsum Dolor sit amet" + PDF,
			120,
			UNICODE_BIDI_RL,
			6,
			std::u32string{RLO} + U"3 2\n" + PDF,
			{
				"\n2 3 meroL",
				" muspI",
				" tis roloD",
				"tema"
			},
			6,
			10,
		},

		// Test 22
		{
			std::u32string{RLO} +
			U"Lorem Ipsum Dolor sit amet" + PDF,
			120,
			UNICODE_BIDI_RL,
			6,
			std::u32string{RLO} +
			U"3 2\n5 4" + PDF,
			{
				"\n2 3 meroL",
				" muspI4 5",
				" tis roloD",
				"tema"
			},
			6,
			13,
		},

		// Test 23
		{
			std::u32string{RLO} +
			U"Lorem Ipsum Dolor sit amet" + PDF,
			120,
			UNICODE_BIDI_RL,
			6,
			std::u32string{RLO} +
			U"3 2\n5 4\n7 6" + PDF,
			{
				"\n2 3 meroL",
				"\n4 5",
				" muspI6 7",
				" tis roloD",
				"tema"
			},
			6,
			17,
		},
		// Test 24
		{
			std::u32string{RLO} +
			U"Lorem\nIpsum Dolor sit amet" + PDF,
			120,
			UNICODE_BIDI_RL,
			5,
			std::u32string{LRO} + U"2 3" + PDF,
			{
				"\n2 3meroL",
				" roloD muspI",
				"tema tis"
			},
			5,
			8,
		},
		// Test 25
		{
			std::u32string{RLO} +
			U"Lorem\nIpsum Dolor sit amet" + PDF,
			120,
			UNICODE_BIDI_RL,
			5,
			U"Epsilon",
			{
				"meroL",
				"\nEpsilon",
				" roloD muspI",
				"tema tis"
			},
			5,
			12,
		},
	};

	size_t testnum=0;

	for (const auto &t:tests)
	{
		++testnum;

		// Right to left tests.
		richtext_options options;
		options.paragraph_embedding_level=t.level;
		options.unprintable_char=' ';
		options.initial_width=t.wrap_width;
		auto richtext=richtext::create(richtextstring{
				t.base_string,
				{
					{0, metalr},
				},

			}, options);


		dump(IN_THREAD, richtext);

		std::cout << "----------------\n";

		auto iter=richtext->at(t.insert_pos, new_location::bidi);

		auto orig=iter->insert(IN_THREAD,
				       richtextstring{
					       t.insert_string,
					       {
						       {0, metalr},
					       }
				       });
		dump(IN_THREAD, richtext);
		std::cout << "\n";

		if (orig->pos() != t.expected_orig)
			throw EXCEPTION("testrichtext8: test "
					<< testnum
					<< " unexpected orig iter position ('"
					<< printable_char{
						orig->at(IN_THREAD).character
					}
					<< "', "
					<< orig->pos()
					<< ")");

		if (iter->pos() != t.expected_insert)
			throw EXCEPTION("testrichtext8: test "
					<< testnum
					<< " unexpected insert iter position ('"
					<< printable_char{
						iter->at(IN_THREAD).character
					}
					<< "', "
					<< iter->pos()
					<< ")");

		std::vector<std::string> actual;

		extract(IN_THREAD, richtext, [&]
			(const auto &f)
		{
			auto &s=f->string.get_string();

			actual.push_back({s.begin(), s.end()});
		});

		if (actual != t.expected)
			throw EXCEPTION("testrichtext8: test "
					<< testnum
					<< " unexpected results:\n"
					<< format(actual));
	}
}

void testrichtext9(ONLY IN_THREAD)
{
	static const struct {
		std::u32string orig_text;
		std::optional<unicode_bidi_level_t> paragraph_embedding_level;
		std::optional<halign> alignment;

		unicode_bidi_level_t final_paragraph_embedding_level;
		halign               final_alignment;

		std::u32string new_text;

		unicode_bidi_level_t updated_paragraph_embedding_level;
		halign               updated_alignment;
	} tests[]={
		// Test 1
		{
			U"Hello world",
			std::nullopt,
			std::nullopt,
			UNICODE_BIDI_LR,
			halign::left,
			U"Rolem Ipsum Dolor",
			UNICODE_BIDI_LR,
			halign::left,
		},
		// Test 2
		{
			std::u32string{RLM} + U"Hello world",
			std::nullopt,
			std::nullopt,
			UNICODE_BIDI_RL,
			halign::right,
			std::u32string{RLM} + U"Rolem Ipsum Dolor",
			UNICODE_BIDI_RL,
			halign::right,
		},
		// Test 3
		{
			U"Hello world",
			std::nullopt,
			std::nullopt,
			UNICODE_BIDI_LR,
			halign::left,
			std::u32string{RLM} + U"Rolem Ipsum Dolor",
			UNICODE_BIDI_RL,
			halign::right,
		},
		// Test 4
		{
			std::u32string{RLM} + U"Hello world",
			std::nullopt,
			std::nullopt,
			UNICODE_BIDI_RL,
			halign::right,
			U"Rolem Ipsum Dolor",
			UNICODE_BIDI_LR,
			halign::left,
		},
		// Test 5
		{
			U"Hello world",
			UNICODE_BIDI_RL,
			halign::center,
			UNICODE_BIDI_RL,
			halign::center,
			U"Rolem Ipsum Dolor",
			UNICODE_BIDI_RL,
			halign::center,
		},
		// Test 6
		{
			U"Hello world",
			UNICODE_BIDI_RL,
			halign::center,
			UNICODE_BIDI_RL,
			halign::center,
			std::u32string{RLM} + U"Rolem Ipsum Dolor",
			UNICODE_BIDI_RL,
			halign::center,
		},
	};

	size_t testcase=0;

	for (const auto &t:tests)
	{
		++testcase;

		richtext_options options;

		options.paragraph_embedding_level=t.paragraph_embedding_level;
		options.alignment=t.alignment;

		auto richtext=richtext::create(richtextstring{
				t.orig_text,
				{
					{0, richtextmeta{}}
				}},
			options);

		if (richtext->get_paragraph_embedding_level(IN_THREAD)
		    != t.final_paragraph_embedding_level)
			throw EXCEPTION
				("testrichtext9, test " << testcase <<
				 ": unexpected final_paragraph_embedding_level"
				 );

		if (richtext->get_alignment(IN_THREAD)
		    != t.final_alignment)
			throw EXCEPTION
				("testrichtext9, test " << testcase <<
				 ": unexpected final_alignment"
				 );

		richtext->set(IN_THREAD,
			      richtextstring{
				      t.new_text,
				      {
					      {0, richtextmeta{}}
				      }});

		if (richtext->get_paragraph_embedding_level(IN_THREAD)
		    != t.updated_paragraph_embedding_level)
			throw EXCEPTION
				("testrichtext9, test " << testcase <<
				 ": set: "
				 "unexpected updated_paragraph_embedding_level"
				 );

		if (richtext->get_alignment(IN_THREAD)
		    != t.updated_alignment)
			throw EXCEPTION
				("testrichtext9, test " << testcase <<
				 ": set: unexpected updated_alignment"
				 );

		if (richtext->size(IN_THREAD) != 17)
			throw EXCEPTION
				("testrichtext9, test " << testcase <<
				 ": set: unexpected final size");

		options.is_editor=1;

		richtext=richtext::create(richtextstring{
				U"\n",
				{
					{0, richtextmeta{}}
				}},
			options);

		auto cursor=richtext->end();
		auto before_cursor=
			cursor->insert(IN_THREAD,
				       richtextstring{
					       t.new_text,
					       {
						       {0, richtextmeta{}}
					       }});

		if (richtext->get_paragraph_embedding_level(IN_THREAD)
		    != t.updated_paragraph_embedding_level)
			throw EXCEPTION
				("testrichtext9, test " << testcase <<
				 ": insert: "
				 "unexpected updated_paragraph_embedding_level"
				 );

		if (richtext->get_alignment(IN_THREAD)
		    != t.updated_alignment)
			throw EXCEPTION
				("testrichtext9, test " << testcase <<
				 ": insert: unexpected updated_alignment"
				 );

		if (cursor->compare(cursor->end()) != 0)
			throw EXCEPTION
				("testrichtext9, test " << testcase <<
				 ": insert: cursor not at the end "
				 << "(" << cursor->pos() << "/"
				 << cursor->end()->pos() << ")"
				 );
		if (before_cursor->compare(cursor->begin()) != 0)
			throw EXCEPTION
				("testrichtext9, test " << testcase <<
				 ": insert: insert position not at the start"
				 );

		richtext=richtext::create(richtextstring{
				t.orig_text + U"\n",
				{
					{0, richtextmeta{}}
				}},
			options);

		richtext->begin()->replace
			(IN_THREAD, richtext->end(),
			 richtextstring{
				 t.new_text,
				 {
					 {0, richtextmeta{}}
				 }},
			 false);

		if (richtext->get_paragraph_embedding_level(IN_THREAD)
		    != t.updated_paragraph_embedding_level)
			throw EXCEPTION
				("testrichtext9, test " << testcase <<
				 ": replace(1): "
				 "unexpected updated_paragraph_embedding_level"
				 );

		if (richtext->get_alignment(IN_THREAD)
		    != t.updated_alignment)
			throw EXCEPTION
				("testrichtext9, test " << testcase <<
				 ": replace(1): unexpected updated_alignment"
				 );

		if (richtext->size(IN_THREAD) != 18)
			throw EXCEPTION
				("testrichtext9, test " << testcase <<
				 ": replace(1): unexpected final size");

		richtext=richtext::create(richtextstring{
				t.orig_text + U"\n",
				{
					{0, richtextmeta{}}
				}},
			options);

		richtext->end()->replace
			(IN_THREAD, richtext->begin(),
			 richtextstring{
				 t.new_text,
				 {
					 {0, richtextmeta{}}
				 }},
			 false);

		if (richtext->get_paragraph_embedding_level(IN_THREAD)
		    != t.updated_paragraph_embedding_level)
			throw EXCEPTION
				("testrichtext9, test " << testcase <<
				 ": replace(2): "
				 "unexpected updated_paragraph_embedding_level"
				 );

		if (richtext->get_alignment(IN_THREAD)
		    != t.updated_alignment)
			throw EXCEPTION
				("testrichtext9, test " << testcase <<
				 ": replace(2): unexpected updated_alignment"
				 );
		if (richtext->size(IN_THREAD) != 18)
			throw EXCEPTION
				("testrichtext9, test " << testcase <<
				 ": replace(1): unexpected final size");
	}
}

// Functor used below to remove something from a richtext

struct richtext_remove_between {

	size_t pos1;
	size_t pos2;

	richtext_remove_between(size_t pos1,
				size_t pos2) : pos1{pos1}, pos2{pos2}
	{
	}

	void operator()(ONLY IN_THREAD, const richtext &t) const
	{
		t->at(pos1, new_location::bidi)->
			remove(IN_THREAD, t->at(pos2, new_location::bidi));
	}
};

// Functor used below to insert something into a richtext

struct richtext_insert_into {

	size_t pos;
	const char32_t *str;

	richtext_insert_into(size_t pos, const char32_t *str)
		: pos{pos}, str{str}
	{
	}

	void operator()(ONLY IN_THREAD, const richtext &t) const
	{
		skip_hotspot_sanity_check=true;
		t->at(pos, new_location::bidi)->insert(IN_THREAD, str);
		skip_hotspot_sanity_check=false;
	}
};

// Functor used below to replace a hotspot

struct richtext_replace_hotspot {

	text_hotspot hotspot;
	std::u32string str;

	richtext_replace_hotspot(const text_hotspot &hotspot,
				 std::u32string str)
		: hotspot{hotspot}, str{std::move(str)}
	{
	}

	void operator()(ONLY IN_THREAD, const richtext &t) const
	{
		richtextstring rts{str,
				   {
					   {0, {0, 0, 0}}
				   }};

		t->replace_hotspot(IN_THREAD, rts, hotspot);
	}
};

typedef std::variant<richtext_remove_between, richtext_insert_into,
		     richtext_replace_hotspot> richtext_op;

void testrichtext10(ONLY IN_THREAD)
{
	static constexpr richtextmeta meta0{0, 0, 0};
	static constexpr richtextmeta meta1{0, 0, 1};
	static constexpr richtextmeta meta2{0, 0, 2};

	static const struct {
		richtextstring input;
		std::optional<unicode_bidi_level_t> embedding_level;
		dim_t wrap_to_width;
		decoded_hotspot_info_t expected_hotspot_collection;

		std::vector<std::tuple<richtext_op,
				       decoded_hotspot_info_t>
			    > modifications;
	} tests[]={
		// Test 1
		{
			{
				U"Lorem Ipsum Dolor Sit "
				U"Amet consectetur "
				U"adipisicing elit",
				{
					{0, meta0},
					{6, meta1},
					{51, meta0},
				},
			},
			UNICODE_BIDI_LR,
			200,
			{
				{
					1, 0, 2,
					{
						{6, 23},
						{0, 17},
						{0, 13}
					}
				},

			},
			{
				{
					richtext_op{
						std::in_place_type_t<richtext_remove_between>{},
						23, 40
					},
					decoded_hotspot_info_t{
						{
							1, 0, 1,
							{
								{6, 23},
								{0, 13}
							},
						}
					},
				},
				{
					richtext_op{
						std::in_place_type_t<richtext_insert_into>{},
						23, U"Hello World ",
					},
					decoded_hotspot_info_t{
						{
							1, 0, 3,
							{
								{6, 23},
								{0, 17},
								{0, 12},
								{0, 13}
							},
						}
					},
				},
				{
					richtext_op{
						std::in_place_type_t<richtext_insert_into>{},
						23, U"Hello World\n",
					},
					decoded_hotspot_info_t{
						{
							1, 0, 3,
							{
								{6, 23},
								{0, 12},
								{0, 17},
								{0, 13}
							},
						}
					},
				},

				{
					richtext_op{
						std::in_place_type_t<richtext_replace_hotspot>{},
						meta1.link,
						U"Hello World!",
					},
					decoded_hotspot_info_t{
						{
							1, 0, 0,
							{
								{6, 20},
							},
						}
					},
				},
			}
		},

		// Test 2
		{
			{
				U"Lorem Ipsum "
				U"Dolor Sit "
				U"Amet "
				U"consectetur "
				U"adipisicing "
				U"elit",
				{
					{0, meta0},
					{6, meta1},
					{53, meta0},
				},
			},
			UNICODE_BIDI_LR,
			120,
			{
				{
					1, 0, 5,
					{
						{6, 13},
						{0, 10},
						{0, 5},
						{0, 12},
						{0, 12},
						{0, 3},
					}
				},
			},
			{
				{
					richtext_op{
						std::in_place_type_t<richtext_remove_between>{},
						23, 40
					},

					decoded_hotspot_info_t{
						{
							1, 0, 3,
							{
								{6, 13},
								{0, 10},
								{0, 12},
								{0, 3},
							}
						}
					}
				},
			},
		},

		// Test 3

		{
			{
				U"Lorem Ipsum Dolor Sit\n"
				U"Amet consectetur "
				U"adipisicing elit",
				{
					{0, meta0},
					{6, meta1},
					{51, meta0},
				},
			},
			UNICODE_BIDI_LR,
			200,
			{
				{
					1, 0, 2,
					{
						{6, 23},
						{0, 17},
						{0, 13}
					}
				},

			},



			{
				{
					richtext_op{
						std::in_place_type_t<richtext_remove_between>{},
						13, 28
					},

					decoded_hotspot_info_t{
						{
							1, 0, 2,
							{
								{6, 13},
								{0, 12},
								{0, 13}
							},
						},
					}
				}
			}
		},

		// Test 4

		{
			{
				U"Lorem Ipsum Dolor Sit "
				U"Amet_consectetur "
				U"adipisicing elit sed "
				U"do eiusmod tem",
				{
					{0, meta0},
					{12, meta2},
					{18, meta1},
					{71, meta0},
				},
			},
			UNICODE_BIDI_LR,
			220,
			{
				{
					1, 0, 3,
					{
						{20, 25},
						{0, 17},
						{0, 21},
						{0, 12},
					}
				},
				{
					2, 0, 0,
					{
						{12, 20},
					}
				},
			},

			{
				{
					richtext_op{
						std::in_place_type_t<richtext_remove_between>{},
						12, 20
					},

					decoded_hotspot_info_t{
						{
							1, 0, 3,
							{
								{12, 17},
								{0, 17},
								{0, 21},
								{0, 12},
							},
						},
					}
				},
				{
					richtext_op{
						std::in_place_type_t<richtext_remove_between>{},
						20, 25
					},

					decoded_hotspot_info_t{
						{
							1, 1, 3,
							{
								{1, 18},
								{0, 21},
								{0, 12},
							},
						},
						{
							2, 0, 1,
							{
								{12, 19},
								{0, 1},
							},
						},
					}
				},
				{
					richtext_op{
						std::in_place_type_t<richtext_replace_hotspot>{},
						meta1.link,
						U"Hello World!",
					},
					decoded_hotspot_info_t{
						{
							1, 0, 1,
							{
								{20, 27},
								{0, 7},
							},
						},
						{
							2, 0, 0,
							{
								{12, 20},
							},
						},
					},
				},
				{
					richtext_op{
						std::in_place_type_t<richtext_replace_hotspot>{},
						meta2.link,
						U"Hello World!",
					},
					decoded_hotspot_info_t{
						{
							1, 0, 3,
							{
								{26, 27},
								{0, 21},
								{0, 21},
								{0, 12},
							},
						},
						{
							2, 0, 0,
							{
								{12, 26},
							},
						},
					},
				},
			},
		},

		// Test 5
		{
			{
				U" 7",
				{
					{0, meta1},
				},
			},
			UNICODE_BIDI_RL,
			200,
			{
				{
					1, 0, 0,
					{
						{
							{0, 4},
						},
					},
				},
			},

			{
				{
					richtext_op{
						std::in_place_type_t<richtext_replace_hotspot>{},
						meta1.link,
						U" 8",
					},
					decoded_hotspot_info_t{
						{
							1, 0, 0,
							{
								{0, 4},
							},
						},
					},
				},
				{
					richtext_op{
						std::in_place_type_t<richtext_replace_hotspot>{},
						meta1.link,
						U" 9",
					},
					decoded_hotspot_info_t{
						{
							1, 0, 0,
							{
								{0, 4},
							},
						},
					},
				},
				{
					richtext_op{
						std::in_place_type_t<richtext_replace_hotspot>{},
						meta1.link,
						U"14",
					},
					decoded_hotspot_info_t{
						{
							1, 0, 0,
							{
								{0, 4},
							},
						},
					},
				},
				{
					richtext_op{
						std::in_place_type_t<richtext_replace_hotspot>{},
						meta1.link,
						U"15",
					},
					decoded_hotspot_info_t{
						{
							1, 0, 0,
							{
								{0, 4},
							},
						},
					},
				},
			},
		},


		// Test 6
		{
			{
				std::u32string{RLO} +
				U"Lorem Ipsum 11111 111 amet" + PDF,
				{
					{0, meta0},
					{13, meta1},
					{22, meta0},
				},
			},
			UNICODE_BIDI_RL,
			120,
			{
				{
					1, 1, 2,
					{
						{
							{0, 7},
							{5, 9},
						},
					},
				},

			},

			{
				{
					richtext_op{
						std::in_place_type_t<richtext_replace_hotspot>{},
						meta1.link,
						std::u32string{RLO} +
						U"AAAAA AAA",
					},
					decoded_hotspot_info_t{
						{
							1, 1, 2,
							{
								{0, 7},
								{5, 9},
							},
						},
					},
				},
			},
		},
	};
	size_t casenum=0;

	for (const auto &t:tests)
	{
		std::string test_name=({
				std::ostringstream o;

				o << "testrichtext10, test " << ++casenum << ": ";
				o.str();
			});

		richtext_options options;

		options.paragraph_embedding_level=t.embedding_level;
		options.initial_width=t.wrap_to_width;

		auto richtext=richtext::create((richtextstring)t.input,options);

		dump(IN_THREAD, richtext);
		auto [v, num_chars, hotspot_collection]=
			validate_richtext_structure(IN_THREAD, richtext,
						    test_name);

		std::cout << dump_hotspot_info(hotspot_collection);
		assert_or_throw(hotspot_collection ==
				t.expected_hotspot_collection,
				(test_name + " hotspot collection mismatch")
				.c_str());

		size_t modification_number=0;

		for (const auto &[op, res]:t.modifications)
		{
			std::string mod_test_name=({
					std::ostringstream o;

					o << test_name << "modification "
					  << ++modification_number << ": ";
					o.str();
				});

			auto richtext=richtext::create((richtextstring)t.input,
						  options);

			std::visit([&]
				   (const auto &op)
			{
				op(IN_THREAD, richtext);
			}, op);

			dump(IN_THREAD, richtext);
			auto [v, num_chars, hotspot_collection]=
				validate_richtext_structure(IN_THREAD, richtext,
							    mod_test_name);

			std::cout << dump_hotspot_info(hotspot_collection);
			assert_or_throw(hotspot_collection == res,
					(mod_test_name +
					 " hotspot collection mismatch")
					.c_str());
		}
	}
}

struct test11_info {
	const richtext text;
	richtextiterator cursor;
};

struct test11_insert {

	const char32_t *string;
	test11_insert(const char32_t *string) : string{string} {}

	void operator()(ONLY IN_THREAD, test11_info &info) const
	{
		std::cout << info.cursor->pos() << "-";
		info.cursor->insert(IN_THREAD, string);
		std::cout << info.cursor->pos() << "\n";
	}
};

struct test11_setpos {

	size_t pos;

	test11_setpos(size_t pos) : pos{pos} {}

	void operator()(ONLY IN_THREAD, test11_info &info) const
	{
		info.cursor=info.cursor->pos(pos);
	}
};

struct test11_remove_between {

	size_t pos1;
	size_t pos2;

	test11_remove_between(size_t pos1,
			      size_t pos2) : pos1{pos1}, pos2{pos2}
	{
	}

	void operator()(ONLY IN_THREAD, const test11_info &info) const
	{
		info.text->at(pos1, new_location::bidi)->
			remove(IN_THREAD,
			       info.text->at(pos2, new_location::bidi));
	}
};

typedef std::variant<test11_insert, test11_setpos,
		     test11_remove_between> test11_step;

void testrichtext11(ONLY IN_THREAD)
{
	static constexpr richtextmeta meta0{0, 0};
	// static constexpr richtextmeta meta1{0, 1};

	static const struct {
		std::optional<unicode_bidi_level_t> embedding_level;

		std::vector<std::tuple<test11_step,
				       richtextstring>> tests;
	} tests[]={

		// Test 1
		{
			std::nullopt,

			{
				// Step 1
				{
					test11_step{
						std::in_place_type_t<test11_insert>{},
						U" "
					},
					{
						U" ",
						{
							{0, meta0}
						},
					}
				},
				// Step 2
				{
					test11_step{
						std::in_place_type_t<test11_insert>{},
						U"ש"
					},
					{
						U" ש",
						{
							{0, meta0},
						},
					}
				},
				// Step 3
				{
					test11_step{
						std::in_place_type_t<test11_insert>{},
						U"ל"
					},
					{
						U" לש",
						{
							{0, meta0},
						},
					}
				},
				// Step 4
				{
					test11_step{
						std::in_place_type_t<test11_setpos>{},
						0,
					},
					{
						U" לש",
						{
							{0, meta0},
						},
					}
				},
				// Step 5
				{
					test11_step{
						std::in_place_type_t<test11_insert>{},
						U"ש"
					},
					{
						U" לשש",
						{
							{0, meta0},
						},
					}
				},
				// Step 6
				{
					test11_step{
						std::in_place_type_t<test11_insert>{},
						U"ל"
					},
					{
						U" לששל",
						{
							{0, meta0},
						},
					}
				},
			},
		},
		// Test 2
		{
			std::nullopt,

			{
				// Step 1
				{
					test11_step{
						std::in_place_type_t<test11_insert>{},
						U"Hello"
					},
					{
						U"Hello",
						{
							{0, meta0}
						},
					}
				},
				// Step 2
				{
					test11_step{
						std::in_place_type_t<test11_setpos>{},
						4,
					},
					{
						U"Hello",
						{
							{0, meta0},
						},
					}
				},
				// Step 3
				{
					test11_step{
						std::in_place_type_t<test11_insert>{},
						U"ש"
					},
					{
						U"Helloש",
						{
							{0, meta0}
						},
					}
				},
				// Step 4
				{
					test11_step{
						std::in_place_type_t<test11_insert>{},
						U"ל"
					},
					{
						U"Helloשל",
						{
							{0, meta0}
						},
					}
				},
			},
		},

		// Test 3
		{
			std::nullopt,

			{
				// Step 1
				{
					test11_step{
						std::in_place_type_t<test11_insert>{},
						U"של"
					},
					{
						U"של",
						{
							{0, meta0}
						},
					}
				},
				// Step 2
				{
					test11_step{
						std::in_place_type_t<test11_insert>{},
						U"ו"
					},
					{
						U"שלו",
						{
							{0, meta0}
						},
					}
				},
			},
		},

		// Test 4
		{
			std::nullopt,
			{
				// Step 1
				{
					test11_step{
						std::in_place_type_t<test11_insert>{},
						U"של"
					},
					{
						U"של",
						{
							{0, meta0}
						},
					}
				},
				// Step 2
				{
					test11_step{
						std::in_place_type_t<test11_remove_between>{},
						1, 2,
					},
					{
						U"ש",
						{
							{0, meta0}
						},
					}
				},
			},
		},
	};

	// שלום
	size_t casenum=0;
	for (const auto &t:tests)
	{
		++casenum;

		richtext_options options;

		options.is_editor=1;

		options.paragraph_embedding_level=t.embedding_level;

		auto richtext=richtext::create(richtextstring{
				U"\n",
				{
					{0, meta0}
				}}, options);

		test11_info info{richtext, richtext->end()};

		size_t testnum=0;

		for (const auto &[op, expected] : t.tests)
		{
			++testnum;

			std::visit(
				   [&](const auto &op)
				   {
					   op(IN_THREAD, info);
				   }, op);

			auto actual=
				richtext->begin()->get_richtextstring
				(richtext->end(),
				 bidi_format::none);

			if (actual != expected)
			{
				throw EXCEPTION("testrichtext11 test "
						<< casenum
						<< ", step "
						<< testnum
						<< ": unexpected result");
			}
		}
	}
}

void testrichtext12(ONLY IN_THREAD)
{
	static constexpr richtextmeta meta0{0};

	static const struct {
		const char32_t *input;
		dim_t wrap_to_width;
		std::vector<std::tuple<size_t, size_t,
				       const char32_t *>> tests;
	} tests[]={
		// Test 1
		{
			U"Lorem IpsumשלוLoremIpsumDolorשלו\n",

			130,
			{
				{0, 1, U"orem IpsumשלוLoremIpsumDolorשלו"},
				{10, 11, U"Lorem IpsuשלוLoremIpsumDolorשלו"},
				{11, 14, U"Lorem IpsumשלLoremIpsumDolorשלו"},
				{12, 11, U"Lorem IpsumשוLoremIpsumDolorשלו"},
			},
		},
		// Test 2
		{
			U"שלוLorem IpsumשלוLorem Ipsum\n",
			130,
		},
	};

	size_t casenum=0;

	for (const auto &t:tests)
	{
		++casenum;

		size_t testnum=0;

		for (const auto [pos, moved_pos, expected]:t.tests)
		{
			richtext_options options;

			options.initial_width=t.wrap_to_width;

			auto richtext=
				richtext::create(richtextstring
						 {std::u32string{t.input},
							 {
								 {0, meta0}
							 }
						 },
						 options);

			if (testnum++ == 0)
				dump(IN_THREAD, richtext);

			auto iter=richtext->at(pos, new_location::bidi);

			auto new_iter=iter->clone();

			new_iter->move_for_delete(IN_THREAD);

			if (new_iter->pos() != moved_pos)
			{
				std::ostringstream o;

				o << "testrichtext12, test "
				  << casenum << ", case " << testnum
				  << ": ended up at position "
				  << new_iter->pos()
				  << " instead of " << moved_pos;

				throw EXCEPTION(o.str());
			}

			iter->remove(IN_THREAD, new_iter);

			iter=iter->begin();
			new_iter=iter->end();

			auto s=iter->get_richtextstring(new_iter,
							bidi_format::none);

			if (s.get_string() != expected)
			{
				std::ostringstream o;

				o << "testrichtext12, test "
				  << casenum << ", case " << testnum
				  << ": result is \""
				  << unicode::iconvert::convert(s.get_string(),
								unicode::utf_8)
				  << "\" instead of \""
				  << unicode::iconvert::convert(expected,
								unicode::utf_8)
				  << "\"";

				throw EXCEPTION(o.str());
			}
		}
	}
}

void testrichtext13(ONLY IN_THREAD)
{
	auto richtext=
		richtext::create(richtextstring
				 {std::u32string{U"שלום\n"},
				  {
					  {0, {0}}
				  }
				 },
				 richtext_options{});
	auto last=richtext->begin();

	if (last->pos() != 0)
		throw EXCEPTION("testrichtext13: unexpected begin() position");

	auto cursor=last->clone();
	cursor->left(IN_THREAD);

	if (cursor->pos() != 1)
		throw EXCEPTION("testrichtext13: unexpected cursor position");

	auto delete_to=cursor->clone();
	delete_to->move_for_delete(IN_THREAD);

	if (delete_to->pos() != 2)
		throw EXCEPTION("testrichtext13: unexpected delete position");

	cursor->remove(IN_THREAD, delete_to);

	if (last->pos() != 0)
		throw EXCEPTION("testrichtext13: unexpected last position");
}

void testrichtext14(ONLY IN_THREAD)
{

	static constexpr richtextmeta metalr{0, 0};

	static const struct {
		std::u32string test_string;
		dim_t wrap_width;

		std::vector<std::tuple<
				    // position selected
				    size_t,
				    // start
				    size_t,
				    // end
				    size_t
				    >> tests;
	} tests[]={
		{
			U"Rolem  Ipsum Dolor. Sit Amet\n",
			140,
			{
				{0, 0, 5},
				{4, 0, 5},
				{5, 5, 7},
				{6, 5, 7},
				{7, 7, 12},
				{11, 7, 12},
				{12, 12, 12},

				{13, 13, 18},
				{17, 13, 18},
				{18, 18, 19},
				{19, 19, 20},

				{20, 20, 23},

				{23, 23, 24},

				{24, 24, 28},

				{28, 28, 28},
			}
		},
		{
			std::u32string{RLM} + std::u32string{RLO} +
			U"Rolem  Ipsum Dolor. Sit Amet\n",
			140,
			{
				{0, 0, 5},
				{4, 0, 5},
				{5, 5, 7},
				{6, 5, 7},
				{7, 7, 12},
				{11, 7, 12},
				{12, 12, 12},

				{13, 13, 18},
				{17, 13, 18},
				{18, 18, 19},
				{19, 19, 20},

				{20, 20, 23},

				{23, 23, 24},

				{24, 24, 28},

				{28, 28, 28},
			}
		},
		{
			U"XYZ",
			140,
			{
				{0, 0, 2},
				{1, 0, 2},
				{2, 0, 2},
			},
		},
		{
			U"Helloשלום\n",
			140,
			{
				{0, 0, 5},
				{4, 0, 5},
				{5, 5, 9},
				{8, 5, 9},
			},
		},
		{
			U"Hello" + std::u32string{RLO} + U"World\n",
			140,
			{
				{0, 0, 5},
				{4, 0, 5},
				{5, 5, 10},
				{9, 5, 10},
			},
		}
	};

	size_t testnum=0;

	for (const auto &t:tests)
	{
		++testnum;

		// Right to left tests.
		richtext_options options;
		options.unprintable_char=' ';
		options.initial_width=t.wrap_width;
		auto richtext=richtext::create(richtextstring{
				t.test_string,
				{
					{0, metalr},
				},

			}, options);

		size_t casenum=0;
		for (const auto &[pos, begin, end]:t.tests)
		{
			++casenum;

			const auto &[begin_iter, end_iter]=
				richtext->begin()->pos(pos)->select_word();
			if (begin_iter->pos() != begin)
				throw EXCEPTION("testrichtext14: test "
						<< testnum
						<< ", case "
						<< casenum
						<< ": begin position: expected "
						<< begin
						<< ": got "
						<< begin_iter->pos());
			if (end_iter->pos() != end)
				throw EXCEPTION("testrichtext14: test "
						<< testnum
						<< ", case "
						<< casenum
						<< ": end position: expected "
						<< end
						<< ": got "
						<< end_iter->pos());
		}
	}
}

int main(int argc, char **argv)
{
	try {
		LIBCXX_NAMESPACE::property
			::load_property(LIBCXX_NAMESPACE_STR "::themes",
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

		auto IN_THREAD=connection_thread::create();

		testrichtext1(IN_THREAD);
		testrichtext2(IN_THREAD);
		testrichtext3(IN_THREAD);
		testrichtext4(IN_THREAD);
		testrichtext5(IN_THREAD);
		testrichtext6(IN_THREAD);
		testrichtext7(IN_THREAD);
		testrichtext8(IN_THREAD);
		testrichtext9(IN_THREAD);
		testrichtext10(IN_THREAD);
		testrichtext11(IN_THREAD);
		testrichtext12(IN_THREAD);
		testrichtext13(IN_THREAD);
		testrichtext14(IN_THREAD);
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return 0;
}
