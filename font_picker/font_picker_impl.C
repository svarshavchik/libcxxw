/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "font_picker/font_picker_impl.H"
#include "font_picker/font_picker_preview.H"
#include "fonts/fontconfig.H"
#include "fonts/fontlist.H"
#include "fonts/fontpattern.H"
#include "popup/popup.H"
#include "image_button_internal.H"
#include "textlabel.H"
#include "screen.H"
#include "defaulttheme.H"
#include "textlabel.H"
#include "messages.H"
#include "busy.H"
#include "catch_exceptions.H"
#include "x/w/font_picker_config.H"
#include "x/w/font_picker_appearance.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/standard_comboboxlayoutmanager.H"
#include "x/w/editable_comboboxlayoutmanager.H"
#include "x/w/theme_color.H"
#include "x/w/impl/element.H"
#include <courier-unicode.h>
#include <fontconfig/fontconfig.h>
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <set>
#include <unordered_set>
#include <cmath>
#include <charconv>

LIBCXXW_NAMESPACE_START

namespace {
#if 0
}
#endif

struct LIBCXX_HIDDEN font_picker_group_id_hash
	: public std::hash<std::string> {

	using std::hash<std::string>::operator();

	inline size_t operator()(const font_picker_group_id &id) const
	{
		return (*this)(id.family)+(*this)(id.foundry);
	}
};

#if 0
{
#endif
}

const font_picker_int_option_info font_picker_int_option_infos[3]={
	{
		&font::weight,
		&font::standard_weights,
		&font::set_weight,

		&font_picker_popup_fields::font_weight,
		FC_WEIGHT,
		_txt("-- Select font weight --"),
	},
	{
		&font::slant,
		&font::standard_slants,
		&font::set_slant,

		&font_picker_popup_fields::font_slant,
		FC_SLANT,
		_txt("-- Select font slant --"),
	},
	{
		&font::width,
		&font::standard_widths,
		&font::set_width,

		&font_picker_popup_fields::font_width,
		FC_WIDTH,
		_txt("-- Select font width -- "),
	},
#if 0

	// This appears to have no effect
	{
		&font::spacing,
		&font::standard_spacings,
		&font::set_spacing,

		&font_picker_popup_fields::font_spacing,
		FC_SPACING,
		_txt("-- Select font spacing -- "),
	},
#endif
};

// Construct the initial value for fontlist.

static inline auto create_families(const font_picker_config &config)
{
	// Ask fontconfig to return the list of all font patterns, then
	// bucket them by font_picker_group_id

	std::unordered_map<font_picker_group_id,
			   font_family_group_entry,
			   font_picker_group_id_hash> all_fonts;

	auto list=fontconfig::config::create()->create_list();

	for (auto b=list->begin(), e=list->end(); b != e; ++b)
	{
		font_picker_group_id id;

		if (!(*b)->get_string(FC_FAMILY, id.family))
			continue;

		if (config.select_by_foundry)
		{
			if (!(*b)->get_string(FC_FOUNDRY, id.foundry))
				id.foundry.clear();
		}

		auto iter=all_fonts.find(id);

		if (iter == all_fonts.end())
		{
			iter=all_fonts.insert({id, font_family_group_entry
						::create(id)}).first;
		}
		iter->second->patterns.push_back(*b);
	}

	// Now put all entries in a vector.

	std::vector<font_family_group_entry> v;

	v.reserve(all_fonts.size());

	for (const auto &f:all_fonts)
	{
		f.second->patterns.shrink_to_fit();
		v.push_back(f.second);
	}

	// Sort the vector. Sort by font their font group IDs.

	std::sort(v.begin(), v.end(),
		  []
		  (const auto &a, const auto &b)
		  {
			  return a->id < b->id;
		  });

	// If we have multiple font names, but from different foundries,
	// update their name with the foundry's suffix.

	if (!v.empty())
	{
		for (auto b=v.begin(), p=b++; b!=v.end(); )
		{
			if ((*p)->name != (*b)->name)
			{
				p=b;
				++b;
				continue;
			}

			auto n=(*p)->name;

			(*p)->name=(*p)->name + " (" + (*p)->id.foundry + ")";
			(*p)->multiple_foundries=true;

			while (b != v.end() && (*b)->name == n)
			{
				(*b)->multiple_foundries=true;
				(*b)->name=(*b)->name +
					" (" + (*b)->id.foundry + ")";
				p=b;
				++b;
			}
		}
	}

	return std::vector<const_font_family_group_entry>{v.begin(), v.end()};
}

