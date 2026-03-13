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

 /*
  * https://stackoverflow.com/questions/3017162/how-to-get-total-cpu-usage-in-linux-using-c
  *
  * Read /proc/cpuinfo to find the number of CPU/cores available to the systems.
  * Call the getloadavg() (or alternatively read the /proc/loadavg), take the first value,
  * multiply it by 100 (to convert to percents), divide by number of CPU/cores.
  * If the value is greater than 100, truncate it to 100. Done.
  *
  * Relevant documentation: man getloadavg and man 5 proc
  *
  */

 #include <config.h>
 #include <udjat/defs.h>

 #include <udjat/agent/abstract.h>
 #include <udjat/agent/percentage.h>
 #include <udjat/agent/loadavg.h>
 #include <udjat/tools/file/text.h>
 #include <udjat/tools/intl.h>
 #include <udjat/tools/logger.h>
 #include <udjat/tools/xml.h>
 #include <sstream>
 #include <iomanip>
 #include <memory>

 using namespace std;

 namespace Udjat {

	std::shared_ptr<Abstract::Agent> System::LoadAverage::Factory::AgentFactory(const XML::Node &node) const {
		debug("--- Building Load Average agent");
		return std::make_shared<LoadAverage>(node);
	}

	System::LoadAverage::LoadAverage(const char *name) : Agent<Percentage>{name} {
		setup(5);
	}

	System::LoadAverage::LoadAverage(const XML::Node &node) : Agent<Percentage>{node} {
		setup(XML::AttributeFactory(node,"minutes").as_uint(5));
	}

	System::LoadAverage::~LoadAverage() {

	}

	void System::LoadAverage::start() {
		refresh();
		Abstract::Agent::start();
	}

	bool System::LoadAverage::refresh() {

#ifdef HAS_GETLOADAVG

		double loadavg[3];

		if(getloadavg(loadavg,3) < 0) {
			throw system_error(EINVAL,system_category(),_("Can't get system load average"));
		}

		float rc = loadavg[this->type] / ((double) this->cores);

		if(rc > 100.0) {
			rc = 100.0;
		}

		return Agent<Percentage>::set((float) (rc/100));
#else

		logger::String{"No support for getloadavg() function"}.error(name());
		return Agent<Percentage>::set(0);

#endif // HAS_GETLOADAVG

	}

	static inline bool is_empty(const char *str) noexcept {
		return !(str && *str);
	}

	void System::LoadAverage::setup(uint8_t minutes) {

		Logger::String{"Getting samples for every ",minutes,(minutes == 1 ? " minute" : " minutes")}.trace(name());

		//
		// Identify the number of cores.
		//
		{
			cores = 0;
			Udjat::File::Text cpuinfo("/proc/cpuinfo");
			for(auto line : cpuinfo) {
				if(!strncasecmp(line->c_str(),"processor",9)) {
					cores++;
				}
			}
			info() << "Number of CPU cores: " << cores << endl;
		}

		//
		// Setup agent
		//
		Object::properties.icon = "utilities-system-monitor";

		static const struct {
			uint8_t minutes;
			const char *label;
			const char *summary;
		} types[] = {

			// 0 = 1 minute average.
			{
				1,
				N_( "System load in the last minute" ),
				N_( "Average system load in the last minute" )
			},

			// 1 = 5 minutes average.
			{
				5,
				N_( "System load in the last 5 minutes" ),
				N_( "Average system load in the last five minutes" )
			},

			// 2 = 15 minutes average.
			{
				15,
				N_( "System load in the last 15 minutes" ),
				N_( "Average system load in the last fifteen minutes" )
			}

		};

		for(size_t type = 0; type < (sizeof(types)/sizeof(types[0])); type++) {
			if(minutes == types[type].minutes) {
				type = type;

				if(is_empty(Object::properties.label)) {
#ifdef GETTEXT_PACKAGE
					Object::properties.label = dgettext(GETTEXT_PACKAGE,types[type].label);
#else
					Object::properties.label = types[type].label;
#endif				
				}

				if(is_empty(Object::properties.summary)) {
#ifdef GETTEXT_PACKAGE
					Object::properties.summary = dgettext(GETTEXT_PACKAGE,types[type].summary);
#else
					Object::properties.summary = types[type].summary;
#endif				
				}

				return;
			}
		}

		throw system_error(EINVAL,system_category(),"The update time should be 1, 5 or 15 minutes");

	}

	std::shared_ptr<Abstract::State> System::LoadAverage::computeState() {

		float current = (float) this->get();
		for(auto state : states) {
			if(state->compare(current))
				return state;
		}

		// No States, use the defaults.
		static const struct  {
			float value;			///< @brief State max value.
			const char * name;		///< @brief State name.
			Udjat::Level level;		///< @brief State level.
			const char * summary;	///< @brief State summary.
			const char * body;		///< @brief State description
		} default_states[] = {
			{
				0.5,
				"good",
				Udjat::ready,
				N_( "System load is ${value}" ),
				""
			},
			{
				0.8,
				"gt50",
				Udjat::warning,
				N_( "System load is ${value}" ),
				""
			},
			{
				0.95,
				"gt90",
				Udjat::error,
				N_( "System load is ${value}" ),
				""
			},
			{
				1.0,
				"full",
				Udjat::critical,
				N_( "System load is ${value}" ),
				""
			}
		};	

#ifdef DEBUG
		{
			string str;
			getProperty("value",str);
			debug(name()," --------------------------> ",str.c_str());
		}
#endif

		for(const auto &state : default_states) {
			if(current < state.value) {
				return Abstract::Agent::StateFactory(
					state.name,
					state.level,
#ifdef GETTEXT_PACKAGE
					dgettext(GETTEXT_PACKAGE,state.summary),
					dgettext(GETTEXT_PACKAGE,state.body)
#else
					state.summary,
					state.body
#endif // GETTEXT_PACKAGE
				);
			}
		}
		
		return Abstract::Agent::computeState();
	}

 }
