#ifndef _KVI_KVS_EVENTTABLE_H_
#define _KVI_KVS_EVENTTABLE_H_
//=============================================================================
//
//   File : kvi_kvs_eventtable.h
//   Created on Mon 23 Feb 2004 03:23:18 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004 Szymon Stefanek <pragma at kvirc dot net>
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

// Application
#define KviEvent_OnKVIrcStartup                  0
#define KviEvent_OnKVIrcShutdown                 1
// Irc Context
#define KviEvent_OnIrcContextCreated             2
#define KviEvent_OnIrcContextDestroyed           3
// Irc connection
#define KviEvent_OnIrcConnectionEstabilished     4
#define KviEvent_OnIrcConnectionTerminated       5
// Login complete
#define KviEvent_OnIrc                           6
// Channel userlist clicks
#define KviEvent_OnChannelNickPopupRequest          7
#define KviEvent_OnChannelNickDefaultActionRequest  8
// Frame window
#define KviEvent_OnFrameWindowCreated               9
#define KviEvent_OnFrameWindowDestroyed             10
// Text view
#define KviEvent_OnChannelPopupRequest              11
#define KviEvent_OnConsolePopupRequest              12
#define KviEvent_OnQueryPopupRequest                13

#define KviEvent_OnChannelSync                   14

#define KviEvent_OnUrl                           15

#define KviEvent_OnWindowPopupRequest            16

#define KviEvent_OnHostLinkPopupRequest          17
#define KviEvent_OnUrlLinkPopupRequest           18
#define KviEvent_OnServerLinkPopupRequest        19
#define KviEvent_OnChannelLinkPopupRequest       20
#define KviEvent_OnNickLinkPopupRequest          21
// Query nick clicks
#define KviEvent_OnQueryNickPopupRequest         22
#define KviEvent_OnQueryNickDefaultActionRequest 23

#define KviEvent_OnNickLinkDefaultActionRequest  24
#define KviEvent_OnTextViewDoubleClicked            25

#define KviEvent_OnNotifyListDefaultActionRequest 26
#define KviEvent_OnNotifyListPopupRequest 27

#define KviEvent_OnPing 28

#define KviEvent_OnMeJoin 29
#define KviEvent_OnJoin 30

#define KviEvent_OnTopic 31

#define KviEvent_OnMePart 32
#define KviEvent_OnPart 33

#define KviEvent_OnQuit 34

#define KviEvent_OnUserMode 35

#define KviEvent_OnMeKick 36
#define KviEvent_OnKick 37

#define KviEvent_OnMeAway 38
#define KviEvent_OnMeBack 39

