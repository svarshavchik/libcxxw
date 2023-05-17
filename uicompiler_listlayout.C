/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "uicompiler_list_items.H"
#include "messages.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/uigenerators.H"
#include "x/w/uielements.H"
#include <x/xml/xpath.H>

LIBCXXW_NAMESPACE_START

void uicompiler::list_items_parse_info_t::name(const std::string &name) const
{
	labels->push_back(name);
}

void uicompiler::list_items_parse_info_t::label(const text_param &label) const
{
	params->emplace_back(
		[label]
		(std::vector<list_item_param> &params,
		 uielements &elements)
		{
			params.emplace_back(label);
		});
}

void uicompiler::list_items_parse_info_t::image(const std::string &name) const
{
	params->emplace_back
		([image=image_param{name}]
		 (std::vector<list_item_param> &params,
		  uielements &elements)
		{
			params.emplace_back(image);
		});
}

void uicompiler::list_items_parse_info_t::add_separator() const
{
	params->emplace_back
		([]
		 (std::vector<list_item_param> &params,
		  uielements &elements)
		{
			params.emplace_back(separator{});
		});
}

void uicompiler::list_items_parse_info_t::add_shortcut(const shortcut &sc) const
{
	params->emplace_back([sc]
			     (std::vector<list_item_param> &params,
			      uielements &elements)
	{
		params.emplace_back(sc);
	});
}

void uicompiler::list_items_parse_info_t::add_inactive_shortcut(
	const shortcut &sc
) const
{
	params->emplace_back([sc=inactive_shortcut{sc}]
			     (std::vector<list_item_param> &params,
			      uielements &elements)
	{
		params.emplace_back(sc);
	});
}

void uicompiler::list_items_parse_info_t::add_hierindent(size_t n) const
{
	params->emplace_back([n]
			     (std::vector<list_item_param> &params,
			      uielements &elements)
	{
		params.emplace_back(hierindent{n});
	});
}

void uicompiler::list_items_parse_info_t::add_menuoption(
	const std::string &groupname
) const
{
	params->emplace_back([groupname]
			     (std::vector<list_item_param> &params,
			      uielements &elements)
	{
		params.emplace_back(menuoption{groupname});
	});
}

void uicompiler::list_items_parse_info_t::add_selected() const
{
	params->emplace_back([]
			     (std::vector<list_item_param> &params,
			      uielements &elements)
	{
		params.emplace_back(selected{});
	});
}

void uicompiler::list_items_parse_info_t::add_submenu(
	const listlayoutmanager_generator &gens
) const
{
	// We're going to compile this closure and stash
	// it in params, which gets returned as list_item_params
	//
	// The list_item_params gets captured, by value
	// into the final compiled generator for this list and
	// captured, by value into a closure.
	//
	// The closure calls create(), passing in uielements.
	//
	// create() returns the vector of list_items,
	// which gets passed to the list layout manager method
	// that creates new items.
	//
	// That method is going to process all the list items,
	// and invoke the submenu{} closure, which captured
	// the uielements object, and uses it to generate
	// the new submenu contents.
	//
	// The uielements is guaranteed not to go out of scope
	// until the list layout manager method that creates
	// new items gets returned.

	params->emplace_back([gens]
			     (std::vector<list_item_param> &params,
			      uielements &elements)
	{
		params.emplace_back(
			submenu{
				[gens, &elements]
					(const auto &lm)
				{
					gens(lm, elements);
				}});
	});
}

void uicompiler::list_items_parse_info_t::add_status_change(
	const std::string &name
) const
{
	params->emplace_back(
		[name]
		(std::vector<list_item_param> &params,
		 uielements &elements)
		{
			auto &cb=elements.list_item_status_change_callbacks;

			auto iter=cb.find(name);

			if (iter == cb.end())
			{
				throw EXCEPTION(
					gettextmsg(
						_("List item status change "
						  "callback %1% not defined"),
						name));
			}
			params.emplace_back(iter->second);
		});
}

uicompiler::list_items_params
uicompiler::list_items_param_value(const ui::parser_lock &orig_lock,
				   const char *element,
				   const char *parent)
{
	auto params=vector<functionref<void (std::vector<list_item_param> &,
					       uielements &elements)>>
		::create();

	auto labels=vector<std::string>::create();

	auto lock=orig_lock->clone();

	lock->get_xpath(element)->to_node();

	// The actual list_item_params will be the number of child elements,
	// except for the <name> elements. If there's at least one <name>
	// element we'll add one more item.

	auto xpath=lock->get_xpath("name");

	size_t n_names=xpath->count();

	size_t n=lock->get_child_element_count();

	params->reserve(n-n_names + (n_names ? 1:0));
	labels->reserve(n_names);

	list_items_parse_info_t parse_info{params, labels};

	auto ret=list_items_parseconfig(lock);

	const list_items_parse_info_t *ptr=&parse_info;

	for (const auto &generator:*ret)
	{
		generator(ptr);
	}

	if (!labels->empty())
		params->emplace_back
			([]
			 (std::vector<list_item_param> &params,
			  uielements &elements)
			 {
				 params.emplace_back(get_new_items{});
			 });

	return {params, labels};
}

std::vector<list_item_param> uicompiler::list_items_params
::create(uielements &elements) const
{
	std::vector<list_item_param> v;

	v.reserve(creators->size());

	for (const auto &c:*creators)
	{
		c(v, elements);
	}

	return v;
}

// Compiled code invokes this to process new list items

static void process_new_items(const new_items_ret &ret,
			      const uicompiler::list_items_params &params,
			      uielements &elements)
{
	if (params.labels->empty())
		return;

	size_t label_size=params.labels->size();
	size_t handles_size=ret.handles.size();

	if (label_size != handles_size)
		throw EXCEPTION(gettextmsg(_("Number of new list items (%1%) "
					     "doesn't match the number of "
					     "<label>s (%2%)"),
					   handles_size,
					   label_size));

	auto b=params.labels->begin();

	for (const auto &h:ret.handles)
		elements.new_list_item_handles.insert_or_assign(*b++, h);
}

listlayoutmanager_generator
uicompiler::listlayout_parseconfig(const ui::parser_lock &lock,
				   const char *element,
				   const char *parent)
{
	auto element_lock=lock->clone();
	element_lock->get_xpath(element)->to_node();

	return [generators=listlayout_parseconfig(element_lock)]
		(const listlayoutmanager &layout, uielements &elements)
	       {
		       for (const auto &generator:*generators)
			       generator(layout, elements);
	       };
}

static void append_copy_cut_paste(const listlayoutmanager &layout,
				  uielements &elements,
				  const std::string &parent)
{
	elements.new_copy_cut_paste_menu_items=
		layout->append_copy_cut_paste(elements.get_element(parent));
}

#include "uicompiler.inc.H/listlayout_parse_parameters.H"
#include "uicompiler.inc.H/listlayout_parser.H"

LIBCXXW_NAMESPACE_END
