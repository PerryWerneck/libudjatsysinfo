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

	static const struct {
		const char *name;
		const char *label;
		const char *summary;
	} stats[] = {
		{
			"user",		// 0
			"Normal processes",
			"CPU used by normal processes executing in user mode"
		},
		{
			"nice",		// 1
			"Niced processes",
			"CPU used by niced processes executing in user mode"
		},
		{
			"system",	// 2
			"Kernel mode",
			"CPU used by processes executing in kernel mode"
		},
		{
			"idle",		// 3
			"IDLE",
			"twiddling thumbs"
		},
		{
			"iowait",	// 4
			"Waiting for I/O",
			"CPU waiting for I/O to complete"
		},
		{
			"irq",		// 5
			"Interrupts",
			"CPU used when servicing interrupts"
		},
		{
			"softirq",	// 6
			"Soft IRQS",
			"CPU used when servicing softirqs"
		}
	};

	static short getFieldFromNode(const xml_node &node) {

		const char * field = Attribute(node,"field-name").as_string("cpu-use");

		for(unsigned short ix = 0; ix < N_ELEMENTS(stats); ix++) {
			if(!strcasecmp(stats[ix].name,field)) {
				return ix;
			}
		}

		if(!strcasecmp("cpu-use",field)) {
			return -1;
		}

		throw runtime_error(string{"Invalid field name: '"} + field + "'");
	}

	class SysInfo::SysStat::Agent : public Abstract::Agent {
	private:

		/// @brief System stat state.
		class State : public Udjat::State<float> {
		private:
			short type = -1;

		public:
			State(const xml_node &node) : Udjat::State<float>(node), type(getFieldFromNode(node)) {
			}

			inline short getType() const noexcept {
				return type;
			}

		};

		/// @brief Agent states.
		std::vector<std::shared_ptr<State>> states;

		/// @brief Selected type.
		short type = -1;

		/// @brief Saved values from last cycle.
		unsigned long saved[N_ELEMENTS(stats)];

		/// @brief Current values (in %).
		float values[N_ELEMENTS(stats)];

		/// @brief Read values from /proc/stat.
		static void read(unsigned long * values ) {

			ifstream in("/proc/stat", ifstream::in);

			in.ignore(3);

			for(size_t ix = 0; ix < N_ELEMENTS(stats); ix++) {
				in >> values[ix];
			}

		}

	protected:

		float getValue(short type) const noexcept {

			if(type >= 0) {
				return values[type];
			}

			float total = 0;

			for(size_t ix = 0; ix < N_ELEMENTS(stats); ix++) {
				total += values[ix];
			}

			if(type == -1) {
				return total - values[3];
			}

			return 0;
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
		Agent(const xml_node &node) : Abstract::Agent("sysstat") {

			this->icon = "utilities-system-monitor";
			this->type = getFieldFromNode(node);

			if(this->type >= 0) {
				this->summary = stats[this->type].summary;
				this->label = stats[this->type].label;
			} else if(this->type == -1) {
				this->summary = "Total CPU used by all processes and interrupts";
				this->label = "CPU use";
			}

			Abstract::Agent::load(node);

			if(!getUpdateInterval()) {
				throw runtime_error("System stats requires an update timer");
			}

			for(size_t ix = 0; ix < N_ELEMENTS(stats); ix++) {
				values[ix] = 0;
			}

			read(saved);

		}

		Value & get(Value &value) override {
			return value.setFraction(getValue(this->type));
		}

		void get(const Request &request, Response &response) override {

			Abstract::Agent::get(request,response);

			auto &cpu = response["details"];
			for(size_t ix = 0; ix < N_ELEMENTS(stats); ix++) {
				cpu[stats[ix].name].setFraction(values[ix]);
			}

		}

		void get(const Request &request, Report &report) override {

			report.start("name","label","summary","value",nullptr);

			for(size_t ix = 0; ix < N_ELEMENTS(stats); ix++) {
				report 	<< stats[ix].name
						<< stats[ix].label
						<< stats[ix].summary
						<< (values[ix] * 100);

			}
		}

		std::string to_string() const override {
			std::stringstream out;
			out << std::fixed << std::setprecision(2) << (getValue(this->type) * 100) << "%";
			return out.str();
		}

		bool refresh() override {

			// Get current system usage.
			unsigned long current[N_ELEMENTS(stats)];
			read(current);

			// Compute differences from last cycle.
			float total = 0;
			for(size_t ix = 0; ix < N_ELEMENTS(stats); ix++) {
				unsigned long up = current[ix];
				if(current[ix] >= saved[ix]) {
					current[ix] -= saved[ix];
				} else {
					current[ix] = 0;
				}
				total += (float) current[ix];
				saved[ix] = up;
			}

			// Compute values on %
			for(size_t ix = 0; ix < N_ELEMENTS(stats); ix++) {

				values[ix] = ((float) current[ix]) / total;

#ifdef DEBUG
				cout << stats[ix].name << " = " << fixed << setprecision(2) << (values[ix] * 100) << " %" << endl;
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
