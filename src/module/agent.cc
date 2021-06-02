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

 }
