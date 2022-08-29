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
 #include "private.h"
 #include <udjat/tools/file.h>
 #include <sstream>
 #include <iomanip>
 #include <udjat/tools/intl.h>

 #include "private.h"
 #include <udjat/moduleinfo.h>

 namespace Udjat {

	static const Udjat::ModuleInfo moduleinfo {"Get system load average use in the latest 1, 5 or 15 minutes"};

	static const SysInfo::Percent::StateDescription internal_states[] = {
		{
			0.5,
			"good",
			Udjat::ready,
			N_( "System load is lower than 50%" ),
			""
		},
		{
			0.8,
			"gt50",
			Udjat::warning,
			N_( "System load is higher than 50%" ),
			""
		},
		{
			0.95,
			"gt90",
			Udjat::error,
			N_( "System load is higher than 80%" ),
			""
		},
		{
			1.0,
			"full",
			Udjat::critical,
			N_( "System load is too high" ),
			""
		}
	};

	class SysInfo::LoadAverage::Agent : public Percent {
	private:

		uint16_t cores;
		uint8_t type;

		/// @brief Setup agent by interval
		void setup(uint8_t minutes) {

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

			// Identify type, setup values
			for(size_t type = 0; type < (sizeof(types)/sizeof(types[0])); type++) {
				if(minutes == types[type].minutes) {
					this->type = type;
					Object::properties.label = types[type].label;
					Object::properties.summary = types[type].summary;
					return;
				}
			}

			throw system_error(EINVAL,system_category(),"The update time should be 1, 5 or 15 minutes");

		}


	protected:

		float getValue() const override {

			double loadavg[3];

			if(getloadavg(loadavg,3) < 0) {
				throw system_error(EINVAL,system_category(),_("Can't get system load average"));
			}

			float rc = loadavg[this->type] / ((double) this->cores);

			if(rc > 100.0) {
				rc = 100.0;
			}

			return rc;
		};

	public:
		Agent(const xml_node &node) : Percent("loadavg") {

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

			Abstract::Agent::load(node);

			time_t timer = getUpdateInterval();

#ifdef DEBUG
			cout << "Update timer: " << timer << endl;
#endif // DEBUG

			if(!timer) {
				throw runtime_error("Update interval is required");
			}

			setup(timer/60);
			Abstract::Agent::load(node);
			load(internal_states,sizeof(internal_states)/sizeof(internal_states[0]));

		}

		virtual ~Agent() {
		}

	};

	SysInfo::LoadAverage::LoadAverage() : Udjat::Factory("load-average",moduleinfo) {
	}

	SysInfo::LoadAverage::~LoadAverage() {
	}

	bool SysInfo::LoadAverage::parse(Abstract::Agent &parent, const pugi::xml_node &node) const {
		parent.insert(make_shared<Agent>(node));
		return true;
	}

 }
