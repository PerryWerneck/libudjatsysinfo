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
 #include <udjat/tools/timestamp.h>
 #include <udjat/tools/intl.h>
 #include <udjat/agent.h>
 #include <udjat/agent/systime.h>

 namespace Udjat {

	std::shared_ptr<Abstract::Agent> System::Time::Factory::AgentFactory(const XML::Node &node) const {
		return std::make_shared<Time>(node);
	}

	System::Time::Time(const char *nm) : Abstract::Agent{nm} {

		// https://specifications.freedesktop.org/icon-naming-spec/latest/
		Object::properties.icon = "utilities-system-monitor";
		Object::properties.label = _( "System Time" );
	}

	static inline bool is_empty(const char *str) noexcept {
		return !(str && *str);
	}

	System::Time::Time(const XML::Node &node) : Abstract::Agent{node} {
		
#ifdef DEBUG
		Logger::String{"Building system time agent"}.info(name());
#endif

		if(is_empty(Object::properties.icon))
			Object::properties.icon = "utilities-system-monitor";

		if(is_empty(Object::properties.label))
			Object::properties.label = _( "System Time" );

	}

	System::Time::~Time() {
	}

	Udjat::Value & System::Time::get(Udjat::Value &value) const {
		value.set(TimeStamp());
		return value;
	}

	std::string System::Time::to_string() const noexcept {
		return TimeStamp().to_string();
	}

 }
