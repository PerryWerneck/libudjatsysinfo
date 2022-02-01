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

 // https://www.kernel.org/doc/html/latest/filesystems/proc.html#miscellaneous-kernel-statistics-in-proc-stat

 #include <iostream>
 #include <sstream>
 #include <fstream>
 #include <iomanip>
 #include <cstring>
 #include <udjat/tools/system/stat.h>
 #include <ostream>

 using namespace std;

 static const char * typenames[] = {
	"user",
	"nice",
	"system",
	"idle",
	"iowait",
	"irq",
	"softirq",
	"steal",
	"guest",
	"guest_nice",
	"total"
 };

 static const Udjat::System::Stat::TypeInfo typeinfo[] = {
	{
		"Normal processes",
		"CPU used by normal processes executing in user mode"
	},
	{
		"Niced processes",
		"Ticks used by niced processes executing in user mode"
	},
	{
		"Kernel mode",
		"Ticks used by processes executing in kernel mode"
	},
	{
		"IDLE",
		"twiddling thumbs"
	},
	{
		"Waiting for I/O",
		"Ticks waiting for I/O to complete"
	},
	{
		"Interrupts",
		"Ticks spent servicing interrupts"
	},
	{
		"Soft IRQS",
		"Ticks spent servicing softirqs"
	},
	{
		"Involuntary wait",
		"Ticks spent executing other virtual hosts (in virtual environments like Xen)" // Is this description ok?
	},
	{
		"Running a normal guest",
		""
	},
	{
		"Running a niced guest",
		""
	},

	{
		"Total use of CPU",
		"Total CPU used by all processes and interrupts",
	},
	{
		nullptr,
		nullptr
	}

 };

 namespace Udjat {

	System::Stat::Stat() {

		ifstream in("/proc/stat", ifstream::in);
		in.ignore(3);

		in	>> user
			>> nice
			>> system
			>> idle
			>> iowait
			>> irq
			>> softirq
			>> steal
			>> guest
			>> guest_nice;
	}

	UDJAT_API System::Stat::Type System::Stat::TypeFactory(const char *name) {

		for(unsigned short ix = 0; ix < (sizeof(typenames)/sizeof(typenames[0])); ix++) {
			if(!strcasecmp(name,typenames[ix])) {
				return (System::Stat::Type) ix;
			}
		}

		if(!strcasecmp(name,"total")) {
			return System::Stat::TOTAL;
		}

		throw runtime_error(string{"Unexpected property name '"} + name + "'");
	}

	UDJAT_API System::Stat::Type System::Stat::TypeFactory(const pugi::xml_node &node) {
		return TypeFactory(node.attribute("field-name").as_string("total"));
	}

	unsigned long System::Stat::operator[](const char *name) const {
		return (*this)[(const System::Stat::Type) TypeFactory(name)];
	}

	unsigned long System::Stat::operator[](const System::Stat::Type ix) const {

		switch(ix) {
		case USER:
			return user;

		case NICE:
			return nice;

		case SYSTEM:
			return system;

		case IDLE:
			return idle;

		case IOWAIT:
			return iowait;

		case IRQ:
			return irq;

		case SOFTIRQ:
			return softirq;

		case STEAL:
			return steal;

		case GUEST:
			return guest;

		case GUEST_NICE:
			return guest_nice;

		case TOTAL:
			return total();

		}

		throw system_error(EINVAL,system_category(),"Invalid type");

	}

	unsigned long System::Stat::total() const noexcept {
		return user+nice+system+idle+iowait+irq+softirq+steal+guest+guest_nice;
	}

	unsigned long System::Stat::getUsage() const noexcept {
		return user+nice+system+idle+iowait+irq+softirq+steal+guest+guest_nice;
	}

	unsigned long System::Stat::getRunning() const noexcept {
		return user+nice+system+iowait+irq+softirq+steal+guest+guest_nice;
	}

	System::Stat & System::Stat::operator-=(const System::Stat &stat) {

		user -= stat.user;
		nice -= stat.nice;
		system -= stat.system;
		idle -= stat.idle;
		iowait -= stat.iowait;
		irq -= stat.irq;
		softirq -= stat.softirq;
		steal -= stat.steal;
		guest -= stat.guest;
		guest_nice -= stat.guest_nice;

		return *this;
	}

	const char * System::Stat::getLabel(const Type ix) noexcept {
		return typeinfo[ix].label;
	}

	const char * System::Stat::getSummary(const Type ix) noexcept {
		return typeinfo[ix].summary;
	}

 }

 namespace std {

 	const char * to_string(const Udjat::System::Stat::Type type) noexcept {

 		if(type > (sizeof(typenames)/sizeof(typenames[0]))) {
			return "undefined";
 		}

 		return typenames[type];

 	}

 }




