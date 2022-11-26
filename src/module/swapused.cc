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
 #include <udjat/tools/intl.h>
 #include <sstream>
 #include <iomanip>
 #include <udjat/moduleinfo.h>

 #include "private.h"

 namespace Udjat {

	static const Udjat::ModuleInfo moduleinfo{"Get swap use"};

	static const SysInfo::Percent::StateDescription internal_states[] = {
		{
			0.1,
			"low",
			Udjat::ready,
			N_( "Swap usage is lower than 10%" ),
			""
		},
		{
			0.8,
			"low",
			Udjat::ready,
			N_( "Swap usage is lower than 80%" ),
			""
		},
		{
			0.9,
			"medium",
			Udjat::warning,
			N_( "Swap usage is lower than 90%" ),
			""
		},
		{
			1.0,
			"high",
			Udjat::error,
			N_( "Swap usage is higher than 90%" ),
			""
		}
	};

	class SysInfo::SwapUsed::Agent : public Percent {
	protected:

		float getValue() const override {

			struct sysinfo info;
			memset(&info,0,sizeof(info));

			if(sysinfo(&info) < 0) {
				throw system_error(errno,system_category(),_("Can't get system information"));
			}

			float free = (float) info.freeswap;
			float total = (float) info.totalswap;
			float rc = (total-free) / total;

			return rc;

		};

	public:
		Agent(const xml_node &node) : Percent(node, _( "Used Swap Percentage" )) {
			Object::properties.icon = "utilities-system-monitor";
			load(internal_states,sizeof(internal_states)/sizeof(internal_states[0]));
		}

		virtual ~Agent() {
		}

	};

	SysInfo::SwapUsed::SwapUsed() : Udjat::Factory("swap-used",moduleinfo) {
	}

	SysInfo::SwapUsed::~SwapUsed() {
	}

	std::shared_ptr<Abstract::Agent> SysInfo::SwapUsed::AgentFactory(const Abstract::Object &parent, const pugi::xml_node &node)  const {
		return make_shared<Agent>(node);
	}


 }
