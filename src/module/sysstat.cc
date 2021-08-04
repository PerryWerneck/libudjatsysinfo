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
 #include <iostream>
 #include <fstream>

 #include "private.h"

 using namespace std;

 namespace Udjat {

	static const Udjat::ModuleInfo moduleinfo{
		PACKAGE_NAME,												// The module name.
		"Get System stats", 										// The module description.
		PACKAGE_VERSION, 											// The module version.
		PACKAGE_URL, 												// The package URL.
		PACKAGE_BUGREPORT 											// The bug report address.
	};

	static const struct {
		const char *name;
		const char *label;
	} stats[] = {
		{
			"user",
			"CPU used by normal processes executing in user mode"
		},
		{
			"nice",
			"CPU used by niced processes executing in user mode"
		},
		{
			"system",
			"CPU used by processes executing in kernel mode"
		},
		{
			"idle",
			"twiddling thumbs"
		},
		{
			"iowait",
			"CPU waiting for I/O to complete"
		},
		{
			"irq",
			"CPU used when servicing interrupts"
		},
		{
			"softirq",
			"CPU used when servicing softirqs"
		}
	};

	class SysInfo::SysStat::Agent : public Abstract::Agent {
	private:

		/// @brief Saved values from last cycle.
		unsigned long saved[N_ELEMENTS(stats)];

		/// @brief Current values (in %).
		float values[N_ELEMENTS(stats)];

		/// @brief Read values from /proc/stat.
		static void read(unsigned long * values ) {

			ifstream in("/proc/stat", ifstream::in);

			in.ignore(3);

			for(size_t ix = 0; ix < N_ELEMENTS(stats); ix++) {
				in >> values[ix];
			}

		}

	public:
		Agent(const xml_node &node) : Abstract::Agent("sysstat") {

			this->icon = "utilities-system-monitor";
			Abstract::Agent::load(node);

			if(!getUpdateInterval()) {
				throw runtime_error("System stats requires an update timer");
			}

			for(size_t ix = 0; ix < N_ELEMENTS(stats); ix++) {
				values[ix] = 0;
			}

			read(saved);

		}

		bool refresh() override {

			// Get current system usage.
			unsigned long current[N_ELEMENTS(stats)];
			read(current);

			// Compute differences from last cycle.
			for(size_t ix = 0; ix < N_ELEMENTS(stats); ix++) {
				unsigned long up = current[ix];
				if(current[ix] >= saved[ix]) {
					current[ix] -= saved[ix];
				} else {
					current[ix] = 0;
				}
				saved[ix] = up;
			}

			// Compute values on %

			return true;
		}

	};

	SysInfo::SysStat::SysStat() : Udjat::Factory("system-stat",&moduleinfo) {
	}

	SysInfo::SysStat::~SysStat() {
	}

	bool SysInfo::SysStat::parse(Abstract::Agent &parent, const pugi::xml_node &node) const {
		parent.insert(make_shared<Agent>(node));
		return true;
	}

 }
