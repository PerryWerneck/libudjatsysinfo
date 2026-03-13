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

 /// @brief Declares partition monitor.
 
 #include <udjat/defs.h>
 #include <udjat/agent/abstract.h>
 #include <udjat/tools/xml.h>
 
 namespace Udjat {

	namespace Disk {

		/// @brief Agent watching a logical disk.
		class UDJAT_API Logical : public Abstract::Agent {
		protected:

			/// @brief Device name.
			const char *devname;

		public:

			class Factory : public Abstract::Agent::Factory {
			public:
				Factory(const char *name = "LogicalDisk") : Udjat::Abstract::Agent::Factory{name} {
				}

				std::shared_ptr<Abstract::Agent> AgentFactory(const XML::Node &node) const override;
			};

			static String NameFactory(const char * devname, bool required = true);
			static String DeviceNameFactory(const char * devname, bool required = true);
			static String DeviceNameFactory(const XML::Node &node, bool required = true);

			Logical(const char *name);
			Logical(const pugi::xml_node &node);
			Logical(const char *name, const pugi::xml_node &node);
			virtual ~Logical();

			/// @brief Get device status, update state.
			bool refresh() override;

			std::shared_ptr<Abstract::State> computeState() override;

		};

	}

 }

