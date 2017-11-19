/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include <x/exception.H>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include "metrics_axis.H"
#include "dirlisting/filedircontents.H"
#include "dirlisting/filedir_file.H"
#include <x/dir.H>
#include <x/fd.H>
#include <x/mpobj.H>
#include <set>

using namespace LIBCXX_NAMESPACE;
using namespace LIBCXX_NAMESPACE::w;

struct contentsObj : virtual public obj {

	mpcobj<std::set<std::string>> names;
};

void testfiledircontents()
{
	dir::base::rmrf("testfiledircontents.dir");
	mkdir("testfiledircontents.dir", 0777);

	for (int i=0; i<100; ++i)
	{
		std::ostringstream o;

		o << "testfiledircontents.dir/file" << std::setw(2)
		  << std::setfill('0') << i;

		fd::create(o.str(), 0600);
	}

	auto c=ref<contentsObj>::create();

	auto fc=filedircontents::create
		("testfiledircontents.dir",
		 [c]
		 (const auto &changes)
		 {
			 mpcobj<std::set<std::string>>::lock lock{c->names};

			 for (const auto &f:changes->files)
			 {
				 if (f.removed)
				 {
					 std::cout << "Removed "
						   << f.name << std::endl;

					 lock->erase(f.name);
				 }
				 else
				 {
					 std::cout << "Added "
						   << f.name << std::endl;

					 lock->insert(f.name);
				 }
			 }
			 lock.notify_all();
		 });

	mpcobj<std::set<std::string>>::lock lock{c->names};

	lock.wait([&]
		  {
			  return (*lock) == std::set<std::string>{
				  "file00",
				  "file01",
				  "file02",
				  "file03",
				  "file04",
				  "file05",
				  "file06",
				  "file07",
				  "file08",
				  "file09",
				  "file10",
				  "file11",
				  "file12",
				  "file13",
				  "file14",
				  "file15",
				  "file16",
				  "file17",
				  "file18",
				  "file19",
				  "file20",
				  "file21",
				  "file22",
				  "file23",
				  "file24",
				  "file25",
				  "file26",
				  "file27",
				  "file28",
				  "file29",
				  "file30",
				  "file31",
				  "file32",
				  "file33",
				  "file34",
				  "file35",
				  "file36",
				  "file37",
				  "file38",
				  "file39",
				  "file40",
				  "file41",
				  "file42",
				  "file43",
				  "file44",
				  "file45",
				  "file46",
				  "file47",
				  "file48",
				  "file49",
				  "file50",
				  "file51",
				  "file52",
				  "file53",
				  "file54",
				  "file55",
				  "file56",
				  "file57",
				  "file58",
				  "file59",
				  "file60",
				  "file61",
				  "file62",
				  "file63",
				  "file64",
				  "file65",
				  "file66",
				  "file67",
				  "file68",
				  "file69",
				  "file70",
				  "file71",
				  "file72",
				  "file73",
				  "file74",
				  "file75",
				  "file76",
				  "file77",
				  "file78",
				  "file79",
				  "file80",
				  "file81",
				  "file82",
				  "file83",
				  "file84",
				  "file85",
				  "file86",
				  "file87",
				  "file88",
				  "file89",
				  "file90",
				  "file91",
				  "file92",
				  "file93",
				  "file94",
				  "file95",
				  "file96",
				  "file97",
				  "file98",
				  "file99",
			  };
		  });
	unlink("testfiledircontents.dir/file00");
	sleep(1);
	unlink("testfiledircontents.dir/file01");
	fd::create("testfiledircontents.dir/file00", 0600);
	fd::create("testfiledircontents.dir/file01", 0600);
	fd::create("testfiledircontents.dir/file100", 0600);

	lock.wait([&]
		  {
			  return (*lock) == std::set<std::string>{
				  "file00",
				  "file01",
				  "file02",
				  "file03",
				  "file04",
				  "file05",
				  "file06",
				  "file07",
				  "file08",
				  "file09",
				  "file10",
				  "file11",
				  "file12",
				  "file13",
				  "file14",
				  "file15",
				  "file16",
				  "file17",
				  "file18",
				  "file19",
				  "file20",
				  "file21",
				  "file22",
				  "file23",
				  "file24",
				  "file25",
				  "file26",
				  "file27",
				  "file28",
				  "file29",
				  "file30",
				  "file31",
				  "file32",
				  "file33",
				  "file34",
				  "file35",
				  "file36",
				  "file37",
				  "file38",
				  "file39",
				  "file40",
				  "file41",
				  "file42",
				  "file43",
				  "file44",
				  "file45",
				  "file46",
				  "file47",
				  "file48",
				  "file49",
				  "file50",
				  "file51",
				  "file52",
				  "file53",
				  "file54",
				  "file55",
				  "file56",
				  "file57",
				  "file58",
				  "file59",
				  "file60",
				  "file61",
				  "file62",
				  "file63",
				  "file64",
				  "file65",
				  "file66",
				  "file67",
				  "file68",
				  "file69",
				  "file70",
				  "file71",
				  "file72",
				  "file73",
				  "file74",
				  "file75",
				  "file76",
				  "file77",
				  "file78",
				  "file79",
				  "file80",
				  "file81",
				  "file82",
				  "file83",
				  "file84",
				  "file85",
				  "file86",
				  "file87",
				  "file88",
				  "file89",
				  "file90",
				  "file91",
				  "file92",
				  "file93",
				  "file94",
				  "file95",
				  "file96",
				  "file97",
				  "file98",
				  "file99",
				  "file100",
			  };
		  });
}

int main()
{
	try {
		testfiledircontents();
		dir::base::rmrf("testfiledircontents.dir");
	} catch (const exception &e)
	{
		e->caught();
		exit(1);
	}
	exit(0);
}
