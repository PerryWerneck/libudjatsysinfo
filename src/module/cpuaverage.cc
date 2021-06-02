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

 #include "private.h"
 #include <udjat/tools/file.h>
 #include <sstream>
 #include <iomanip>

 namespace Udjat {

	SysInfo::CPUAverage::CPUAverage(const char *name, uint8_t minutes) : SysInfo::Agent(name) {
		setup(minutes);
		setDefaultStates();
	}

	void SysInfo::CPUAverage::setup(uint8_t minutes) {

		this->icon = "utilities-system-monitor";

		//
		// Identify the number of cores.
		//
		{
			this->cores = 0;
			Udjat::File::Text cpuinfo("/proc/cpuinfo");

			for(auto line : cpuinfo) {

				if(!strncasecmp(line->c_str(),"processor",9)) {
					cores++;
				}
			}

			cout << getName() << "\tNumber of CPU cores: " << cores << endl;

		}

		//
		// Setup by interval
		//
		static const struct {
			uint8_t minutes;
			const char *label;
			const char *summary;
		} types[] = {

			// 0 = 1 minute average.
			{
				1,
				"System usage in 1 minute",
				"Average system and I/O utilization of the last one minute"
			},

			// 1 = 5 minutes average.
			{
				5,
				"System usage in 5 minutes",
				"Average system and I/O utilization of the last five minutes"
			},

			// 2 = 15 minutes average.
			{
				15,
				"System usage in 15 minutes",
				"Average system and I/O utilization of the last fifteen minutes"
			}

		};

		// Identify type, setup values
		for(size_t type = 0; type < (sizeof(types)/sizeof(types[0])); type++) {
			if(minutes == types[type].minutes) {
				this->type = type;
				this->label = types[type].label;
				this->summary = types[type].summary;
				return;
			}
		}

		throw system_error(EINVAL,system_category(),"The update time should be 1, 5 or 15 minutes");

	}

	SysInfo::CPUAverage::~CPUAverage() {
	}

	void SysInfo::CPUAverage::setDefaultStates() {

		static const struct {
			float from;
			float to;
			const char 						* name;			///< @brief State name.
			Udjat::Level					  level;		///< @brief State level.
			const char						* summary;		///< @brief State summary.
			const char						* body;			///< @brief State description
		} states[] = {
			{
				0.0,
				50.0,
				"good",
				Udjat::ready,
				"CPU usage is lower than 50%",
				""
			},
			{
				50.0,
				80.0,
				"gt50",
				Udjat::warning,
				"CPU usage is higher than 50%",
				""
			},
			{
				80.0,
				95.0,
				"gt90",
				Udjat::error,
				"CPU usage is higher than 80%",
				""
			},
			{
				95.0,
				100,
				"full",
				Udjat::critical,
				"CPU usage is too high",
				""
			}
		};

		cout << getName() << "\tUsing default states" << endl;

		for(size_t ix = 0; ix < (sizeof(states)/ sizeof(states[0])); ix++) {

			this->states.push_back(
				make_shared<Udjat::State<float>>(
					states[ix].name,
					states[ix].from,
					states[ix].to,
					states[ix].level,
					states[ix].summary,
					states[ix].body
				)
			);

		}

	}

	float SysInfo::CPUAverage::get() {

		/*
		float values[3];

		Udjat::File::Text avg("/proc/loadavg");

#ifdef DEBUG
		cout << "avg=" << avg.c_str() << endl;
#endif // DEBUG

		// Can't use scanf or any other 'locale' dependant method.
		const char * ptr = avg.c_str();
		for(size_t ix = 0; ix < (sizeof(values)/sizeof(values[0])); ix++) {

			// Skip spaces.
			while(*ptr && isspace(*ptr))
				ptr++;

			if(!*ptr) {
				throw runtime_error("Unexpected value on /proc/loadavg");
			}

			// https://cboard.cprogramming.com/c-programming/169272-convert-string-float-without-using-library-functions.html
			float result = 0.0;
			float scale = 0.0;

			while(*ptr) {
				if((*ptr >= '0') && (*ptr <= '9')) {

					result = 10.0 * result + (float)(*ptr - '0');
					scale *= 10.0;

				} else if(*ptr == '.')

					scale = 1.0;

				else {
					break;
				}
				ptr++;
			}

			values[ix] = result / (scale == 0.0 ? 1.0 : scale);

		}
		*/

		double loadavg[3];

		if(getloadavg(loadavg,3) < 0) {
			throw system_error(EINVAL,system_category(),"Can't get system load average");
		}

		float rc = (loadavg[this->type] * 100) / ((double) this->cores);

		if(rc > 100.0) {
			rc = 100.0;
		}

		return set(rc);

	}

 }
