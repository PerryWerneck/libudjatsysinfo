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
		class Percent : public Udjat::Agent<float> {
		public:

			struct StateDescription {
				float value;				///< @brief State max value.
				const char * name;		///< @brief State name.
				Udjat::Level level;		///< @brief State level.
				const char * summary;	///< @brief State summary.
				const char * body;		///< @brief State description
			};

		protected:

			/// @brief Load states (if necessary).
			/// @param states List of states to load.
			/// @param length length of the state list.
			void load(const StateDescription *states, size_t length);

			/// @brief Get Value from system.
			virtual float getValue() const = 0;

		public:

			Percent(const char *name);
			virtual ~Percent();

			bool refresh() override;

			Udjat::Value & get(Udjat::Value &value) const override;

			std::string to_string() const noexcept override;

			std::shared_ptr<Abstract::State> StateFactory(const pugi::xml_node &node) override;

		};

		class SwapUsed : public Udjat::Factory {
		private:
			class Agent;

		public:
			SwapUsed();
			virtual ~SwapUsed();

			std::shared_ptr<Abstract::Agent> AgentFactory(const Abstract::Object &parent, const pugi::xml_node &node) const override;

		};

		class LoadAverage : public Udjat::Factory {
		private:
			class Agent;

		public:
			LoadAverage();
			virtual ~LoadAverage();

			std::shared_ptr<Abstract::Agent> AgentFactory(const Abstract::Object &parent, const pugi::xml_node &node) const override;

		};

		class MemUsed : public Udjat::Factory {
		private:
			class Agent;

		public:
			MemUsed();
			virtual ~MemUsed();

			std::shared_ptr<Abstract::Agent> AgentFactory(const Abstract::Object &parent, const pugi::xml_node &node) const override;

		};

		class UpTime : public Udjat::Factory {
		private:
			class Agent;

		public:
			UpTime();
			virtual ~UpTime();

			std::shared_ptr<Abstract::Agent> AgentFactory(const Abstract::Object &parent, const pugi::xml_node &node) const override;

		};

		class SysTime : public Udjat::Factory {
		private:
			class Agent;

		public:
			SysTime();
			virtual ~SysTime();

			std::shared_ptr<Abstract::Agent> AgentFactory(const Abstract::Object &parent, const pugi::xml_node &node) const override;

		};


		class DiskStat : public Udjat::Factory {
		private:
			class Agent;

		public:
			DiskStat();
			virtual ~DiskStat();

			std::shared_ptr<Abstract::Agent> AgentFactory(const Abstract::Object &parent, const pugi::xml_node &node) const override;

		};

		class SysStat : public Udjat::Factory {
		private:
			class Agent;

		public:
			SysStat();
			virtual ~SysStat();

			std::shared_ptr<Abstract::Agent> AgentFactory(const Abstract::Object &parent, const pugi::xml_node &node) const override;

		};


	}

}


