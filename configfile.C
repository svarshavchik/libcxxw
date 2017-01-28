#include "libcxxw_config.h"
#include "configfile.H"
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <x/pwd.H>

namespace LIBCXX_NAMESPACE {
	namespace w {
#if 0
	}
}
#endif

static std::string configfilename()
{
	const char *home=getenv("HOME");

	if (!home || !home)
	{
		try {
			home=passwd(getuid())->pw_dir;
		} catch (const exception &e)
		{
			std::cerr << getuid() << ": " << e << std::endl;
		}
	}

	if (!home || !home)
		home="/";

	return std::string(home)+"/.cxxw";
}

static xml::doc do_read_config()
{
	auto filename=configfilename();

	try {
		if (access(filename.c_str(), 0) == 0)
			return xml::doc::create(filename, "nonet xinclude");
	} catch (const exception &e)
	{
		std::cerr << filename << ": " << e << std::endl;
	}

	return xml::doc::create();
}

xml::doc read_config()
{
	auto doc=do_read_config();

	auto lock=doc->writelock();

	if (!lock->get_root())
		lock->create_child()
			->element(xml::doc::base::newelement("cxxw"));

	return doc;
}

void save_config(const xml::doc &d)
{
	auto filename=configfilename();

	try {
		d->readlock()->save_file(filename);
	} catch (const exception &e)
	{
		std::cerr << filename << ": " << e << std::endl;
	}
}

#if 0
{
	{
#endif
	}
}
