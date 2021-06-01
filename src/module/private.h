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

	namespace SysInfo {

		/// @brief CPU Load agent based on /proc/loadavg
		class UDJAT_API CPUAverage : public Abstract::Agent {
		private:
			uint8_t type = 0xff;

			/// @brief Number of cores.
			unsigned int cores = 0;

			/// @brief Active state.
			std::shared_ptr<State<float>> active_state;

			/// @brief Get CPU load, update active state.
			float get();

			/// @brief Agent states.
			std::vector<std::shared_ptr<State<float>>> states;

			/// @brief Use default states.
			void setDefaultStates();

			/// @brief Setup agent.
			void setup(uint8_t minutes);

		protected:

			/// @brief Get active state.
			std::shared_ptr<Abstract::State> find_state() const override;

		public:
			CPUAverage(const char *name = "cpu", uint8_t minutes = 1);
			virtual ~CPUAverage();

			void refresh() override;

			void get(const char *name, Json::Value &value) override;

			std::string to_string() const override;

			void append_state(const pugi::xml_node &node) override;

		};

	}

}


