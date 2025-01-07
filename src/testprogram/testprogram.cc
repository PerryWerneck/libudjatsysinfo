/* SPDX-License-Identifier: LGPL-3.0-or-later */

/*
 * Copyright (C) 2021 Perry Werneck <perry.werneck@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

 #include <config.h>
 #include <udjat/defs.h>
 #include <udjat/tests.h>
 #include <udjat/module.h>
 #include <udjat/agent/percentage.h>
 #include <udjat/agent/loadavg.h>
 #include <udjat/tools/disk/stat.h>

 using namespace Udjat;

 int main(int argc, char **argv) {

	static const ModuleInfo info{"sysinfo-tester"};
	
	return Testing::run(argc,argv,info,[](Application &){

		{
			Agent<Percentage> percent{"test-percent",0.1};
			debug("Expanding----> '",String{"The percent value is ${value}"}.expand(percent).c_str(),"'");

		}

		for(const auto &disk : Disk::Stat::get()) {
			debug("----> Disk '",disk.name,"' is ",(disk.physical() ? "Physical" : "Not physical"));
		}

	 	udjat_module_init();

	});

 }
