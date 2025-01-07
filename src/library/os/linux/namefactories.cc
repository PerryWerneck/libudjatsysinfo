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
 #include <udjat/tools/disk/stat.h>
 #include <udjat/tools/string.h>
 #include <cstring>
 #include <stdexcept>
 #include <unistd.h>

 using namespace std;

 namespace Udjat {

	String Disk::NameFactory(const char * devname, bool required) {

		if(!(devname && *devname)) {
			if(required) {
				throw runtime_error("A device name is required");
			}
			return "";
		}

		const char *ptr = strrchr(devname,'/');

		String result{(ptr && ptr[1]) ? (ptr+1) : devname};

		if(required && result.empty()) {
			throw runtime_error("Invalid device name");
		}

		return result;

	}

	String Disk::NameFactory(const XML::Node &node, bool required) {

		String devname{node,"name"};
		if(!devname.empty()) {
			return NameFactory(devname.c_str(),required);
		}

		return NameFactory(String{node,"device-name"}.c_str(),required);
	}

	String Disk::DeviceNameFactory(const char * devname, bool required) {

		if(!(devname && *devname)) {
			if(required) {
				throw runtime_error("A device name is required");
			}
			return "";
		}

		String result{devname};

		if(result.has_prefix("/dev/")) {
			return result;
		}

		return String{"/dev/",devname};

	}

	String Disk::DeviceNameFactory(const XML::Node &node, bool required) {

		String devname{node,"device-name"};
		if(!devname.empty()) {
			return DeviceNameFactory(devname.c_str(),required);
		}

		return DeviceNameFactory(String{node,"name"}.c_str(),required);
	}

 }

