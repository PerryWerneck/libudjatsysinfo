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

 #include <udjat/tools/vmstat.h>
 #include <iostream>
 #include <sstream>
 #include <iomanip>

 #include "private.h"

 namespace Udjat {

	VMStat::CPUUsage::CPUUsage(const char *name) : Abstract::Agent(name) {
		loadDefaultStates();
	}

	VMStat::CPUUsage::~CPUUsage() {
	}

	float VMStat::CPUUsage::get() {
		float value = 10;

		// Update active state based on CPU use value.

		return value;
	}

	void VMStat::CPUUsage::refresh() {
		get();
	}

	void VMStat::CPUUsage::get(const char *name, Json::Value &value) override {
		value["name"] = this->get();
	}

	std::string VMStat::CPUUsage::to_string() const override {
		// https://stackoverflow.com/questions/14432043/float-formatting-in-c
		stringstream out;
		out << std::fixed << setprecision(2) << get() << "%";
		return out.str();
	}

	void VMStat::CPUUsage::append_state(const pugi::xml_node &node) override {
	}

	void VMStat::CPUUsage::loadDefaultStates() {
	}

 }
