#ifndef _KVI_KVS_EVENTTABLE_H_
#define _KVI_KVS_EVENTTABLE_H_
//=============================================================================
//
//   File : KviKvsEventTable.h
//   Creation date : Mon 23 Feb 2004 03:23:18 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2010 Szymon Stefanek <pragma at kvirc dot net>
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

/**
* \file KviKvsEventTable.h
* \author Szymon Stefanek
* \brief Events table
*/

// Application
/**
* \def KviEvent_OnKVIrcStartup Triggered at KVIrc startup
* \def KviEvent_OnKVIrcShutdown Triggered at KVIrc shutdown
* \def KviEvent_OnIrcContextCreated Triggered when the IRC context is created
* \def KviEvent_OnIrcContextDestroyed Triggered when the IRC context is destroyed
* \def KviEvent_OnFrameWindowCreated Triggered when the frame window is created
* \def KviEvent_OnFrameWindowDestroyed Triggered when the frame window is destroyed
* \def KviEvent_OnURL An URL appears in one of the output windows
* \def KviEvent_OnError Triggered when an ERROR message is received from the server
* \def KviEvent_OnAccelKeyPressed An accelerator key was pressed
* \def KviEvent_OnHighlight A channel or query message is going to be highlighted
* \def KviEvent_OnWindowActivated A window has just been activated
* \def KviEvent_OnNotifyOnLine A user in the notify list is on-line
* \def KviEvent_OnNotifyOffLine A user in the notify list went off-line
* \def KviEvent_OnPing Server ping handler
* \def KviEvent_OnPong Server pong handler
* \def KviEvent_OnLagAlarmTimeUp High-Lag Alarm Started
* \def KviEvent_OnLagAlarmTimeDown High-Lag Alarm Stopped
* \def KviEvent_OnLagCheck The lag metering engine has checked the lag
* \def KviEvent_OnTextInput The user has just entered some text in the text input
* \def KviEvent_OnNotifyListDefaultActionRequest Notify list double click customization
* \def KviEvent_OnWallops A WALLOPS message is received from the server
* \def KviEvent_OnIgnoredMessage Triggered when a message is ignored
* \def KviEvent_OnServerNotice A server NOTICE have been issued
* \def KviEvent_OnWindowTitleRequest A window title is needed
* \def KviEvent_OnInputBarPaste User pasted some text on the inputbar
*/
#define KviEvent_OnKVIrcStartup 0
#define KviEvent_OnKVIrcShutdown 1
#define KviEvent_OnIrcContextCreated 2
#define KviEvent_OnIrcContextDestroyed 3
#define KviEvent_OnFrameWindowCreated 4
#define KviEvent_OnFrameWindowDestroyed 5
#define KviEvent_OnURL 6
#define KviEvent_OnError 7
#define KviEvent_OnAccelKeyPressed 8
#define KviEvent_OnHighlight 9
#define KviEvent_OnWindowActivated 10
#define KviEvent_OnNotifyOnLine 11
#define KviEvent_OnNotifyOffLine 12
#define KviEvent_OnPing 13
#define KviEvent_OnPong 14
#define KviEvent_OnLagAlarmTimeUp 15
#define KviEvent_OnLagAlarmTimeDown 16
#define KviEvent_OnLagCheck 17
#define KviEvent_OnTextInput 18
#define KviEvent_OnNotifyListDefaultActionRequest 19
#define KviEvent_OnWallops 20
#define KviEvent_OnIgnoredMessage 21
#define KviEvent_OnServerNotice 22
#define KviEvent_OnWindowTitleRequest 145
#define KviEvent_OnInputBarPaste 146
#define KviEvent_OnBroadcastNotice 148
#define KviEvent_OnCap 157

