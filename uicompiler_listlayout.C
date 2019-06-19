/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "uicompiler.H"
#include "messages.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/uigenerators.H"
#include "x/w/uielements.H"

LIBCXXW_NAMESPACE_START

uicompiler::list_items_params
uicompiler::list_items_param_value(const theme_parser_lock &orig_lock,
				   const char *element,
				   const char *parent)
{
	list_items_params params
		{
		 vector<list_item_param>::create(),
		 vector<std::string>::create()
		};

	auto lock=orig_lock->clone();

	lock->get_xpath(element)->to_node();

	// The actual list_item_params will be the number of child elements,
	// except for the <name> elements. If there's at least one <name>
	// element we'll add one more item.

	auto xpath=lock->get_xpath("name");

	size_t n_names=xpath->count();

	xpath=lock->get_xpath("*");

	size_t n=xpath->count();

	params.params->reserve(n-n_names + (n_names ? 1:0));
	params.labels->reserve(n_names);

	for (size_t i=1; i <= n; ++i)
	{
		xpath->to_node(i);

		auto name=lock->name();

		if (name == "name")
		{
			params.labels->push_back(single_value(lock, ".",
							      element));
			continue;
		}

		if (name == "label")
		{
			params.params->emplace_back(text_param_value(lock,
								     ".",
								     element));
			continue;
		}

		if (name == "image")
		{
			params.params->emplace_back
				(image_param{
					     single_value(lock, ".", element)
				});
			continue;
		}

		if (name == "separator")
		{
			params.params->emplace_back(separator{});
			continue;
		}

		if (name == "shortcut")
		{
			params.params->emplace_back(shortcut_value(lock, ".",
								   element));
			continue;
		}

		if (name == "inactive_shortcut")
		{
			params.params->emplace_back
				(inactive_shortcut{
						   shortcut_value(lock, ".",
								  element)
				});
			continue;
		}

		if (name == "hierindent")
		{
			params.params->emplace_back
				(hierindent{
					    to_size_t(lock, ".", element)
				});
			continue;
		}
		throw EXCEPTION(gettextmsg(_("Unknown <%1%> element in <%2%>"),
					   name, element));
	}

	if (!params.labels->empty())
		params.params->emplace_back( get_new_items{} );
	return params;
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
		elements.new_list_item_handles.emplace(*b++, h);
}

#include "uicompiler.inc.H/listlayout_parse_parameters.H"
#include "uicompiler.inc.H/listlayout_parser.H"

LIBCXXW_NAMESPACE_END
