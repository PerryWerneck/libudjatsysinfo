/* SPDX-License-Identifier: LGPL-3.0-or-later */

/*
 * Copyright (C) 2025 Perry Werneck <perry.werneck@gmail.com>
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
 #include <udjat/tools/logger.h>
 #include <udjat/tools/unit-test.h>
 #include <string>
 #include <udjat/agent/percentage.h>
 #include <udjat/tools/storage/stat.h>

 using namespace Udjat;
 using namespace std;

 #ifdef DEBUG 

 UDJAT_API void enum_udjat_unit_tests(Udjat::UnitTests &tests) noexcept {

	tests.append(
		UnitTests::Worker{
			"Test percent agent",
			[]() {				
				Agent<Percentage> percent{"test-percent",0.1};
				debug("Expanding----> '",String{"The percent value is ${value}"}.expand(percent).c_str(),"'");
				return true;
			}
		},
		UnitTests::Worker{
			"Test storage enumerator",
			[]() {
				for(const auto &disk : Storage::Stat::get()) {
					debug("----> Disk '",disk.name(),"' is ",(disk.physical() ? "Physical" : "Not physical"));
				}
				return true;
			}
		}
	);

 }  

 #endif


