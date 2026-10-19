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
 #include <udjat/tools/testsuite.h>
 #include <string>
 #include <udjat/agent/percentage.h>
 #include <udjat/tools/storage/stat.h>

 using namespace Udjat;
 using namespace std;

 #ifdef DEBUG 

 UDJAT_API void udjat_register_tests(Udjat::TestSuite &suite) noexcept {

	using Case = TestSuite::Case;

	suite.add(
		PACKAGE_DESCRIPTION,
		Case {
			"Test percent agent",
			[](std::ostream &stream) {				
				Agent<Percentage> percent{"test-percent",0.1};
				stream << String{"The percent value is ${value}"}.expand(percent).c_str() << "'" << endl;
				return "Percent agent ok";
			}
		},
		Case {
			"Test storage enumerator",
			[](std::ostream &stream) {
				for(const auto &disk : Storage::Stat::get()) {
					stream << disk.name() << "' is " << (disk.physical() ? "Physical" : "Not physical") << endl;
				}
				return "Storage enumerator ok";
			}
		}
	);

 }  

 #endif


