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

 #include <iostream>
 #include <sstream>
 #include <fstream>
 #include <iomanip>
 #include <udjat/tools/system/stat.h>
 #include <udjat/agent/abstract.h>
 #include <udjat/moduleinfo.h>

 #include "private.h"

 using namespace std;

 namespace Udjat {

	static const Udjat::ModuleInfo moduleinfo{"Get System stats"};

	class SysInfo::SysStat::Agent : public Abstract::Agent {
	private:

		/// @brief System stat state.
		class State : public Udjat::State<float> {
		private:
			System::Stat::Type type = System::Stat::TOTAL;

		public:
			State(const xml_node &node) : Udjat::State<float>(node), type(System::Stat::TypeFactory(node)) {
			}

			inline System::Stat::Type getType() const noexcept {
				return type;
			}

		};

		/// @brief Agent states.
		std::vector<std::shared_ptr<State>> states;

		/// @brief Selected type.
		System::Stat::Type type = System::Stat::TOTAL;

		/// @brief Saved values from last cycle.
		System::Stat saved;

		/// @brief Current values (in %).
		float values[System::Stat::TOTAL];


	protected:

		float getValue(System::Stat::Type type) const noexcept {

			if(type < System::Stat::TOTAL) {
				return values[type];
			}

			float total = 0;

			for(size_t ix = 0; ix < N_ELEMENTS(values); ix++) {
				total += values[ix];
			}

			return total - values[System::Stat::IDLE];

		}

		std::shared_ptr<Abstract::State> computeState() override {

			for(auto state : states) {

				if(state->compare(getValue(state->getType()) * 100)) {
					return state;
				}

			}

			return Abstract::Agent::computeState();
		}


	public:
		Agent(const xml_node &node) : Abstract::Agent(node), type(System::Stat::TypeFactory(node)) {

			Object::properties.icon = "utilities-system-monitor";
			Object::properties.summary = System::Stat::getSummary(this->type);
			Object::properties.label = System::Stat::getLabel(this->type);

			if(!timer()) {
				throw runtime_error("System stats requires an update timer");
			}

			for(size_t ix = 0; ix < N_ELEMENTS(values); ix++) {
				values[ix] = 0;
			}

		}

		Value & get(Value &value) const override {
			value.setFraction(getValue(this->type));
			return value;
		}


		Udjat::Value & getProperties(Udjat::Value &value) const noexcept override {

			super::getProperties(value);

			auto &cpu = value["cpu"];
			for(size_t ix = 0; ix < N_ELEMENTS(values); ix++) {
				(cpu[std::to_string((System::Stat::Type) ix)]).setFraction(values[ix]);
			}

			return value;
		}

		bool getProperties(const char *path, Response::Table &report) const override {

			if(super::getProperties(path,report)) {
				return true;
			}

			if(*path) {
				return false;
			}

			report.start("name","label","summary","value",nullptr);

			for(size_t ix = 0; ix < N_ELEMENTS(values); ix++) {
				report 	<< string{std::to_string((System::Stat::Type) ix)}
						<< string{System::Stat::getLabel((System::Stat::Type) ix)}
						<< string{System::Stat::getSummary((System::Stat::Type) ix)}
						<< std::to_string((values[ix] * 100));

			}

			return true;
		}

		std::string to_string() const noexcept override {
			std::stringstream out;
			out << std::fixed << std::setprecision(2) << (getValue(this->type) * 100) << "%";
			return out.str();
		}

		bool refresh() override {

			System::Stat current;
			System::Stat diff(current);

			diff -= this->saved;	// Get only the difference.
			this->saved = current;	// Update saved value.

			float total = (float) diff.total();
			for(size_t ix = 0; ix < N_ELEMENTS(values); ix++) {
				values[ix] = ((float) diff[(System::Stat::Type) ix]) / total;
#ifdef DEBUG
				cout << ((System::Stat::Type) ix) << " = " << fixed << setprecision(2) << (values[ix] * 100) << " %" << endl;
#endif // DEBUG
			}

			updated(true);
			return true;
		}

		std::shared_ptr<Abstract::State> StateFactory(const pugi::xml_node &node) override {
			auto state = std::make_shared<State>(node);
			states.push_back(state);
			return state;
		}

	};

	SysInfo::SysStat::SysStat() : Udjat::Factory("system-stat",moduleinfo) {
	}

	SysInfo::SysStat::~SysStat() {
	}

	std::shared_ptr<Abstract::Agent> SysInfo::SysStat::AgentFactory(const Abstract::Object UDJAT_UNUSED(&parent), const pugi::xml_node &node)  const {
		return make_shared<Agent>(node);
	}

 }
