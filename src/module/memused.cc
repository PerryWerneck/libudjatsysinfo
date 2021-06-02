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
 #include <udjat/tools/sysconfig.h>
 #include <sstream>
 #include <iomanip>

/**
 * @page procmeminfo /proc/meminfo fields
 *
 * Name                   | Description
 * -----------------------|----------------------------------------------------------------------------------------------------------
 * MemTotal               |Total amount of physical RAM, in kilobytes.
 * MemFree                |The amount of physical RAM, in kilobytes, left unused by the system.
 * Buffers                |The amount of physical RAM, in kilobytes, used for file buffers.
 * Cached                 |The amount of physical RAM, in kilobytes, used as cache memory.
 * SwapCached             |The amount of swap, in kilobytes, used as cache memory.
 * Active                 |The total amount of buffer or page cache memory, in kilobytes, that is in active use. This is memory that has been recently used and is usually not reclaimed for other purposes.
 * Inactive               |The total amount of buffer or page cache memory, in kilobytes, that are free and available. This is memory that has not been recently used and can be reclaimed for other purposes.
 * HighTotal and HighFree |The total and free amount of memory, in kilobytes, that is not directly mapped into kernel space. The HighTotal value can vary based on the type of kernel used.
 * LowTotal and LowFree   |The total and free amount of memory, in kilobytes, that is directly mapped into kernel space. The LowTotal value can vary based on the type of kernel used.
 * SwapTotal              |The total amount of swap available, in kilobytes.
 * SwapFree               |The total amount of swap free, in kilobytes.
 * Dirty                  |The total amount of memory, in kilobytes, waiting to be written back to the disk.
 * Writeback              |The total amount of memory, in kilobytes, actively being written back to the disk.
 * Mapped                 |The total amount of memory, in kilobytes, which have been used to map devices, files, or libraries using the mmap command.
 * Slab                   |The total amount of memory, in kilobytes, used by the kernel to cache data structures for its own use.
 * Committed_AS           |The total amount of memory, in kilobytes, estimated to complete the workload. This value represents the worst case scenario value, and also includes swap memory.
 * PageTables             |The total amount of memory, in kilobytes, dedicated to the lowest page table level.
 * VMallocTotal           |The total amount of memory, in kilobytes, of total allocated virtual address space.
 * VMallocUsed            |The total amount of memory, in kilobytes, of used virtual address space.
 * VMallocChunk           |The largest contiguous block of memory, in kilobytes, of available virtual address space.
 * HugePages_Total        |The total number of hugepages for the system. The number is derived by dividing Hugepagesize by the megabytes set aside for hugepages specified in /proc/sys/vm/hugetlb_pool. This statistic only appears on the x86, Itanium, and AMD64 architectures.
 * HugePages_Free         |The total number of hugepages available for the system. This statistic only appears on the x86, Itanium, and AMD64 architectures.
 * Hugepagesize           |The size for each hugepages unit in kilobytes. By default, the value is 4096 KB on uniprocessor kernels for 32 bit architectures. For SMP, hugemem kernels, and AMD64, the default is 2048 KB. For Itanium architectures, the default is 262144 KB. This statistic only appears on the x86, Itanium, and AMD64 architectures.
 *
 */

 #include "private.h"

 namespace Udjat {

	static const Udjat::ModuleInfo moduleinfo{
		PACKAGE_NAME,												// The module name.
		"Get memory use", 											// The module description.
		PACKAGE_VERSION, 											// The module version.
		PACKAGE_URL, 												// The package URL.
		PACKAGE_BUGREPORT 											// The bug report address.
	};

	static const SysInfo::Percent::StateDescription internal_states[] = {
		{
			80.0,
			"low",
			Udjat::ready,
			"Memory usage is lower than 80%",
			""
		},
		{
			90.0,
			"medium",
			Udjat::warning,
			"Memory usage is lower than 90%",
			""
		},
		{
			100.0,
			"high",
			Udjat::error,
			"Memory usage is higher than 90%",
			""
		}
	};

	double get_scaled(const Udjat::SysConfig::Value &v) {

		// https://github.com/GNOME/libgtop/blob/master/sysdeps/linux/glibtop_private.c

        const char    *ptr = v.value.c_str();
        char          *next;
        unsigned long long value;

        value = strtoull(ptr, &next, 0);

        for ( ; *next; ++next) {
			if (*next == 'k') {
				value *= 1024;
				break;
			} else if (*next == 'M') {
				value *= 1024 * 1024;
				break;
			}
        }

        return (float) value;
	}

	class SysInfo::MemUsed::Agent : public Percent {
	protected:

		float getValue() override {
			/**
			 * @page free-memory Determining free memory on Linux
			 *
			 * <http://blog.scoutapp.com/articles/2010/10/06/determining-free-memory-on-linux>
			 *
			 * When checking the amount of free memory on a Linux server, it’s easy to think you’re running out of memory when you’re not.
			 *
			 * Memory Caching & Buffers
			 *
			 * Reading data from a disk is far slower than accessing data from memory. Linux caches blocks from the disk in memory.
			 * In fact, Linux uses all free RAM for the buffer cache to make reading data as efficient as possible.
			 *
			 * What happens if a program needs more memory than what’s available? The buffer cache will shrink to accommodate the increased
			 * memory needs. The buffer cache works like your most efficient coworker: when things aren’t busy, he runs around making things
			 * run smoother. When an important task comes up, he drops the less important chores.
			 *
			 * Actual Free Memory
			 *
			 * The actual free memory available is:
			 *
			 * Actual Free Memory = Free (39 MB) + Buffers (95) + Cached (3590) = 3,724 MB
			 *
			 */

			// The actual free memory available is:
			// Actual Free Memory = Free (39 MB) + Buffers (95) + Cached (3590) = 3,724 MB
			// That’s 95x more free memory than than we initially thought.
			// If you are using Scout, Eric Lindvall’s Memory Profiler plugin already takes the buffer

			// https://github.com/GNOME/libgtop/blob/master/sysdeps/linux/mem.c
			// https://www.thegeekdiary.com/understanding-proc-meminfo-file-analyzing-memory-utilization-in-linux/

			Udjat::SysConfig::File meminfo("/proc/meminfo",":");

			auto total  = get_scaled(meminfo["MemTotal"]);
			auto user	= total - get_scaled(meminfo["MemAvailable"]);

			// auto free   = get_scaled(meminfo["MemFree"]);
			// auto shared = get_scaled(meminfo["Shmem"]);
			// auto buffer = get_scaled(meminfo["Buffers"]);
			// auto cached = get_scaled(meminfo["Cached"]) + get_scaled(meminfo["Slab"]);
			// auto used   = total - free;

			return (user * 100) /total;
		};

	public:
		Agent(const xml_node &node) : Percent("memory") {
			this->icon = "utilities-system-monitor";
			this->label = "Used Memory Percentage";
			Abstract::Agent::load(node);
			load(internal_states,sizeof(internal_states)/sizeof(internal_states[0]));
		}

		virtual ~Agent() {
		}

	};

	SysInfo::MemUsed::MemUsed() : Udjat::Factory("mem-used",&moduleinfo) {
	}

	SysInfo::MemUsed::~MemUsed() {
	}

	void SysInfo::MemUsed::parse(Abstract::Agent &parent, const pugi::xml_node &node) const {
		parent.insert(make_shared<Agent>(node));
	}

 }