// Connection
/**
* \def KviEvent_OnIrcConnectionEstablished Triggered when the IRC connection is established
* \def KviEvent_OnIrcConnectionTerminated Triggered when the IRC connection is terminated
* \def KviEvent_OnIrc Triggered when the login is completed
* \def KviEvent_OnNetsplit A netsplit has been detected
* \def KviEvent_OnDisconnectRequest Triggered when the user requests a disconnection
* \def KviEvent_OnHTTPGetTerminated A http.get command has terminated execution
* \def KviEvent_OnUnhandledLiteral An unhandled literal server message has been received
* \def KviEvent_OnOutboundTraffic Triggered when text is sent to the server
*/
#define KviEvent_OnIRCConnectionEstablished 23
#define KviEvent_OnIRCConnectionTerminated 24
#define KviEvent_OnIRC 25
#define KviEvent_OnNetsplit 26
#define KviEvent_OnDisconnectRequest 27
#define KviEvent_OnHTTPGetTerminated 28
#define KviEvent_OnUnhandledLiteral 29
#define KviEvent_OnOutboundTraffic 30
#define KviEvent_OnMeHostChange 153

// Popups
/**
* \def KviEvent_OnChannelPopupRequest Triggered when the channel text view is right-clicked
* \def KviEvent_OnConsolePopupRequest Triggered when the console text view is right-clicked
* \def KviEvent_OnQueryPopupRequest Triggered when the query text view is right-clicked
* \def KviEvent_OnWindowPopupRequest Triggered when the user middle-clicks in a text view (or right-clicks with the Ctrl key pressed)
* \def KviEvent_OnHostLinkPopupRequest Triggered when a user right clicks on a hostname link in the text view
* \def KviEvent_OnURLLinkPopupRequest URL link popup customization
* \def KviEvent_OnServerLinkPopupRequest Server link popup customization
* \def KviEvent_OnChannelLinkPopupRequest Channel link popup customization
* \def KviEvent_OnNickLinkPopupRequest Nick link popup customization
* \def KviEvent_OnQueryNickPopupRequest Query nicklist popup customization
* \def KviEvent_OnChannelNickPopupRequest Allows customization of the nick-on-channel context popup
* \def KviEvent_OnNotifyListPopupRequest Notify list context popup customization
* \def KviEvent_OnDCCChatPopupRequest Triggered when the DCC chat text view is right-clicked
*/
#define KviEvent_OnChannelPopupRequest 31
#define KviEvent_OnConsolePopupRequest 32
#define KviEvent_OnQueryPopupRequest 33
#define KviEvent_OnWindowPopupRequest 34
#define KviEvent_OnHostLinkPopupRequest 35
#define KviEvent_OnURLLinkPopupRequest 36
#define KviEvent_OnServerLinkPopupRequest 37
#define KviEvent_OnChannelLinkPopupRequest 38
#define KviEvent_OnNickLinkPopupRequest 39
#define KviEvent_OnQueryNickPopupRequest 40
#define KviEvent_OnChannelNickPopupRequest 41
#define KviEvent_OnNotifyListPopupRequest 42
#define KviEvent_OnDCCChatPopupRequest 43

// Channels
/**
* \def KviEvent_OnChannelWindowCreated A channel window has just been opened
* \def KviEvent_OnChannelSync Triggered when all the channel information have been received
* \def KviEvent_OnChannelMessage A message has been received from the channel
* \def KviEvent_OnChannelNotice A NOTICE have been issued to a channel
*/
#define KviEvent_OnChannelWindowCreated 44
#define KviEvent_OnChannelSync 45
#define KviEvent_OnChannelMessage 46
#define KviEvent_OnChannelNotice 47

// Queries
/**
* \def KviEvent_OnQueryMessage A message has been received from a query
* \def KviEvent_OnQueryNotice A private notice has been received
* \def KviEvent_OnQueryWindowRequest A new query window is going to be opened because a remote message has been received
* \def KviEvent_OnQueryWindowCreated A query window has just been opened
* \def KviEvent_OnQueryTargetAdded A new query target has been added
* \def KviEvent_OnQueryFileDropped A file is dropped in a query
*/
#define KviEvent_OnQueryMessage 48
#define KviEvent_OnQueryNotice 49
#define KviEvent_OnQueryWindowRequest 50
#define KviEvent_OnQueryWindowCreated 51
#define KviEvent_OnQueryTargetAdded 52
#define KviEvent_OnQueryFileDropped 53