static std::optional<size_t>
search_font_family(const std::vector<const_font_family_group_entryptr> &list,
		   const font &f)
{
	std::optional<size_t> found_font_family;

	// Find our font. Ideally the foundry and font family
	// will match. If not, we'll take the first matching
	// font family.

	auto initial_font_family=unicode::tolower(f.family);
	auto initial_font_foundry=unicode::tolower(f.foundry);

	for (size_t i=0, n=list.size(); i<n; ++i)
	{
		auto group=list.at(i);

		if (!group)
			continue;

		if (unicode::tolower(group->id.family) != initial_font_family)
			continue;

		if (!initial_font_foundry.empty() &&
		    initial_font_foundry==unicode::tolower(group->id.foundry))
		{
			found_font_family=i;
			break;
		}
		if (!found_font_family)
			found_font_family=i;
	}

	return found_font_family;
}

static size_t search_font_family_with_default
(const std::vector<const_font_family_group_entryptr> &list,
 const font &f)
{
	auto found_font_family=search_font_family(list, f);

	// We asked for some font that does not exist.

	// Find the system default font.
	if (!found_font_family)
		found_font_family=
			search_font_family(list, {});

	// If not found, take the first foundry we see.
	size_t i=0;

	if (found_font_family)
	{
		i=*found_font_family;
	}
	else
	{
		while (i<list.size() && !list.at(i))
			++i;
	}

	return i;
}

// After created_families(), compute the initial list of sorted families.
//
// Called from the constructor, also passing in the font family combo-box's
// layout manager. Calls compute_family_combobox_items(), uses its returned
// list of sorted families, places the matching list_item_params as the
// initial contents of the combo-box, and selects the first list item.

static inline std::vector<const_font_family_group_entryptr>
initial_sorted_families(const std::vector<const_font_family_group_entry>
			&families,
			const std::vector<font_picker_group_id>
			&most_recently_used,
			std::vector<font_picker_group_id>
			&validated_most_recently_used,
			const standard_comboboxlayoutmanager &lm,
			const label &current_font_shown,
			font &initial_font,
			const font_picker_config &config,
			const std::optional<font> &conf_initial_font,
			font_pickerObj::implObj::official_values_t
			&official_values)
{
	std::optional<size_t> dummy;
	std::vector<list_item_param> items;

	auto list=font_pickerObj::implObj
		::compute_font_family_combobox_items
		(config.selection_required,
		 families,
		 most_recently_used,
		 validated_most_recently_used,
		 items,
		 {}, dummy);
	std::cout << "=========\n";

	lm->replace_all_items(items);
	if (!items.empty())
	{
		size_t i=0;

		if (config.selection_required || conf_initial_font)
		{
			i=search_font_family_with_default(list, initial_font);

			// In all cases, make sure that initial_font's family
			// and foundry matches the initial combo-box's value.

			if (i < list.size())
			{
				auto group=list.at(i);
				lm->autoselect(i);
				initial_font.family=group->id.family;
				initial_font.foundry=group->id.foundry;
			}
			else
			{
				i=0;
			}
		}
		else
		{
			// The first element is "Please select font"
			i=0;
			lm->autoselect(0);
		}
		auto &string=std::get<text_param>(items.at(i));

		official_values.official_font=initial_font;
		official_values.official_font_label=string;
		official_values.saved_font_group=list.at(i);
		current_font_shown->label_impl->update(string, {});
	}

	if (conf_initial_font || config.selection_required)
		// The input field will be set in create_font_picker()
		official_values.saved_font_size=initial_font.point_size;
	return list;
}

