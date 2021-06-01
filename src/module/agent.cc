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

 /*
  * https://stackoverflow.com/questions/3017162/how-to-get-total-cpu-usage-in-linux-using-c
  *
  * Read /proc/cpuinfo to find the number of CPU/cores available to the systems.
  * Call the getloadavg() (or alternatively read the /proc/loadavg), take the first value,
  * multiply it by 100 (to convert to percents), divide by number of CPU/cores.
  * If the value is greater than 100, truncate it to 100. Done.
  *
  * Relevant documentation: man getloadavg and man 5 proc
  *
  */

 #include "private.h"
 #include <udjat/tools/file.h>
 #include <sstream>
 #include <iomanip>

 namespace Udjat {

	SysInfo::Agent::Agent(const char *name) : Abstract::Agent(name) {
	}

	SysInfo::Agent::~Agent() {
	}

	void SysInfo::Agent::refresh() {
		get();
	}

	void SysInfo::Agent::get(const char *name, Json::Value &value) {
		value[name] = get();
	}

	void SysInfo::Agent::append_state(const pugi::xml_node &node) {
		this->states.push_back(std::make_shared<State<float>>(node));
	}

	std::string SysInfo::Agent::to_string() const {
		std::stringstream out;
		out << std::fixed << std::setprecision(2) << const_cast<SysInfo::Agent &>(*this).get() << "%";
		return out.str();
	}

	std::shared_ptr<Abstract::State> SysInfo::Agent::find_state() const {
		return active_state;
	}

	float SysInfo::Agent::set(float value) {

		// Find the correct state.
		for(auto st : this->states) {

			if(st->compare(value)) {

				if(st != active_state) {

					// State has changed.
					this->active_state = st;
					updated(true);

				} else {

					// State has not changed
					updated(false);

				}

				break;
			}

		}

		return value;

	}

 }
