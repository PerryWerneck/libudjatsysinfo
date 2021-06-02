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
 #include <udjat/tools/sysconfig.h>
 #include <sstream>
 #include <iomanip>

 namespace Udjat {

	static const Udjat::ModuleInfo moduleinfo{
		PACKAGE_NAME,												// The module name.
		"Get swap use", 											// The module description.
		PACKAGE_VERSION, 											// The module version.
		PACKAGE_URL, 												// The package URL.
		PACKAGE_BUGREPORT 											// The bug report address.
	};

	SysInfo::SwapUsed::Factory::Factory() : Udjat::Factory("swap-used",&moduleinfo) {
	}

	void SysInfo::SwapUsed::Factory::parse(Abstract::Agent &parent, const pugi::xml_node &node) const {
		parent.insert(make_shared<SysInfo::SwapUsed>(node));
	}

	SysInfo::SwapUsed::SwapUsed(const char *name) : SysInfo::Agent(name) {
		setup();
		setDefaultStates();
	}

	SysInfo::SwapUsed::SwapUsed(const pugi::xml_node &node) : SysInfo::Agent("swap") {
		setup();
		load(node);
		if(!hasStates()) {
			setDefaultStates();
		};
	}

	void SysInfo::SwapUsed::setup() {
		this->icon = "utilities-system-monitor";
		this->label = "Used Swap Percentage";
	}

	SysInfo::SwapUsed::~SwapUsed() {
	}

	void SysInfo::SwapUsed::setDefaultStates() {

		static const struct {
			float from;
			float to;
			const char 						* name;			///< @brief State name.
			Udjat::Level					  level;		///< @brief State level.
			const char						* summary;		///< @brief State summary.
			const char						* body;			///< @brief State description
		} states[] = {
			{
				0.0,
				10.0,
				"low",
				Udjat::ready,
				"Swap usage is lower than 10%",
				""
			},
			{
				10.0,
				80.0,
				"low",
				Udjat::ready,
				"Swap usage is lower than 80%",
				""
			},
			{
				80.0,
				90.0,
				"medium",
				Udjat::warning,
				"Swap usage is lower than 90%",
				""
			},
			{
				90.0,
				100.0,
				"high",
				Udjat::error,
				"Swap usage is higher than 90%",
				""
			}

		};

		cout << getName() << "\tUsing default states" << endl;

		for(size_t ix = 0; ix < (sizeof(states)/ sizeof(states[0])); ix++) {

			this->states.push_back(
				make_shared<Udjat::State<float>>(
					states[ix].name,
					states[ix].from,
					states[ix].to,
					states[ix].level,
					states[ix].summary,
					states[ix].body
				)
			);

		}

	}

	float SysInfo::SwapUsed::get() {

		Udjat::SysConfig::File meminfo("/proc/meminfo",":");

		float free =  to_bytes(meminfo["SwapFree"]);
		float total = to_bytes(meminfo["SwapTotal"]);
		float used = total - free;

		return set( (used * 100) /total );

	}

 }
