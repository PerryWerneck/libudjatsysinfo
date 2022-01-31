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

 #pragma once

 #include <udjat/defs.h>
 #include <udjat/tools/disk/stat.h>
 #include <pugixml.hpp>
 #include <string>
 #include <list>

 namespace Udjat {

	namespace Disk {

		/// @brief Disk length unity.
		struct UDJAT_API Unit {
			float value;						///< @brief Conversion from bytes.
			const char *id;						///< @brief Unity APU.
			const char *speed;					///< @brief Label for 'speed' values.

			static const Unit * get(const pugi::xml_node &node, const char *attr = "size-unit", const char *def="M");
			static const Unit * get(const char *str = "MB");

		};

		/// @brief Disk stats from /proc/diskstats.
		struct UDJAT_API Stat {

			unsigned short major = 0;			///< @brief The major number of the disk.
			unsigned short minor = 0;			///< @brief The minor number of the disk.

			std::string name;					///< @brief Device name.

			struct {
				unsigned long count = 0;		///< @brief The total number of reads completed successfully.
				unsigned long merged = 0;		///< @brief The number of reads merged.
				unsigned long blocks = 0;		///< @brief The total number of sectors read successfully.
				unsigned int  time = 0;			///< @brief The total number of milliseconds spent by all reads.
			} read;

			struct {
				unsigned long count = 0;		///< @brief The total number of writes completed successfully.
				unsigned long merged = 0;		///< @brief The number of writes merged.
				unsigned long blocks = 0;		///< @brief The total number of sectors written successfully.
				unsigned int  time = 0;			///< @brief The total number of milliseconds spent by all writes.
			} write;

			struct {
				unsigned int  inprogress = 0;	///< @brief The number of I/Os currently in progress.
				unsigned int  time = 0;			///< @brief The time spent doing I/Os.
				unsigned int  weighted = 0;		///< @brief Total wait time for all requests.
			} io;

			struct {
				unsigned long count = 0;		///< @brief The total number of discards completed successfully.
				unsigned long merged = 0;		///< @brief The number of discards merged.
				unsigned long blocks = 0;		///< @brief The total number of sectors discarded successfully.
				unsigned long time = 0;			///< @brief The number of milliseconds spent discarding.
			} discards;

			/// @brief Create an empty device.
			Stat() {
			}

			/// @brief Is a physical disk?
			inline bool physical() const noexcept {
				return major != 0 && minor == 0;
			}

			/// @brief Is a logical disk?
			inline bool logical() const noexcept {
				return major != 0 && minor != 0;
			}

			/// @brief Get from device name.
			/// @param name Device name (ex: sda) or empty to all disks.
			Stat(const char *name);

			/// @brief Load /proc/diskstats.
			static std::list<Stat> get();

			/// @brief Get device sector size (in bytes).
			size_t getBlockSize() const;

			Stat & operator+=(const Stat &s);

			/// @brief Convenience data for disk read/write speed computation.
			struct Data {

				float read = 0;						///< @brief The read speed in bytes/second.
				float write = 0;					///< @brief The write speed in bytes/second.

				/// @brief Values from last cycle.
				struct {

					struct {
						float bytes = 0;				///< @brief The total number of bytes read successfully.
						unsigned int time = 0;			///< @brief The total number of milliseconds spent by all reads.
					} read;

					struct {
						float bytes = 0;				///< @brief The total number of bytes written successfully.
						unsigned int time = 0;			///< @brief The total number of milliseconds spent by all writes.
					} write;

				} saved;

				Data & operator+=(const Disk::Stat &stat);
				Data() = default;
				Data(const Disk::Stat &stat);

				Data & operator+=(const Data &data);
				Data & update(const Data &data);

			};

			/// @brief Reset values for computation on next cycle.
			Data & reset(Data &data) const;

			/// @brief Compute read/write speed based on saved data.
			Data & compute(Data &data) const;

		};

	}

 }

 namespace std {

	inline string to_string(const Udjat::Disk::Stat &st) {
		return st.name;
	}

	inline ostream& operator<< (ostream& os, const Udjat::Disk::Stat &st) {
		return os << st.name;
	}

 }