static inline void update_option_combobox
(const standard_comboboxlayoutmanager &lm,
 const ref<screenObj::implObj> &screen,
 const function<font_pickerObj::implObj::
 update_font_properties_t> &set_func,
 const font::values_t &options,
 const const_font_family_group_entryptr
 &current_group,
 unsigned current_point_size,
 const char *property_name,
 const const_font_picker_appearance &appearance,
 const std::string &initial_option_label)
{
	// If a font family and point size is specified, examine the
	// current_group. Check each property_name in the current_group.
	// If some value in the options list is not found in any of the
	// current_group's patterns, show it used "unavailable" color.

	std::unordered_set<int> valid_options;

	if (!current_group)
	{
		// No group, draw all possible options as valid.

		for (const auto &option:options)
			valid_options.insert(option.value);
	}
	else
	{
		// possible_options is a sorted list of all known option values.
		std::vector<int> possible_options;

		possible_options.reserve(options.size());

		for (const auto &option:options)
			possible_options.push_back(option.value);

		// Check each pattern.
		for (const auto &pattern:current_group->patterns)
		{
			// If current_point_size is specified, it must match
			// the pattern's point size. A pattern point size of
			// 0 is scalable, and matches anything.

			auto s=pattern->get_point_size(screen);

			if (current_point_size > 0 && s > 0 &&
			    s != current_point_size)
				continue;

			int value;
			auto b=possible_options.begin(),
				e=possible_options.end();

			for (size_t index=0;
			     pattern->get_integer(property_name, value, index);
			     ++index)
			{
				// Handle the possibility that a pattern
				// specifies a value that's not in our options.

				if (!possible_options.empty())
				{
					// Use upper_bound to find the /next/
					// higher option value.

					auto p=std::upper_bound(b, e, value);

					// Decrement p, to the previous value
					// if: p is end(), the given value
					// is higher than the highest option
					// value (pick the last one),
					// or the given value is <= the
					// midpoint between the higher option
					// value and the lower one.

					if (p == e ||
					    (p>b &&
					     value <= (p[0]-p[-1])/2+p[-1]))
						--p;

					valid_options.insert(*p);
				}
			}
		}
	}

	std::vector<list_item_param> item_options;

	item_options.reserve(options.size()+1);

	item_options.emplace_back(initial_option_label);

	for (const auto &opt:options)
	{
		text_param option;

		if (valid_options.find(opt.value) == valid_options.end())
			option(appearance->unsupported_option_color);
		option(opt.description);

		item_options.emplace_back(option);
	}

	auto n=lm->selected();

	// Initially the combo-box is empty, we'll autoselect the first one.
	set_func(lm, item_options, n ? *n:0);
}

// Reset the font option combo-box to a particular value.
//
// We have a value of some option in the font object, and the list of options
// in the combo-box. Find the value's index and set the combo-box's currently
// selected item to that index.

static int reset_option_combobox(const standard_comboboxlayoutmanager &lm,
				 const function<void(const listlayoutmanager &,
						     size_t)> &set_func,
				 int value,
				 const font::values_t &options)
{
	if (value >= 0)
	{
		std::vector<int> values;

		values.reserve(options.size());

		for (const auto &opt:options)
			values.push_back(opt.value);

		if (!values.empty())
		{
			size_t n=std::lower_bound(values.begin(), values.end(),
						  value)
				- values.begin();

			if (n==values.size())
				--n;

			set_func(lm, n+1); // #0 is "Select" option label;
			return values.at(n);
		}
	}

	set_func(lm, 0); // The "select" option label.
	return -1;
}

