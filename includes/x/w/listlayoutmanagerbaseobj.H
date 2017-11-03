/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_listlayoutmanagerbaseobj_h
#define x_w_listlayoutmanagerbaseobj_h

#include <x/w/listlayoutmanagerbasefwd.H>
#include <x/w/listlayoutmanagerfwd.H>
#include <x/w/text_paramfwd.H>
#include <x/obj.H>
#include <x/mpobj.H>

#include <vector>
#include <utility>
#include <optional>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see listlayoutmanagerbase

class listlayoutmanagerbaseObj : virtual public obj {

public:

	//! Thread-safe contained for new list item callback factory.

	typedef mpobj<std::function<list_item_status_change_callback_factory_t>,
		      std::recursive_mutex> callback_factory_container_t;

	//! Current factory for new list items.

	callback_factory_container_t callback_factory_container;

	//! Set a callback factory for new list items.

	template<typename callback>
	inline void callback_factory(callback &&cb)
	{
		this->callback_factory_container=cb;
	}

	//! Set a callback factory for new list items.

	inline auto callback_factory()
	{
		return callback_factory_container.get();
	}

	//! Remove the current callback factory.

	void remove_callback_factory();

	//! Return the next callback.
	std::function<list_item_status_change_callback_t> next_callback();

	//! Thread-safe contained for new list item shortcut factory.

	typedef mpobj<shortcut_factory_t,
		      std::recursive_mutex> shortcut_factory_container_t;

	//! New list item shortcut factory.
	shortcut_factory_container_t shortcut_factory_container;

	//! Set a shortcut factory for new list items.

	template<typename callback>
	inline void shortcut_factory(callback &&cb)
	{
		this->shortcut_factory_container=cb;
	}

	//! Set a callback factory for new list items.

	inline auto shortcut_factory()
	{
		return this->shortcut_factory_container.get();
	}

	//! Remove the current shortcut factory.
	void remove_shortcut_factory();

	//! Return the next shortcut.
	shortcut next_shortcut();

	//! Constructor
	listlayoutmanagerbaseObj() LIBCXX_HIDDEN;

	//! Destructor
	~listlayoutmanagerbaseObj() LIBCXX_HIDDEN;

	//! Append a new list item

	template<typename ...Args>
	void append_item(const text_param &text, Args && ...args)
	{
		std::vector<text_param>
			items{text, std::forward<Args>(args)...};
		append_item(items);
	}

	//! Remove a list item
	virtual void remove_item(size_t item_number)=0;

	//! Append a new list item

	//! \overload
	virtual void append_item(const std::vector<text_param> &items)=0;

	//! Insert a new list item

	template<typename ...Args>
	void insert_item(size_t item_number,
			 const text_param &text, Args && ...args)
	{
		std::vector<text_param>
			items{text, std::forward<Args>(args)...};
		insert_item(item_number, items);
	}

	//! Insert a new list item

	//! \overload
	virtual void insert_item(size_t item_number,
				 const std::vector<text_param> &items)=0;

	//! Replace a list item

	template<typename ...Args>
	void replace_item(size_t item_number,
			  const text_param &text, Args && ...args)
	{
		std::vector<text_param>
			items{text, std::forward<Args>(args)...};
		replace_item(item_number, items);
	}

	//! Replace a list item

	//! \overload
	virtual void replace_item(size_t item_number,
				  const std::vector<text_param> &items)=0;

	//! Replace all list items

	virtual void replace_all_items(const std::vector<text_param> &items)=0;

	//! Number of items in the list

	virtual size_t size() const=0;

	//! Whether list item \#i is selected right now.
	virtual bool selected(size_t i) const=0;

	//! Return the currently selected item.

	//! For convenience of a single_selection_type() list, returns
	//! the index of the currently selected item.

	virtual std::optional<size_t> selected() const=0;

	//! Return a list of all selected items.
	virtual std::vector<size_t> all_selected() const=0;

	//! Select or unselect item \#i
	virtual void selected(size_t i, bool selected_flag,
			      const callback_trigger_t &trigger)=0;
	//! As if item \#i was clicked on.

	virtual void autoselect(size_t i,
				const callback_trigger_t &trigger)=0;

	//! Shortcut

	//! Invoke selected() with the default trigger.
	//! \overload
	void selected(size_t i, bool selected_flag);

	//! Shortcut

	//! Invoke autoselect() with the default trigger.
	//! \overload
	void autoselect(size_t i);

	//! Unselect all selected items.
	virtual void unselect()=0;

	//! Whether the given list item is enabled().

	virtual bool enabled(size_t i) const=0;

	//! Enable or disable the list item.

	virtual void enabled(size_t i, bool flag)=0;

};

LIBCXXW_NAMESPACE_END

#endif
