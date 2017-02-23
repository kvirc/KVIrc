//=============================================================================
//
//   File : KviIrcServerParser_tables.cpp
//   Creation date : Sun Jun 30 2000 18:10:19 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)
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
//=============================================================================

#include "KviIrcServerParser.h"

#define PTM(m) KVI_PTR2MEMBER(KviIrcServerParser::m)

KviLiteralMessageParseStruct KviIrcServerParser::m_literalParseProcTable[] = {
	// clang-format off
	{ "ACCOUNT"      , PTM(parseLiteralAccount)      },
	{ "AUTHENTICATE" , PTM(parseLiteralAuthenticate) },
	{ "AWAY"         , PTM(parseLiteralAway)         },
	{ "CAP"          , PTM(parseLiteralCap)          },
	{ "CHGHOST"      , PTM(parseLiteralChghost)      },
	{ "ERROR"        , PTM(parseLiteralError)        },
	{ "INVITE"       , PTM(parseLiteralInvite)       },
	{ "JOIN"         , PTM(parseLiteralJoin)         },
	{ "KICK"         , PTM(parseLiteralKick)         },
	{ "MODE"         , PTM(parseLiteralMode)         },
	{ "NICK"         , PTM(parseLiteralNick)         },
	{ "NOTICE"       , PTM(parseLiteralNotice)       },
	{ "PART"         , PTM(parseLiteralPart)         },
	{ "PING"         , PTM(parseLiteralPing)         },
	{ "PONG"         , PTM(parseLiteralPong)         },
	{ "PRIVMSG"      , PTM(parseLiteralPrivmsg)      },
	{ "QUIT"         , PTM(parseLiteralQuit)         },
	{ "TOPIC"        , PTM(parseLiteralTopic)        },
	{ "WALLOPS"      , PTM(parseLiteralWallops)      },
	{ nullptr        , nullptr                       }
	// clang-format on
};

#define REQ(f) parseCtcpRequest##f
#define RPL(f) parseCtcpReply##f

KviCtcpMessageParseStruct KviIrcServerParser::m_ctcpParseProcTable[] = {
	// clang-format off
	{ "ACTION"     , PTM(REQ(Action))     , PTM(REQ(Action))   , 0 },
	{ "AVATAR"     , PTM(REQ(Avatar))     , PTM(RPL(Avatar))   , 0 },
	{ "CLIENTINFO" , PTM(REQ(Clientinfo)) , PTM(RPL(Generic))  , 0 },
	{ "DCC"        , PTM(REQ(Dcc))        , PTM(REQ(Dcc))      , 0 },
	{ "FINGER"     , PTM(REQ(Finger))     , PTM(RPL(Generic))  , 0 },
	{ "LAGCHECK"   , nullptr              , PTM(RPL(Lagcheck)) , KVI_CTCP_MESSAGE_PARSE_TRIGGERNOEVENT },
	{ "PAGE"       , PTM(REQ(Page))       , PTM(RPL(Generic))  , 0 },
	{ "PING"       , PTM(REQ(Ping))       , PTM(RPL(Ping))     , 0 },
	{ "SOURCE"     , PTM(REQ(Source))     , PTM(RPL(Generic))  , 0 },
	{ "TDCC"       , PTM(REQ(Dcc))        , PTM(REQ(Dcc))      , 0 },
	{ "TIME"       , PTM(REQ(Time))       , PTM(RPL(Generic))  , 0 },
	{ "USERINFO"   , PTM(REQ(Userinfo))   , PTM(RPL(Userinfo)) , 0 },
	{ "VERSION"    , PTM(REQ(Version))    , PTM(RPL(Generic))  , 0 },
	{ "XDCC"       , PTM(REQ(Dcc))        , PTM(REQ(Dcc))      , 0 },
	{ nullptr      , nullptr              , nullptr            , 0 }
	// clang-format on
};

#undef REQ
#undef RPL

