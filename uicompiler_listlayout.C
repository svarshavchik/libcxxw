/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "uicompiler.H"
#include "messages.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/uigenerators.H"
#include "x/w/uielements.H"
#include <x/xml/xpath.H>

LIBCXXW_NAMESPACE_START

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

	xpath=lock->get_xpath("*");

	size_t n=xpath->count();

	params->reserve(n-n_names + (n_names ? 1:0));
	labels->reserve(n_names);

	for (size_t i=1; i <= n; ++i)
	{
		xpath->to_node(i);

		auto name=lock->name();

		if (name == "name")
		{
			labels->push_back(single_value(lock, ".",
						       element));
			continue;
		}

		if (name == "label")
		{
			params->emplace_back
				([label=text_param_value(lock,
							 ".",
							 element)]
				 (std::vector<list_item_param> &params,
				  uielements &elements)
				 {
					 params.emplace_back(label);
				 });
			continue;
		}

		if (name == "image")
		{
			params->emplace_back
				([image=image_param{single_value(lock, ".",
								 element)}]
					(std::vector<list_item_param> &params,
					 uielements &elements)
				 {
					 params.emplace_back(image);
				 });
			continue;
		}

		if (name == "separator")
		{
			params->emplace_back
				([]
				 (std::vector<list_item_param> &params,
				  uielements &elements)
				 {
					 params.emplace_back(separator{});
				 });
			continue;
		}

		if (name == "shortcut")
		{
			params->emplace_back
				([sc=shortcut_value(lock, ".",
						    element)]
				 (std::vector<list_item_param> &params,
				  uielements &elements)
				 {
					 params.emplace_back(sc);
				 });
			continue;
		}

		if (name == "inactive_shortcut")
		{
			params->emplace_back
				([sc=inactive_shortcut
						{shortcut_value(lock, ".",
								element)
						}]
				 (std::vector<list_item_param> &params,
				  uielements &elements)
				 {
					 params.emplace_back(sc);
				 });
			continue;
		}

		if (name == "hierindent")
		{
			params->emplace_back
				([n=to_size_t(lock, ".", element)]
				 (std::vector<list_item_param> &params,
				  uielements &elements)
				 {
					 params.emplace_back(hierindent{n});
				 });
			continue;
		}

		if (name == "menuoption")
		{
			params->emplace_back
				([groupname=optional_value(lock, ".",
							   element)]
				 (std::vector<list_item_param> &params,
				  uielements &elements)
				 {
					 params.emplace_back(menuoption{
							 groupname
						 });
				 });
			continue;
		}

		if (name == "selected")
		{
			params->emplace_back
				([]
				 (std::vector<list_item_param> &params,
				  uielements &elements)
				 {
					 params.emplace_back(selected{});
				 });
			continue;
		}

		if (name == "submenu")
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

			params->emplace_back
				([gens=listlayout_parseconfig(lock, ".",
							      element)]
				 (std::vector<list_item_param> &params,
				  uielements &elements)
				 {
					 params.emplace_back
						 (submenu
						  {[gens, &elements]
						   (const auto &lm)
						   {
							   gens(lm, elements);
						   }});
				 });
			continue;
		}

		throw EXCEPTION(gettextmsg(_("Unknown <%1%> element in <%2%>"),
					   name, element));
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
