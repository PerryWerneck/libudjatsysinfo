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

 #pragma once

 #include <udjat/defs.h>
 #include <udjat/tools/actions/abstract.h>
 #include <udjat/tools/disk/stat.h>
 #include <string>

 namespace Udjat {

	namespace Storage {

		class UDJAT_API Action : public Udjat::Action {
		private:
			Disk::Unit unit;

		public:

			class Factory : public Udjat::Action::Factory {
			public:
				Factory(const char *name = "storage") : Udjat::Action::Factory{name} {
				}

				std::shared_ptr<Udjat::Action> ActionFactory(const XML::Node &node) const override;

			};

			Action(const XML::Node &node);

			int call(Udjat::Request &request, Udjat::Response &response, bool except) override;

		};


	}

 }

