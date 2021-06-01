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

 #include "private.h"
 #include <udjat/tools/mainloop.h>

 namespace Udjat {

	SysUsage::Controller::Controller() {

#ifdef DEBUG
		cout << "sysusage\tStarting system usage controller" << endl;
#endif // DEBUG

		// Update values on every 10 seconds.
		MainLoop::getInstance().insert(this, 10, [this](const time_t now) {

			// Update timers.
			cout << "Time!" << endl;

			return true;
		});

	}

	SysUsage::Controller::~Controller() {

#ifdef DEBUG
		cout << "sysusage\tStopping system usage controller" << endl;
#endif // DEBUG

		MainLoop::getInstance().remove(this);
	}

	SysUsage::Controller & SysUsage::Controller::getInstance() {
		static Controller instance;
		return instance;
	}

 }

