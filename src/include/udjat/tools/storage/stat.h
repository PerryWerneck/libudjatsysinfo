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
 #include <udjat/tools/storage/stat.h>
 #include <pugixml.hpp>
 #include <string>
 #include <list>
 #include <udjat/tools/string.h>

 namespace Udjat {

	namespace Storage {

		/// @brief Disk stats from /proc/diskstats.
		class UDJAT_API Stat {
		public:

			class Device : public String {
			public:
			
				static String NameFactory(const char * devname, bool required = true);
				static String NameFactory(const XML::Node &node, bool required = true);

				Device() : String{} {
				}

				Device(const char *name, bool required = true) : String{NameFactory(name,required)} {
				}

				Device(const XML::Node &node, bool required = true) : String{NameFactory(node,required)} {
				}

				inline const char *name() const {
					return c_str();
				}

				inline bool operator==(const char *n) const {
					return strcasecmp(c_str(),n) == 0;
				}	

				inline bool operator==(const Device &d) const {
					return strcasecmp(c_str(),d.c_str()) == 0;
				}	

			};

			Device device;

			unsigned short major = 0;			///< @brief The major number of the disk.
			unsigned short minor = 0;			///< @brief The minor number of the disk.

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

			/// @brief Build an empty device.
			Stat() {
			}

			/// @brief Build device by name.
			/// @param name Device name (ex: sda) or empty to all disks.
			Stat(const char *name);

			/// @brief Load device stats from system.
			void load();

			/// @brief Is a logical disk?
			inline bool logical() const noexcept {
				return major != 0 && minor != 0;
			}

			/// @brief Is a physical device?
			bool physical() const;

			/// @brief Load /proc/diskstats.
			static std::list<Stat> get();

			/// @brief Get device sector size (in bytes).
			size_t blocksize() const;

			Stat & operator+=(const Stat &s);

			inline bool operator==(const Stat &s) const {
				return major == s.major && minor == s.minor;
			}	

			inline bool operator==(const char *name) const {
				return device == name;
			}	

			inline const char *name() const {
				return device.c_str();
			}

		};

	}

 }

 namespace std {

	inline string to_string(const Udjat::Storage::Stat &st) {
		return st.device;
	}

	inline ostream& operator<< (ostream& os, const Udjat::Storage::Stat &st) {
		return os << st.device;
	}

 }
