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

 #pragma once

 #include <udjat/defs.h>
 #include <udjat/tools/disk/stat.h>
 #include <pugixml.hpp>
 #include <string>
 #include <vector>
 #include <udjat/tools/string.h>
 #include <udjat/tools/container.h>
 #include <udjat/tools/timer.h>

 namespace Udjat {

	namespace Disk {

		/// @brief Convenience data for disk read/write speed computation.
		struct Data {

			const std::string devname;

			float blocksize;					///< @brief The disk block size in bytes.
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

			Data(const Disk::Stat &stat, float bsize = 512.0) : devname{stat.name}, blocksize(bsize) {
			}

			inline bool operator==(const Stat &stat) const {
				return strcmp(devname.c_str(),stat.name.c_str()) == 0;
			}	

		};

		class UDJAT_PRIVATE Controller : private std::vector<Data>, private MainLoop::Timer {
		private:
		
			Controller() = default;
	
		protected:

			/// @brief Update disk speed.
			void on_timer() override;

		public:
			static Controller & getInstance();

			/// @brief Insert disk into controller.
			/// @param stat The disk to insert.
			/// @return true if the disk was inserted, false if it was already present.
			bool push_back(const Disk::Stat &stat);

			auto begin() const {
				return std::vector<Data>::begin();
			}

			auto end() const {
				return std::vector<Data>::end();
			}
			
		};



	}

 }

