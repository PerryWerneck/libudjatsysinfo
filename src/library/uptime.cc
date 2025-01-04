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
 #include <udjat/tools/timestamp.h>
 #include <udjat/tools/intl.h>
 #include <udjat/agent.h>
 #include <udjat/agent/uptime.h>
 #include <udjat/moduleinfo.h>
 #include <sys/sysinfo.h>

 namespace Udjat {

	std::shared_ptr<Abstract::Agent> System::UpTime::Factory::AgentFactory(const Abstract::Object &, const XML::Node &node) const {
		return std::make_shared<UpTime>(node);
	}

	System::UpTime::UpTime(const char *nm) : Abstract::Agent{nm} {

		// https://specifications.freedesktop.org/icon-naming-spec/latest/
		Object::properties.icon = "utilities-system-monitor";
		Object::properties.label = _( "System Up Time" );
	}

	static inline bool is_empty(const char *str) noexcept {
		return !(str && *str);
	}

	System::UpTime::UpTime(const XML::Node &node) : Abstract::Agent{node} {
		
#ifdef DEBUG
		Logger::String{"Building system up time agent"}.info(name());
#endif

		if(is_empty(Object::properties.icon))
			Object::properties.icon = "utilities-system-monitor";

		if(is_empty(Object::properties.label))
			Object::properties.label = _( "System UP Time" );

	}

	System::UpTime::~UpTime() {
	}

	time_t System::UpTime::get() {
		struct sysinfo info;
		memset(&info,0,sizeof(info));
		if(sysinfo(&info) < 0) {
			throw std::system_error(errno,std::system_category(),_("Can't get system information"));
		}
		return info.uptime;
	}

	Udjat::Value & System::UpTime::get(Udjat::Value &value) const {
		value = get();
		return value;
	}

	std::string System::UpTime::to_string() const noexcept {
		return TimeStamp(get()).to_verbose_string();
	}

 }
