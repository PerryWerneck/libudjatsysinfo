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
 #include <udjat/tools/intl.h>
 #include <sstream>
 #include <iomanip>
 #include <sstream>
 #include <udjat/moduleinfo.h>

 #include "private.h"

 namespace Udjat {

	static const Udjat::ModuleInfo moduleinfo{"Get system uptime"};

	class SysInfo::UpTime::Agent : public Abstract::Agent {
	private:

		std::vector<std::shared_ptr<State<long>>> states;

		long getUptime() const {
			struct sysinfo info;
			memset(&info,0,sizeof(info));

			if(sysinfo(&info) < 0) {
				throw system_error(errno,system_category(),"Cant get system information");
			}

			return info.uptime;
		}

	protected:

		std::shared_ptr<Abstract::State> stateFromValue() const override {

			struct sysinfo info;
			memset(&info,0,sizeof(info));

			if(sysinfo(&info) < 0) {
				throw system_error(errno,system_category(),"Cant get system information");
			}

			for(auto state : states) {
				if(state->compare(info.uptime))
					return state;
			}

			return Abstract::Agent::stateFromValue();

		}

	public:
		Agent(const xml_node &node) : Abstract::Agent("uptime") {
			Object::properties.icon = "utilities-system-monitor";
			Object::properties.label = "System uptime";
			Abstract::Agent::load(node);
		}

		virtual ~Agent() {
		}

		std::shared_ptr<Abstract::State> StateFactory(const pugi::xml_node &node) override {
			auto state = std::make_shared<State<long>>(node);
			states.push_back(state);
			return state;
		}

		bool refresh() {
			getUptime();
			updated(true);
			return true;
		}

		Udjat::Value & get(Udjat::Value &value) const override {
			value = to_string();
			return value;
		}

		std::string to_string() const override {

			long uptime = getUptime();

			if(uptime < 60) {
				return _( "Less than one minute" );
			}

			long updays = uptime / 86400;
			long uphours = (uptime - (updays * 86400)) / 3600;
			long upmins = (uptime - (updays * 86400) - (uphours * 3600)) / 60;

			uint8_t key = (updays > 0 ? 1 : 0) + (uphours > 0 ? 2 : 0) + (upmins > 0 ? 4 : 0);

			static const struct {
				uint8_t	key;
				const char * fmt;
			} itens[] = {
				{ 1,	N_( "{d} {D}" ) 						},	// Only days.
				{ 2,	N_( "{h} {H}" )							},	// Only hours.
				{ 3,	N_( "{d} {D} and {h} {H}" )				},	// Days and hours.
				{ 4,	N_( "{m} {M}" )							},	// Only minutes.
				{ 5,	N_( "{d} {D} and {m} {M}" )				},	// Days and minutes.
				{ 6,	N_( "{h} {H} and {m} {M}" )				},	// Hours and minutes.
				{ 7,	N_( "{d} {D}, {h} {H} and {m} {M}" )	},	// Days, hours and minutes.
			};

			struct {
				const char *tag;
				string text;
			} values[] = {
				{ "{d}", std::to_string(updays) },
				{ "{D}", string( updays > 1 ?  _("days") : _("day")) },
				{ "{h}", std::to_string(uphours) },
				{ "{H}", string( uphours > 1 ? _("hours") : _("hour")) },
				{ "{m}", std::to_string(upmins) },
				{ "{M}", string( upmins > 1 ? _("minutes") : _("minute")) }
			};

#ifdef DEBUG
			cout << "days: " << updays << " hours: " << uphours << " minutes: " << upmins << " key: " << ((int) key) << endl;
#endif // DEBUG

			string out;
			for(size_t item = 0; item < (sizeof(itens)/sizeof(itens[0]));item++) {

				if(itens[item].key == key) {

#ifdef GETTEXT_PACKAGE
					out = dgettext(GETTEXT_PACKAGE,itens[item].fmt);
#else
					out = itens[item].fmt;
#endif // GETTEXT_PACKAGE

					for(size_t value = 0; value < (sizeof(values)/sizeof(values[0])); value++) {
						size_t pos = out.find(values[value].tag);
						if(pos != string::npos) {
							out.replace(pos,strlen(values[value].tag),values[value].text);
						}
					}

					break;
				}

			}

			return out;
		}

	};

	SysInfo::UpTime::UpTime() : Udjat::Factory("system-uptime",moduleinfo) {
	}

	SysInfo::UpTime::~UpTime() {
	}

	std::shared_ptr<Abstract::Agent> SysInfo::UpTime::AgentFactory(const Abstract::Object &parent, const pugi::xml_node &node)  const {
		return make_shared<Agent>(node);
	}

 }
