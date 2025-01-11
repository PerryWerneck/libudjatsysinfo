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

 #include <config.h>
 #include <udjat/defs.h>
 #include <string>
 #include <udjat/tools/xml.h>

 namespace Udjat {

	namespace Storage {

		/// @brief Disk length unity.
		enum Unit : unsigned char {
			Byte,
			Kilobyte,
			Megabyte,
			Gigabyte,
			Terabyte
		};

		Unit UDJAT_API UnitFactory(const XML::Node &node);

	} // Storage
 }

 namespace std {

	std::string UDJAT_API to_string(const float value, const Udjat::Storage::Unit unit);

 }

