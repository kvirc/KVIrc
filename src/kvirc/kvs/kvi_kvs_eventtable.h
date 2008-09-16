#ifndef _KVI_KVS_EVENTTABLE_H_
#define _KVI_KVS_EVENTTABLE_H_
//=============================================================================
//
//   File : kvi_kvs_eventtable.h
//   Created on Mon 23 Feb 2004 03:23:18 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2008 Szymon Stefanek <pragma at kvirc dot net>
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

/**
* \file kvi_kvs_eventtable.h
* \author Szymon Stefanek
* \brief Events table
*/

// Application
/**
* \def KviEvent_OnKVIrcStartup Triggered at KVIrc startup
* \def KviEvent_OnKVIrcShutdown Triggered at KVIrc shutdown
* \def KviEvent_OnIrcContextCreated Triggered when the IRC context is created
* \def KviEvent_OnIrcContextDestroyed Triggered when the IRC context is destroyed



*/
#define KviEvent_OnKVIrcStartup                    0
#define KviEvent_OnKVIrcShutdown                   1
#define KviEvent_OnIrcContextCreated               2
#define KviEvent_OnIrcContextDestroyed             3
#define KviEvent_OnFrameWindowCreated              4
#define KviEvent_OnFrameWindowDestroyed            5
#define KviEvent_OnURL                             6
#define KviEvent_OnError                           7
#define KviEvent_OnAccelKeyPressed                 8
#define KviEvent_OnHighlight                       9
#define KviEvent_OnWindowActivated                10
#define KviEvent_OnNotifyOnLine                   11
#define KviEvent_OnNotifyOffLine                  12
#define KviEvent_OnPing                           13
#define KviEvent_OnPong                           14
#define KviEvent_OnLagAlarmTimeUp                 15
#define KviEvent_OnLagAlarmTimeDown               16
#define KviEvent_OnLagCheck                       17
#define KviEvent_OnTextInput                      18
#define KviEvent_OnNotifyListDefaultActionRequest 19
#define KviEvent_OnNotifyListPopupRequest         20
#define KviEvent_OnWallops                        21
#define KviEvent_OnIgnoredMessage                 22
#define KviEvent_OnServerNotice                   23
#define KviEvent_OnTextViewDoubleClicked          24
#define KviEvent_OnNickLinkDefaultActionRequest   25

// Connection
/**
* \def KviEvent_OnIrcConnectionEstabilished Triggered when the IRC connection is established
* \def KviEvent_OnIrcConnectionTerminated Triggered when the IRC connection is terminated
* \def KviEvent_OnIrc Triggered when the login is completed

* \def KviEvent_OnOutboundTraffic Triggered when text is sent to the server
*/
#define KviEvent_OnIRCConnectionEstabilished 26
#define KviEvent_OnIRCConnectionTerminated   27
#define KviEvent_OnIRC                       28
#define KviEvent_OnNetsplit                  29
#define KviEvent_OnDisconnectRequest         30
#define KviEvent_OnHTTPGetTerminated         31
#define KviEvent_OnUnhandledLiteral          32
#define KviEvent_OnOutboundTraffic           33

// Popups
#define KviEvent_OnChannelPopupRequest     34
#define KviEvent_OnConsolePopupRequest     35
#define KviEvent_OnQueryPopupRequest       36
#define KviEvent_OnWindowPopupRequest      37
#define KviEvent_OnHostLinkPopupRequest    38
#define KviEvent_OnURLLinkPopupRequest     39
#define KviEvent_OnServerLinkPopupRequest  40
#define KviEvent_OnChannelLinkPopupRequest 41
#define KviEvent_OnNickLinkPopupRequest    42
#define KviEvent_OnQueryNickPopupRequest   43
#define KviEvent_OnChannelNickPopupRequest 44

// Channels
#define KviEvent_OnChannelNickDefaultActionRequest 45
#define KviEvent_OnChannelWindowCreated            46
#define KviEvent_OnChannelSync                     47
#define KviEvent_OnChannelMessage                  48
#define KviEvent_OnChannelNotice                   49

// Queries
#define KviEvent_OnQueryMessage                  50
#define KviEvent_OnQueryNotice                   51
#define KviEvent_OnQueryWindowRequest            52
#define KviEvent_OnQueryWindowCreated            53
#define KviEvent_OnQueryTargetAdded              54
#define KviEvent_OnQueryFileDropped              55
#define KviEvent_OnQueryNickDefaultActionRequest 56

