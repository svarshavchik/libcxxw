/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifdef x_w_editable_comboboxlayoutmanager_h
#ifdef x_w_input_field_h

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
	this->locked_input_field->set(std::forward<Args>(args)...);
}
#endif
#endif
