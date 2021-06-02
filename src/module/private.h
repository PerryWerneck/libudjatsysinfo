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

#include <config.h>
#include <udjat/defs.h>
#include <udjat/agent.h>
#include <udjat/factory.h>
#include <udjat/tools/sysconfig.h>
#include <sys/sysinfo.h>

using namespace pugi;
using namespace Udjat;
using namespace std;

namespace Udjat {

	namespace SysInfo {

		/// @brief Percentual value agent.
		class Percent : public Abstract::Agent {
		public:

			struct StateDescription {
				float value;				///< @brief State max value.
				const char * name;		///< @brief State name.
				Udjat::Level level;		///< @brief State level.
				const char * summary;	///< @brief State summary.
				const char * body;		///< @brief State description
			};

		private:

			/// @brief Active state.
			std::shared_ptr<State<float>> active_state;

			/// @brief Agent states.
			std::vector<std::shared_ptr<State<float>>> states;

		protected:

			/// @brief Load states (if necessary).
			/// @param states List of states to load.
			/// @param length length of the state list.
			void load(const StateDescription *states, size_t length);

			/// @brief Get Value from system.
			virtual float getValue() = 0;

		public:

			Percent(const char *name);
			virtual ~Percent();

			/// @brief Get active state.
			std::shared_ptr<Abstract::State> find_state() const override;

			void refresh() override;

			/// @brief Get updated value.
			float get();

			void get(const char *name, Json::Value &value) override;

			std::string to_string() const override;

			void append_state(const pugi::xml_node &node) override;

		};

		class SwapUsed : public Udjat::Factory {
		private:
			class Agent;

		public:
			SwapUsed();
			virtual ~SwapUsed();

			void parse(Abstract::Agent &parent, const pugi::xml_node &node) const override;

		};

		class LoadAverage : public Udjat::Factory {
		private:
			class Agent;

		public:
			LoadAverage();
			virtual ~LoadAverage();

			void parse(Abstract::Agent &parent, const pugi::xml_node &node) const override;

		};

		class MemUsed : public Udjat::Factory {
		private:
			class Agent;

		public:
			MemUsed();
			virtual ~MemUsed();

			void parse(Abstract::Agent &parent, const pugi::xml_node &node) const override;

		};

		class UpTime : public Udjat::Factory {
		private:
			class Agent;

		public:
			UpTime();
			virtual ~UpTime();

			void parse(Abstract::Agent &parent, const pugi::xml_node &node) const override;

		};

	}

}