// Actions
/**
* \def KviEvent_OnMeAway Local user sets the AWAY status
* \def KviEvent_OnMeBack Local user cancels the AWAY status
* \def KviEvent_OnJoin Non-local user joining a channel
* \def KviEvent_OnMeJoin Local user joining a channel
* \def KviEvent_OnPart Non-local user is leaving a channel
* \def KviEvent_OnMePart Local user is leaving a channel
* \def KviEvent_OnKick Non-local user is being kicked from a channel
* \def KviEvent_OnMeKick Local user is being kicked from a channel
* \def KviEvent_OnTopic Topic change on a channel
* \def KviEvent_OnQuit An user is leaving IRC
*/
#define KviEvent_OnMeAway 54
#define KviEvent_OnMeBack 55
#define KviEvent_OnJoin 56
#define KviEvent_OnMeJoin 57
#define KviEvent_OnPart 58
#define KviEvent_OnMePart 59
#define KviEvent_OnKick 60
#define KviEvent_OnMeKick 61
#define KviEvent_OnTopic 62
#define KviEvent_OnQuit 63
#define KviEvent_OnHostChange 154
#define KviEvent_OnAccount 155
#define KviEvent_OnAway 156

// IRC modes
/**
* \def KviEvent_OnChannelModeChange The channel mode has been changed
* \def KviEvent_OnUserModeChange The local user has changed mode
* \def KviEvent_OnBan Someone has set a +b flag on the channel
* \def KviEvent_OnUnban Someone has set a -b flag on the channel
* \def KviEvent_OnMeBan Local user is being banned in a channel
* \def KviEvent_OnMeUnban Local user is being de-banned in a channel
* \def KviEvent_OnBanException Someone has set a +e flag on the channel
* \def KviEvent_OnBanExceptionRemove Someone has set a -e flag on the channel
* \def KviEvent_OnMeBanException Someone has set a +e flag on the local user
* \def KviEvent_OnMeBanExceptionRemove Someone has set a -e flag on the local user
* \def KviEvent_OnInvite Triggered when a user invites someone in the channel
* \def KviEvent_OnInviteException A +I mode has been set
* \def KviEvent_OnInviteExceptionRemove Mode -I has been set
* \def KviEvent_OnMeInviteException A +I mode has been set on the local user
* \def KviEvent_OnMeInviteExceptionRemove A -I mode has been set on the local user
* \def KviEvent_OnLimitSet The channel users limit has been set
* \def KviEvent_OnLimitUnset The channel users limit has been unset
* \def KviEvent_OnKeySet The channel users key has been set
* \def KviEvent_OnKeyUnset The channel users key has been unset
* \def KviEvent_OnNickChange Someone has changed his nickname
* \def KviEvent_OnMeNickChange The local user has changed his nickname
* \def KviEvent_OnChanOwner Someone has given a user the +q flag
* \def KviEvent_OnDeChanOwner Someone has given a user the -q flag
* \def KviEvent_OnMeChanOwner Someone has given the +q flag to the local user
* \def KviEvent_OnMeDeChanOwner Someone has given the -q flag to the local user
* \def KviEvent_OnChanAdmin Someone has given a user the +a flag
* \def KviEvent_OnDeChanAdmin Someone has given a user the -a flag
* \def KviEvent_OnMeChanAdmin Someone has given the +a flag to the local user
* \def KviEvent_OnMeDeChanAdmin Someone has given the -a flag to the local user
* \def KviEvent_OnOp Someone has given a user the +o flag
* \def KviEvent_OnDeOp Someone has given a user the -o flag
* \def KviEvent_OnMeOp Someone has given a user the +o flag to the local user
* \def KviEvent_OnMeDeOp Someone has given a user the -o flag to the local user
* \def KviEvent_OnHalfOp Someone has given a user the +h flag
* \def KviEvent_OnDeHalfOp Someone has given a user the -h flag
* \def KviEvent_OnMeHalfOp Someone has given the local user the +h flag
* \def KviEvent_OnMeDeHalfOp Someone has given the local user the -h flag
* \def KviEvent_OnVoice Someone has given a user the +v flag
* \def KviEvent_OnDeVoice Someone has given a user the -v flag
* \def KviEvent_OnMeVoice Someone has given the local user the +v flag
* \def KviEvent_OnMeDeVoice Someone has given the local user the -v flag
* \def KviEvent_OnUserOp Someone has given a user the +u flag
* \def KviEvent_OnDeUserOp Someone has given a user the -u flag
* \def KviEvent_OnMeUserOp Someone has given the local user the +u flag
* \def KviEvent_OnMeDeUserOp Someone has given the local user the -u flag
* \def KviEvent_OnIrcOp Someone has given a user the +O flag
* \def KviEvent_OnDeIrcOp Someone has given a user the -O flag
* \def KviEvent_OnMeIrcOp Someone has given the local user the +O flag
* \def KviEvent_OnMeDeIrcOp Someone has given the local user the -O flag
* \def KviEvent_OnQuietBan Someone has set a +q flag on the channel
* \def KviEvent_OnQuietUnban Someone has set a -q flag on the channel
* \def KviEvent_OnMeQuietBan Local user is being quiet-banned in a channel
* \def KviEvent_OnMeQuietUnban Local user is being quiet-debanned in a channel
*/
#define KviEvent_OnChannelModeChange 64
#define KviEvent_OnUserModeChange 65
#define KviEvent_OnBan 66
#define KviEvent_OnUnban 67
#define KviEvent_OnMeBan 68
#define KviEvent_OnMeUnban 69
#define KviEvent_OnBanException 70
#define KviEvent_OnBanExceptionRemove 71
#define KviEvent_OnMeBanException 72
#define KviEvent_OnMeBanExceptionRemove 73
#define KviEvent_OnInvite 74
#define KviEvent_OnInviteException 75
#define KviEvent_OnInviteExceptionRemove 76
#define KviEvent_OnMeInviteException 77
#define KviEvent_OnMeInviteExceptionRemove 78
#define KviEvent_OnQuietBan 149
#define KviEvent_OnQuietUnban 150
#define KviEvent_OnMeQuietBan 151
#define KviEvent_OnMeQuietUnban 152
#define KviEvent_OnLimitSet 79
#define KviEvent_OnLimitUnset 80
#define KviEvent_OnKeySet 81
#define KviEvent_OnKeyUnset 82
#define KviEvent_OnNickChange 83
#define KviEvent_OnMeNickChange 84
#define KviEvent_OnChanOwner 85
#define KviEvent_OnDeChanOwner 86
#define KviEvent_OnMeChanOwner 87
#define KviEvent_OnMeDeChanOwner 88
#define KviEvent_OnChanAdmin 89
#define KviEvent_OnDeChanAdmin 90
#define KviEvent_OnMeChanAdmin 91
#define KviEvent_OnMeDeChanAdmin 92
#define KviEvent_OnOp 93
#define KviEvent_OnDeOp 94
#define KviEvent_OnMeOp 95
#define KviEvent_OnMeDeOp 96
#define KviEvent_OnHalfOp 97
#define KviEvent_OnDeHalfOp 98
#define KviEvent_OnMeHalfOp 99
#define KviEvent_OnMeDeHalfOp 100
#define KviEvent_OnVoice 101
#define KviEvent_OnDeVoice 102
#define KviEvent_OnMeVoice 103
#define KviEvent_OnMeDeVoice 104
#define KviEvent_OnUserOp 105
#define KviEvent_OnDeUserOp 106
#define KviEvent_OnMeUserOp 107
#define KviEvent_OnMeDeUserOp 108
#define KviEvent_OnIrcOp 109
#define KviEvent_OnDeIrcOp 110
#define KviEvent_OnMeIrcOp 111
#define KviEvent_OnMeDeIrcOp 112