messageParseProc KviIrcServerParser::m_numericParseProcTable[1000] = {
	nullptr,                                      // 000
	PTM(parseNumeric001),                         // 001 RPL_WELCOME
	PTM(parseNumeric002),                         // 002 RPL_YOURHOST
	PTM(parseNumeric003),                         // 003 RPL_CREATED
	PTM(parseNumeric004),                         // 004 RPL_MYINFO
	PTM(parseNumeric005),                         // 005 RPL_BOUNCE, RPL_PROTOCTL, RPL_ISUPPORT
	nullptr,                                      // 006 RPL_MAP
	nullptr,                                      // 007 RPL_MAPEND
	PTM(parseNumericSnomask),                     // 008 RPL_SNOMASK
	nullptr,                                      // 009 RPL_STATMEMTOT
	nullptr,                                      // 010 RPL_BOUNCE, RPL_REDIR, RPL_STATMEM
	nullptr,                                      // 011
	nullptr,                                      // 012
	nullptr,                                      // 013
	nullptr,                                      // 014 RPL_YOURCOOKIE
	nullptr,                                      // 015 RPL_MAP
	nullptr,                                      // 016 RPL_MAPMORE
	nullptr,                                      // 017 RPL_MAPEND
	nullptr,                                      // 018
	nullptr,                                      // 019
	PTM(parseNumeric020),                         // 020
	nullptr,                                      // 021
	nullptr,                                      // 022
	nullptr,                                      // 023
	nullptr,                                      // 024
	nullptr,                                      // 025
	nullptr,                                      // 026
	nullptr,                                      // 027
	nullptr,                                      // 028
	nullptr,                                      // 029
	nullptr,                                      // 030
	nullptr,                                      // 031
	nullptr,                                      // 032
	nullptr,                                      // 033
	nullptr,                                      // 034
	nullptr,                                      // 035
	nullptr,                                      // 036
	nullptr,                                      // 037
	nullptr,                                      // 038
	nullptr,                                      // 039
	nullptr,                                      // 040
	nullptr,                                      // 041
	PTM(parseNumericYourUID),                     // 042 RPL_YOURID
	nullptr,                                      // 043 RPL_SAVENICK
	nullptr,                                      // 044
	nullptr,                                      // 045
	nullptr,                                      // 046
	nullptr,                                      // 047
	nullptr,                                      // 048
	nullptr,                                      // 049
	nullptr,                                      // 050 RPL_ATTEMPTINGJUNC
	nullptr,                                      // 051 RPL_ATTEMPTINGREROUTE
	nullptr,                                      // 052
	nullptr,                                      // 053
	nullptr,                                      // 054
	nullptr,                                      // 055
	nullptr,                                      // 056
	nullptr,                                      // 057
	nullptr,                                      // 058
	nullptr,                                      // 059
	nullptr,                                      // 060
	nullptr,                                      // 061
	nullptr,                                      // 062
	nullptr,                                      // 063
	nullptr,                                      // 064
	nullptr,                                      // 065
	nullptr,                                      // 066
	nullptr,                                      // 067
	nullptr,                                      // 068
	nullptr,                                      // 069
	nullptr,                                      // 070
	nullptr,                                      // 071
	nullptr,                                      // 072
	nullptr,                                      // 073
	nullptr,                                      // 074
	nullptr,                                      // 075
	nullptr,                                      // 076
	nullptr,                                      // 077
	nullptr,                                      // 078
	nullptr,                                      // 079
	nullptr,                                      // 080
	nullptr,                                      // 081
	nullptr,                                      // 082
	nullptr,                                      // 083
	nullptr,                                      // 084
	nullptr,                                      // 085
	nullptr,                                      // 086
	nullptr,                                      // 087
	nullptr,                                      // 088
	nullptr,                                      // 089
	nullptr,                                      // 090
	nullptr,                                      // 091
	nullptr,                                      // 092
	nullptr,                                      // 093
	nullptr,                                      // 094
	nullptr,                                      // 095
	nullptr,                                      // 096
	nullptr,                                      // 097
	nullptr,                                      // 098
	nullptr,                                      // 099
	nullptr,                                      // 100
	nullptr,                                      // 101
	nullptr,                                      // 102
	nullptr,                                      // 103
	nullptr,                                      // 104
	nullptr,                                      // 105
	nullptr,                                      // 106
	nullptr,                                      // 107
	nullptr,                                      // 108
	nullptr,                                      // 109
	nullptr,                                      // 110
	nullptr,                                      // 111
	nullptr,                                      // 112
	nullptr,                                      // 113
	nullptr,                                      // 114
	nullptr,                                      // 115
	nullptr,                                      // 116
	nullptr,                                      // 117
	nullptr,                                      // 118
	nullptr,                                      // 119
	nullptr,                                      // 120
	nullptr,                                      // 121
	nullptr,                                      // 122
	nullptr,                                      // 123
	nullptr,                                      // 124
	nullptr,                                      // 125
	nullptr,                                      // 126
	nullptr,                                      // 127
	nullptr,                                      // 128
	nullptr,                                      // 129
	nullptr,                                      // 130
	nullptr,                                      // 131
	nullptr,                                      // 132
	nullptr,                                      // 133
	nullptr,                                      // 134
	nullptr,                                      // 135
	nullptr,                                      // 136
	nullptr,                                      // 137
	nullptr,                                      // 138
	nullptr,                                      // 139
	nullptr,                                      // 140
	nullptr,                                      // 141
	nullptr,                                      // 142
	nullptr,                                      // 143
	nullptr,                                      // 144
	nullptr,                                      // 145
	nullptr,                                      // 146
	nullptr,                                      // 147
	nullptr,                                      // 148
	nullptr,                                      // 149
	nullptr,                                      // 150
	nullptr,                                      // 151
	nullptr,                                      // 152
	nullptr,                                      // 153
	nullptr,                                      // 154
	nullptr,                                      // 155
	nullptr,                                      // 156
	nullptr,                                      // 157
	nullptr,                                      // 158
	nullptr,                                      // 159
	nullptr,                                      // 160
	nullptr,                                      // 161
	nullptr,                                      // 162
	nullptr,                                      // 163
	nullptr,                                      // 164
	nullptr,                                      // 165
	nullptr,                                      // 166
	nullptr,                                      // 167
	nullptr,                                      // 168
	nullptr,                                      // 169
	nullptr,                                      // 170
	nullptr,                                      // 171
	nullptr,                                      // 172
	nullptr,                                      // 173
	nullptr,                                      // 174
	nullptr,                                      // 175
	nullptr,                                      // 176
	nullptr,                                      // 177
	nullptr,                                      // 178
	nullptr,                                      // 179
	nullptr,                                      // 180
	nullptr,                                      // 181
	nullptr,                                      // 182
	nullptr,                                      // 183
	nullptr,                                      // 184
	nullptr,                                      // 185
	nullptr,                                      // 186
	nullptr,                                      // 187
	nullptr,                                      // 188
	nullptr,                                      // 189
	nullptr,                                      // 190
	nullptr,                                      // 191
	nullptr,                                      // 192
	nullptr,                                      // 193
	nullptr,                                      // 194
	nullptr,                                      // 195
	nullptr,                                      // 196
	nullptr,                                      // 197
	nullptr,                                      // 198
	nullptr,                                      // 199
	nullptr,                                      // 200 RPL_TRACELINK
	nullptr,                                      // 201 RPL_TRACECONNECTING
	nullptr,                                      // 202 RPL_TRACEHANDSHAKE
	nullptr,                                      // 203 RPL_TRACEUNKNOWN
	nullptr,                                      // 204 RPL_TRACEOPERATOR
	nullptr,                                      // 205 RPL_TRACEUSER
	nullptr,                                      // 206 RPL_TRACESERVER
	nullptr,                                      // 207 RPL_TRACESERVICE
	nullptr,                                      // 208 RPL_TRACENEWTYPE
	nullptr,                                      // 209 RPL_TRACECLASS
	nullptr,                                      // 210 RPL_TRACERECONNECT, RPL_STATS
	nullptr,                                      // 211 RPL_STATSLINKINFO
	nullptr,                                      // 212 RPL_STATSCOMMANDS
	nullptr,                                      // 213 RPL_STATSCLINE
	nullptr,                                      // 214 RPL_STATSNLINE
	nullptr,                                      // 215 RPL_STATSILINE
	nullptr,                                      // 216 RPL_STATSKLINE
	nullptr,                                      // 217 RPL_STATSQLINE, RPL_STATSPLINE
	nullptr,                                      // 218 RPL_STATSYLINE
	PTM(parseNumericEndOfStats),                  // 219 RPL_ENDOFSTATS
	nullptr,                                      // 220 RPL_STATSPLINE, RPL_STATSBLINE
	PTM(parseNumericUserMode),                    // 221 RPL_UMODEIS
	PTM(parseNumericCodePageSet),                 // 222 RusNet Codepage set, RPL_MODLIST, RPL_SQLINE_NICK, RPL_STATSBLINE
	nullptr,                                      // 223 RPL_STATSELINE, RPL_STATSGLINE
	nullptr,                                      // 224 RPL_STATSFLINE, RPL_STATSTLINE
	nullptr,                                      // 225 RPL_STATSDLINE, RPL_STATSZLINE, RPL_STATSELINE
	nullptr,                                      // 226 RPL_STATSCOUNT, RPL_STATSNLINE
	nullptr,                                      // 227 RPL_STATSGLINE, RPL_STATSVLINE
	nullptr,                                      // 228 RPL_STATSQLINE
	nullptr,                                      // 229
	nullptr,                                      // 230
	nullptr,                                      // 231 RPL_SERVICEINFO
	nullptr,                                      // 232 RPL_ENDOFSERVICES, RPL_RULES
	nullptr,                                      // 233 RPL_SERVICE
	nullptr,                                      // 234 RPL_SERVLIST
	nullptr,                                      // 235 RPL_SERVLISTEND
	nullptr,                                      // 236 RPL_STATSVERBOSE
	nullptr,                                      // 237 RPL_STATSENGINE
	nullptr,                                      // 238 RPL_STATSFLINE
	PTM(parseNumericStats),                       // 239 RPL_STATSIAUTH
	PTM(parseNumericStats),                       // 240 RPL_STATSVLINE
	PTM(parseNumericStats),                       // 241 RPL_STATSLLINE
	PTM(parseNumericStats),                       // 242 RPL_STATSUPTIME
	PTM(parseNumericStats),                       // 243 RPL_STATSOLINE
	PTM(parseNumericStats),                       // 244 RPL_STATSHLINE
	PTM(parseNumericStats),                       // 245 RPL_STATSSLINE
	PTM(parseNumericStats),                       // 246 RPL_STATSPING RPL_STATSTLINE
	PTM(parseNumericStats),                       // 247 RPL_STATSBLINE_I RPL_STATSGLINE RPL_STATSXLINE
	PTM(parseNumericStats),                       // 248 RPL_STATSDEFINE RPL_STATSULINE
	PTM(parseNumericStats),                       // 249 RPL_STATSDEBUG
	PTM(parseNumericStats),                       // 250 RPL_STATSCONN RPL_STATSDLINE_I
	PTM(parseNumericStats),                       // 251 RPL_LUSERCLIENT
	PTM(parseNumericStats),                       // 252 RPL_LUSEROP
	PTM(parseNumericStats),                       // 253 RPL_LUSERUNKNOWN
	PTM(parseNumericStats),                       // 254 RPL_LUSERCHANNELS
	PTM(parseNumericStats),                       // 255 RPL_LUSERME
	PTM(parseNumericServerAdminInfoTitle),        // 256 RPL_ADMINME
	PTM(parseNumericServerAdminInfoServerName),   // 257 RPL_ADMINLOC1
	PTM(parseNumericServerAdminInfoAdminName),    // 258 RPL_ADMINLOC2
	PTM(parseNumericServerAdminInfoAdminContact), // 259 RPL_ADMINEMAIL
	nullptr,                                      // 260
	nullptr,                                      // 261 RPL_TRACELOG
	nullptr,                                      // 262 RPL_TRACEPING, RPL_TRACEEND
	PTM(parseNumericTryAgain),                    // 263 RPL_TRYAGAIN
	nullptr,                                      // 264
	PTM(parseNumericStats),                       // 265 RPL_LOCALUSERS
	PTM(parseNumericStats),                       // 266 RPL_GLOBALUSERS
	PTM(parseNumericStats),                       // 267 RPL_SHAREDUSERS, RPL_START_NETSTAT
	nullptr,                                      // 268 RPL_NETSTAT
	nullptr,                                      // 269 RPL_END_NETSTAT
	nullptr,                                      // 270 RPL_PRIVS, RPL_MAPUSERS
	nullptr,                                      // 271 RPL_SILELIST
	nullptr,                                      // 272 RPL_ENDOFSILELIST
	nullptr,                                      // 273 RPL_NOTIFY
	nullptr,                                      // 274 RPL_ENDOFNOTIFY, RPL_STATSDELTA
	PTM(parseNumericWhoisOther),                  // 275 RPL_WHOISSSL, RPL_STATSDLINE
	nullptr,                                      // 276 RPL_VCHANEXIST
	nullptr,                                      // 277 RPL_VCHANLIST
	nullptr,                                      // 278 RPL_VCHANHELP
	nullptr,                                      // 279
	nullptr,                                      // 280 RPL_GLIST
	nullptr,                                      // 281 RPL_ENDOFGLIST, RPL_ACCEPTLIST
	nullptr,                                      // 282 RPL_ENDOFACCEPT, RPL_JUPELIST
	nullptr,                                      // 283 RPL_ALIST, RPL_ENDOFJUPELIST
	nullptr,                                      // 284 RPL_ENDOFALIST, RPL_FEATURE
	nullptr,                                      // 285 RPL_FEATURE, RPL_CHANINFO_HANDLE
	nullptr,                                      // 286 RPL_CHANINFO_USERS, RPL_CHKHEAD
	nullptr,                                      // 287 RPL_CHANINFO_CHOPS, RPL_CHANUSER
	nullptr,                                      // 288 RPL_CHANINFO_VOICES, RPL_PATCHHEAD
	nullptr,                                      // 289 RPL_CHANINFO_AWAY, RPL_PATCHCON
	nullptr,                                      // 290 RPL_CHANINFO_OPERS, RPL_HELPHDR, RPL_DATASTR
	nullptr,                                      // 291 RPL_CHANINFO_BANNED, RPL_HELPOP, RPL_ENDOFCHECK
	nullptr,                                      // 292 RPL_CHANINFO_BANS, RPL_HELPTLR
	nullptr,                                      // 293 RPL_CHANINFO_INVITE, RPL_HELPHLP
	nullptr,                                      // 294 RPL_CHANINFO_INVITES, RPL_HELPFWD
	nullptr,                                      // 295 RPL_CHANINFO_KICK, RPL_HELPIGN
	nullptr,                                      // 296 RPL_CHANINFO_KICKS
	nullptr,                                      // 297
	nullptr,                                      // 298
	nullptr,                                      // 299 RPL_END_CHANINFO
	nullptr,                                      // 300 RPL_NONE
	PTM(parseNumericWhoisAway),                   // 301 RPL_AWAY
	PTM(parseNumericUserhost),                    // 302 RPL_USERHOST
	PTM(parseNumericIsOn),                        // 303 RPL_ISON
	nullptr,                                      // 304 RPL_TEXT, RPL_SYNTAX
	PTM(parseNumericBackFromAway),                // 305 RPL_UNAWAY
	PTM(parseNumericAway),                        // 306 RPL_NOWAWAY
	PTM(parseNumericWhoisRegistered),             // 307 RPL_WHOISREGNICK
	PTM(parseNumericWhoisOther),                  // 308 RPL_WHOISADMIN, RPL_RULESTART
	PTM(parseNumericWhoisOther),                  // 309 RPL_WHOISSADMIN, RPL_RULESEND
	PTM(parseNumericWhoisOther),                  // 310 RPL_WHOISHELPOP
	PTM(parseNumericWhoisUser),                   // 311 RPL_WHOISUSER
	PTM(parseNumericWhoisServer),                 // 312 RPL_WHOISSERVER
	PTM(parseNumericWhoisOther),                  // 313 RPL_WHOISOPERATOR
	PTM(parseNumericWhowasUser),                  // 314 RPL_WHOWASUSER
	PTM(parseNumericEndOfWho),                    // 315 RPL_ENDOFWHO
	PTM(parseNumericWhoisOther),                  // 316 RPL_WHOISCHANOP
	PTM(parseNumericWhoisIdle),                   // 317 RPL_WHOISIDLE
	PTM(parseNumericEndOfWhois),                  // 318 RPL_ENDOFWHOIS
	PTM(parseNumericWhoisChannels),               // 319 RPL_WHOISCHANNELS
	PTM(parseNumericWhoisOther),                  // 320 RPL_WHOISIDENTIFIED
	PTM(parseNumericListStart),                   // 321 RPL_LISTSTART
	PTM(parseNumericList),                        // 322 RPL_LIST
	PTM(parseNumericListEnd),                     // 323 RPL_LISTEND
	PTM(parseNumericChannelModeIs),               // 324 RPL_CHANNELMODEIS
	nullptr,                                      // 325 RPL_UNIQOPIS, RPL_CHANNELPASSIS
	nullptr,                                      // 326 RPL_NOCHANPASS
	nullptr,                                      // 327 RPL_CHPASSUNKNOWN
	PTM(parseNumericChanUrl),                     // 328 RPL_CHANNEL_URL
	PTM(parseNumericCreationTime),                // 329 RPL_CREATIONTIME, RPL_CHANNELCREATED
	PTM(parseNumericWhoisAuth),                   // 330 RPL_WHOISAUTH
	PTM(parseNumericNoTopic),                     // 331 RPL_NOTOPIC, RPL_NOTOPICSET
	PTM(parseNumericTopic),                       // 332 RPL_TOPIC
	PTM(parseNumericTopicWhoTime),                // 333 RPL_TOPICWHOTIME, RPL_TOPICTIME
	PTM(parseNumericCommandSyntax),               // 334 RPL_COMMANDSYNTAX
	nullptr,                                      // 335 RPL_WHOISBOT
	nullptr,                                      // 336
	nullptr,                                      // 337
	PTM(parseNumericWhoisActually),               // 338 RPL_WHOISACTUALLY, RPL_CHANPASSOK
	PTM(parseNumericWhoisOther),                  // 339 RPL_BADCHANPASS
	nullptr,                                      // 340 RPL_USERIP
	PTM(parseNumericInviting),                    // 341 RPL_INVITING
	nullptr,                                      // 342 RPL_SUMMONING
	nullptr,                                      // 343
	PTM(parseNumeric344),                         // 344 RPL_REOPLIST, RPL_QUIETLIST
	PTM(parseNumeric345),                         // 345 RPL_ENDOFREOPLIST, RPL_INVITED, RPL_QUIETLISTEND
	PTM(parseNumericInviteList),                  // 346 RPL_INVITELIST
	PTM(parseNumericEndOfInviteList),             // 347 RPL_ENDOFINVITELIST
	PTM(parseNumericExceptList),                  // 348 RPL_EXCEPTLIST
	PTM(parseNumericEndOfExceptList),             // 349 RPL_ENDOFEXCEPTLIST
	nullptr,                                      // 350
	nullptr,                                      // 351 RPL_VERSION
	PTM(parseNumericWhoReply),                    // 352 RPL_WHOREPLY
	PTM(parseNumericNames),                       // 353 RPL_NAMREPLY
	PTM(parseNumericWhospcrpl),                   // 354 RPL_WHOSPCRPL
	nullptr,                                      // 355 RPL_NAMREPLY_
	nullptr,                                      // 356
	nullptr,                                      // 357 RPL_MAP
	nullptr,                                      // 358 RPL_MAPMORE
	nullptr,                                      // 359 RPL_MAPEND
	nullptr,                                      // 360
	nullptr,                                      // 361 RPL_KILLDONE
	nullptr,                                      // 362 RPL_CLOSING
	nullptr,                                      // 363 RPL_CLOSEEND
	PTM(parseNumericLinks),                       // 364 RPL_LINKS
	PTM(parseNumericEndOfLinks),                  // 365 RPL_ENDOFLINKS
	PTM(parseNumericEndOfNames),                  // 366 RPL_ENDOFNAMES
	PTM(parseNumeric367),                         // 367 RPL_BANLIST
	PTM(parseNumeric368),                         // 368 RPL_ENDOFBANLIST
	PTM(parseNumericEndOfWhowas),                 // 369 RPL_ENDOFWHOWAS
	nullptr,                                      // 370
	PTM(parseNumericInfo),                        // 371 RPL_INFO
	PTM(parseNumericMotd),                        // 372 RPL_MOTD
	PTM(parseNumericInfoStart),                   // 373 RPL_INFOSTART
	PTM(parseNumericInfoEnd),                     // 374 RPL_ENDOFINFO
	PTM(parseNumericMotd),                        // 375 RPL_MOTDSTART
	PTM(parseNumericMotd),                        // 376 RPL_ENDOFMOTD
	PTM(parseNumericMotd),                        // 377 RPL_MOTD2, RPL_KICKEXPIRED, RPL_SPAM
	PTM(parseNumericWhoisOther),                  // 378 RPL_WHOISHOST, RPL_BANEXPIRED
	nullptr,                                      // 379 RPL_KICKLINKED, RPL_WHOISMODES, RPL_WHOWASIP
	nullptr,                                      // 380 RPL_BANLINKED, RPL_YOURHELPER
	PTM(parseNumericYoureOper),                   // 381 RPL_YOUREOPER
	nullptr,                                      // 382 RPL_REHASHING
	nullptr,                                      // 383 RPL_YOURESERVICE
	nullptr,                                      // 384 RPL_MYPORTIS
	nullptr,                                      // 385 RPL_NOTOPERANYMORE
	PTM(parseNumericQList),                       // 386 RPL_QLIST
	PTM(parseNumericEndOfQList),                  // 387 RPL_ENDOFQLIST, RPL_ENDOFIRCOPS
	PTM(parseNumericAList),                       // 388 RPL_ALIST
	PTM(parseNumericEndOfAList),                  // 389 RPL_ENDOFALIST
	nullptr,                                      // 390
	PTM(parseNumericTime),                        // 391 RPL_TIME
	nullptr,                                      // 392 RPL_USERSSTART
	nullptr,                                      // 393 RPL_USERS
	nullptr,                                      // 394 RPL_ENDOFUSERS
	nullptr,                                      // 395 RPL_NOUSERS
	PTM(parseNumericHiddenHost),                  // 396 RPL_HOSTHIDDEN, RPL_YOURDISPLAYEDHOST
	nullptr,                                      // 397
	nullptr,                                      // 398
	nullptr,                                      // 399
	nullptr,                                      // 400 ERR_UNKNOWNERROR
	PTM(parseNumericNoSuchNick),                  // 401 ERR_NOSUCHNICK
	PTM(parseNumericNoSuchServer),                // 402 ERR_NOSUCHSERVER
	PTM(parseNumericNoSuchChannel),               // 403 ERR_NOSUCHCHANNEL
	PTM(parseNumericCannotSend),                  // 404 ERR_CANNOTSENDTOCHAN
	nullptr,                                      // 405 ERR_TOOMANYCHANNELS
	PTM(parseNumericNoSuchNick),                  // 406 ERR_WASNOSUCHNICK
	nullptr,                                      // 407 ERR_TOOMANYTARGETS
	PTM(parseNumericCannotSendColor),             // 408 ERR_NOCOLORSONCHAN, ERR_NOSUCHSERVICE
	nullptr,                                      // 409 ERR_NOORIGIN
	nullptr,                                      // 410 ERR_INVALIDCAPSUBCOMMAND
	nullptr,                                      // 411 ERR_NORECIPIENT
	nullptr,                                      // 412 ERR_NOTEXTTOSEND
	nullptr,                                      // 413 ERR_NOTOPLEVEL
	nullptr,                                      // 414 ERR_WILDTOPLEVEL
	PTM(otherChannelError),                       // 415 ERR_BADMASK
	nullptr,                                      // 416 ERR_TOOMANYMATCHES, ERR_QUERYTOOLONG
	nullptr,                                      // 417
	nullptr,                                      // 418
	nullptr,                                      // 419 ERR_LENGTHTRUNCATED
	nullptr,                                      // 420
	PTM(parseNumericUnknownCommand),              // 421 ERR_UNKNOWNCOMMAND
	PTM(parseNumericMotdMissing),                 // 422 ERR_NOMOTD
	nullptr,                                      // 423 ERR_NOADMININFO
	nullptr,                                      // 424 ERR_FILEERROR
	nullptr,                                      // 425 ERR_NOOPERMOTD
	nullptr,                                      // 426
	nullptr,                                      // 427
	nullptr,                                      // 428
	nullptr,                                      // 429 ERR_TOOMANYAWAY
	nullptr,                                      // 430 ERR_EVENTNICKCHANGE
	nullptr,                                      // 431 ERR_NONICKNAMEGIVEN
	PTM(parseNumericNicknameProblem),             // 432 ERR_ERRONEUSNICKNAME
	PTM(parseNumericNicknameProblem),             // 433 ERR_NICKNAMEINUSE
	nullptr,                                      // 434 ERR_SERVICENAMEINUSE, ERR_NORULES
	PTM(parseNumericBanOnChan),                   // 435 ERR_SERVICECONFUSED, ERR_BANONCHAN
	nullptr,                                      // 436 ERR_NICKCOLLISION
	PTM(parseNumericUnavailResource),             // 437 ERR_UNAVAILRESOURCE, ERR_BANNICKCHANGE
	nullptr,                                      // 438 ERR_NICKTOOFAST, ERR_DEAD
	nullptr,                                      // 439 ERR_TARGETTOOFAST
	nullptr,                                      // 440 ERR_SERVICESDOWN
	nullptr,                                      // 441 ERR_USERNOTINCHANNEL
	nullptr,                                      // 442 ERR_NOTONCHANNEL
	PTM(parseNumericUserInChan),                  // 443 ERR_USERONCHANNEL
	nullptr,                                      // 444 ERR_NOLOGIN
	nullptr,                                      // 445 ERR_SUMMONDISABLED
	nullptr,                                      // 446 ERR_USERSDISABLED
	nullptr,                                      // 447 ERR_NONICKCHANGE, ERR_CANTCHANGENICK
	nullptr,                                      // 448
	nullptr,                                      // 449 ERR_NOTIMPLEMENTED
	nullptr,                                      // 450
	PTM(parseNumericNotRegistered),               // 451 ERR_NOTREGISTERED
	nullptr,                                      // 452 ERR_IDCOLLISION
	nullptr,                                      // 453 ERR_NICKLOST
	nullptr,                                      // 454
	nullptr,                                      // 455 ERR_HOSTILENAME
	nullptr,                                      // 456 ERR_ACCEPTFULL
	nullptr,                                      // 457 ERR_ACCEPTEXIST
	nullptr,                                      // 458 ERR_ACCEPTNOT
	nullptr,                                      // 459 ERR_NOHIDING
	nullptr,                                      // 460 ERR_NOTFORHALFOPS
	PTM(parseNumericNotEnoughParams),             // 461 ERR_NEEDMOREPARAMS
	PTM(parseNumericAlreadyRegistered),           // 462 ERR_ALREADYREGISTERED
	nullptr,                                      // 463 ERR_NOPERMFORHOST
	PTM(parseNumericPasswordIncorrect),           // 464 ERR_PASSWDMISMATCH
	nullptr,                                      // 465 ERR_YOUREBANNEDCREEP
	nullptr,                                      // 466 ERR_YOUWILLBEBANNED
	PTM(otherChannelError),                       // 467 ERR_KEYSET
	nullptr,                                      // 468 ERR_ONLYSERVERSCANCHANGE
	nullptr,                                      // 469 ERR_LINKSET
	PTM(parseNumericForward),                     // 470 ERR_LINKCHANNEL, ERR_KICKEDFROMCHAN
	PTM(parseNumericCantJoinChannel),             // 471 ERR_CHANNELISFULL
	PTM(otherChannelError),                       // 472 ERR_UNKNOWNMODE
	PTM(parseNumericCantJoinChannel),             // 473 ERR_INVITEONLYCHAN
	PTM(parseNumericCantJoinChannel),             // 474 ERR_BANNEDFROMCHAN
	PTM(parseNumericCantJoinChannel),             // 475 ERR_BADCHANNELKEY
	nullptr,                                      // 476 ERR_BADCHANMASK
	PTM(parseChannelHelp),                        // 477 RPL_CHANNELHELP
	nullptr,                                      // 478 ERR_BANLISTFULL
	nullptr,                                      // 479 ERR_BADCHANNAME, ERR_LINKFAIL
	PTM(parseNumeric480),                         // 480 ERR_NOULINE, ERR_CANNOTKNOCK, ERR_SSLONLYCHAN
	PTM(parseNumericNoPrivs),                     // 481 ERR_NOPRIVILEGES
	PTM(otherChannelError),                       // 482 ERR_CHANOPRIVSNEEDED
	nullptr,                                      // 483 ERR_CANTKILLSERVER
	nullptr,                                      // 484 ERR_RESTRICTED, ERR_ISCHANSERVICE, ERR_DESYNC, ERR_ATTACKDENY
	nullptr,                                      // 485 ERR_UNIQOPRIVSNEEDED, ERR_KILLDENY, ERR_CANTKICKADMIN, ERR_ISREALSERVICE
	PTM(parseNumeric486),                         // 486 ERR_NONONREG, ERR_HTMDISABLED, ERR_ACCOUNTONLY
	nullptr,                                      // 487 ERR_CHANTOORECENT, ERR_MSGSERVICES
	nullptr,                                      // 488 ERR_TSLESSCHAN
	nullptr,                                      // 489 ERR_VOICENEEDED, ERR_SECUREONLYCHAN
	nullptr,                                      // 490 ERR_ALLMUSTSSL
	nullptr,                                      // 491 ERR_NOOPERHOST
	nullptr,                                      // 492 ERR_NOSERVICEHOST, ERR_NOCTCPALLOWED
	nullptr,                                      // 493 ERR_NOFEATURE
	nullptr,                                      // 494 ERR_BADFEATURE
	nullptr,                                      // 495 ERR_BADLOGTYPE, ERR_DELAYREJOIN
	nullptr,                                      // 496 ERR_BADLOGSYS
	nullptr,                                      // 497 ERR_BADLOGVALUE
	nullptr,                                      // 498 ERR_ISOPERLCHAN
	nullptr,                                      // 499 ERR_CHANOWNPRIVNEEDED
	nullptr,                                      // 500
	nullptr,                                      // 501 ERR_UMODEUNKNOWNFLAG, ERR_UNKNOWNSNOMASK
	PTM(parseNumericUsersDontMatch),              // 502 ERR_USERSDONTMATCH
	nullptr,                                      // 503 ERR_GHOSTEDCLIENT
	nullptr,                                      // 504 ERR_USERNOTONSERV
	nullptr,                                      // 505
	nullptr,                                      // 506
	nullptr,                                      // 507
	nullptr,                                      // 508
	nullptr,                                      // 509
	nullptr,                                      // 510
	nullptr,                                      // 511 ERR_SILELISTFULL
	nullptr,                                      // 512 ERR_TOOMANYWATCH
	nullptr,                                      // 513 ERR_BADPING
	nullptr,                                      // 514 ERR_INVALID_ERROR, ERR_TOOMANYDCC
	nullptr,                                      // 515 ERR_BADEXPIRE
	nullptr,                                      // 516 ERR_DONTCHEAT
	nullptr,                                      // 517 ERR_DISABLED
	nullptr,                                      // 518 ERR_NOINVITE, ERR_LONGMASK
	nullptr,                                      // 519 ERR_ADMONLY, ERR_TOOMANYUSERS
	nullptr,                                      // 520 ERR_OPERONLY, ERR_MASKTOOWIDE, ERR_WHOTRUNC, ERR_CANTJOINOPERSONLY
	nullptr,                                      // 521 ERR_LISTSYNTAX
	nullptr,                                      // 522 ERR_WHOSYNTAX
	nullptr,                                      // 523 ERR_WHOLIMEXCEED
	nullptr,                                      // 524 ERR_QUARANTINED, ERR_OPERSPVERIFY
	nullptr,                                      // 525 ERR_REMOTEPFX
	nullptr,                                      // 526 ERR_PFXUNROUTABLE
	nullptr,                                      // 527
	nullptr,                                      // 528
	nullptr,                                      // 529
	nullptr,                                      // 530
	nullptr,                                      // 531 ERR_CANTSENDTOUSER
	nullptr,                                      // 532
	nullptr,                                      // 533
	nullptr,                                      // 534
	nullptr,                                      // 535
	nullptr,                                      // 536
	nullptr,                                      // 537
	nullptr,                                      // 538
	nullptr,                                      // 539
	nullptr,                                      // 540
	nullptr,                                      // 541
	nullptr,                                      // 542
	nullptr,                                      // 543
	nullptr,                                      // 544
	nullptr,                                      // 545
	nullptr,                                      // 546
	nullptr,                                      // 547
	nullptr,                                      // 548
	nullptr,                                      // 549
	nullptr,                                      // 550 ERR_BADHOSTMASK
	nullptr,                                      // 551 ERR_HOSTUNAVAIL
	nullptr,                                      // 552 ERR_USINGSLINE
	nullptr,                                      // 553 ERR_STATSSLINE
	nullptr,                                      // 554
	nullptr,                                      // 555
	nullptr,                                      // 556
	nullptr,                                      // 557
	nullptr,                                      // 558
	nullptr,                                      // 559
	nullptr,                                      // 560
	nullptr,                                      // 561
	nullptr,                                      // 562
	nullptr,                                      // 563
	nullptr,                                      // 564
	nullptr,                                      // 565
	nullptr,                                      // 566
	nullptr,                                      // 567
	nullptr,                                      // 568
	nullptr,                                      // 569
	nullptr,                                      // 570
	nullptr,                                      // 571
	nullptr,                                      // 572
	nullptr,                                      // 573
	nullptr,                                      // 574
	nullptr,                                      // 575
	nullptr,                                      // 576
	nullptr,                                      // 577
	nullptr,                                      // 578
	nullptr,                                      // 579
	nullptr,                                      // 580
	nullptr,                                      // 581
	nullptr,                                      // 582
	nullptr,                                      // 583
	nullptr,                                      // 584
	nullptr,                                      // 585
	nullptr,                                      // 586
	nullptr,                                      // 587
	nullptr,                                      // 588
	nullptr,                                      // 589
	nullptr,                                      // 590
	nullptr,                                      // 591
	nullptr,                                      // 592
	nullptr,                                      // 593
	nullptr,                                      // 594
	nullptr,                                      // 595
	nullptr,                                      // 596
	nullptr,                                      // 597
	nullptr,                                      // 598
	nullptr,                                      // 599
	PTM(parseNumericWatch),                       // 600 RPL_LOGON
	PTM(parseNumericWatch),                       // 601 RPL_LOGOFF
	PTM(parseNumericWatch),                       // 602 RPL_WATCHOFF
	nullptr,                                      // 603 RPL_WATCHSTAT
	PTM(parseNumericWatch),                       // 604 RPL_NOWON
	PTM(parseNumericWatch),                       // 605 RPL_NOWOFF
	nullptr,                                      // 606 RPL_WATCHLIST
	nullptr,                                      // 607 RPL_ENDOFWATCH
	nullptr,                                      // 608 RPL_WATCHCLEAR
	nullptr,                                      // 609
	nullptr,                                      // 610 RPL_MAPMORE, RPL_ISOPER
	nullptr,                                      // 611 RPL_ISLOCOP
	nullptr,                                      // 612 RPL_ISNOTOPER
	nullptr,                                      // 613 RPL_ENDOFISOPER
	nullptr,                                      // 614
	nullptr,                                      // 615 RPL_MAPMORE
	nullptr,                                      // 616 RPL_WHOISHOST
	nullptr,                                      // 617 RPL_DCCSTATUS, RPL_WHOISBOT
	nullptr,                                      // 618 RPL_DCCLIST
	nullptr,                                      // 619 RPL_ENDOFDCCLIST, RPL_WHOWASHOST
	nullptr,                                      // 620 RPL_DCCINFO, RPL_RULESSTART
	nullptr,                                      // 621 RPL_RULES
	nullptr,                                      // 622 RPL_ENDOFRULES
	nullptr,                                      // 623 RPL_MAPMORE
	nullptr,                                      // 624 RPL_OMOTDSTART
	nullptr,                                      // 625 RPL_OMOTD
	nullptr,                                      // 626 RPL_ENDOFO
	nullptr,                                      // 627
	nullptr,                                      // 628
	nullptr,                                      // 629
	nullptr,                                      // 630 RPL_SETTINGS
	nullptr,                                      // 631 RPL_ENDOFSETTINGS
	nullptr,                                      // 632
	nullptr,                                      // 633
	nullptr,                                      // 634
	nullptr,                                      // 635
	nullptr,                                      // 636
	nullptr,                                      // 637
	nullptr,                                      // 638
	nullptr,                                      // 639
	nullptr,                                      // 640 RPL_DUMPING
	nullptr,                                      // 641 RPL_DUMPRPL
	nullptr,                                      // 642 RPL_EODUMP
	nullptr,                                      // 643
	nullptr,                                      // 644
	nullptr,                                      // 645
	nullptr,                                      // 646
	nullptr,                                      // 647
	nullptr,                                      // 648
	nullptr,                                      // 649
	nullptr,                                      // 650
	nullptr,                                      // 651
	nullptr,                                      // 652
	nullptr,                                      // 653
	nullptr,                                      // 654
	nullptr,                                      // 655
	nullptr,                                      // 656
	nullptr,                                      // 657
	nullptr,                                      // 658
	nullptr,                                      // 659
	nullptr,                                      // 660 RPL_TRACEROUTE_HOP
	nullptr,                                      // 661 RPL_TRACEROUTE_START
	nullptr,                                      // 662 RPL_MODECHANGEWARN
	nullptr,                                      // 663 RPL_CHANREDIR
	nullptr,                                      // 664 RPL_SERVMODEIS
	nullptr,                                      // 665 RPL_OTHERUMODEIS
	nullptr,                                      // 666 RPL_ENDOF_GENERIC
	nullptr,                                      // 667
	nullptr,                                      // 668
	nullptr,                                      // 669
	PTM(parseNumericStartTls),                    // 670 RPL_STARTTLSOK, RPL_WHOWASDETAILS
	PTM(parseNumericWhoisOther),                  // 671 RPL_WHOISSECURE
	nullptr,                                      // 672 RPL_UNKNOWNMODES
	nullptr,                                      // 673 RPL_CANNOTSETMODES
	nullptr,                                      // 674
	nullptr,                                      // 675
	nullptr,                                      // 676
	nullptr,                                      // 677
	nullptr,                                      // 678 RPL_LUSERSTAFF
	nullptr,                                      // 679 RPL_TIMEONSERVERIS
	nullptr,                                      // 680
	nullptr,                                      // 681
	nullptr,                                      // 682 RPL_NETWORKS
	nullptr,                                      // 683
	nullptr,                                      // 684
	nullptr,                                      // 685
	nullptr,                                      // 686
	nullptr,                                      // 687 RPL_YOURLANGUAGEIS
	nullptr,                                      // 688 RPL_LANGUAGE
	nullptr,                                      // 689 RPL_WHOISSTAFF
	nullptr,                                      // 690 RPL_WHOISLANGUAGE
	PTM(parseNumericStartTls),                    // 691 RPL_STARTTLSFAIL
	nullptr,                                      // 692
	nullptr,                                      // 693
	nullptr,                                      // 694
	nullptr,                                      // 695
	nullptr,                                      // 696
	nullptr,                                      // 697
	nullptr,                                      // 698
	nullptr,                                      // 699
	PTM(parseNumericCodePageSet),                 // 700 RPL_CODEPAGESET
	nullptr,                                      // 701
	nullptr,                                      // 702 RPL_MODLIST, RPL_COMMANDS
	PTM(parseNumericCodePageScheme),              // 703 RPL_WHOISSCHEME, RPL_ENDOFMODLIST, RPL_COMMANDSEND
	PTM(parseCommandSyntaxHelp),                  // 704 RPL_COMMANDSYNTAX, RPL_HELPSTART
	PTM(parseCommandHelp),                        // 705 RPL_COMMANDHELP, RPL_HELPTXT
	PTM(parseCommandEndOfHelp),                   // 706 RPL_ENDOFCOMMANDHELP, RPL_ENDOFHELP
	nullptr,                                      // 707
	nullptr,                                      // 708 RPL_ETRACEFULL
	nullptr,                                      // 709 RPL_ETRACE
	nullptr,                                      // 710 RPL_KNOCK
	nullptr,                                      // 711 RPL_KNOCKDLVR
	nullptr,                                      // 712 ERR_TOOMANYKNOCK
	nullptr,                                      // 713 ERR_CHANOPEN
	nullptr,                                      // 714 ERR_KNOCKONCHAN
	nullptr,                                      // 715 ERR_KNOCKDISABLED
	PTM(parseNumericNotifyGeneric),               // 716 RPL_TARGUMODEG
	PTM(parseNumericNotifyGeneric),               // 717 RPL_TARGNOTIFY
	PTM(parseNumericYouHaveCallerID),             // 718 RPL_UMODEGMSG
	nullptr,                                      // 719
	nullptr,                                      // 720 RPL_OMOTDSTART
	nullptr,                                      // 721 RPL_OMOTD
	nullptr,                                      // 722 RPL_ENDOFOMOTD
	nullptr,                                      // 723 ERR_NOPRIVS
	nullptr,                                      // 724 RPL_TESTMARK
	nullptr,                                      // 725 RPL_TESTLINE
	nullptr,                                      // 726 RPL_NOTESTLINE
	nullptr,                                      // 727
	PTM(parseNumeric728),                         // 728 RPL_QUIETLIST
	PTM(parseNumeric729),                         // 729 RPL_QUIETLISTEND
	nullptr,                                      // 730
	nullptr,                                      // 731
	nullptr,                                      // 732
	nullptr,                                      // 733
	nullptr,                                      // 734
	nullptr,                                      // 735
	nullptr,                                      // 736
	nullptr,                                      // 737
	nullptr,                                      // 738
	nullptr,                                      // 739
	nullptr,                                      // 740
	nullptr,                                      // 741
	PTM(parseNumeric742),                         // 742
	nullptr,                                      // 743
	nullptr,                                      // 744
	nullptr,                                      // 745
	nullptr,                                      // 746
	nullptr,                                      // 747
	nullptr,                                      // 748
	nullptr,                                      // 749
	nullptr,                                      // 750
	nullptr,                                      // 751
	nullptr,                                      // 752
	nullptr,                                      // 753
	nullptr,                                      // 754
	nullptr,                                      // 755
	nullptr,                                      // 756
	nullptr,                                      // 757
	nullptr,                                      // 758
	nullptr,                                      // 759
	nullptr,                                      // 760
	nullptr,                                      // 761
	nullptr,                                      // 762
	nullptr,                                      // 763
	nullptr,                                      // 764
	nullptr,                                      // 765
	nullptr,                                      // 766
	nullptr,                                      // 767
	nullptr,                                      // 768
	nullptr,                                      // 769
	nullptr,                                      // 770
	nullptr,                                      // 771 RPL_XINFO
	nullptr,                                      // 772
	nullptr,                                      // 773 RPL_XINFOSTART
	nullptr,                                      // 774 RPL_XINFOEND
	nullptr,                                      // 775
	nullptr,                                      // 776
	nullptr,                                      // 777
	nullptr,                                      // 778
	nullptr,                                      // 779
	nullptr,                                      // 780
	nullptr,                                      // 781
	nullptr,                                      // 782
	nullptr,                                      // 783
	nullptr,                                      // 784
	nullptr,                                      // 785
	nullptr,                                      // 786
	nullptr,                                      // 787
	nullptr,                                      // 788
	nullptr,                                      // 789
	nullptr,                                      // 790
	nullptr,                                      // 791
	nullptr,                                      // 792
	nullptr,                                      // 793
	nullptr,                                      // 794
	nullptr,                                      // 795
	nullptr,                                      // 796
	nullptr,                                      // 797
	nullptr,                                      // 798
	nullptr,                                      // 799
	nullptr,                                      // 800
	nullptr,                                      // 801
	nullptr,                                      // 802
	nullptr,                                      // 803
	nullptr,                                      // 804
	nullptr,                                      // 805
	nullptr,                                      // 806
	nullptr,                                      // 807
	nullptr,                                      // 808
	nullptr,                                      // 809
	nullptr,                                      // 810
	nullptr,                                      // 811
	nullptr,                                      // 812
	nullptr,                                      // 813
	nullptr,                                      // 814
	nullptr,                                      // 815
	nullptr,                                      // 816
	nullptr,                                      // 817
	nullptr,                                      // 818
	nullptr,                                      // 819
	nullptr,                                      // 820
	nullptr,                                      // 821
	nullptr,                                      // 822
	nullptr,                                      // 823
	nullptr,                                      // 824
	nullptr,                                      // 825
	nullptr,                                      // 826
	nullptr,                                      // 827
	nullptr,                                      // 828
	nullptr,                                      // 829
	nullptr,                                      // 830
	nullptr,                                      // 831
	nullptr,                                      // 832
	nullptr,                                      // 833
	nullptr,                                      // 834
	nullptr,                                      // 835
	nullptr,                                      // 836
	nullptr,                                      // 837
	nullptr,                                      // 838
	nullptr,                                      // 839
	nullptr,                                      // 840
	nullptr,                                      // 841
	nullptr,                                      // 842
	nullptr,                                      // 843
	nullptr,                                      // 844
	nullptr,                                      // 845
	nullptr,                                      // 846
	nullptr,                                      // 847
	nullptr,                                      // 848
	nullptr,                                      // 849
	nullptr,                                      // 850
	nullptr,                                      // 851
	nullptr,                                      // 852
	nullptr,                                      // 853
	nullptr,                                      // 854
	nullptr,                                      // 855
	nullptr,                                      // 856
	nullptr,                                      // 857
	nullptr,                                      // 858
	nullptr,                                      // 859
	nullptr,                                      // 860
	nullptr,                                      // 861
	nullptr,                                      // 862
	nullptr,                                      // 863
	nullptr,                                      // 864
	nullptr,                                      // 865
	nullptr,                                      // 866
	nullptr,                                      // 867
	nullptr,                                      // 868
	nullptr,                                      // 869
	nullptr,                                      // 870
	nullptr,                                      // 871
	nullptr,                                      // 872
	nullptr,                                      // 873
	nullptr,                                      // 874
	nullptr,                                      // 875
	nullptr,                                      // 876
	nullptr,                                      // 877
	nullptr,                                      // 878
	nullptr,                                      // 879
	nullptr,                                      // 880
	nullptr,                                      // 881
	nullptr,                                      // 882
	nullptr,                                      // 883
	nullptr,                                      // 884
	nullptr,                                      // 885
	nullptr,                                      // 886
	nullptr,                                      // 887
	nullptr,                                      // 888
	nullptr,                                      // 889
	nullptr,                                      // 890
	nullptr,                                      // 891
	nullptr,                                      // 892
	nullptr,                                      // 893
	nullptr,                                      // 894
	nullptr,                                      // 895
	nullptr,                                      // 896
	nullptr,                                      // 897
	nullptr,                                      // 898
	nullptr,                                      // 899
	PTM(parseNumericSaslLogin),                   // 900 RPL_SASLLOGIN
	nullptr,                                      // 901
	nullptr,                                      // 902
	PTM(parseNumericSaslSuccess),                 // 903 RPL_SASLSUCCESS
	PTM(parseNumericSaslFail),                    // 904 RPL_SASLFAILED
	PTM(parseNumericSaslFail),                    // 905 RPL_SASLERROR
	PTM(parseNumericSaslFail),                    // 906 RPL_SASLABORT
	PTM(parseNumericSaslFail),                    // 907 RPL_SASLALREADYAUTH
	PTM(parseNumericSaslFail),                    // 908 RPL_SASLMECHS
	nullptr,                                      // 909
	PTM(parseNumericChanAccessList),              // 910 RPL_CHANACCESS
	PTM(parseNumericEndOfChanAccessList),         // 911 RPL_ENDOFCHANACCESS
	nullptr,                                      // 912
	nullptr,                                      // 913
	nullptr,                                      // 914
	nullptr,                                      // 915
	nullptr,                                      // 916
	nullptr,                                      // 917
	nullptr,                                      // 918
	nullptr,                                      // 919
	nullptr,                                      // 920
	nullptr,                                      // 921
	nullptr,                                      // 922
	nullptr,                                      // 923
	nullptr,                                      // 924
	nullptr,                                      // 925
	nullptr,                                      // 926
	nullptr,                                      // 927
	nullptr,                                      // 928
	nullptr,                                      // 929
	nullptr,                                      // 930
	nullptr,                                      // 931
	nullptr,                                      // 932
	nullptr,                                      // 933
	nullptr,                                      // 934
	PTM(otherChannelError),                       // 935 RPL_WORDFILTERTOOLONG
	nullptr,                                      // 936 RPL_WORDFILTERED
	PTM(otherChannelError),                       // 937 RPL_WORDALREADYEXISTS
	PTM(otherChannelError),                       // 938 RPL_NOSUCHWORDFILTER
	PTM(otherChannelError),                       // 939 RPL_WORDFILTERFULL
	PTM(parseNumericEndOfSpamFilterList),         // 940 RPL_SPAMFILTERLIST
	PTM(parseNumericSpamFilterList),              // 941 RPL_ENDOFSPAMFILTERLIST
	nullptr,                                      // 942
	nullptr,                                      // 943
	nullptr,                                      // 944
	nullptr,                                      // 945
	nullptr,                                      // 946
	nullptr,                                      // 947
	nullptr,                                      // 948
	nullptr,                                      // 949
	nullptr,                                      // 950
	nullptr,                                      // 951
	nullptr,                                      // 952
	PTM(parseNumericEndOfExemptChanOpList),       // 953 RPL_ENDOFEXEMPTCHANOPLIST
	PTM(parseNumericExemptChanOpList),            // 954 RPL_EXEMPTCHANOPLIST
	nullptr,                                      // 955
	nullptr,                                      // 956
	nullptr,                                      // 957
	nullptr,                                      // 958
	nullptr,                                      // 959
	nullptr,                                      // 960
	nullptr,                                      // 961
	nullptr,                                      // 962
	nullptr,                                      // 963
	nullptr,                                      // 964
	nullptr,                                      // 965
	nullptr,                                      // 966
	nullptr,                                      // 967
	nullptr,                                      // 968
	nullptr,                                      // 969
	nullptr,                                      // 970
	nullptr,                                      // 971
	nullptr,                                      // 972 ERR_CANNOTDOCOMMAND, ERR_CANTUNLOADMODULE
	nullptr,                                      // 973 ERR_CANNOTCHANGEUMODE, RPL_UNLOADEDMODULE
	nullptr,                                      // 974 ERR_CANNOTCHANGECHANMODE, ERR_CANTLOADMODULE
	nullptr,                                      // 975 ERR_CANNOTCHANGESERVERMODE, RPL_LOADEDMODULE
	nullptr,                                      // 976 ERR_CANNOTSENDTONICK
	nullptr,                                      // 977 ERR_UNKNOWNSERVERMODE
	nullptr,                                      // 978
	nullptr,                                      // 979 ERR_SERVERMODELOCK
	nullptr,                                      // 980 ERR_BADCHARENCODING
	nullptr,                                      // 981 ERR_TOOMANYLANGUAGES
	nullptr,                                      // 982 ERR_NOLANGUAGE
	nullptr,                                      // 983 ERR_TEXTTOOSHORT
	nullptr,                                      // 984
	nullptr,                                      // 985
	nullptr,                                      // 986
	nullptr,                                      // 987
	nullptr,                                      // 988
	nullptr,                                      // 989
	nullptr,                                      // 990
	nullptr,                                      // 991
	nullptr,                                      // 992
	nullptr,                                      // 993
	nullptr,                                      // 994
	nullptr,                                      // 995
	nullptr,                                      // 996
	nullptr,                                      // 997
	nullptr,                                      // 998
	nullptr                                       // 999 ERR_NUMERIC_ERR
};
