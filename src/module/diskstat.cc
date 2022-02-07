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
 #include <udjat/tools/disk/stat.h>
 #include <sstream>
 #include <iomanip>
 #include <sstream>
 #include <udjat/tools/xml.h>
 #include <sys/time.h>
 #include <sstream>

 namespace Udjat {

	static const Udjat::ModuleInfo moduleinfo{
		PACKAGE_NAME,												// The module name.
		"Get Disk Read/Write speed average", 						// The module description.
		PACKAGE_VERSION, 											// The module version.
		PACKAGE_URL, 												// The package URL.
		PACKAGE_BUGREPORT 											// The bug report address.
	};

	static const char *labels[] = {
		"Average disk speed",
		"Read disk speed",
		"Write disk speed"
	};

	class SysInfo::DiskStat::Agent : public Abstract::Agent {
	private:
		unsigned short type;

		const Disk::Unit *unit= nullptr;

		Disk::Stat::Data diskstat;

		/// @brief Get stats from all physical disks.
		/*
		static Disk::Stat & load(Disk::Stat &stat) {

			for(Disk::Stat &disk : Disk::Stat::get()) {

				if(disk.physical()) {
					stat += disk;
				}

			}

			return stat;

		}
		*/

		float getAverage() const {
			return ((diskstat.read + diskstat.write) / 2) / unit->value;
		}

		float getRead() const {
			return diskstat.read / unit->value;
		}

		float getWrite() const {
			return diskstat.write / unit->value;
		}

		float getValueByType() const {

			switch(type) {
			case 0: // Average disk speed
				return getAverage();

			case 1: // Read disk speed
				return getRead();

			case 2: // Write disk speed
				return getWrite();
			}

			return 0;

		}

	public:
		Agent(const xml_node &node) : Abstract::Agent("diskstat"), type(Attribute(node,"stat-type").select("average","read","write",nullptr)) {

			Object::properties.icon = "utilities-system-monitor";
			this->unit = Udjat::Disk::Unit::get(node);

			Abstract::Agent::load(node);

			if(!(Object::properties.label && *Object::properties.label)) {
				Object::properties.label = Quark(string{labels[type]} + " in " + unit->speed).c_str();
			}

			if(!getUpdateInterval()) {
				throw runtime_error("Disk stats requires an update timer");
			}

			for(Disk::Stat &disk : Disk::Stat::get()) {
				if(disk.physical()) {
					diskstat += Disk::Stat::Data(disk);
				}
			}

			/*
			load(st);
			st.reset(diskstat);
			*/

		}

		virtual ~Agent() {
		}

		Udjat::Value &getProperties(Udjat::Value &value) const noexcept override {
			value["value"] = getValueByType();
			return value;
		}

		bool refresh() override {

			Disk::Stat::Data current;
			for(Disk::Stat &disk : Disk::Stat::get()) {
				if(disk.physical()) {
					current += Disk::Stat::Data(disk);
				}
			}

			diskstat.update(current);

#ifdef DEBUG
			cout << "Read: " << this->diskstat.read << " Write: " << this->diskstat.write << endl;
#endif // DEBUG

			return true;
		}

		std::string to_string() const override {
			std::stringstream out;
			out << std::fixed << std::setprecision(2) << getValueByType() << " " << unit->speed;
			return out.str();
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