// Services
/**
* \def KviEvent_OnChanServNotice A notice from ChanServ has been received
* \def KviEvent_OnNickServNotice A notice from NickServ has been received
* \def KviEvent_OnNickServAuth Triggered when IRCd sets a registered user mode.
* \def KviEvent_OnMemoServNotice A notice from MemoServ has been received
*/
#define KviEvent_OnChanServNotice 113
#define KviEvent_OnNickServNotice 114
#define KviEvent_OnNickServAuth 115
#define KviEvent_OnMemoServNotice 147

// CTCPs
/**
* \def KviEvent_OnAction Someone issued a CTCP ACTION
* \def KviEvent_OnMeAction User issued a CTCP ACTION
* \def KviEvent_OnCTCPRequest A CTCP request has been received
* \def KviEvent_OnCTCPReply A CTCP reply has been received
* \def KviEvent_OnCTCPFlood Someone tried to flood you with CTCP requests
* \def KviEvent_OnDCCSessionCreated A new DCC session has just been created
* \def KviEvent_OnDCCSessionDestroyed A new DCC session has just been destroyed
* \def KviEvent_OnDCCChatWindowCreated A DCC Chat window has been just created
* \def KviEvent_OnDCCChatConnectionInProgress A DCC Chat connection has just been started
* \def KviEvent_OnDCCChatConnected A DCC Chat connection has been established
* \def KviEvent_OnDCCChatMessage A DCC Chat message has just been received
* \def KviEvent_OnDCCChatError A DCC Chat error has just been triggered
* \def KviEvent_OnDCCChatDisconnected A DCC Chat has been just disconnected
* \def KviEvent_OnDCCChatWindowClosing A DCC Chat is going to be closed
* \def KviEvent_OnDCCFileTransferConnectionInProgress Triggered when a DCC File transfer is waiting for a connection
* \def KviEvent_OnDCCFileTransferBegin Triggered when a DCC File transfer begins
* \def KviEvent_OnDCCFileTransferSuccess Triggered when a DCC File transfer succeeds
* \def KviEvent_OnDCCFileTransferFailed Triggered when a DCC File transfer fails
*/
#define KviEvent_OnAction 116
#define KviEvent_OnMeAction 117
#define KviEvent_OnCTCPRequest 118
#define KviEvent_OnCTCPReply 119
#define KviEvent_OnCTCPFlood 120
#define KviEvent_OnDCCSessionCreated 121
#define KviEvent_OnDCCSessionDestroyed 122
#define KviEvent_OnDCCChatWindowCreated 123
#define KviEvent_OnDCCChatConnectionInProgress 124
#define KviEvent_OnDCCChatConnected 125
#define KviEvent_OnDCCChatMessage 126
#define KviEvent_OnDCCChatError 127
#define KviEvent_OnDCCChatDisconnected 128
#define KviEvent_OnDCCChatWindowClosing 129
#define KviEvent_OnDCCFileTransferConnectionInProgress 130
#define KviEvent_OnDCCFileTransferBegin 131
#define KviEvent_OnDCCFileTransferSuccess 132
#define KviEvent_OnDCCFileTransferFailed 133