font_picker_impl_init_params::
font_picker_impl_init_params(
	const image_button_internal &popup_button,
	const label &current_font_shown,
	const font_picker_popup_fieldsptr &popup_fields,
	const font_picker_config &config,
	const std::optional<font> &conf_initial_font,
	const std::vector<font_picker_group_id> &conf_most_recently_used,
	const font_pickerObj::implObj::current_state &initial_state)
	: popup_button{popup_button},
	  current_font_shown{current_font_shown},
	  popup_fields{popup_fields},
	  font_family_lm{popup_fields.font_family->get_layoutmanager()},
	  config{config},
	  conf_initial_font{conf_initial_font},
	  initial_state{initial_state},

	  initial_font{conf_initial_font ?
			  *conf_initial_font:font{}},
	  families{create_families(config)}
{
	// Some edit checking

	if (initial_font.point_size < 0)
		initial_font.point_size=0;

	for (const auto &f:font_picker_int_option_infos)
	{
		if (initial_font.*(f.font_field) < 0)
			continue;

		auto valid_values=(f.valid_values)();

		if (valid_values.empty())
			throw EXCEPTION(_("Internal error, empty value list"));

		std::vector<int> sorted_values;

		sorted_values.reserve(valid_values.size());

		for (const auto valid_value:valid_values)
			sorted_values.push_back(valid_value.value);

		auto b=sorted_values.begin();
		auto e=sorted_values.end();

		auto p=std::upper_bound(b, e, initial_font.*(f.font_field));

		// Decrement p, to the previous value
		// if: p is end(), the given value
		// is higher than the highest option
		// value (pick the last one),
		// or the given value is <= the
		// midpoint between the higher option
		// value and the lower one.

		if (p == e || (p>b && initial_font.*(f.font_field)
			       <= (p[0]-p[-1])/2+p[-1]))
			--p;

		initial_font.*(f.font_field)=*p;
	}

	mpobj<std::vector<font_picker_group_id>>::lock mru_lock{
		initial_state->validated_most_recently_used
			};

	font_pickerObj::implObj::official_font_t::lock font_lock{
		initial_state->official_font
			};

	sorted_families=initial_sorted_families
		(families,
		 conf_most_recently_used,
		 *mru_lock,
		 font_family_lm,
		 current_font_shown,
		 // Update the initial
		 // current_font based on the
		 // actual available fonts.
		 //
		 // If the initial_font does
		 // not exist, we will correct
		 // it here.
		 initial_font,
		 config,
		 conf_initial_font,
		 // initial_sorted_families()
		 // populates this.
		 *font_lock);
}

font_pickerObj::implObj::implObj(const font_picker_impl_init_params
				 &init_params)
	: families{init_params.families},
	  popup_button{init_params.popup_button},
	  popup_fields{init_params.popup_fields},
	  selection_required{init_params.config.selection_required},
	  foundry_required{init_params.config.foundry_required},
	  current_font_thread_only{init_params.initial_font},
	  callback_thread_only{init_params.config.callback},
	  appearance{init_params.config.appearance},
	  sorted_families{init_params.sorted_families},

	  state{init_params.initial_state},

	  current_font_shown{init_params.current_font_shown}
{
	// Initialize the placeholder font family name label element.
	update_font_properties(init_params.initial_state->official_font.get()
			       .saved_font_group,
			       (init_params.conf_initial_font ||
				init_params.config.selection_required)
			       ? init_params.initial_font.point_size:0,
			       make_function<update_font_properties_t>
			       ([]
				(const listlayoutmanager &lm,
				 const std::vector<list_item_param> &items,
				 const std::optional<size_t> &n)
				{
					lm->replace_all_items(items);
					if (n)
						lm->autoselect(*n);
				}));

	if (init_params.conf_initial_font ||
	    init_params.config.selection_required)
	{
		reset_font_options(current_font_thread_only,
				   make_function<void(const listlayoutmanager &,
						      size_t)>
				   ([]
				    (const auto &lm, size_t n)
				    {
					    lm->autoselect(n);
				    }));
	}
}

font_pickerObj::implObj::~implObj()=default;

void font_pickerObj::implObj
::finish_initialization(const font_picker &fp)
{
	public_object=fp;

	current_font_shown->in_thread
		([impl=ref(this)]
		 (ONLY IN_THREAD)
		 {
			 auto official_font_value=
				 impl->state->official_font.get();

			 impl->invoke_callback(IN_THREAD, official_font_value,
					       initial{});
		  });
}

namespace {

