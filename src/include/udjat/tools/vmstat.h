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

/**
 * @brief Declares VMStat object.
 *
 * References:
 *
 *	<https://www.linuxhowtos.org/System/procstat.htm>
 *
 */

#pragma once

#include <udjat/udjat/defs.h>

namespace Udjat {

	class UDJAT_API SysStat {
	private:

		struct {
			unsigned long user;		///< @brief normal processes executing in user mode
			unsigned long nice;		///< @brief niced processes executing in user mode
			unsigned long system;	///< @brief processes executing in kernel mode
			unsigned long idle;		///< @brief twiddling thumbs
			unsigned long iowait;	///< @brief waiting for I/O to complete
			unsigned long irq;		///< @brief servicing interrupts
			unsigned long softirq;	///< @brief servicing softirqs
		} cpu;

	public:
		SysStat();

	};

}
