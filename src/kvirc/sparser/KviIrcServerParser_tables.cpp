//=============================================================================
//
//   File : KviIrcServerParser_tables.cpp
//   Creation date : Sun Jun 30 2000 18:10:19 by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2000 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your opinion) any later version.
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

#define PTM(__m) KVI_PTR2MEMBER(KviIrcServerParser::__m)

KviLiteralMessageParseStruct KviIrcServerParser::m_literalParseProcTable[]=
{
	{ "AUTHENTICATE" , PTM(parseLiteralAuthenticate) },
	{ "CAP"          , PTM(parseLiteralCap)          },
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
	{ 0              , 0                             }
};

// FIXME: #warning "Merge these two tables into one"
KviCtcpMessageParseStruct KviIrcServerParser::m_ctcpRequestParseProcTable[]=
{
	{ "ACTION"     , PTM(parseCtcpRequestAction)     , 0 },
	{ "AVATAR"     , PTM(parseCtcpRequestAvatar)     , 0 },
	{ "CLIENTINFO" , PTM(parseCtcpRequestClientinfo) , 0 },
	{ "DCC"        , PTM(parseCtcpRequestDcc)        , 0 },
	{ "FINGER"     , PTM(parseCtcpRequestFinger)     , 0 },
	{ "PAGE"       , PTM(parseCtcpRequestPage)       , 0 },
	{ "PING"       , PTM(parseCtcpRequestPing)       , 0 },
	{ "SOURCE"     , PTM(parseCtcpRequestSource)     , 0 },
	{ "TDCC"       , PTM(parseCtcpRequestDcc)        , 0 },
	{ "TIME"       , PTM(parseCtcpRequestTime)       , 0 },
	{ "USERINFO"   , PTM(parseCtcpRequestUserinfo)   , 0 },
	{ "VERSION"    , PTM(parseCtcpRequestVersion)    , 0 },
	{ "XDCC"       , PTM(parseCtcpRequestDcc)        , 0 },
	{ 0            , 0                               , 0 }
};

KviCtcpMessageParseStruct KviIrcServerParser::m_ctcpReplyParseProcTable[]=
{
	{ "ACTION"     , PTM(parseCtcpRequestAction)     , 0 },
	{ "AVATAR"     , PTM(parseCtcpReplyAvatar)       , 0 },
	{ "CLIENTINFO" , PTM(parseCtcpReplyGeneric)      , 0 },
	{ "DCC"        , PTM(parseCtcpRequestDcc)        , 0 },
	{ "FINGER"     , PTM(parseCtcpReplyGeneric)      , 0 },
	{ "LAGCHECK"   , PTM(parseCtcpReplyLagcheck)     , KVI_CTCP_MESSAGE_PARSE_TRIGGERNOEVENT },
	{ "PAGE"       , PTM(parseCtcpReplyGeneric)      , 0 },
	{ "PING"       , PTM(parseCtcpReplyPing)         , 0 },
	{ "SOURCE"     , PTM(parseCtcpReplyGeneric)      , 0 },
	{ "TDCC"       , PTM(parseCtcpRequestDcc)        , 0 },
	{ "TIME"       , PTM(parseCtcpReplyGeneric)      , 0 },
	{ "USERINFO"   , PTM(parseCtcpReplyUserinfo)     , 0 },
	{ "VERSION"    , PTM(parseCtcpReplyGeneric)      , 0 },
	{ "XDCC"       , PTM(parseCtcpRequestDcc)        , 0 },
	{ 0            , 0                               , 0 }
};

