/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifdef x_w_editable_comboboxlayoutmanager_h_defined
#ifdef x_w_input_field_h_defined

template<typename ...Args>
struct editable_comboboxlayoutmanagerObj::set_args_t {

	typedef decltype(std::declval<input_fieldObj &>()
			 .set(std::forward<Args>(std::declval<Args &&>())...))
	void_t;
};

template<typename ...Args>
typename editable_comboboxlayoutmanagerObj::set_args_t<Args...>
::void_t editable_comboboxlayoutmanagerObj::set(Args && ...args)
{
	this->notmodified();
	this->locked_input_field->set(std::forward<Args>(args)...);
}


template<typename ...Args>
auto editable_comboboxlayoutmanagerObj::set_validator(Args && ...args)
{
	this->notmodified();
	return this->locked_input_field->set_validator
		(std::forward<Args>(args)...);
}

template<typename T, typename ...Args>
auto editable_comboboxlayoutmanagerObj::set_string_validator(Args && ...args)
{
	this->notmodified();
	return this->locked_input_field->set_string_validator<T>
		(std::forward<Args>(args)...);
}

template<typename ...Args>
bool editable_comboboxlayoutmanagerObj::validate_modified(Args && ...args)
{
	this->notmodified();
	return this->locked_input_field->validate_modified
		(std::forward<Args>(args)...);
}

#endif
#endif
