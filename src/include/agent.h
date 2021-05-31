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

#pragma once

#include <udjat/defs.h>
#include <udjat/agent.h>

using namespace Udjat;
using namespace std;

namespace Udjat {

	namespace VMStat {

		/// @brief CPU Usage agent.
		class CPUUsage : public Abstract::Agent {
		private:

			/// @brief Active state (the one selected by get() method).
			std::shared_ptr<State<float>> active_state;

			/// @brief Agent states.
			std::vector<std::shared_ptr<State<float>>> states;

			/// @brief Get value, update state.
			float get();

			/// @brief Load default status.
			void loadDefaultStates();

			/// @brief Find state (allways return active_state).
			std::shared_ptr<Abstract::State> find_state() const override;

		public:
			CPUUsage(const char *name = "cpuusage");
			virtual ~CPUUsage();

			void refresh() override;
			void get(const char *name, Json::Value &value) override;
			std::string to_string() const override;

			void append_state(const pugi::xml_node &node) override;
			bool hasStates() const noexcept override {
				return !states.empty();
			}


		};

	}

}


