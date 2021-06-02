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

 namespace Udjat {

	SysInfo::Percent::Percent(const char *name) : Abstract::Agent(name) {
		active_state = make_shared<State<float>>("Undefined",0,Udjat::unimportant);
	}

	SysInfo::Percent::~Percent() {
	}

	std::shared_ptr<Abstract::State> SysInfo::Percent::find_state() const {
		return active_state;
	}

	void SysInfo::Percent::load(const StateDescription *states, size_t length) {

		if(this->hasStates())
			return;

		cout << getName() << "\tUsing default states" << endl;

		float from = 0;

		for(size_t ix = 0; ix < length; ix++) {

			this->states.push_back(
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

	float SysInfo::Percent::get() {

		float value = this->getValue();

		for(auto state : this->states) {
			if(state->compare(value)) {

				if(state != this->active_state) {
					// State has changed.
					this->active_state = state;
					updated(true);
				} else {
					// State has not changed.
					updated(false);
				}
				break;
			}
		}

		return value;

	}

	void SysInfo::Percent::refresh() {
		this->get();
	}

	void SysInfo::Percent::get(const char *name, Json::Value &value) {
		value[name] = this->get();
	}

	std::string SysInfo::Percent::to_string() const {
		std::stringstream out;
		out << std::fixed << std::setprecision(2) << const_cast<SysInfo::Percent &>(*this).get() << "%";
		return out.str();
	}

	void SysInfo::Percent::append_state(const pugi::xml_node &node) {
		this->states.push_back(std::make_shared<State<float>>(node));
	}

 }