#define KviEvent_OnChannelWindowCreated 40
#define KviEvent_OnQueryWindowCreated 41
#define KviEvent_OnBan 42
#define KviEvent_OnUnban 43
#define KviEvent_OnMeBan 44
#define KviEvent_OnMeUnban 45
#define KviEvent_OnBanException 46
#define KviEvent_OnBanExceptionRemove 47
#define KviEvent_OnMeBanException 48
#define KviEvent_OnMeBanExceptionRemove 49
#define KviEvent_OnOp 50
#define KviEvent_OnDeOp 51
#define KviEvent_OnMeOp 52
#define KviEvent_OnMeDeOp 53
#define KviEvent_OnLimitSet 54
#define KviEvent_OnLimitUnset 55
#define KviEvent_OnKeySet 56
#define KviEvent_OnKeyUnset 57
#define KviEvent_OnInvite 58
#define KviEvent_OnChannelMessage 59
#define KviEvent_OnQueryMessage 60
#define KviEvent_OnVoice 61
#define KviEvent_OnDeVoice 62
#define KviEvent_OnMeVoice 63
#define KviEvent_OnMeDeVoice 64
#define KviEvent_OnNickChange 65
#define KviEvent_OnMeNickChange 66
#define KviEvent_OnCtcpRequest 67
#define KviEvent_OnCtcpReply 68
#define KviEvent_OnUnhandledLiteral 69
#define KviEvent_OnMeHalfOp 70
#define KviEvent_OnMeDeHalfOp 71
#define KviEvent_OnHalfOp 72
#define KviEvent_OnDeHalfOp 73
#define KviEvent_OnInviteException 74
#define KviEvent_OnInviteExceptionRemove 75
#define KviEvent_OnMeInviteException 76
#define KviEvent_OnMeInviteExceptionRemove 77
#define KviEvent_OnAction 78
#define KviEvent_OnCtcpFlood 79
#define KviEvent_OnChannelNotice 80
#define KviEvent_OnServerNotice 81
#define KviEvent_OnDCCChatWindowCreated 82
#define KviEvent_OnDCCChatConnectionInProgress 83
#define KviEvent_OnDCCChatConnected 84
#define KviEvent_OnDCCChatMessage 85
#define KviEvent_OnDCCChatError 86
#define KviEvent_OnDCCChatDisconnected 87
#define KviEvent_OnDCCChatWindowClosing 88
#define KviEvent_OnAccelKeyPressed 89
#define KviEvent_OnHighlight 90
#define KviEvent_OnWindowActivated 91
#define KviEvent_OnNotifyOnLine 92
#define KviEvent_OnNotifyOffLine 93
#define KviEvent_OnPong 94
#define KviEvent_OnLagAlarmTimeUp 95
#define KviEvent_OnLagAlarmTimeDown 96
#define KviEvent_OnLagCheck 97

#define KviEvent_OnQueryNotice 98
#define KviEvent_OnChanServNotice 99
#define KviEvent_OnNickServNotice 100

#define KviEvent_OnChanAdmin 101
#define KviEvent_OnDeChanAdmin 102
#define KviEvent_OnMeChanAdmin 103
#define KviEvent_OnMeDeChanAdmin 104

#define KviEvent_OnMeUserOp 105
#define KviEvent_OnMeDeUserOp 106
#define KviEvent_OnUserOp 107
#define KviEvent_OnDeUserOp 108

#define KviEvent_OnQueryWindowRequest 109
#define KviEvent_OnQueryTargetAdded 110

#define KviEvent_OnNetsplit 111

#define KviEvent_OnDCCSessionCreated 112
#define KviEvent_OnDCCSessionDestroyed 113

#define KviEvent_OnTextInput 114

#define KviEvent_OnDCCChatPopupRequest 115

#define KviEvent_OnDCCFileTransferConnectionInProgress 116
#define KviEvent_OnDCCFileTransferBegin 117
#define KviEvent_OnDCCFileTransferSuccess 118
#define KviEvent_OnDCCFileTransferFailed 119

#define KviEvent_OnWallops 120

#define KviEvent_OnDisconnectRequest 121

#define KviEvent_OnChanOwner 122
#define KviEvent_OnDeChanOwner 123
#define KviEvent_OnMeChanOwner 124
#define KviEvent_OnMeDeChanOwner 125
#define KviEvent_OnError 126
#define KviEvent_OnQueryFileDropped 127

#define KviEvent_OnHTTPGetTerminated 128

#define KviEvent_OnChannelModeChange 129

#define KviEvent_OnChannelNickLinkClick 130
#define KviEvent_OnQueryNickLinkClick 131
#define KviEvent_OnConsoleNickLinkClick 132
#define KviEvent_OnHostLinkClick 133
#define KviEvent_OnUrlLinkClick 134
#define KviEvent_OnChannelLinkClick 135
#define KviEvent_OnServerLinkClick 136
#define KviEvent_OnIgnoredMessage 137

#define KviEvent_OnNickServAuth 138

#define KviEvent_OnMeAction 139

#define KVI_KVS_NUM_APP_EVENTS 140
// compat
#define KVI_NUM_SCRIPT_EVENTS KVI_KVS_NUM_APP_EVENTS

#endif //!_KVI_KVS_EVENTTABLE_H_
