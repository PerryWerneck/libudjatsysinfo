/* SPDX-License-Identifier: LGPL-3.0-or-later */

/*
 * Copyright (C) 2023 Perry Werneck <perry.werneck@gmail.com>
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
 #include <udjat/version.h>
 #include <udjat/module/abstract.h>

 // Agent types
 #include <udjat/agent/systime.h>
 #include <udjat/agent/loadavg.h>
 #include <udjat/agent/swapusage.h>
 #include <udjat/agent/memusage.h>
 #include <udjat/agent/uptime.h>
 #include <udjat/tools/actions/storage.h>

 namespace Udjat {

	namespace SysInfo {

		/// @brief Generic system information module.
		class UDJAT_API Module : public Udjat::Module {
		private:
			System::Time::Factory			systimefactory;
			System::LoadAverage::Factory	loadavgfactory;
			System::SwapUsage::Factory		swapusagefactory;
			System::MemoryUsage::Factory	memusagefactory;
			System::UpTime::Factory			uptimefactory;
			Storage::Action::Factory		storagefactory;	

		public:

			Module(const char *name = "SysInfo");
			virtual ~Module();

		};

	}

 }