// Actions
/** \def KviEvent_OnMeAction Triggered when we perform an action */
#define KviEvent_OnMeAway    57
#define KviEvent_OnMeBack    58
#define KviEvent_OnAction    59
#define KviEvent_OnMeAction  60
#define KviEvent_OnJoin      61
#define KviEvent_OnMeJoin    62
#define KviEvent_OnPart      63
#define KviEvent_OnMePart    64
#define KviEvent_OnKick      65
#define KviEvent_OnMeKick    66
#define KviEvent_OnTopic     67
#define KviEvent_OnQuit      68

// IRC modes
#define KviEvent_OnChannelModeChange        69
#define KviEvent_OnUserMode                 70
#define KviEvent_OnBan                      71
#define KviEvent_OnUnban                    72
#define KviEvent_OnMeBan                    73
#define KviEvent_OnMeUnban                  74
#define KviEvent_OnBanException             75
#define KviEvent_OnBanExceptionRemove       76
#define KviEvent_OnMeBanException           77
#define KviEvent_OnMeBanExceptionRemove     78
#define KviEvent_OnInviteException          79
#define KviEvent_OnInviteExceptionRemove    80
#define KviEvent_OnMeInviteException        81
#define KviEvent_OnMeInviteExceptionRemove  82
#define KviEvent_OnLimitSet                 83
#define KviEvent_OnLimitUnset               84
#define KviEvent_OnKeySet                   85
#define KviEvent_OnKeyUnset                 86
#define KviEvent_OnNickChange               87
#define KviEvent_OnMeNickChange             88
#define KviEvent_OnInvite                   89
#define KviEvent_OnChanOwner                90
#define KviEvent_OnDeChanOwner              91
#define KviEvent_OnMeChanOwner              92
#define KviEvent_OnMeDeChanOwner            93
#define KviEvent_OnChanAdmin                94
#define KviEvent_OnDeChanAdmin              95
#define KviEvent_OnMeChanAdmin              96
#define KviEvent_OnMeDeChanAdmin            97
#define KviEvent_OnOp                       98
#define KviEvent_OnDeOp                     99
#define KviEvent_OnMeOp                    100
#define KviEvent_OnMeDeOp                  101
#define KviEvent_OnHalfOp                  102
#define KviEvent_OnDeHalfOp                103
#define KviEvent_OnMeHalfOp                104
#define KviEvent_OnMeDeHalfOp              105
#define KviEvent_OnVoice                   106
#define KviEvent_OnDeVoice                 107
#define KviEvent_OnMeVoice                 108
#define KviEvent_OnMeDeVoice               109
#define KviEvent_OnUserOp                  110
#define KviEvent_OnDeUserOp                111
#define KviEvent_OnMeUserOp                112
#define KviEvent_OnMeDeUserOp              113

// Services
#define KviEvent_OnChanServNotice 114
#define KviEvent_OnNickServNotice 115
#define KviEvent_OnNickServAuth   116

// CTCPs
#define KviEvent_OnCTCPRequest                         117
#define KviEvent_OnCTCPReply                           118
#define KviEvent_OnCTCPFlood                           119
#define KviEvent_OnDCCSessionCreated                   120
#define KviEvent_OnDCCSessionDestroyed                 121
#define KviEvent_OnDCCChatPopupRequest                 122
#define KviEvent_OnDCCChatWindowCreated                123
#define KviEvent_OnDCCChatConnectionInProgress         124
#define KviEvent_OnDCCChatConnected                    125
#define KviEvent_OnDCCChatMessage                      126
#define KviEvent_OnDCCChatError                        127
#define KviEvent_OnDCCChatDisconnected                 128
#define KviEvent_OnDCCChatWindowClosing                129
#define KviEvent_OnDCCFileTransferConnectionInProgress 130
#define KviEvent_OnDCCFileTransferBegin                131
#define KviEvent_OnDCCFileTransferSuccess              132
#define KviEvent_OnDCCFileTransferFailed               133

// Link clicks
#define KviEvent_OnChannelNickLinkClick 134
#define KviEvent_OnQueryNickLinkClick   135
#define KviEvent_OnConsoleNickLinkClick 136
#define KviEvent_OnHostLinkClick        137
#define KviEvent_OnURLLinkClick         138
#define KviEvent_OnChannelLinkClick     139
#define KviEvent_OnServerLinkClick      140

/** \def KVI_KVS_NUM_APP_EVENTS Defines the number of events */
#define KVI_KVS_NUM_APP_EVENTS 141

#endif //_KVI_KVS_EVENTTABLE_H_