// Link clicks
/**
* \def KviEvent_OnChannelNickLinkClick User clicked on nick in channel
* \def KviEvent_OnQueryNickLinkClick User clicked on nickname in query window
* \def KviEvent_OnConsoleNickLinkClick User clicked on nickname in console window
* \def KviEvent_OnHostLinkClick Triggered when user clicks on any hostname in any window
* \def KviEvent_OnURLLinkClick Triggered when user clicks on any URL link in any window
* \def KviEvent_OnChannelLinkClick Triggered when user clicks on any channel name in any window
* \def KviEvent_OnServerLinkClick Triggered when user clicks on server name in any window
* \def KviEvent_OnTextViewDoubleClicked Text view link double click customization
* \def KviEvent_OnNickLinkDefaultActionRequest Nick link double click customization
* \def KviEvent_OnChannelNickDefaultActionRequest Nicknames double click customization
* \def KviEvent_OnQueryNickDefaultActionRequest Query double click customization
*/
#define KviEvent_OnChannelNickLinkClick 134
#define KviEvent_OnQueryNickLinkClick 135
#define KviEvent_OnConsoleNickLinkClick 136
#define KviEvent_OnHostLinkClick 137
#define KviEvent_OnURLLinkClick 138
#define KviEvent_OnChannelLinkClick 139
#define KviEvent_OnServerLinkClick 140
#define KviEvent_OnTextViewDoubleClicked 141
#define KviEvent_OnNickLinkDefaultActionRequest 142
#define KviEvent_OnChannelNickDefaultActionRequest 143
#define KviEvent_OnQueryNickDefaultActionRequest 144

/** \def KVI_KVS_NUM_APP_EVENTS Defines the number of events */
#define KVI_KVS_NUM_APP_EVENTS 158

#endif //_KVI_KVS_EVENTTABLE_H_