	// For quick lookup we store pointers to most_recently_used
	// fonts. This is the comparator class for the pointer map.
	//
	// We store the pointers to most_recently_used. Then, we check
	// each family's group id, and search the map for the same pointer,
	// allegedly.
	//
	// Except that we implement the comparator operator by comparing the
	// dereferenced group ids. Hence, in this manner we locate the
	// group ids that are most_recently_used.

	struct compare_font_picker_group_id_ptrs {

		// Comparison operator.
		bool operator()(const font_picker_group_id *a,
				const font_picker_group_id *b) const
		{
			return *a < *b;
		}
	};
}

void font_pickerObj::implObj
::most_recently_used(ONLY IN_THREAD,
		     const std::vector<font_picker_group_id> &mru)
{
	standard_comboboxlayoutmanager lm=
		popup_fields.font_family->get_layoutmanager();

	// Obtain the currently selected font family.

	auto n=lm->selected();

	const_font_family_group_entryptr e;

	if (n)
		e=sorted_families.at(*n);

	// Recompute the contents of the combo-box.
	std::vector<list_item_param> items;

	{
		mpobj<std::vector<font_picker_group_id>>
			::lock validated_most_recently_used_lock{
			state->validated_most_recently_used
				};

		auto new_sorted_families=compute_font_family_combobox_items
			(selection_required,
			 families,
			 mru,
			 *validated_most_recently_used_lock,
			 items,
			 e,
			 n);

		if (new_sorted_families==sorted_families)
			return; // Unchanged.

		sorted_families=new_sorted_families;
	}

	lm->replace_all_items(IN_THREAD, items);

	// Re-select the font family that was selected originally.

	if (n)
		lm->autoselect(IN_THREAD, *n, {});
}

std::vector<const_font_family_group_entryptr> font_pickerObj::implObj
::compute_font_family_combobox_items
(bool selection_required,
 const std::vector<const_font_family_group_entry> &families, // Sorted by ID.
 const std::vector<font_picker_group_id> &most_recently_used,
 std::vector<font_picker_group_id> &validated_most_recently_used,
 std::vector<list_item_param> &items,
 const const_font_family_group_entryptr &current_family_sel,
 std::optional<size_t> &n)
{
	n=std::nullopt;
	items.clear();
	std::vector<const_font_family_group_entryptr> sorted_families;

	items.reserve(families.size()+2);
	sorted_families.reserve(families.size()+2);
	validated_most_recently_used.clear();
	validated_most_recently_used.reserve(most_recently_used.size());

	if (!selection_required)
	{
		items.emplace_back(_("-- Select a font family --"));
		sorted_families.emplace_back(nullptr);
	}

	std::map<const font_picker_group_id *, size_t,
		 compare_font_picker_group_id_ptrs> lookup_mru;

	{
		auto b=most_recently_used.begin();

		for (size_t i=0, n=most_recently_used.size(); i<n; ++i)
			lookup_mru.insert({&b[i], i});
	}

	// Now, iterate over families, and drop them into mru_families
	// and all_other_families

	std::map<size_t, const_font_family_group_entry> mru_families;
	std::vector<const_font_family_group_entry> all_other_families;

	all_other_families.reserve(families.size());

	for (const auto &f:families)
	{
		auto iter=lookup_mru.find(&f->id);

		if (iter != lookup_mru.end())
			mru_families.insert({iter->second, f});
		else
			all_other_families.push_back(f);
	}

	// Decision time: if we have any MRU families, insert them into
	// sorted_families first, followed by a separator line.

	if (!mru_families.empty())
	{
		for (const auto &f:mru_families)
		{
			items.emplace_back(f.second->name);
			sorted_families.emplace_back(f.second);
			validated_most_recently_used.push_back(f.second->id);

			if (current_family_sel && current_family_sel==f.second)
				n=items.size()-1;
		}

		items.emplace_back(separator{});
		sorted_families.emplace_back(nullptr);
	}

	// Now insert all other families.
	for (const auto &f:all_other_families)
	{
		items.emplace_back(f->name);
		sorted_families.emplace_back(f);

		if (current_family_sel && current_family_sel == f)
			n=items.size()-1;
	}

	return sorted_families;
}