messageParseProc KviIrcServerParser::m_numericParseProcTable[1000]=
{
	0,                                               // 000
	PTM(parseNumeric001)                           , // 001 RPL_WELCOME
	PTM(parseNumeric002)                           , // 002 RPL_YOURHOST
	PTM(parseNumeric003)                           , // 003 RPL_CREATED
	PTM(parseNumeric004)                           , // 004 RPL_MYINFO
	PTM(parseNumeric005)                           , // 005 RPL_BOUNCE, RPL_PROTOCTL, RPL_ISUPPORT
	0,                                               // 006 RPL_MAP
	0,                                               // 007 RPL_MAPEND
	0,                                               // 008 RPL_SNOMASK
	0,                                               // 009 RPL_STATMEMTOT
	0,                                               // 010 RPL_BOUNCE, RPL_REDIR, RPL_STATMEM
	0,                                               // 011
	0,                                               // 012
	0,                                               // 013
	0,                                               // 014 RPL_YOURCOOKIE
	0,                                               // 015 RPL_MAP
	0,                                               // 016 RPL_MAPMORE
	0,                                               // 017 RPL_MAPEND
	0,                                               // 018
	0,                                               // 019
	PTM(parseNumeric020)                           , // 020
	0,                                               // 021
	0,                                               // 022
	0,                                               // 023
	0,                                               // 024
	0,                                               // 025
	0,                                               // 026
	0,                                               // 027
	0,                                               // 028
	0,                                               // 029
	0,                                               // 030
	0,                                               // 031
	0,                                               // 032
	0,                                               // 033
	0,                                               // 034
	0,                                               // 035
	0,                                               // 036
	0,                                               // 037
	0,                                               // 038
	0,                                               // 039
	0,                                               // 040
	0,                                               // 041
	0,                                               // 042 RPL_YOURID
	0,                                               // 043 RPL_SAVENICK
	0,                                               // 044
	0,                                               // 045
	0,                                               // 046
	0,                                               // 047
	0,                                               // 048
	0,                                               // 049
	0,                                               // 050 RPL_ATTEMPTINGJUNC
	0,                                               // 051 RPL_ATTEMPTINGREROUTE
	0,                                               // 052
	0,                                               // 053
	0,                                               // 054
	0,                                               // 055
	0,                                               // 056
	0,                                               // 057
	0,                                               // 058
	0,                                               // 059
	0,                                               // 060
	0,                                               // 061
	0,                                               // 062
	0,                                               // 063
	0,                                               // 064
	0,                                               // 065
	0,                                               // 066
	0,                                               // 067
	0,                                               // 068
	0,                                               // 069
	0,                                               // 070
	0,                                               // 071
	0,                                               // 072
	0,                                               // 073
	0,                                               // 074
	0,                                               // 075
	0,                                               // 076
	0,                                               // 077
	0,                                               // 078
	0,                                               // 079
	0,                                               // 080
	0,                                               // 081
	0,                                               // 082
	0,                                               // 083
	0,                                               // 084
	0,                                               // 085
	0,                                               // 086
	0,                                               // 087
	0,                                               // 088
	0,                                               // 089
	0,                                               // 090
	0,                                               // 091
	0,                                               // 092
	0,                                               // 093
	0,                                               // 094
	0,                                               // 095
	0,                                               // 096
	0,                                               // 097
	0,                                               // 098
	0,                                               // 099
	0,                                               // 100
	0,                                               // 101
	0,                                               // 102
	0,                                               // 103
	0,                                               // 104
	0,                                               // 105
	0,                                               // 106
	0,                                               // 107
	0,                                               // 108
	0,                                               // 109
	0,                                               // 110
	0,                                               // 111
	0,                                               // 112
	0,                                               // 113
	0,                                               // 114
	0,                                               // 115
	0,                                               // 116
	0,                                               // 117
	0,                                               // 118
	0,                                               // 119
	0,                                               // 120
	0,                                               // 121
	0,                                               // 122
	0,                                               // 123
	0,                                               // 124
	0,                                               // 125
	0,                                               // 126
	0,                                               // 127
	0,                                               // 128
	0,                                               // 129
	0,                                               // 130
	0,                                               // 131
	0,                                               // 132
	0,                                               // 133
	0,                                               // 134
	0,                                               // 135
	0,                                               // 136
	0,                                               // 137
	0,                                               // 138
	0,                                               // 139
	0,                                               // 140
	0,                                               // 141
	0,                                               // 142
	0,                                               // 143
	0,                                               // 144
	0,                                               // 145
	0,                                               // 146
	0,                                               // 147
	0,                                               // 148
	0,                                               // 149
	0,                                               // 150
	0,                                               // 151
	0,                                               // 152
	0,                                               // 153
	0,                                               // 154
	0,                                               // 155
	0,                                               // 156
	0,                                               // 157
	0,                                               // 158
	0,                                               // 159
	0,                                               // 160
	0,                                               // 161
	0,                                               // 162
	0,                                               // 163
	0,                                               // 164
	0,                                               // 165
	0,                                               // 166
	0,                                               // 167
	0,                                               // 168
	0,                                               // 169
	0,                                               // 170
	0,                                               // 171
	0,                                               // 172
	0,                                               // 173
	0,                                               // 174
	0,                                               // 175
	0,                                               // 176
	0,                                               // 177
	0,                                               // 178
	0,                                               // 179
	0,                                               // 180
	0,                                               // 181
	0,                                               // 182
	0,                                               // 183
	0,                                               // 184
	0,                                               // 185
	0,                                               // 186
	0,                                               // 187
	0,                                               // 188
	0,                                               // 189
	0,                                               // 190
	0,                                               // 191
	0,                                               // 192
	0,                                               // 193
	0,                                               // 194
	0,                                               // 195
	0,                                               // 196
	0,                                               // 197
	0,                                               // 198
	0,                                               // 199
	0,                                               // 200 RPL_TRACELINK
	0,                                               // 201 RPL_TRACECONNECTING
	0,                                               // 202 RPL_TRACEHANDSHAKE
	0,                                               // 203 RPL_TRACEUNKNOWN
	0,                                               // 204 RPL_TRACEOPERATOR
	0,                                               // 205 RPL_TRACEUSER
	0,                                               // 206 RPL_TRACESERVER
	0,                                               // 207 RPL_TRACESERVICE
	0,                                               // 208 RPL_TRACENEWTYPE
	0,                                               // 209 RPL_TRACECLASS
	0,                                               // 210 RPL_TRACERECONNECT, RPL_STATS
	0,                                               // 211 RPL_STATSLINKINFO
	0,                                               // 212 RPL_STATSCOMMANDS
	0,                                               // 213 RPL_STATSCLINE
	0,                                               // 214 RPL_STATSNLINE
	0,                                               // 215 RPL_STATSILINE
	0,                                               // 216 RPL_STATSKLINE
	0,                                               // 217 RPL_STATSQLINE, RPL_STATSPLINE
	0,                                               // 218 RPL_STATSYLINE
	PTM(parseNumericEndOfStats),                     // 219 RPL_ENDOFSTATS
	0,                                               // 220 RPL_STATSPLINE, RPL_STATSBLINE
	PTM(parseNumericUserMode)                      , // 221 RPL_UMODEIS
	PTM(parseNumericCodePageSet),                    // 222 RusNet Codepage set, RPL_MODLIST, RPL_SQLINE_NICK, RPL_STATSBLINE
	0,                                               // 223 RPL_STATSELINE, RPL_STATSGLINE
	0,                                               // 224 RPL_STATSFLINE, RPL_STATSTLINE
	0,                                               // 225 RPL_STATSDLINE, RPL_STATSZLINE, RPL_STATSELINE
	0,                                               // 226 RPL_STATSCOUNT, RPL_STATSNLINE
	0,                                               // 227 RPL_STATSGLINE, RPL_STATSVLINE
	0,                                               // 228 RPL_STATSQLINE
	0,                                               // 229
	0,                                               // 230
	0,                                               // 231 RPL_SERVICEINFO
	0,                                               // 232 RPL_ENDOFSERVICES, RPL_RULES
	0,                                               // 233 RPL_SERVICE
	0,                                               // 234 RPL_SERVLIST
	0,                                               // 235 RPL_SERVLISTEND
	0,                                               // 236 RPL_STATSVERBOSE
	0,                                               // 237 RPL_STATSENGINE
	0,                                               // 238 RPL_STATSFLINE
	PTM(parseNumericStats)                         , // 239 RPL_STATSIAUTH
	PTM(parseNumericStats)                         , // 240 RPL_STATSVLINE
	PTM(parseNumericStats)                         , // 241 RPL_STATSLLINE
	PTM(parseNumericStats)                         , // 242 RPL_STATSUPTIME
	PTM(parseNumericStats)                         , // 243 RPL_STATSOLINE
	PTM(parseNumericStats)                         , // 244 RPL_STATSHLINE
	PTM(parseNumericStats)                         , // 245 RPL_STATSSLINE
	PTM(parseNumericStats)                         , // 246 RPL_STATSPING RPL_STATSTLINE
	PTM(parseNumericStats)                         , // 247 RPL_STATSBLINE_I RPL_STATSGLINE RPL_STATSXLINE
	PTM(parseNumericStats)                         , // 248 RPL_STATSDEFINE RPL_STATSULINE
	PTM(parseNumericStats)                         , // 249 RPL_STATSDEBUG
	PTM(parseNumericStats)                         , // 250 RPL_STATSCONN RPL_STATSDLINE_I
	PTM(parseNumericStats)                         , // 251 RPL_LUSERCLIENT
	PTM(parseNumericStats)                         , // 252 RPL_LUSEROP
	PTM(parseNumericStats)                         , // 253 RPL_LUSERUNKNOWN
	PTM(parseNumericStats)                         , // 254 RPL_LUSERCHANNELS
	PTM(parseNumericStats)                         , // 255 RPL_LUSERME
	PTM(parseNumericServerAdminInfoTitle)          , // 256 RPL_ADMINME
	PTM(parseNumericServerAdminInfoServerName)     , // 257 RPL_ADMINLOC1
	PTM(parseNumericServerAdminInfoAdminName)      , // 258 RPL_ADMINLOC2
	PTM(parseNumericServerAdminInfoAdminContact)   , // 259 RPL_ADMINEMAIL
	0,                                               // 260
	0,                                               // 261 RPL_TRACELOG
	0,                                               // 262 RPL_TRACEPING, RPL_TRACEEND
	0,                                               // 263 RPL_TRYAGAIN
	0,                                               // 264
	PTM(parseNumericStats)                         , // 265 RPL_LOCALUSERS
	PTM(parseNumericStats)                         , // 266 RPL_GLOBALUSERS
	PTM(parseNumericStats)                         , // 267 RPL_SHAREDUSERS, RPL_START_NETSTAT
	0,                                               // 268 RPL_NETSTAT
	0,                                               // 269 RPL_END_NETSTAT
	0,                                               // 270 RPL_PRIVS, RPL_MAPUSERS
	0,                                               // 271 RPL_SILELIST
	0,                                               // 272 RPL_ENDOFSILELIST
	0,                                               // 273 RPL_NOTIFY
	0,                                               // 274 RPL_ENDOFNOTIFY, RPL_STATSDELTA
	PTM(parseNumericWhoisOther)                    , // 275 RPL_WHOISSSL, RPL_STATSDLINE
	0,                                               // 276 RPL_VCHANEXIST
	0,                                               // 277 RPL_VCHANLIST
	0,                                               // 278 RPL_VCHANHELP
	0,                                               // 279
	0,                                               // 280 RPL_GLIST
	0,                                               // 281 RPL_ENDOFGLIST, RPL_ACCEPTLIST
	0,                                               // 282 RPL_ENDOFACCEPT, RPL_JUPELIST
	0,                                               // 283 RPL_ALIST, RPL_ENDOFJUPELIST
	0,                                               // 284 RPL_ENDOFALIST, RPL_FEATURE
	0,                                               // 285 RPL_FEATURE, RPL_CHANINFO_HANDLE
	0,                                               // 286 RPL_CHANINFO_USERS, RPL_CHKHEAD
	0,                                               // 287 RPL_CHANINFO_CHOPS, RPL_CHANUSER
	0,                                               // 288 RPL_CHANINFO_VOICES, RPL_PATCHHEAD
	0,                                               // 289 RPL_CHANINFO_AWAY, RPL_PATCHCON
	0,                                               // 290 RPL_CHANINFO_OPERS, RPL_HELPHDR, RPL_DATASTR
	0,                                               // 291 RPL_CHANINFO_BANNED, RPL_HELPOP, RPL_ENDOFCHECK
	0,                                               // 292 RPL_CHANINFO_BANS, RPL_HELPTLR
	0,                                               // 293 RPL_CHANINFO_INVITE, RPL_HELPHLP
	0,                                               // 294 RPL_CHANINFO_INVITES, RPL_HELPFWD
	0,                                               // 295 RPL_CHANINFO_KICK, RPL_HELPIGN
	0,                                               // 296 RPL_CHANINFO_KICKS
	0,                                               // 297
	0,                                               // 298
	0,                                               // 299 RPL_END_CHANINFO
	0,                                               // 300 RPL_NONE
	PTM(parseNumericWhoisAway)                     , // 301 RPL_AWAY
	PTM(parseNumericUserhost)                      , // 302 RPL_USERHOST
	PTM(parseNumericIsOn)                          , // 303 RPL_ISON
	0,                                               // 304 RPL_TEXT, RPL_SYNTAX
	PTM(parseNumericBackFromAway)                  , // 305 RPL_UNAWAY
	PTM(parseNumericAway)                          , // 306 RPL_NOWAWAY
	PTM(parseNumericWhoisAuth)                     , // 307 RPL_WHOISREGNICK
	PTM(parseNumericWhoisOther)                    , // 308 RPL_WHOISADMIN, RPL_RULESTART
	PTM(parseNumericWhoisOther)                    , // 309 RPL_WHOISSADMIN, RPL_RULESEND
	PTM(parseNumericWhoisOther)                    , // 310 RPL_WHOISHELPOP
	PTM(parseNumericWhoisUser)                     , // 311 RPL_WHOISUSER
	PTM(parseNumericWhoisServer)                   , // 312 RPL_WHOISSERVER
	PTM(parseNumericWhoisOther)                    , // 313 RPL_WHOISOPERATOR
	PTM(parseNumericWhowasUser)                    , // 314 RPL_WHOWASUSER
	PTM(parseNumericEndOfWho)                      , // 315 RPL_ENDOFWHO
	PTM(parseNumericWhoisOther)                    , // 316 RPL_WHOISCHANOP
	PTM(parseNumericWhoisIdle)                     , // 317 RPL_WHOISIDLE
	PTM(parseNumericEndOfWhois)                    , // 318 RPL_ENDOFWHOIS
	PTM(parseNumericWhoisChannels)                 , // 319 RPL_WHOISCHANNELS
	PTM(parseNumericWhoisOther)                    , // 320 RPL_WHOISIDENTIFIED
	PTM(parseNumericListStart)                     , // 321 RPL_LISTSTART
	PTM(parseNumericList)                          , // 322 RPL_LIST
	PTM(parseNumericListEnd)                       , // 323 RPL_LISTEND
	PTM(parseNumericChannelModeIs)                 , // 324 RPL_CHANNELMODEIS
	0,                                               // 325 RPL_UNIQOPIS, RPL_CHANNELPASSIS
	0,                                               // 326 RPL_NOCHANPASS
	0,                                               // 327 RPL_CHPASSUNKNOWN
	PTM(parseNumericChanUrl)                       , // 328 RPL_CHANNEL_URL
	PTM(parseNumericCreationTime)                  , // 329 RPL_CREATIONTIME, RPL_CHANNELCREATED
	PTM(parseNumericWhoisAuth)                     , // 330 RPL_WHOISAUTH
	PTM(parseNumericNoTopic)                       , // 331 RPL_NOTOPIC, RPL_NOTOPICSET
	PTM(parseNumericTopic)                         , // 332 RPL_TOPIC
	PTM(parseNumericTopicWhoTime)                  , // 333 RPL_TOPICWHOTIME, RPL_TOPICTIME
	PTM(parseNumericCommandSyntax)                 , // 334 RPL_COMMANDSYNTAX
	0,                                               // 335 RPL_WHOISBOT
	0,                                               // 336
	0,                                               // 337
	PTM(parseNumericWhoisActually),                  // 338 RPL_WHOISACTUALLY, RPL_CHANPASSOK
	PTM(parseNumericWhoisOther),                     // 339 RPL_BADCHANPASS
	0,                                               // 340 RPL_USERIP
	PTM(parseNumericInviting)                      , // 341 RPL_INVITING
	0,                                               // 342 RPL_SUMMONING
	0,                                               // 343
	PTM(parseNumericReopList)                      , // 344 RPL_REOPLIST
	PTM(parseNumericEndOfReopList)                 , // 345 RPL_ENDOFREOPLIST
	PTM(parseNumericInviteList)                    , // 346 RPL_INVITELIST
	PTM(parseNumericEndOfInviteList)               , // 347 RPL_ENDOFINVITELIST
	PTM(parseNumericExceptList)                    , // 348 RPL_EXCEPTLIST
	PTM(parseNumericEndOfExceptList)               , // 349 RPL_ENDOFEXCEPTLIST
	0,                                               // 350
	0,                                               // 351 RPL_VERSION
	PTM(parseNumericWhoReply)                      , // 352 RPL_WHOREPLY
	PTM(parseNumericNames)                         , // 353 RPL_NAMREPLY
	0,                                               // 354 RPL_WHOSPCRPL
	0,                                               // 355 RPL_NAMREPLY_
	0,                                               // 356
	0,                                               // 357 RPL_MAP
	0,                                               // 358 RPL_MAPMORE
	0,                                               // 359 RPL_MAPEND
	0,                                               // 360
	0,                                               // 361 RPL_KILLDONE
	0,                                               // 362 RPL_CLOSING
	0,                                               // 363 RPL_CLOSEEND
	PTM(parseNumericLinks)                         , // 364 RPL_LINKS
	PTM(parseNumericEndOfLinks)                    , // 365 RPL_ENDOFLINKS
	PTM(parseNumericEndOfNames)                    , // 366 RPL_ENDOFNAMES
	PTM(parseNumeric367)                           , // 367 RPL_BANLIST
	PTM(parseNumeric368)                           , // 368 RPL_ENDOFBANLIST
	PTM(parseNumericEndOfWhowas)                   , // 369 RPL_ENDOFWHOWAS
	0,                                               // 370
	PTM(parseNumericInfo)                          , // 371 RPL_INFO
	PTM(parseNumericMotd)                          , // 372 RPL_MOTD
	PTM(parseNumericInfoStart)                     , // 373 RPL_INFOSTART
	PTM(parseNumericInfoEnd)                       , // 374 RPL_ENDOFINFO
	PTM(parseNumericMotd)                          , // 375 RPL_MOTDSTART
	PTM(parseNumericMotd)                          , // 376 RPL_ENDOFMOTD
	PTM(parseNumericMotd)                          , // 377 RPL_MOTD2, RPL_KICKEXPIRED, RPL_SPAM
	PTM(parseNumericWhoisOther)                    , // 378 RPL_WHOISHOST, RPL_BANEXPIRED
	0,                                               // 379 RPL_KICKLINKED, RPL_WHOISMODES, RPL_WHOWASIP
	0,                                               // 380 RPL_BANLINKED, RPL_YOURHELPER
	0,                                               // 381 RPL_YOUREOPER
	0,                                               // 382 RPL_REHASHING
	0,                                               // 383 RPL_YOURESERVICE
	0,                                               // 384 RPL_MYPORTIS
	0,                                               // 385 RPL_NOTOPERANYMORE
	PTM(parseNumericQList)                         , // 386 RPL_QLIST
	PTM(parseNumericEndOfQList)                    , // 387 RPL_ENDOFQLIST, RPL_ENDOFIRCOPS
	PTM(parseNumericAList)                         , // 388 RPL_ALIST
	PTM(parseNumericEndOfAList)                    , // 389 RPL_ENDOFALIST
	0,                                               // 390
	PTM(parseNumericTime)                          , // 391 RPL_TIME
	0,                                               // 392 RPL_USERSSTART
	0,                                               // 393 RPL_USERS
	0,                                               // 394 RPL_ENDOFUSERS
	0,                                               // 395 RPL_NOUSERS
	0,                                               // 396 RPL_HOSTHIDDEN, RPL_YOURDISPLAYEDHOST
	0,                                               // 397
	0,                                               // 398
	0,                                               // 399
	0,                                               // 400 ERR_UNKNOWNERROR
	PTM(parseNumericNoSuchNick)                    , // 401 ERR_NOSUCHNICK
	PTM(parseNumericNoSuchServer)                  , // 402 ERR_NOSUCHSERVER
	PTM(parseNumericNoSuchChannel)                 , // 403 ERR_NOSUCHCHANNEL
	PTM(parseNumericCannotSend)                    , // 404 ERR_CANNOTSENDTOCHAN
	0,                                               // 405 ERR_TOOMANYCHANNELS
	PTM(parseNumericNoSuchNick)                    , // 406 ERR_WASNOSUCHNICK
	0,                                               // 407 ERR_TOOMANYTARGETS
	PTM(parseNumericCannotSendColor)               , // 408 ERR_NOCOLORSONCHAN, ERR_NOSUCHSERVICE
	0,                                               // 409 ERR_NOORIGIN
	0,                                               // 410 ERR_INVALIDCAPSUBCOMMAND
	0,                                               // 411 ERR_NORECIPIENT
	0,                                               // 412 ERR_NOTEXTTOSEND
	0,                                               // 413 ERR_NOTOPLEVEL
	0,                                               // 414 ERR_WILDTOPLEVEL
	0,                                               // 415 ERR_BADMASK
	0,                                               // 416 ERR_TOOMANYMATCHES, ERR_QUERYTOOLONG
	0,                                               // 417
	0,                                               // 418
	0,                                               // 419 ERR_LENGTHTRUNCATED
	0,                                               // 420
	0,                                               // 421 ERR_UNKNOWNCOMMAND
	0,                                               // 422 ERR_NOMOTD
	0,                                               // 423 ERR_NOADMININFO
	0,                                               // 424 ERR_FILEERROR
	0,                                               // 425 ERR_NOOPERMOTD
	0,                                               // 426
	0,                                               // 427
	0,                                               // 428
	0,                                               // 429 ERR_TOOMANYAWAY
	0,                                               // 430 ERR_EVENTNICKCHANGE
	0,                                               // 431 ERR_NONICKNAMEGIVEN
	PTM(parseNumericNicknameProblem)               , // 432 ERR_ERRONEUSNICKNAME
	PTM(parseNumericNicknameProblem)               , // 433 ERR_NICKNAMEINUSE
	0,                                               // 434 ERR_SERVICENAMEINUSE, ERR_NORULES
	0,                                               // 435 ERR_SERVICECONFUSED, ERR_BANONCHAN
	0,                                               // 436 ERR_NICKCOLLISION
	PTM(parseNumericUnavailResource)               , // 437 ERR_UNAVAILRESOURCE, ERR_BANNICKCHANGE
	0,                                               // 438 ERR_NICKTOOFAST, ERR_DEAD
	0,                                               // 439 ERR_TARGETTOOFAST
	0,                                               // 440 ERR_SERVICESDOWN
	0,                                               // 441 ERR_USERNOTINCHANNEL
	0,                                               // 442 ERR_NOTONCHANNEL
	0,                                               // 443 ERR_USERONCHANNEL
	0,                                               // 444 ERR_NOLOGIN
	0,                                               // 445 ERR_SUMMONDISABLED
	0,                                               // 446 ERR_USERSDISABLED
	0,                                               // 447 ERR_NONICKCHANGE, ERR_CANTCHANGENICK
	0,                                               // 448
	0,                                               // 449 ERR_NOTIMPLEMENTED
	0,                                               // 450
	PTM(parseNumericNotRegistered)                 , // 451 ERR_NOTREGISTERED
	0,                                               // 452 ERR_IDCOLLISION
	0,                                               // 453 ERR_NICKLOST
	0,                                               // 454
	0,                                               // 455 ERR_HOSTILENAME
	0,                                               // 456 ERR_ACCEPTFULL
	0,                                               // 457 ERR_ACCEPTEXIST
	0,                                               // 458 ERR_ACCEPTNOT
	0,                                               // 459 ERR_NOHIDING
	0,                                               // 460 ERR_NOTFORHALFOPS
	0,                                               // 461 ERR_NEEDMOREPARAMS
	0,                                               // 462 ERR_ALREADYREGISTERED
	0,                                               // 463 ERR_NOPERMFORHOST
	0,                                               // 464 ERR_PASSWDMISMATCH
	0,                                               // 465 ERR_YOUREBANNEDCREEP
	0,                                               // 466 ERR_YOUWILLBEBANNED
	PTM(otherChannelError),                          // 467 ERR_KEYSET
	0,                                               // 468 ERR_ONLYSERVERSCANCHANGE
	0,                                               // 469 ERR_LINKSET
	0,                                               // 470 ERR_LINKCHANNEL, ERR_KICKEDFROMCHAN
	PTM(parseNumericCantJoinChannel)               , // 471 ERR_CHANNELISFULL
	PTM(otherChannelError),                          // 472 ERR_UNKNOWNMODE
	PTM(parseNumericCantJoinChannel)               , // 473 ERR_INVITEONLYCHAN
	PTM(parseNumericCantJoinChannel)               , // 474 ERR_BANNEDFROMCHAN
	PTM(parseNumericCantJoinChannel)               , // 475 ERR_BADCHANNELKEY
	0,                                               // 476 ERR_BADCHANMASK
	PTM(parseChannelHelp),                           // 477 RPL_CHANNELHELP
	0,                                               // 478 ERR_BANLISTFULL
	0,                                               // 479 ERR_BADCHANNAME, ERR_LINKFAIL
	0,                                               // 480 ERR_NOULINE, ERR_CANNOTKNOCK
	0,                                               // 481 ERR_NOPRIVILEGES
	PTM(otherChannelError),                          // 482 ERR_CHANOPRIVSNEEDED
	0,                                               // 483 ERR_CANTKILLSERVER
	0,                                               // 484 ERR_RESTRICTED, ERR_ISCHANSERVICE, ERR_DESYNC, ERR_ATTACKDENY
	0,                                               // 485 ERR_UNIQOPRIVSNEEDED, ERR_KILLDENY, ERR_CANTKICKADMIN, ERR_ISREALSERVICE
	0,                                               // 486 ERR_NONONREG, ERR_HTMDISABLED, ERR_ACCOUNTONLY
	0,                                               // 487 ERR_CHANTOORECENT, ERR_MSGSERVICES
	0,                                               // 488 ERR_TSLESSCHAN
	0,                                               // 489 ERR_VOICENEEDED, ERR_SECUREONLYCHAN
	0,                                               // 490 ERR_ALLMUSTSSL
	0,                                               // 491 ERR_NOOPERHOST
	0,                                               // 492 ERR_NOSERVICEHOST, ERR_NOCTCPALLOWED
	0,                                               // 493 ERR_NOFEATURE
	0,                                               // 494 ERR_BADFEATURE
	0,                                               // 495 ERR_BADLOGTYPE, ERR_DELAYREJOIN
	0,                                               // 496 ERR_BADLOGSYS
	0,                                               // 497 ERR_BADLOGVALUE
	0,                                               // 498 ERR_ISOPERLCHAN
	0,                                               // 499 ERR_CHANOWNPRIVNEEDED
	0,                                               // 500
	0,                                               // 501 ERR_UMODEUNKNOWNFLAG, ERR_UNKNOWNSNOMASK
	0,                                               // 502 ERR_USERSDONTMATCH
	0,                                               // 503 ERR_GHOSTEDCLIENT
	0,                                               // 504 ERR_USERNOTONSERV
	0,                                               // 505
	0,                                               // 506
	0,                                               // 507
	0,                                               // 508
	0,                                               // 509
	0,                                               // 510
	0,                                               // 511 ERR_SILELISTFULL
	0,                                               // 512 ERR_TOOMANYWATCH
	0,                                               // 513 ERR_BADPING
	0,                                               // 514 ERR_INVALID_ERROR, ERR_TOOMANYDCC
	0,                                               // 515 ERR_BADEXPIRE
	0,                                               // 516 ERR_DONTCHEAT
	0,                                               // 517 ERR_DISABLED
	0,                                               // 518 ERR_NOINVITE, ERR_LONGMASK
	0,                                               // 519 ERR_ADMONLY, ERR_TOOMANYUSERS
	0,                                               // 520 ERR_OPERONLY, ERR_MASKTOOWIDE, ERR_WHOTRUNC, ERR_CANTJOINOPERSONLY
	0,                                               // 521 ERR_LISTSYNTAX
	0,                                               // 522 ERR_WHOSYNTAX
	0,                                               // 523 ERR_WHOLIMEXCEED
	0,                                               // 524 ERR_QUARANTINED, ERR_OPERSPVERIFY
	0,                                               // 525 ERR_REMOTEPFX
	0,                                               // 526 ERR_PFXUNROUTABLE
	0,                                               // 527
	0,                                               // 528
	0,                                               // 529
	0,                                               // 530
	0,                                               // 531 ERR_CANTSENDTOUSER
	0,                                               // 532
	0,                                               // 533
	0,                                               // 534
	0,                                               // 535
	0,                                               // 536
	0,                                               // 537
	0,                                               // 538
	0,                                               // 539
	0,                                               // 540
	0,                                               // 541
	0,                                               // 542
	0,                                               // 543
	0,                                               // 544
	0,                                               // 545
	0,                                               // 546
	0,                                               // 547
	0,                                               // 548
	0,                                               // 549
	0,                                               // 550 ERR_BADHOSTMASK
	0,                                               // 551 ERR_HOSTUNAVAIL
	0,                                               // 552 ERR_USINGSLINE
	0,                                               // 553 ERR_STATSSLINE
	0,                                               // 554
	0,                                               // 555
	0,                                               // 556
	0,                                               // 557
	0,                                               // 558
	0,                                               // 559
	0,                                               // 560
	0,                                               // 561
	0,                                               // 562
	0,                                               // 563
	0,                                               // 564
	0,                                               // 565
	0,                                               // 566
	0,                                               // 567
	0,                                               // 568
	0,                                               // 569
	0,                                               // 570
	0,                                               // 571
	0,                                               // 572
	0,                                               // 573
	0,                                               // 574
	0,                                               // 575
	0,                                               // 576
	0,                                               // 577
	0,                                               // 578
	0,                                               // 579
	0,                                               // 580
	0,                                               // 581
	0,                                               // 582
	0,                                               // 583
	0,                                               // 584
	0,                                               // 585
	0,                                               // 586
	0,                                               // 587
	0,                                               // 588
	0,                                               // 589
	0,                                               // 590
	0,                                               // 591
	0,                                               // 592
	0,                                               // 593
	0,                                               // 594
	0,                                               // 595
	0,                                               // 596
	0,                                               // 597
	0,                                               // 598
	0,                                               // 599
	PTM(parseNumericWatch)                         , // 600 RPL_LOGON
	PTM(parseNumericWatch)                         , // 601 RPL_LOGOFF
	PTM(parseNumericWatch)                         , // 602 RPL_WATCHOFF
	0,                                               // 603 RPL_WATCHSTAT
	PTM(parseNumericWatch)                         , // 604 RPL_NOWON
	PTM(parseNumericWatch)                         , // 605 RPL_NOWOFF
	0,                                               // 606 RPL_WATCHLIST
	0,                                               // 607 RPL_ENDOFWATCH
	0,                                               // 608 RPL_WATCHCLEAR
	0,                                               // 609
	0,                                               // 610 RPL_MAPMORE, RPL_ISOPER
	0,                                               // 611 RPL_ISLOCOP
	0,                                               // 612 RPL_ISNOTOPER
	0,                                               // 613 RPL_ENDOFISOPER
	0,                                               // 614
	0,                                               // 615 RPL_MAPMORE
	0,                                               // 616 RPL_WHOISHOST
	0,                                               // 617 RPL_DCCSTATUS, RPL_WHOISBOT
	0,                                               // 618 RPL_DCCLIST
	0,                                               // 619 RPL_ENDOFDCCLIST, RPL_WHOWASHOST
	0,                                               // 620 RPL_DCCINFO, RPL_RULESSTART
	0,                                               // 621 RPL_RULES
	0,                                               // 622 RPL_ENDOFRULES
	0,                                               // 623 RPL_MAPMORE
	0,                                               // 624 RPL_OMOTDSTART
	0,                                               // 625 RPL_OMOTD
	0,                                               // 626 RPL_ENDOFO
	0,                                               // 627
	0,                                               // 628
	0,                                               // 629
	0,                                               // 630 RPL_SETTINGS
	0,                                               // 631 RPL_ENDOFSETTINGS
	0,                                               // 632
	0,                                               // 633
	0,                                               // 634
	0,                                               // 635
	0,                                               // 636
	0,                                               // 637
	0,                                               // 638
	0,                                               // 639
	0,                                               // 640 RPL_DUMPING
	0,                                               // 641 RPL_DUMPRPL
	0,                                               // 642 RPL_EODUMP
	0,                                               // 643
	0,                                               // 644
	0,                                               // 645
	0,                                               // 646
	0,                                               // 647
	0,                                               // 648
	0,                                               // 649
	0,                                               // 650
	0,                                               // 651
	0,                                               // 652
	0,                                               // 653
	0,                                               // 654
	0,                                               // 655
	0,                                               // 656
	0,                                               // 657
	0,                                               // 658
	0,                                               // 659
	0,                                               // 660 RPL_TRACEROUTE_HOP
	0,                                               // 661 RPL_TRACEROUTE_START
	0,                                               // 662 RPL_MODECHANGEWARN
	0,                                               // 663 RPL_CHANREDIR
	0,                                               // 664 RPL_SERVMODEIS
	0,                                               // 665 RPL_OTHERUMODEIS
	0,                                               // 666 RPL_ENDOF_GENERIC
	0,                                               // 667
	0,                                               // 668
	0,                                               // 669
	PTM(parseNumericStartTls)                      , // 670 RPL_STARTTLSOK, RPL_WHOWASDETAILS
	PTM(parseNumericWhoisOther)                    , // 671 RPL_WHOISSECURE
	0,                                               // 672 RPL_UNKNOWNMODES
	0,                                               // 673 RPL_CANNOTSETMODES
	0,                                               // 674
	0,                                               // 675
	0,                                               // 676
	0,                                               // 677
	0,                                               // 678 RPL_LUSERSTAFF
	0,                                               // 679 RPL_TIMEONSERVERIS
	0,                                               // 680
	0,                                               // 681
	0,                                               // 682 RPL_NETWORKS
	0,                                               // 683
	0,                                               // 684
	0,                                               // 685
	0,                                               // 686
	0,                                               // 687 RPL_YOURLANGUAGEIS
	0,                                               // 688 RPL_LANGUAGE
	0,                                               // 689 RPL_WHOISSTAFF
	0,                                               // 690 RPL_WHOISLANGUAGE
	PTM(parseNumericStartTls)                      , // 691 RPL_STARTTLSFAIL
	0,                                               // 692
	0,                                               // 693
	0,                                               // 694
	0,                                               // 695
	0,                                               // 696
	0,                                               // 697
	0,                                               // 698
	0,                                               // 699
	PTM(parseNumericCodePageSet)                   , // 700 RPL_CODEPAGESET
	0,                                               // 701
	0,                                               // 702 RPL_MODLIST, RPL_COMMANDS
	PTM(parseNumericCodePageScheme)                , // 703 RPL_WHOISSCHEME, RPL_ENDOFMODLIST, RPL_COMMANDSEND
	PTM(parseCommandSyntaxHelp),                     // 704 RPL_COMMANDSYNTAX, RPL_HELPSTART
	PTM(parseCommandHelp),                           // 705 RPL_COMMANDHELP, RPL_HELPTXT
	PTM(parseCommandEndOfHelp),                      // 706 RPL_ENDOFCOMMANDHELP, RPL_ENDOFHELP
	0,                                               // 707
	0,                                               // 708 RPL_ETRACEFULL
	0,                                               // 709 RPL_ETRACE
	0,                                               // 710 RPL_KNOCK
	0,                                               // 711 RPL_KNOCKDLVR
	0,                                               // 712 ERR_TOOMANYKNOCK
	0,                                               // 713 ERR_CHANOPEN
	0,                                               // 714 ERR_KNOCKONCHAN
	0,                                               // 715 ERR_KNOCKDISABLED
	0,                                               // 716 RPL_TARGUMODEG
	0,                                               // 717 RPL_TARGNOTIFY
	0,                                               // 718 RPL_UMODEGMSG
	0,                                               // 719
	0,                                               // 720 RPL_OMOTDSTART
	0,                                               // 721 RPL_OMOTD
	0,                                               // 722 RPL_ENDOFOMOTD
	0,                                               // 723 ERR_NOPRIVS
	0,                                               // 724 RPL_TESTMARK
	0,                                               // 725 RPL_TESTLINE
	0,                                               // 726 RPL_NOTESTLINE
	0,                                               // 727
	0,                                               // 728
	0,                                               // 729
	0,                                               // 730
	0,                                               // 731
	0,                                               // 732
	0,                                               // 733
	0,                                               // 734
	0,                                               // 735
	0,                                               // 736
	0,                                               // 737
	0,                                               // 738
	0,                                               // 739
	0,                                               // 740
	0,                                               // 741
	0,                                               // 742
	0,                                               // 743
	0,                                               // 744
	0,                                               // 745
	0,                                               // 746
	0,                                               // 747
	0,                                               // 748
	0,                                               // 749
	0,                                               // 750
	0,                                               // 751
	0,                                               // 752
	0,                                               // 753
	0,                                               // 754
	0,                                               // 755
	0,                                               // 756
	0,                                               // 757
	0,                                               // 758
	0,                                               // 759
	0,                                               // 760
	0,                                               // 761
	0,                                               // 762
	0,                                               // 763
	0,                                               // 764
	0,                                               // 765
	0,                                               // 766
	0,                                               // 767
	0,                                               // 768
	0,                                               // 769
	0,                                               // 770
	0,                                               // 771 RPL_XINFO
	0,                                               // 772
	0,                                               // 773 RPL_XINFOSTART
	0,                                               // 774 RPL_XINFOEND
	0,                                               // 775
	0,                                               // 776
	0,                                               // 777
	0,                                               // 778
	0,                                               // 779
	0,                                               // 780
	0,                                               // 781
	0,                                               // 782
	0,                                               // 783
	0,                                               // 784
	0,                                               // 785
	0,                                               // 786
	0,                                               // 787
	0,                                               // 788
	0,                                               // 789
	0,                                               // 790
	0,                                               // 791
	0,                                               // 792
	0,                                               // 793
	0,                                               // 794
	0,                                               // 795
	0,                                               // 796
	0,                                               // 797
	0,                                               // 798
	0,                                               // 799
	0,                                               // 800
	0,                                               // 801
	0,                                               // 802
	0,                                               // 803
	0,                                               // 804
	0,                                               // 805
	0,                                               // 806
	0,                                               // 807
	0,                                               // 808
	0,                                               // 809
	0,                                               // 810
	0,                                               // 811
	0,                                               // 812
	0,                                               // 813
	0,                                               // 814
	0,                                               // 815
	0,                                               // 816
	0,                                               // 817
	0,                                               // 818
	0,                                               // 819
	0,                                               // 820
	0,                                               // 821
	0,                                               // 822
	0,                                               // 823
	0,                                               // 824
	0,                                               // 825
	0,                                               // 826
	0,                                               // 827
	0,                                               // 828
	0,                                               // 829
	0,                                               // 830
	0,                                               // 831
	0,                                               // 832
	0,                                               // 833
	0,                                               // 834
	0,                                               // 835
	0,                                               // 836
	0,                                               // 837
	0,                                               // 838
	0,                                               // 839
	0,                                               // 840
	0,                                               // 841
	0,                                               // 842
	0,                                               // 843
	0,                                               // 844
	0,                                               // 845
	0,                                               // 846
	0,                                               // 847
	0,                                               // 848
	0,                                               // 849
	0,                                               // 850
	0,                                               // 851
	0,                                               // 852
	0,                                               // 853
	0,                                               // 854
	0,                                               // 855
	0,                                               // 856
	0,                                               // 857
	0,                                               // 858
	0,                                               // 859
	0,                                               // 860
	0,                                               // 861
	0,                                               // 862
	0,                                               // 863
	0,                                               // 864
	0,                                               // 865
	0,                                               // 866
	0,                                               // 867
	0,                                               // 868
	0,                                               // 869
	0,                                               // 870
	0,                                               // 871
	0,                                               // 872
	0,                                               // 873
	0,                                               // 874
	0,                                               // 875
	0,                                               // 876
	0,                                               // 877
	0,                                               // 878
	0,                                               // 879
	0,                                               // 880
	0,                                               // 881
	0,                                               // 882
	0,                                               // 883
	0,                                               // 884
	0,                                               // 885
	0,                                               // 886
	0,                                               // 887
	0,                                               // 888
	0,                                               // 889
	0,                                               // 890
	0,                                               // 891
	0,                                               // 892
	0,                                               // 893
	0,                                               // 894
	0,                                               // 895
	0,                                               // 896
	0,                                               // 897
	0,                                               // 898
	0,                                               // 899
	PTM(parseNumericSaslLogin),                      // 900 RPL_SASLLOGIN
	0,                                               // 901
	0,                                               // 902
	PTM(parseNumericSaslSuccess),                    // 903 RPL_SASLSUCCESS
	PTM(parseNumericSaslFail),                       // 904 RPL_SASLFAILED
	PTM(parseNumericSaslFail),                       // 905 RPL_SASLERROR
	PTM(parseNumericSaslFail),                       // 906 RPL_SASLABORT
	PTM(parseNumericSaslFail),                       // 907 RPL_SASLALREADYAUTH
	0,                                               // 908
	0,                                               // 909
	0,                                               // 910
	0,                                               // 911
	0,                                               // 912
	0,                                               // 913
	0,                                               // 914
	0,                                               // 915
	0,                                               // 916
	0,                                               // 917
	0,                                               // 918
	0,                                               // 919
	0,                                               // 920
	0,                                               // 921
	0,                                               // 922
	0,                                               // 923
	0,                                               // 924
	0,                                               // 925
	0,                                               // 926
	0,                                               // 927
	0,                                               // 928
	0,                                               // 929
	0,                                               // 930
	0,                                               // 931
	0,                                               // 932
	0,                                               // 933
	0,                                               // 934
	0,                                               // 935
	0,                                               // 936 RPL_WORDFILTERED
	0,                                               // 937
	0,                                               // 938
	0,                                               // 939
	PTM(parseNumericEndOfSpamFilterList),            // 940 RPL_SPAMFILTERLIST
	PTM(parseNumericSpamFilterList),                 // 941 RPL_ENDOFSPAMFILTERLIST
	0,                                               // 942
	0,                                               // 943
	0,                                               // 944
	0,                                               // 945
	0,                                               // 946
	0,                                               // 947
	0,                                               // 948
	0,                                               // 949
	0,                                               // 950
	0,                                               // 951
	0,                                               // 952
	0,                                               // 953
	0,                                               // 954
	0,                                               // 955
	0,                                               // 956
	0,                                               // 957
	0,                                               // 958
	0,                                               // 959
	0,                                               // 960
	0,                                               // 961
	0,                                               // 962
	0,                                               // 963
	0,                                               // 964
	0,                                               // 965
	0,                                               // 966
	0,                                               // 967
	0,                                               // 968
	0,                                               // 969
	0,                                               // 970
	0,                                               // 971
	0,                                               // 972 ERR_CANNOTDOCOMMAND, ERR_CANTUNLOADMODULE
	0,                                               // 973 ERR_CANNOTCHANGEUMODE, RPL_UNLOADEDMODULE
	0,                                               // 974 ERR_CANNOTCHANGECHANMODE, ERR_CANTLOADMODULE
	0,                                               // 975 ERR_CANNOTCHANGESERVERMODE, RPL_LOADEDMODULE
	0,                                               // 976 ERR_CANNOTSENDTONICK
	0,                                               // 977 ERR_UNKNOWNSERVERMODE
	0,                                               // 978
	0,                                               // 979 ERR_SERVERMODELOCK
	0,                                               // 980 ERR_BADCHARENCODING
	0,                                               // 981 ERR_TOOMANYLANGUAGES
	0,                                               // 982 ERR_NOLANGUAGE
	0,                                               // 983 ERR_TEXTTOOSHORT
	0,                                               // 984
	0,                                               // 985
	0,                                               // 986
	0,                                               // 987
	0,                                               // 988
	0,                                               // 989
	0,                                               // 990
	0,                                               // 991
	0,                                               // 992
	0,                                               // 993
	0,                                               // 994
	0,                                               // 995
	0,                                               // 996
	0,                                               // 997
	0,                                               // 998
	0                                                // 999 ERR_NUMERIC_ERR
};
