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

 #include <udjat/agent/swapusage.h>
 #include <udjat/agent/percentage.h>
 #include <udjat/agent.h>
 #include <udjat/tools/xml.h>
 #include <udjat/tools/intl.h>

 #include <memory>
 #include <sys/sysinfo.h>

 using namespace std;

 namespace Udjat {

	std::shared_ptr<Abstract::Agent> System::SwapUsage::Factory::AgentFactory(const Abstract::Object &, const XML::Node &node) const {
		debug("--- Building Swap Usage agent");
		return std::make_shared<SwapUsage>(node);
	}

	System::SwapUsage::SwapUsage(const char *name) : Agent<Percentage>{name} {
	}

	System::SwapUsage::SwapUsage(const XML::Node &node) : Agent<Percentage>{node} {
	}

	System::SwapUsage::~SwapUsage() {

	}

	bool System::SwapUsage::refresh() {

		struct sysinfo info;
		memset(&info,0,sizeof(info));

		if(sysinfo(&info) < 0) {
			throw system_error(errno,system_category(),_("Can't get system information"));
		}

		float free = (float) info.freeswap;
		float total = (float) info.totalswap;
		float usage = (total-free) / total;

		return set(usage);
	}

	static inline bool is_empty(const char *str) noexcept {
		return !(str && *str);
	}

	std::shared_ptr<Abstract::State> System::SwapUsage::computeState() {

		if(!states.empty()) {
			return super::computeState();
		}

		static const struct  {
			float value;			///< @brief State max value.
			const char * name;		///< @brief State name.
			Udjat::Level level;		///< @brief State level.
			const char * summary;	///< @brief State summary.
			const char * body;		///< @brief State description
		} default_states[] = {
			{
				0.1,
				"low",
				Udjat::ready,
				N_( "Swap usage is lower than 10%" ),
				""
			},
			{
				0.5,
				"low",
				Udjat::ready,
				N_( "Swap usage is lower than 50%" ),
				""
			},
			{
				0.8,
				"medium",
				Udjat::warning,
				N_( "Swap usage is lower than 80%" ),
				""
			},
			{
				0.9,
				"high",
				Udjat::error,
				N_( "Swap usage is lower than 90%" ),
				""
			},
			{
				1.0,
				"critical",
				Udjat::critical,
				N_( "Swap usage is higher than 90%" ),
				""
			}
		};	

		float current = (float) this->get();
		for(const auto &state : default_states) {
			if(current < state.value) {
				return make_shared<Abstract::State>(
							state.name,
							state.level,
#ifdef GETTEXT_PACKAGE
							dgettext(GETTEXT_PACKAGE,state.summary),
							dgettext(GETTEXT_PACKAGE,state.body)
#else
							state.summary,
							state.body
#endif // GETTEXT_PACKAGE
						);
			}
		}

		return Abstract::Agent::computeState();
	}

 }
