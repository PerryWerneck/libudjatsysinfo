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
 #include <udjat/tools/actions/abstract.h>
 #include <udjat/tools/actions/storage.h>
 #include <udjat/tools/response.h>
 #include <udjat/tools/report.h>
 #include <udjat/tools/disk/stat.h>
 #include <private/diskcontroller.h>
 #include <udjat/tools/string.h>
 #include <memory>
 
 using namespace std;

 namespace Udjat {

	std::shared_ptr<Udjat::Action> Storage::Action::Factory::ActionFactory(const XML::Node &node) const {
		return make_shared<Storage::Action>(node);	
	}

	Storage::Action::Action(const XML::Node &node) : Udjat::Action{node}, unit{Disk::UnitFactory(node)} {
		
		auto &controller = Disk::Controller::getInstance();
		for(const auto &unit : Disk::Stat::get()) {
			if(unit.physical()) {
				controller.push_back(unit);
			}
		}

	}

	Storage::Action::~Action() {
	}

	int Storage::Action::call(Udjat::Request &request, Udjat::Response &response, bool except) {

		return exec(response,except,[&]() -> int {

			auto &report = response.ReportFactory(
				"name",
				"read",
				"write",

				nullptr
			);

			for(const auto &unit : Disk::Controller::getInstance()) {
			
				report	<< unit.devname
						<< to_string(unit.read,this->unit)
						<< to_string(unit.write,this->unit);

			}
		
		});

	}

 }

