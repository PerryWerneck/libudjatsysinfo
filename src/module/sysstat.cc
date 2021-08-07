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
 #include <iostream>
 #include <sstream>
 #include <fstream>
 #include <iomanip>
 #include <udjat/tools/system/stat.h>

 #include "private.h"

 using namespace std;

 namespace Udjat {

	static const Udjat::ModuleInfo moduleinfo{
		PACKAGE_NAME,												// The module name.
		"Get System stats", 										// The module description.
		PACKAGE_VERSION, 											// The module version.
		PACKAGE_URL, 												// The package URL.
		PACKAGE_BUGREPORT 											// The bug report address.
	};

	static System::Stat::Type getFieldFromNode(const xml_node &node) {
		return System::Stat::getIndex(Attribute(node,"field-name").as_string("total"));
	}

	class SysInfo::SysStat::Agent : public Abstract::Agent {
	private:

		/// @brief System stat state.
		class State : public Udjat::State<float> {
		private:
			System::Stat::Type type = System::Stat::TOTAL;

		public:
			State(const xml_node &node) : Udjat::State<float>(node), type(getFieldFromNode(node)) {
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

		std::shared_ptr<Abstract::State> stateFromValue() const override {

			for(auto state : states) {

				if(state->compare(getValue(state->getType()) * 100)) {
					return state;
				}

			}

			return Abstract::Agent::stateFromValue();
		}


	public:
		Agent(const xml_node &node) : Abstract::Agent("sysstat"), type(getFieldFromNode(node)) {

			this->icon = "utilities-system-monitor";
			this->summary = System::Stat::getSummary(this->type);
			this->label = System::Stat::getLabel(this->type);

			Abstract::Agent::load(node);

			if(!getUpdateInterval()) {
				throw runtime_error("System stats requires an update timer");
			}

			for(size_t ix = 0; ix < N_ELEMENTS(values); ix++) {
				values[ix] = 0;
			}

		}

		Value & get(Value &value) override {
			return value.setFraction(getValue(this->type));
		}

		void get(const Request &request, Response &response) override {

			Abstract::Agent::get(request,response);

			auto &cpu = response["details"];
			for(size_t ix = 0; ix < N_ELEMENTS(values); ix++) {
				cpu[System::Stat::typenames[ix]].setFraction(values[ix]);
			}

		}

		void get(const Request &request, Report &report) override {

			report.start("name","label","summary","value",nullptr);

			for(size_t ix = 0; ix < N_ELEMENTS(values); ix++) {
				report 	<< System::Stat::typenames[ix]
						<< System::Stat::getLabel((System::Stat::Type) ix)
						<< System::Stat::getSummary((System::Stat::Type) ix)
						<< (values[ix] * 100);

			}
		}

		std::string to_string() const override {
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
				cout << System::Stat::typenames[ix] << " = " << fixed << setprecision(2) << (values[ix] * 100) << " %" << endl;
#endif // DEBUG
			}

			updated(true);
			return true;
		}

		void append_state(const pugi::xml_node &node) override {
			states.push_back(std::make_shared<State>(node));
		}

	};

	SysInfo::SysStat::SysStat() : Udjat::Factory("system-stat",&moduleinfo) {
	}

	SysInfo::SysStat::~SysStat() {
	}

	bool SysInfo::SysStat::parse(Abstract::Agent &parent, const pugi::xml_node &node) const {
		parent.insert(make_shared<Agent>(node));
		return true;
	}

 }
