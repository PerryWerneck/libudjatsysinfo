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
 #include <udjat/tools/intl.h>
 #include <sstream>
 #include <iomanip>
 #include <sstream>

 #include "private.h"
 #include <udjat/moduleinfo.h>

 namespace Udjat {

	static const Udjat::ModuleInfo moduleinfo{"Get system time"};

	class SysInfo::SysTime::Agent : public Abstract::Agent {
	public:
		Agent(const xml_node &node) : Abstract::Agent("systime") {
			Object::properties.icon = "utilities-system-monitor";
			Object::properties.label = _( "System Time" );
			Abstract::Agent::load(node);
		}

		virtual ~Agent() {
		}

		Udjat::Value &get(Udjat::Value &value) const override {
			value = TimeStamp();
			return value;
		}

		std::string to_string() const override {
			return TimeStamp().to_string();
		}
	};

	SysInfo::SysTime::SysTime() : Udjat::Factory("system-systime",moduleinfo) {
	}

	SysInfo::SysTime::~SysTime() {
	}

	std::shared_ptr<Abstract::Agent> SysInfo::SysTime::AgentFactory(const Abstract::Object &parent, const pugi::xml_node &node)  const {
		return make_shared<Agent>(node);
	}

 }