void font_pickerObj::implObj
::update_font_family(ONLY IN_THREAD,
		     const listlayoutmanager &lm)
{
	update_font_properties(IN_THREAD, lm);

	// If no selection, update current_font with the default font.
	font_picker_group_id default_font{ font{} };

	auto n=lm->selected();

	if (n)
	{
		const auto &selected_family=sorted_families.at(*n);

		// If there's a selection, replace default_font with what's
		// selected.
		if (selected_family)
			default_font=selected_family->id;
	}

	// Now, update current_font.
	default_font.update(current_font(IN_THREAD));

	compute_new_preview(IN_THREAD);
}

void font_pickerObj::implObj::update_font_size(ONLY IN_THREAD,
					       unsigned new_point_size)
{
	save_font_size(IN_THREAD, new_point_size);
	update_font_options(IN_THREAD);
	compute_new_preview(IN_THREAD);
}

void font_pickerObj::implObj
::updated_font_option(ONLY IN_THREAD,
		      const font_picker_int_option_info &option,
		      const standard_comboboxlayoutmanager &lm)
{
	updated_font_option(IN_THREAD, lm,
			    option.valid_values,
			    option.set_value);
	compute_new_preview(IN_THREAD);
}

void font_pickerObj::implObj
::updated_font_option(ONLY IN_THREAD,
		      const standard_comboboxlayoutmanager &lm,
		      font::values_t (*get_standard_values)(),
		      font &(font::*set_function)(int))
{
	auto selected_n=lm->selected();

	// Expect the options are always selected, but...

	if (selected_n)
	{
		auto n=*selected_n;

		if (n > 0) // Not the default one.
		{
			auto values=get_standard_values();

			for (const auto &value:values)
			{
				if (--n == 0)
				{
					(current_font(IN_THREAD).*set_function)
						(value.value);
					return;
				}
			}
		}
	}
	(current_font(IN_THREAD).*set_function)(-1);
}

void font_pickerObj::implObj::save_font_size(ONLY IN_THREAD,
					     double new_point_size)
{
	if (new_point_size == 0) // Default size
	{
		font default_font;

		new_point_size=default_font.point_size;
	}

	current_font(IN_THREAD).point_size=new_point_size;
}

void font_pickerObj::implObj
::update_font_properties(ONLY IN_THREAD,
			 const listlayoutmanager &lm)
{
	const_font_family_group_entryptr e;

	auto n=lm->selected();

	if (n)
		e=sorted_families.at(*n);

	unsigned current_point_size=0;

	auto v=popup_fields.font_size_validated->value();

	if (v)
		current_point_size=*v;

	update_font_properties(e, current_point_size,
			       make_function<update_font_properties_t>
			       ([&]
				(const listlayoutmanager &lm,
				 const std::vector<list_item_param> &items,
				 const std::optional<size_t> &n)
				{
					lm->replace_all_items(IN_THREAD,
							      items);
					if (n)
						lm->autoselect(IN_THREAD, *n,
							       {});
				}));
}

void font_pickerObj::implObj
::update_font_properties(const const_font_family_group_entryptr &current_group,
			 unsigned current_point_size,
			 const function<update_font_properties_t> &set_func)
{
	std::set<unsigned> point_sizes;

	if (current_group)
	{
		auto screen=popup_fields.font_family->elementObj::impl
			->get_screen()->impl;

		for (const auto &pattern:current_group->patterns)
		{
			auto v=pattern->get_point_size(screen);

			if (v)
				point_sizes.insert(v);
		}
	}

	if (point_sizes.empty())
	{
		auto s=font::standard_point_sizes();

		point_sizes.insert(s.begin(), s.end());
	}

	std::vector<list_item_param> items;

	items.reserve(point_sizes.size());

	for (auto point_size:point_sizes)
	{
		char buffer[8];

		auto ret=std::to_chars(buffer, buffer+sizeof(buffer)-1,
				       point_size);

		if (ret.ec != std::errc{}) // Shouldn't happen
			ret.ptr=buffer;
		*ret.ptr=0;

		items.push_back(buffer);
	}

	set_func(popup_fields.font_size->get_layoutmanager(), items, {});
	update_font_options(set_func, current_group, current_point_size);
}

