/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "creator/object_appgenerator_function.H"
#include "creator/setting_handler.H"
#include "x/w/gridfactory.H"
#include "x/w/label.H"
#include "x/w/text_param_literals.H"
#include <x/chrcasecmp.H>

object_generator_valueObj::object_generator_valueObj(parameter_value &&v)
	: parameter_value{std::move(v)}
{
}

object_appgenerator_functionObj::object_appgenerator_functionObj(
	const std::string &object_name,
	const parse_parameter &member,
	parameter_value value)
	: object_name{object_name},
	  member{member},
	  value{object_generator_value::create(
			  std::move(value))}
{
}

object_appgenerator_functionObj::~object_appgenerator_functionObj()=default;

appgenerator_function object_appgenerator_functionObj::clone() const
{
	return clone_me();
}

object_appgenerator_function object_appgenerator_functionObj::clone_me() const
{
	auto c=object_appgenerator_function::create(object_name,
						    member, *value);

	c->value->cloned();

	return c;
}

x::w::text_param object_appgenerator_functionObj::description(
	description_format fmt) const
{
	x::w::text_param t;

	switch (fmt) {
	case description_format::list:
		t(member->get_name());
		break;
	case description_format::title:
		t("mono; scale=2"_theme_font);
		t("<");
		t(object_name);
		t(">");
		break;
	}

	return t;
}

bool object_appgenerator_functionObj::has_ui() const
{
	if (member->handler && member->handler->flag_value())
		return false;

	return true;
}

generator_create_ui_ret_t object_appgenerator_functionObj::create_ui(
	ONLY IN_THREAD,
	const x::w::main_window &mw,
	const x::w::gridlayoutmanager &glm)
	const
{
	auto ret=appinvoke([&]
			   (appObj *me)
	{
		return create_ui(IN_THREAD, me, mw, glm);
	});

	if (!ret)
	{
		// Fail gracefully, as if everything failed
		// validation, so let the cheaps fall as they may.
		ret.emplace([]
		{
			return false;
		});
	}

	return *ret;
}

void object_appgenerator_functionObj::save(const x::xml::writelock &lock,
		  appgenerator_save &info) const
{
	member->save(lock, *value, true, info);
}

generator_create_ui_ret_t object_appgenerator_functionObj::create_ui(
	ONLY IN_THREAD, appObj *app,
	const x::w::main_window &mw,
	const x::w::gridlayoutmanager &glm)
	const
{
	auto f=glm->append_row();

	f->halign(x::w::halign::right);
	f->valign(x::w::valign::middle);

	// Create a meaningful label
	auto s=member->parameter_name + ":";

	s[0] = x::chrcasecmp::toupper(s[0]);

	f->create_label(s);

	f->valign(x::w::valign::middle);

	if (!member->handler)
		throw EXCEPTION("Internal error: handler not "
				"set for " << member->handler_name);

	std::unordered_map<std::string_view,
			   setting_create_ui_ret_t
			   > values_map;

	// Call the parameter's create_ui() and then return a wrapper
	// for it, that updates the stored value object.

	auto func=member->handler->create_ui(IN_THREAD, {
			app,
			mw,
			f,
			glm,
			*value,
			!member->is_optional,
			member->parameter_name,
			member->handler_name,
			values_map,
		});

	return [func, me=x::const_ref{this}]
	{
		auto value=func.validator(true);

		if (!value)
			return false;

		*me->value=*value;
		return true;
	};
}
