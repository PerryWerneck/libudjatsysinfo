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

 /// @brief Declares system time agent.
 
 #include <udjat/defs.h>
 #include <udjat/agent/percentage.h>
 #include <udjat/tools/timestamp.h>
 #include <udjat/agent.h>
 #include <udjat/agent/abstract.h>
 #include <udjat/tools/xml.h>
 #include <cstdlib>
 
 namespace Udjat {

	namespace System {

		class UDJAT_API LoadAverage : public Agent<Percentage> {
		private:
			size_t cores = 0;
			uint8_t type = 0;

			void setup(uint8_t minutes = 5);

		public:

			class Factory : public Abstract::Agent::Factory {
			public:
				Factory(const char *name = "LoadAverage") : Udjat::Abstract::Agent::Factory{name} {
				}

				std::shared_ptr<Abstract::Agent> AgentFactory(const XML::Node &node) const override;
			};

			LoadAverage(const char *name = "LoadAverage");
			LoadAverage(const XML::Node &node);
			virtual ~LoadAverage();

			void start() override;
			bool refresh(bool) override;

			std::shared_ptr<Abstract::State> computeState() override;


		};

	}

 }

