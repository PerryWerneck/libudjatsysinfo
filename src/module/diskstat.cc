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
 #include "private.h"
 #include <udjat/disk/stat.h>
 #include <sstream>
 #include <iomanip>
 #include <sstream>
 #include <udjat/tools/xml.h>
 #include <sys/time.h>

 #include "private.h"

 namespace Udjat {

	static const struct Unit {
		float value;
		const char *id;
		const char *label;
	} units[] = {
		{          1.0,  "B",  "B/s"  },
		{       1024.0, "KB", "KB/s" },
		{       1024.0, "KB", "KB/s" },
		{    1048576.0, "MB", "MB/s" },
		{ 1073741824.0, "GB", "GB/s" }
	};

	static const Udjat::ModuleInfo moduleinfo{
		PACKAGE_NAME,												// The module name.
		"Get Disk Read/Write speed average", 						// The module description.
		PACKAGE_VERSION, 											// The module version.
		PACKAGE_URL, 												// The package URL.
		PACKAGE_BUGREPORT 											// The bug report address.
	};

	class SysInfo::DiskStat::Agent : public Abstract::Agent {
	private:
		unsigned short type;

		const Unit *unit= nullptr;

		struct {
			unsigned long timestamp;
			float read  = 0;
			float write = 0;
		} saved;

		/// @brief Read (Bytes/second);
		float read = 0;

		/// @brief Write (Bytes/second);
		float write = 0;

		static unsigned long getCurrentTime() {

			::timeval tv;

			if(gettimeofday(&tv, NULL) < 0) {
				throw system_error(errno,system_category(),"Cant get time of day");
			}

			return (tv.tv_sec * 1000) + (tv.tv_usec /1000);

		}

	public:
		Agent(const xml_node &node) : Abstract::Agent("diskstat"), type(Attribute(node,"stat-type").select("average","read","write",nullptr)) {

			this->icon = "utilities-system-monitor";
			this->unit = &units[0];

			// Load unit.
			{
				const char * unit = Attribute(node,"size-unit").as_string("MB");

				for(size_t ix = 0; ix < ((sizeof(units)/sizeof(units[0]))); ix++) {
					if(strcasecmp(unit,units[ix].id) == 0 || strcasecmp(unit,units[ix].label)) {
						this->unit = &units[ix];
						break;
					}
				}

			}

			static const char *labels[] = {
				"Average disk speed",
				"Read disk speed",
				"Write disk speed"
			};
			this->label = labels[type];

			Abstract::Agent::load(node);

			if(!getUpdateInterval()) {
				throw runtime_error("Disk stats requires an update time");
			}

			saved.timestamp = getCurrentTime();

			for(Disk::Stat &stat : Disk::Stat::get()) {

				if(stat.physical()) {
					float blocksize = (float) stat.getBlockSize();
					saved.read += (((float) stat.read.blocks) * blocksize);
					saved.write += (((float) stat.write.blocks) * blocksize);
				}

			}

		}

		virtual ~Agent() {
		}

		Udjat::Value &get(Udjat::Value &value) override {


			return value;
		}

		bool refresh() override {

			unsigned long now	= getCurrentTime();
			unsigned long msec = (now - saved.timestamp);

			cout << "msec=" << msec << endl;

			if(msec == 0) {
				return false;
			}

			float read = 0;
			float write = 0;
			for(Disk::Stat &stat : Disk::Stat::get()) {

				if(stat.physical()) {
					float blocksize = (float) stat.getBlockSize();
					read += (((float) stat.read.blocks) * blocksize);
					write += (((float) stat.write.blocks) * blocksize);
				}

			}

			// Compute read/write performances

			if(saved.read && read > saved.read) {
				this->read = (read - saved.read) / ((float) msec / 1000);
			} else {
				this->read = 0;
			}

			if(saved.write && write > saved.write) {
				this->write = (write - saved.write) / ((float) msec / 1000);
			} else {
				this->write = 0;
			}

#ifdef DEBUG
			cout << "Read: " << this->read << " Write: " << this->write << endl;
#endif // DEBUG

			// Save values for next read
			saved.timestamp = now;
			saved.read = read;
			saved.write = write;

			return true;
		}

		std::string to_string() const override {
			return TimeStamp().to_string();
		}
	};

	SysInfo::DiskStat::DiskStat() : Udjat::Factory("system-diskstat",&moduleinfo) {
	}

	SysInfo::DiskStat::~DiskStat() {
	}

	bool SysInfo::DiskStat::parse(Abstract::Agent &parent, const pugi::xml_node &node) const {
		parent.insert(make_shared<Agent>(node));
		return true;
	}

 }
