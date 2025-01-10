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
 #include <cstring>
 #include <udjat/tools/disk/unit.h>
 #include <udjat/tools/xml.h>
 #include <udjat/tools/string.h>

 using namespace std;

 namespace Udjat {

	static const struct {
		float value;
		const char *str;
	} units[] = {
		{             1.0, 	 "B" },
		{          1024.0, 	"KB" },
		{       1048576.0, 	"MB" },
		{    1073741824.0, 	"GB" },
		{ 1099511627776.0,	"TB" }
	};

	Storage::Unit Storage::UnitFactory(const XML::Node &node) {

		String attr{node,"size-unit","KB"};
		
		for(size_t ix = 0; ix < ((sizeof(units)/sizeof(units[0]))); ix++) {
			if(!strcasecmp(attr.c_str(),units[ix].str)) {
				return (Storage::Unit) ix;
			}
		}

		throw runtime_error(String{"Invalid value for size-unit: '",attr.c_str(),"'"});

	}

 }