void font_pickerObj::implObj::update_font_options(ONLY IN_THREAD)
{
	update_font_options(make_function<update_font_properties_t>
			    ([&]
			     (const listlayoutmanager &lm,
			      const std::vector<list_item_param> &items,
			      const std::optional<size_t> &n)
			     {
				     lm->replace_all_items(IN_THREAD,
							   items);
				     if (n)
					     lm->autoselect(IN_THREAD, *n,
							    {});
			     }));
}

void font_pickerObj::implObj
::update_font_options(const function<update_font_properties_t> &set_func)
{
	const_font_family_group_entryptr e;

	{
		listlayoutmanager lm=
			popup_fields.font_family->get_layoutmanager();

		auto n=lm->selected();

		if (n)
			e=sorted_families.at(*n);
	}

	unsigned current_point_size=0;

	auto v=popup_fields.font_size_validated->value();

	if (v)
		current_point_size=*v;

	update_font_options(set_func, e, current_point_size);
}

void font_pickerObj::implObj
::update_font_options(const function<update_font_properties_t> &set_func,
		      const const_font_family_group_entryptr &current_group,
		      unsigned current_point_size)
{
	auto screen=popup_fields.font_family->get_screen()->impl;

	// Update various font options.

	// When invoked from the constructor, the options' combo-boxes will
	// have no options, we'll autoselect the first, empty, icon.

	for (const auto &f:font_picker_int_option_infos)
		update_option_combobox((popup_fields.*(f.value_combobox))
				       ->get_layoutmanager(),
				       screen,
				       set_func,
				       f.valid_values(),
				       current_group,
				       current_point_size,
				       f.fontconfig_name,
				       appearance,
				       cxxwlibmsg(f.select_prompt));
}

void font_pickerObj::implObj
::reset_font_options(const font &font_spec,
		     const function<void (const listlayoutmanager &,
					  size_t)> &set_func)
{
	// Reset various font options to what they should be for the
	// given font.

	for (const auto &f:font_picker_int_option_infos)
		reset_option_combobox((popup_fields.*(f.value_combobox))
				      ->get_layoutmanager(),
				      set_func,
				      font_spec.*(f.font_field),
				      (f.valid_values)());
}


void font_pickerObj::implObj
::compute_new_preview(ONLY IN_THREAD)
{
	popup_fields.preview_label->update_preview(IN_THREAD,
						   current_font(IN_THREAD));
}

void font_pickerObj::implObj::set_font(ONLY IN_THREAD,
				       const font &f)
{
	official_values_t new_values=state->official_font.get();

	// Set the official font, but make sure that the point size is not
	// crazy.

	new_values.official_font=f;

	if (f.point_size < 0)
		new_values.official_font.point_size=0;

	if (f.point_size > 999)
		new_values.official_font.point_size=999;
	new_values.official_font.point_size=
		std::round(new_values.official_font.point_size);

	new_values.saved_font_size=new_values.official_font.point_size;

	// Need to find the font family group, first.
	auto i=search_font_family_with_default(sorted_families, f);

	if (i < sorted_families.size())
	{
		auto group=sorted_families.at(i);

		// Make sure the "official font" is synced up with the
		// font group id.
		new_values.saved_font_group=group;
		group->id.update(new_values.official_font);

		// And set the combo-box.
		standard_comboboxlayoutmanager lm=
			popup_fields.font_family->get_layoutmanager();
		lm->autoselect(IN_THREAD, i, {});
	}

	// Now, pretend that the "Save" button has been pressed.
	popup_fields.font_size_validated->set(new_values.saved_font_size);

	// We now need to validate all other int options, and update their
	// corresponding combo-box values.
	for (const auto &f:font_picker_int_option_infos)
	{
		auto v=reset_option_combobox
			((popup_fields.*(f.value_combobox))
			 ->get_layoutmanager(),
			 make_function<void (const listlayoutmanager &,
					     size_t)>
			 ([&]
			  (const auto &lm, size_t n)
			{
				lm->autoselect(IN_THREAD, n, {});
			}),
			 new_values.official_font.*(f.font_field),
			 (f.valid_values)());

		(new_values.official_font.*(f.set_value))(v);
	}

	// This is now good to go.
	state->official_font=new_values;
	current_font(IN_THREAD)=new_values.official_font;

	set_official_font(IN_THREAD, {});

	// But then reset the stuff in the combo-box to match the "official"
	// font.
	popup_closed(IN_THREAD);
}

