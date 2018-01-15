//===========================================================================
//
//   File : KviCtcpExtensionParser.cpp
//   Creation date : Janurary 13 2018 18:15:20 by Matt Ullman
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 20q8 Matt Ullman (staticfox at staticfox dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your option) any later version.
//
//   This program is distributed in the HOPE that it will be USEFUL,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program. If not, write to the Free Software Foundation,
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//===========================================================================

#include <unordered_map>
#include <functional>

#include "kvi_out.h"
#include "KviCtcpExtensionParser.h"
#include "KviIrcMessage.h"
#include "KviLocale.h"
#include "KviLog.h"

namespace KviExtension {

#define EXT_ARGS KviIrcServerParser *parser, KviCtcpMessage *msg, bool received_data_back
#define EXT_FUNC(func) int func(EXT_ARGS)

static EXT_FUNC(parse_avatar);
static EXT_FUNC(parse_gender);

using ctcp_request_parser = std::function<int(KviIrcServerParser *, KviCtcpMessage *, bool)>;
const std::unordered_map<int, ctcp_request_parser> ctcp_request_parse_map ({
	{ CTCP_KVI_FLAG_GENDER, parse_gender },
	{ CTCP_KVI_FLAG_AVATAR, parse_avatar },
	// { CTCP_KVI_FLAG_AGE   , KVI_CTCP_METHOD(Avatar) },
});

#define WARN_CMD_FAIL(why) \
	msg->msg->console()->output(KVI_OUT_SYSTEMWARNING,                               \
		__tr2qs("Invalid KVIRC CTCP %Q command from \r!n\r%Q\r [%Q@\r!h\r%Q\r]: %Q"),\
		&(msg->pData),                                                               \
		&(msg->pSource->nick()),                                                     \
		&(msg->pSource->user()), &(msg->pSource->host()),                            \
		&msg->szTag, &why)

void
parse_extension(KviIrcServerParser *parser, KviCtcpMessage * msg, bool received_data_back)
{
	uint64_t requests = 0;

	try {
		requests = std::stoi(std::string{msg->pData});
	} catch (std::exception& e) {
		KviLog(LogType::Debug) << "Failed to parse KVI extension: " << e.what();
		WARN_CMD_FAIL(QString{"Invalid request bitmask"});
		return;
	}

	for (unsigned ii = 0; ii < CTCP_KVI_MAX; ++ii) {
		const uint32_t flag = (1 << ii);
		if ((requests & flag) == 0)
			continue;

		auto got = ctcp_request_parse_map.find(flag);
		if (got != ctcp_request_parse_map.end()) {
			auto f = got->second;

			if (f(parser, msg, received_data_back)) {
				WARN_CMD_FAIL("???");
			}
		}
	}

	return;
}

EXT_FUNC(parse_gender)
{
	return 0;
}

EXT_FUNC(parse_avatar)
{
	return 0;
}

#undef EXT_ARGS
#undef EXT_FUNC
#undef WARN_CMD_FAIL

}
