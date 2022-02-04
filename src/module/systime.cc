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
 #include "private.h"
 #include <udjat/tools/timestamp.h>
 #include <sstream>
 #include <iomanip>
 #include <sstream>

 #include "private.h"

 namespace Udjat {

	static const Udjat::ModuleInfo moduleinfo{
		PACKAGE_NAME,												// The module name.
		"Get system time", 											// The module description.
		PACKAGE_VERSION, 											// The module version.
		PACKAGE_URL, 												// The package URL.
		PACKAGE_BUGREPORT 											// The bug report address.
	};

	class SysInfo::SysTime::Agent : public Abstract::Agent {
	public:
		Agent(const xml_node &node) : Abstract::Agent("systime") {
			Object::properties.icon = "utilities-system-monitor";
			Object::properties.label = "System Time";
			Abstract::Agent::load(node);
		}

		virtual ~Agent() {
		}

		Udjat::Value &get(Udjat::Value &value) override {
			value = TimeStamp();
			return value;
		}

		std::string to_string() const override {
			return TimeStamp().to_string();
		}
	};

	SysInfo::SysTime::SysTime() : Udjat::Factory("system-systime",&moduleinfo) {
	}

	SysInfo::SysTime::~SysTime() {
	}

	bool SysInfo::SysTime::parse(Abstract::Agent &parent, const pugi::xml_node &node) const {
		parent.insert(make_shared<Agent>(node));
		return true;
	}

 }