/////////////////////////////////////////////////////////////////////////////

void font_pickerObj::implObj::popup_closed(ONLY IN_THREAD)
{
	// Put everything back where we found it.

	auto official_font_value=state->official_font.get();

	// There are a few more details to do, besides resetting current_font
	current_font(IN_THREAD)=official_font_value.official_font;

	popup_fields.font_size_validated->set(
		official_font_value.saved_font_size
	);
	// update_font_size() does not get called by set(). That's wonderful,
	// because we only need to call save_font_size(), compute_new_preview()
	// gets called as part of update_font_family().
	save_font_size(IN_THREAD, official_font_value.saved_font_size);

	size_t i=0;

	if (official_font_value.saved_font_group)
	{
		auto e=sorted_families.end();
		auto b=sorted_families.begin();

		auto iter=std::find(b, e, official_font_value.saved_font_group);

		if (iter != e)
		{
			i=iter-b;
		}
	}

	// Need to update the options, we must do it before
	// we call update_font_family(), which calls update_font_options().

	reset_font_options(current_font(IN_THREAD),
			   make_function<void (const listlayoutmanager &,
					       size_t)>
			   ([&]
			    (const auto &lm,
			     size_t n)
			    {
				    lm->autoselect(IN_THREAD, n, {});
			    }));


	{
		standard_comboboxlayoutmanager lm=
			popup_fields.font_family->get_layoutmanager();

		lm->autoselect(IN_THREAD, i, initial{});

		// update_font_family() does not get called automatically
		// by autoselect(), we have to do it.
		update_font_family(IN_THREAD, lm);
	}
}

void font_pickerObj::implObj::set_official_font(ONLY IN_THREAD,
						const callback_trigger_t
						&trigger)
{
	{
		official_font_t::lock lock{state->official_font};

		lock->official_font=current_font(IN_THREAD);

		lock->saved_font_group=nullptr;

		standard_comboboxlayoutmanager lm=
			popup_fields.font_family->get_layoutmanager();

		auto n=lm->selected();

		if (n)
		{
			lock->official_font_label=lm->item(*n);
			lock->saved_font_group=sorted_families.at(*n);
		}
		auto v=popup_fields.font_size_validated->value();

		lock->saved_font_size=v ? *v:0;
	}

	auto official_font_value=state->official_font.get();

	current_font_shown->label_impl
		->update(IN_THREAD,
			 official_font_value.official_font_label, {});

	invoke_callback(IN_THREAD, official_font_value, trigger);
}

void font_pickerObj::implObj
::invoke_callback(ONLY IN_THREAD, official_values_t &official_values,
		  const callback_trigger_t &trigger)
{
	if (!callback(IN_THREAD))
		return;

	adjust_font_for_callback(official_values);

	auto e_impl=current_font_shown->elementObj::impl;

	auto fp=public_object.getptr();

	if (!fp)
		return;

	const font_picker_group_id *new_font_group=nullptr;

	if (official_values.saved_font_group)
		new_font_group=&official_values.saved_font_group->id;

	try {
		callback(IN_THREAD)
			(IN_THREAD,
			 official_values.official_font,
			 new_font_group,
			 fp,
			 trigger,
			 busy_impl{*e_impl});
	} REPORT_EXCEPTIONS(e_impl);
}

void font_pickerObj::implObj
::adjust_font_for_callback(official_values_t &official_values)
	const
{
	if (foundry_required)
		return;

	if (!official_values.saved_font_group)
		return;

	if (!official_values.saved_font_group->multiple_foundries)
		official_values.official_font.foundry="";
}

LIBCXXW_NAMESPACE_END
