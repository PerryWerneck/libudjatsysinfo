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

 #include "private.h"
 #include <sstream>
 #include <iomanip>
 #include <udjat/tools/value.h>

 namespace Udjat {

	SysInfo::Percent::Percent(const char *name) : Agent<float>(name) {
	}

	SysInfo::Percent::~Percent() {
	}

	void SysInfo::Percent::load(const StateDescription *states, size_t length) {

		if(this->hasStates())
			return;

		info() << "Using default states" << endl;

		float from = 0.0;

		for(size_t ix = 0; ix < length; ix++) {

			this->push_back(
				make_shared<Udjat::State<float>>(
					states->name,
					from,
					states->value,
					states->level,
					states->summary,
					states->body
				)
			);

			from = states->value;
			states++;
		}

	}

	void SysInfo::Percent::append_state(const pugi::xml_node &node) {

		class State : public Udjat::State<float> {
		public:
			State(const pugi::xml_node &node) : Udjat::State<float>(node) {
				from /= 100;
				to /= 100;
			}

		};

		push_back(std::make_shared<State>(node));

	}

	Udjat::Value & SysInfo::Percent::get(Udjat::Value &value) {
		return value.setFraction(Udjat::Agent<float>::get());
	}

	bool SysInfo::Percent::refresh() {
		set(this->getValue());
		return true;
	}

	std::string SysInfo::Percent::to_string() const {
		std::stringstream out;
		out << std::fixed << std::setprecision(2) << (Udjat::Agent<float>::get() * 100) << '%';
		return out.str();
	}

 }
