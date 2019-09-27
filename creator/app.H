#ifndef creator_app_H
#define creator_app_H

#include <x/obj.H>
#include <x/ref.H>
#include <x/refptr_traits.H>
#include <x/singletonptr.H>
#include <x/functionalrefptr.H>
#include <x/eventqueue.H>
#include <x/mpobj.H>
#include <x/xml/doc.H>
#include "x/w/main_window.H"
#include "x/w/uigenerators.H"
#include "x/w/file_dialogfwd.H"
#include "creator/appelements.inc.H"

class appObj;

typedef x::ref<appObj> app;

//! Singleton application class.

//! Inherits from UI elements, the UI elements in the app window, mostly
//! the main menu.

class appObj : public app_elements_t, virtual public x::obj {

	//! My configuration file

	const std::string configfile;

public:
	//! Constructor
	appObj();

	struct init_args;
private:
	//! Internal constructor
	appObj(init_args &&);

	//! UI event queue type
	typedef x::eventqueue< x::functionref<void (const app &)>> eventqueue_t;

	//! UI events
	const eventqueue_t eventqueue=eventqueue_t::create();

public:
	//! Destructor
	~appObj();

private:

	//! Update everything, after a new file is loaded or created.
	void loaded_file();

	//! Update the application title

	//! (include the name of the current theme)

	void update_title();
public:
	//! Main application loop
	void mainloop();

	//! Currently loaded theme file
	x::mpobj<x::xml::doc> theme=new_file();

	//! Return an empty theme file.
	static x::xml::doc new_file();

	//! Filename
	x::mpobj<std::string> themename;

	//! Whether the theme file has been modified
	x::mpobj<bool> edited=false;

	//////////////////////////////////////////////////////////////////////
	//
	// Events from the UI. They invoke appevent to send the event to
	// main thread for execution.

	friend void appevent(const x::functionref<void (const app &)> &);

	//! File save event.
	void file_save_event();

	//! File save as event
	void file_save_as_event();

	//! File save dialog closed
	void do_check_and_file_save(const x::w::file_dialog &,
				    std::string);

	//! File save dialog closed, checked for overwriting, ok to save now
	void do_file_save(const std::string &);

	//! File quit event.
	void file_quit_event();

private:
	//! The app is running.

	//! Cleared by file_quit_event(), to stop the main execution thread.
	bool running=true;

};

// The singleton instance of the app object.
typedef x::singletonptr<appObj> appsingleton;

//! app events

//! Constructs the app singleton
//! then (attempt to) instantiate the app singleton,
//! then put an event closure into the event queue, to be executed
//! by the main application thread that loops and plucks events
//! off the event queue.

void appevent(const x::functionref<void (const app &)> &);

// Helper for invoking something in the app object.

template<typename T, typename ...Args>
static inline void appinvoke(T t, Args && ...args)
{
	appsingleton singleton;

	if (!singleton)
		return;

	auto &me=*singleton;

	(me.*t)(std::forward<Args>(args)...);
}

#endif
