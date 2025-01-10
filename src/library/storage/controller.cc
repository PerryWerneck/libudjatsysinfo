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
 #include <udjat/defs.h>
 #include <udjat/tools/xml.h>
 #include <udjat/tools/container.h>
 #include <udjat/tools/storage/stat.h>
 #include <udjat/tools/logger.h>

 #include <private/storagecontroller.h>
 
 namespace Udjat {

	Storage::Controller & Storage::Controller::getInstance() {
		static Storage::Controller instance;
		return instance;
	}

	bool Storage::Controller::push_back(const Storage::Stat &stat) {

		for(const auto &st : *this) {
			if(st == stat) {
				return false;
			}
		}

		Logger::String{"Watching ",stat.name.c_str()}.trace();
		emplace_back(stat);

		return true;

	}

	void Storage::Controller::on_timer() {

		try {

			for(auto &stat : *this) {

				debug("Updating ",stat.devname.c_str());


			}

		} catch(const std::exception &e) {

			Logger::String{"Error on disk controller: ",e.what()}.error();
			
		}

	}

 }
