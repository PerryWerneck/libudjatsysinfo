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

 #ifdef LOG_DOMAIN
	#undef LOG_DOMAIN
 #endif
 #define LOG_DOMAIN "storage"
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

		Logger::String{"Watching ",stat.name()}.trace();
		emplace_back(stat);

		return true;

	}

	void Storage::Controller::setup(const XML::Node &node) {
		
		debug("Setting up controller from <",node.name(),"> node (timer-interval=",node.attribute("timer-interval").as_uint(0),")");

		const char *domain = node.attribute("name").as_string(LOG_DOMAIN);

		auto saved_interval = interval();
		if(!saved_interval) {
			saved_interval = 60000L;
		}
		debug("---------------------> ",saved_interval);

		Timer::set(node);
		debug("Set to ",interval()," from xml");

		if(saved_interval < interval() || interval() == 0) {
			Timer::set(saved_interval);
			Logger::String{"Keeping original timer of ",Timer::interval(),"ms"}.trace(domain);
		} else {
			Logger::String{"Update timer set to ",Timer::interval(),"ms"}.trace(domain);
		}

		if(Timer::enable()) {
			Logger::String{"Auto update was enabled"}.trace(domain);
		}

	}

	void Storage::Controller::on_timer() {

		try {

			for(Data &stat : (std::vector<Data>) *this) {

				debug("Updating ",stat.name());

				try {

					// Update disk speed.
					stat.refresh();

					// Complete, reset error.
					stat.error.clear();

				} catch(const std::exception &e) {

					stat.error = e.what();
					Logger::String{"Error updating disk status: ",e.what()}.error();

				}

			}

		} catch(const std::exception &e) {

			Logger::String{"Error on disk controller: ",e.what()}.error();
			
		}

	}

 }
