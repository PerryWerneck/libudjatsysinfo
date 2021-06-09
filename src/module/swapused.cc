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

 #include "private.h"

 namespace Udjat {

	static const Udjat::ModuleInfo moduleinfo{
		PACKAGE_NAME,												// The module name.
		"Get swap use", 											// The module description.
		PACKAGE_VERSION, 											// The module version.
		PACKAGE_URL, 												// The package URL.
		PACKAGE_BUGREPORT 											// The bug report address.
	};

	static const SysInfo::Percent::StateDescription internal_states[] = {
		{
			10.0,
			"low",
			Udjat::ready,
			"Swap usage is lower than 10%",
			""
		},
		{
			80.0,
			"low",
			Udjat::ready,
			"Swap usage is lower than 80%",
			""
		},
		{
			90.0,
			"medium",
			Udjat::warning,
			"Swap usage is lower than 90%",
			""
		},
		{
			100.0,
			"high",
			Udjat::error,
			"Swap usage is higher than 90%",
			""
		}
	};

	class SysInfo::SwapUsed::Agent : public Percent {
	protected:

		float getValue() const override {

			struct sysinfo info;
			memset(&info,0,sizeof(info));

			if(sysinfo(&info) < 0) {
				throw system_error(errno,system_category(),"Cant get system information");
			}

			float free = (float) info.freeswap;
			float total = (float) info.totalswap;
			float rc = ((total-free) * 100) /total;

			return rc;

		};

	public:
		Agent(const xml_node &node) : Percent("swap") {
			this->icon = "utilities-system-monitor";
			this->label = "Used Swap Percentage";
			Abstract::Agent::load(node);
			load(internal_states,sizeof(internal_states)/sizeof(internal_states[0]));
		}

		virtual ~Agent() {
		}

	};

	SysInfo::SwapUsed::SwapUsed() : Udjat::Factory("swap-used",&moduleinfo) {
	}

	SysInfo::SwapUsed::~SwapUsed() {
	}

	void SysInfo::SwapUsed::parse(Abstract::Agent &parent, const pugi::xml_node &node) const {
		parent.insert(make_shared<Agent>(node));
	}

 }
