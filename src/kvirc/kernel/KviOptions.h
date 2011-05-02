#ifndef _KVI_OPTIONS_H_
#define _KVI_OPTIONS_H_

//=============================================================================
//
//   File : KviOptions.h
//   Creation date : Tue Jun 20 2000 12:42:07 by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"
#include "KviCString.h"
#include "KviPixmap.h"
#include "KviMessageTypeSettings.h"

#include <QRect>
#include <QPixmap>
#include <QColor>
#include <QFont>
#include <QStringList>

// option structures


#define DECLARE_OPTION_STRUCT(_cname,_type) \
	class _cname \
	{ \
	public: \
		QString      name; \
		_type        option; \
		int          flags; \
	public: \
		_cname(const QString &n,_type o,int f) \
		: name(n), option(o), flags(f) {}; \
		~_cname() {}; \
	};

DECLARE_OPTION_STRUCT(KviBoolOption,bool)
DECLARE_OPTION_STRUCT(KviRectOption,QRect)
DECLARE_OPTION_STRUCT(KviStringOption,QString)
DECLARE_OPTION_STRUCT(KviColorOption,QColor)
DECLARE_OPTION_STRUCT(KviFontOption,QFont)
DECLARE_OPTION_STRUCT(KviPixmapOption,KviPixmap)
//DECLARE_OPTION_STRUCT(KviIntOption,int)
DECLARE_OPTION_STRUCT(KviUIntOption,unsigned int)
DECLARE_OPTION_STRUCT(KviMessageTypeSettingsOption,KviMessageTypeSettings)
DECLARE_OPTION_STRUCT(KviStringListOption,QStringList)

#define KVI_COLOR_EXT_USER_OP 		50
#define KVI_COLOR_EXT_USER_HALFOP	51
#define KVI_COLOR_EXT_USER_ADMIN	52
#define KVI_COLOR_EXT_USER_OWNER	53
#define KVI_COLOR_EXT_USER_VOICE	54
#define KVI_COLOR_EXT_USER_USEROP	55
#define KVI_COLOR_EXT_USER_NORMAL	56
#define KVI_COLOR_CUSTOM		255
#define KVI_COLOR_OWN			254

// option entry indexes

#define KVI_RECT_OPTIONS_PREFIX "rect"
#define KVI_RECT_OPTIONS_PREFIX_LEN 4

#define KviOption_rectFrameGeometry 0                                /* internal */
/* FIXME: this should be in the options module (local config) ? */
#define KviOption_rectGeneralOptionsDialogGeometry 1                 /* internal */
/* FIXME: this should be in the regusers module (local config) ? */
#define KviOption_rectRegisteredUsersDialogGeometry 2                /* internal */

#define KVI_NUM_RECT_OPTIONS 3



#define KVI_BOOL_OPTIONS_PREFIX "bool"
#define KVI_BOOL_OPTIONS_PREFIX_LEN 4

#define KviOption_boolWindowsRememberProperties 0                    /* interface::features::global */
#define KviOption_boolIrcViewShowImages 1                            /* interface::features::components::ircview */
#define KviOption_boolIrcViewTimestamp 2                             /* interface::features::components::ircview */
#define KviOption_boolIrcViewUrlHighlighting 3                       /* interface::features::components::ircview */
#define KviOption_boolIrcViewWrapMargin 4                            /* interface::features::components::ircview */
#define KviOption_boolInputHistoryCursorAtEnd 5                      /* interface::features::components::input */
#define KviOption_boolAvoidParserWarnings 6                          /* ircengine::uparser */
#define KviOption_boolUseProxyHost 7                                 /* transport */
#define KviOption_boolShowGeneralOptionsDialogAsToplevel 8           /* interface::features::global */
#define KviOption_boolForceBrutalQuit 9                              /* irc */
#define KviOption_boolShowPingPong 10                                /* ircoutput */
#define KviOption_boolPopupNotifierOnNewQueryMessages 11             /* query */
#define KviOption_boolShowExtendedServerInfo 12                      /* ircoutput */
#define KviOption_boolEchoNumericTopic 13                            /* channel */
#define KviOption_boolShowOwnParts 14                                /* ircoutput */
#define KviOption_boolUseAntiSpamOnPrivmsg 15                        /* ircengine::outputcontrol::antispam */
#define KviOption_boolUseExtendedPrivmsgView 16                      /* ircengine::outputcontrol::privmsg */
#define KviOption_boolShowUserAndHostInPrivmsgView 17                /* ircengine::outputcontrol::privmsg */
#define KviOption_boolUseWordHighlighting 18                         /* ircengine::outputcontrol::highlighting */
//#define KviOption_boolUseBigIcons 19                               /* UNUSED internal */
#define KviOption_boolCleanupUnusedModules 20                        /* uparser ? */
#define KviOption_boolIgnoreCtcpPing 21                              /* ctcp */
#define KviOption_boolIgnoreCtcpVersion 22                           /* ctcp */
#define KviOption_boolUseCtcpFloodProtection 23                      /* ctcp */
#define KviOption_boolDisableBrokenEventHandlers 24                  /* ircengine::uparser */
#define KviOption_boolIgnoreCtcpClientinfo 25                        /* ctcp */
#define KviOption_boolIgnoreCtcpUserinfo 26                          /* ctcp */
#define KviOption_boolIgnoreCtcpFinger 27                            /* ctcp */
#define KviOption_boolIgnoreCtcpSource 28                            /* ctcp */
#define KviOption_boolIgnoreCtcpTime 29                              /* ctcp */
#define KviOption_boolRequestMissingAvatars 30                       /* ctcp::avatar */
#define KviOption_boolShowCompactModeChanges 31                      /* ircoutput */
#define KviOption_boolIgnoreCtcpDcc 32                               /* ctcp */
#define KviOption_boolAutoAcceptDccChat 33                           /* dcc::chat */
#define KviOption_boolAutoAcceptDccSend 34                           /* dcc::send */
#define KviOption_boolSendZeroAckInDccRecv 35                        /* dcc::send */
#define KviOption_boolUseFastDccSend 36                              /* dcc::send */
#define KviOption_boolUseCompositingForTransparency 37               /* interface::look::global */
#define KviOption_boolUseWindowListIrcContextIndicator 38            /* interface::features::global */
#define KviOption_boolUseGlobalApplicationFont 39                    /* interface::look::global */
#define KviOption_boolAlwaysQuitKVIrcClosingLastConsole 40           /* internal */
#define KviOption_boolAlwaysDisconnectClosingConnectedConsole 41     /* internal */
#define KviOption_boolFlashWindowOnHighlightedMessages 42            /* highlighting */
#define KviOption_boolPopupNotifierOnHighlightedMessages 43          /* highlighting */
#define KviOption_boolFlashWindowOnNotifyOnLine 44                   /* notify */
#define KviOption_boolIrcViewTimestampUTC 45                         /* interface::features::components::ircview */
#define KviOption_boolUseGlobalPseudoTransparency 46                 /* interface::look::global */
#define KviOption_boolUseNotifyList 47                               /* ircengine::notifylist */
#define KviOption_boolNotifyListChangesToActiveWindow 48             /* ircengine::notifylist */
#define KviOption_boolAutoResumeDccSendWhenAutoAccepted 49           /* dcc::send */
#define KviOption_boolLimitOutgoingTraffic 50                        /* transport */
#define KviOption_boolSendUnknownCommandsAsRaw 51                    /* ircengine::uparser */
#define KviOption_boolUserListViewDrawGrid 52                        /* look & feel::interface look::userlist */
#define KviOption_boolShowChannelSyncTime 53                         /* channel */
#define KviOption_boolRejoinChannelOnKick 54                         /* channel */
#define KviOption_boolKeepChannelOpenOnKick 55                       /* channel */
#define KviOption_boolAutoCloseDccSendOnSuccess 56                   /* dcc::send */
#define KviOption_boolNotifyDccSendSuccessInConsole 57               /* dcc::send */
#define KviOption_boolCreateMinimizedDccSend 58                      /* dcc::send */
#define KviOption_boolCreateMinimizedDccChat 59                      /* dcc::chat */
#define KviOption_boolAutoAcceptIncomingAvatars 60                   /* irc::ctcp::avatar */
#define KviOption_boolUseNickCompletionPostfixForFirstWordOnly 61
#define KviOption_boolUseWindowListIcons 62                             /* interface::features */
#define KviOption_boolCreateMinimizedDccSendWhenAutoAccepted 63      /* dcc::send */
#define KviOption_boolCreateMinimizedDccChatWhenAutoAccepted 64      /* dcc::chat */
#define KviOption_boolDccGuessIpFromServerWhenLocalIsUnroutable 65   /* dcc */
#define KviOption_boolShowRegisteredUsersDialogAsToplevel 66         /* interface::features::global */
#define KviOption_boolAutoLogQueries 67                              /* ircengine::logging */
#define KviOption_boolAutoLogChannels 68                             /* ircendine::logging */
#define KviOption_boolAutoLogDccChat 69                              /* ircengine::logging */
#define KviOption_boolAutoLogConsole 70                              /* ircengine::logging */
#define KviOption_boolSilentAntiSpam 71                              /* ircengine::antispam */
#define KviOption_boolUseIncomingDccMediaTypeSavePath 72             /* dcc::send */
#define KviOption_boolNotifyListSendUserhostForOnlineUsers 73        /* ircengine::notifylist */
#define KviOption_boolUseIntelligentNotifyListManager 74             /* ircengine::notifylist */
#define KviOption_boolUseWatchListIfAvailable 75                     /* ircengine::notifylist */
#define KviOption_boolUseTreeWindowListWindowList 76                    /* interface::features::global */
#define KviOption_boolBindIrcIPv4ConnectionsToSpecifiedAddress 77    /* connection::transport */
#define KviOption_boolBindIrcIPv6ConnectionsToSpecifiedAddress 78    /* connection::transport */
#define KviOption_boolUseAntiSpamOnNotice 79                         /* ircengine::antispam */
#define KviOption_boolSetLastAvatarAsDefaultForRegisteredUsers 80    /* irc::ctcp::avatar */
#define KviOption_boolCantAcceptIncomingDccConnections 81            /* dcc::general */
#define KviOption_boolIgnoreCtcpAvatar 82                            /* irc::ctcp::floodprotection */
#define KviOption_boolCtcpRepliesToActiveWindow 83                   /* ctcp */
#define KviOption_boolAutoAcceptDccCanvas 84
#define KviOption_boolNotifyDccSendSuccessInNotifier 85              /* dcc::send */
#define KviOption_boolKillBrokenTimers 86                            /* ircengine::uparser */
#define KviOption_boolDccVoiceVolumeSliderControlsPCM 87             /* dcc::voice */
#define KviOption_boolDccVoiceForceHalfDuplex 88                     /* dcc::voice */
#define KviOption_boolAutoAcceptDccVoice 89                          /* dcc::voice */
#define KviOption_boolCreateMinimizedDccVoice 90                     /* dcc::voice */
#define KviOption_boolCreateMinimizedDccVoiceWhenAutoAccepted 91     /* dcc::voice */
#define KviOption_boolUseIdentService 92                             /* connection::ident */
#define KviOption_boolShowTipAtStartup 93                            /* internal */
#define KviOption_boolIdentdEnableIPv6 94                            /* connection::ident */
#define KviOption_boolIdentdIPv6ContainsIPv4 95                      /* connection::ident */
#define KviOption_boolScaleAvatars 96                                /* ctcp::avatar */
#define KviOption_boolColorNicks 97                                  /* interface::features::components::ircview */
#define KviOption_boolBoldedNicks 98                                 /* interface::features::components::ircview */
#define KviOption_boolDccListenOnSpecifiedInterfaceByDefault 99      /* dcc */
#define KviOption_boolChangeNickAway 100                             /* ircengine::away */
#define KviOption_boolAutoGeneratedAwayNick 101                      /* ircengine::away */
#define KviOption_boolExitAwayOnInput 102                            /* ircengine::away */
#define KviOption_boolAlwaysHighlightNick 103                        /* ircengine::outputcontrol::highlighting */
#define KviOption_boolShowChannelsJoinOnIrc 104                      /* internal */
#define KviOption_boolShowChannelsJoinDialogAsToplevel 105           /* FIXME: internal ??? */
#define KviOption_boolUserDefinedPortRange 106                       /* dcc */
#define KviOption_boolCreateQueryOnPrivmsg 107                       /* query */
#define KviOption_boolCreateQueryOnNotice 108                        /* query */
#define KviOption_boolCreateIncomingQueriesAsMinimized 109           /* query */
#define KviOption_boolAutoJoinOnInvite 110                           /* channel */
#define KviOption_boolShowServersConnectDialogOnStart 111            /* connection::ircservers */
#define KviOption_boolShowServersConnectDialogAsToplevel 112         /* FIXME: internal ?? */
#define KviOption_boolAcceptBrokenFileNameDccResumeRequests 113      /* dcc::send */
#define KviOption_boolAutoReconnectOnUnexpectedDisconnect 114        /* connection */
#define KviOption_boolRejoinChannelsAfterReconnect 115               /* connection */
#define KviOption_boolReopenQueriesAfterReconnect 116                /* connection */
#define KviOption_boolEnableQueryTracing 117                         /* query */
#define KviOption_boolOperatorMessagesToActiveWindow 118             /* irc::output */
#define KviOption_boolInvitesToActiveWindow 119                      /* irc::output */
#define KviOption_boolShowChannelUserFlagInPrivmsgView 120           /* ircengine::outputcontrol::privmsg */
#define KviOption_boolDccSendFakeAddressByDefault 121                /* dcc::general */
#define KviOption_boolUseWindowListActivityMeter 122                    /* irc::output */
#define KviOption_boolCloseServerWidgetAfterConnect 123 /* IMPLEMENTATION NEEDED !!! */
#define KviOption_boolShowIdentityDialogAsToplevel 124  /* ??? */
#define KviOption_boolShowUserChannelIcons 125                       /* look & feel::interface features::userlist */
#define KviOption_boolShowUserChannelState 126                       /* look & feel::interface features::userlist */
#define KviOption_boolEnableIgnoreOnPrivMsg 127                      /* irc::ignore */
#define KviOption_boolVerboseIgnore 128                              /* irc::ignore */
#define KviOption_boolEnableIgnoreOnNotice 129                       /* irc::ignore */
#define KviOption_boolNotifyFailedDccHandshakes 130
#define KviOption_boolAutoTileWindows 131             /* internal ? */
#define KviOption_boolDrawEmoticons 132                              /* look::interface::features::output */
#define KviOption_boolIgnoreCtcpPage 133                             /* irc::ctcp::floodprotection */
#define KviOption_boolShowDialogOnCtcpPage 134
#define KviOption_boolBashLikeNickCompletion 135
#define KviOption_boolUseSSLCertificate 136                          /* ssl */
#define KviOption_boolUseSSLPrivateKey 137                           /* ssl */
#define KviOption_boolWarnAboutPastingMultipleLines 138              /* internal */
#define KviOption_boolKeepChannelsOpenOnUnexpectedDisconnect 139     /* connection */
#define KviOption_boolKeepQueriesOpenOnUnexpectedDisconnect 140      /* connection */
#define KviOption_boolUseLagMeterEngine 141                          /* irc::lag */
#define KviOption_boolShowLagOnContextDisplay 142                    /* irc::lag */
#define KviOption_boolDisableBanListRequestOnJoin 143                /* channel */
#define KviOption_boolDisableBanExceptionListRequestOnJoin 144       /* channel */
#define KviOption_boolDisableInviteListRequestOnJoin 145             /* channel */
#define KviOption_boolDisableWhoRequestOnJoin 146                    /* channel */
#define KviOption_boolShowDockExtension 147                          /* internal ? */
#define KviOption_boolDccAddressByIrcDns 148                         /* irc::general */
#define KviOption_boolMinimizeConsoleAfterConnect 149                /* irc::connection */
#define KviOption_boolHighlightOnlyNormalMsg 150                     /* irc::output::highlight */
#define KviOption_boolHighlightOnlyNormalMsgQueryToo 151             /* irc::output::highlight */
#define KviOption_boolHighlightOnlyNormalMsgHighlightInChanToo 152   /* irc::output::highlight */
#define KviOption_boolHighlightOnlyAtCostumHighlightLevel 153        /* irc::output::highlight */
#define KviOption_boolMdiManagerInSdiMode 154                        /* internal */
#define KviOption_boolDccBrokenBouncerHack 155                       /* dcc */
#define KviOption_boolExternalMessagesToActiveWindow 156             /* irc::output */
#define KviOption_boolIgnoreChannelAvatarRequestsWhenNoAvatarSet 157 /* ctcp */
#define KviOption_boolFlashQueryWindowOnNewMessages 158              /* query */
#define KviOption_boolConfirmCloseWhenThereAreConnections 159        /* interface features */
#define KviOption_boolUseWindowsDesktopForTransparency 160           /* interface::features::global */
#define KviOption_boolCtcpRequestsToActiveWindow 161                 /* irc::output */
#define KviOption_boolServerRepliesToActiveWindow 162                /* irc::output */
#define KviOption_boolServerNoticesToActiveWindow 163                /* irc::output */
#define KviOption_boolServicesNoticesToActiveWindow 164              /* irc::output */
#define KviOption_boolSortWindowListItemsByName 165                     /* interface::features::global */
#define KviOption_boolShowWindowListToolTips 166                        /* interface::features::global */
#define KviOption_boolShowUserListViewToolTips 167                   /* interface::features::global */
#define KviOption_boolWhoisRepliesToActiveWindow 168                 /* irc::output */
#define KviOption_boolPopupNotifierOnNotifyOnLine 169                /* notify */
#define KviOption_boolEnableNotifier 170                    /* notifier */
#define KviOption_boolDccSendForceIdleStep 171                       /* dcc */
#define KviOption_boolStatusBarVisible 172                           /* internal */
#define KviOption_boolTrackLastReadTextViewLine 173                  /* ircview::features */
#define KviOption_boolKeepChannelOpenOnPart 174                      /* channel */
#define KviOption_boolStripMircColorsInUserMessages 175              /* irc::output */
#define KviOption_boolDoNotSuggestRestoreDefaultScript 176           /* internal */
#define KviOption_boolLimitDccSendSpeed 177                          /* dcc::file transfers */
#define KviOption_boolLimitDccRecvSpeed 178                          /* dcc::file transfers */
#define KviOption_boolIgnoreModuleVersions 179
#define KviOption_boolEnableInputHistory 180                        /* interface::input */
#define KviOption_boolUseSpecialColorForTimestamp 181                /* interface::output */
#define KviOption_boolEnableAwayListUpdates 182                     /* channel */
#define KviOption_boolShowAvatarsInUserlist 183				 /* interface::userlist */
#define KviOption_boolShowUserListStatisticLabel 184		        	 /* interface::userlist */
#define KviOption_boolShowIconsInPopupMenus 185			     /* themes::general */
#define KviOption_boolScriptErrorsToDebugWindow 186                  /* parser */
#define KviOption_boolShowMinimizedDebugWindow 187                   /* parser */
#define KviOption_boolShowExtendedInfoInQueryLabel 188               /* query */
#define KviOption_boolUseUserListColorsAsNickColors 189              /* irc::usermessages */
#define KviOption_boolGzipLogs 190                                   /* ircengine::logging */
#define KviOption_boolUseFullWordHighlighting 191                    /* highlight */
#define KviOption_boolNotifierFlashing 192                                   /* interface::notifier */
#define KviOption_boolCommandlineInUserFriendlyModeByDefault 193     /* interface::input */
#define KviOption_boolEnableVisualEffects 194     /* interface */
#define KviOption_boolDCCFileTransferReplaceOutgoingSpacesWithUnderscores 195
#define KviOption_boolDoNotUpscaleAvatars 196
#define KviOption_boolUseDifferentColorForOwnNick 197 /* interface */
#define KviOption_boolCloseInTray 198                                   /* interface */
#define KviOption_boolStartupMinimized 199                                   /* interface */
#define KviOption_boolSortReceivedByDccFilesByNicks 200                                   /* dcc */
#define KviOption_boolLogChannelHistory 201                                   /* channel */
#define KviOption_boolClassicWindowListSetMaximumButtonWidth 202
#define KviOption_boolKeepChannelsOpenOnDisconnect 203 				/* connection */
#define KviOption_boolKeepQueriesOpenOnDisconnect 204				/* connection */
#define KviOption_boolHideWindowToolButtons 205			/* interface */
#define KviOption_boolHideInputToolButtons 206			/* interface */
#define KviOption_boolStripControlCodesInLogs 207			/* log */
#define KviOption_boolEnableTrayIconFlashing 208
#define KviOption_boolUseFlatClassicWindowListButtons 209
#define KviOption_boolStripMsgTypeInLogs 210				/* log */
#define KviOption_boolUseLevelBasedTrayNotification 211
#define KviOption_boolMuteAllSounds 212
#define KviOption_boolCreateMinimizedChannels 213
#define KviOption_boolShowNetworkNameForConsoleWindowListEntry 214
#define KviOption_boolDrawGenderIcons 215
#define KviOption_boolPrependGenderInfoToRealname 216
#define KviOption_boolUseIdentServiceOnlyOnConnect 217      /* connection::ident */
#define KviOption_boolUseSystemUrlHandlers 218 /* Windoze only*/
#define KviOption_boolScaleAvatarsOnLoad 219
#define KviOption_boolNotifierFading 220 /* interface::notifier */
#define KviOption_boolPickRandomIpAddressForRoundRobinServers 221
#define KviOption_boolPrependAvatarInfoToRealname 222
#define KviOption_boolEnableAnimatedAvatars 223
#define KviOption_boolEnableAnimatedSmiles 224
#define KviOption_boolPlaceNickWithNonAlphaCharsAtEnd 225
#define KviOption_boolUseStartTlsIfAvailable 226
#define KviOption_boolPasteLastLogOnChannelJoin 227                /* channel */
#define KviOption_boolPasteLastLogOnQueryJoin 228                  /* query */
#define KviOption_boolUseSpecifiedSmartColorForOwnNick 229 /* interface::ircview */
#define KviOption_boolZshLikeNickCompletion 230
#define KviOption_boolAutoAcceptDccVideo 231                          /* dcc::video */
#define KviOption_boolCreateMinimizedDccVideo 232                     /* dcc::video */
#define KviOption_boolCreateMinimizedDccVideoWhenAutoAccepted 233     /* dcc::video */
#define KviOption_boolShowTreeWindowListHeader 234
#define KviOption_boolFlashDccChatWindowOnNewMessages 235              /* dcc:chat */
#define KviOption_boolPopupNotifierOnNewDccChatMessages 236            /* dcc:chat */
#define KviOption_boolUseAwayMessage 237                               /* away */
#define KviOption_boolDisableQuietBanListRequestOnJoin 238       /* channel */
#define KviOption_boolUseSaslIfAvailable 239
#define KviOption_boolFrameIsMaximized 240                             /* internal */
#define KviOption_boolPrependNickColorInfoToRealname 241
#define KviOption_boolDontShowNotifierIfActiveWindowIsFullScreen 242   /* notifier */
#define KviOption_boolWheelScrollsWindowsList 243
#define KviOption_boolAcceptMismatchedPortDccResumeRequests 244         /* dcc::send */
#define KviOption_boolEnableUrlLinkToolTip 245                           /* ircview output */
#define KviOption_boolEnableHostLinkToolTip 246                          /* ircview output */
#define KviOption_boolEnableServerLinkToolTip 247                        /* ircview output */
#define KviOption_boolEnableModeLinkToolTip 248                          /* ircview output */
#define KviOption_boolEnableNickLinkToolTip 249                          /* ircview output */
#define KviOption_boolEnableChannelLinkToolTip 250                       /* ircview output */
#define KviOption_boolEnableEscapeLinkToolTip 251                        /* ircview output */
#define KviOption_boolUseDBusNotifier 252                                /* notifier */
#define KviOption_boolUseKDENotifier 253                                 /* notifier */
#define KviOption_boolCaseSensitiveHighlighting 254                      /* highlight */
#define KviOption_boolMinimizeInTray 255                                    /* interface */
#define KviOption_boolDisplayNotifierOnPrimaryScreen 256                 /* notifier */

#define KVI_NUM_BOOL_OPTIONS 257


#define KVI_STRING_OPTIONS_PREFIX "string"
#define KVI_STRING_OPTIONS_PREFIX_LEN 6

//UNUSED ! #define KviOption_stringFrameCaption 0                               /* interfacelookglobal */
#define KviOption_stringNickname1 1                                  /* identity */
#define KviOption_stringNickname2 2                                  /* identity */
#define KviOption_stringNickname3 3                                  /* identity */
#define KviOption_stringNickname4 4                                  /* identity */
#define KviOption_stringUsername 5                                   /* identity */
#define KviOption_stringRealname 6                                   /* identity */
#define KviOption_stringLocalHostIp 7                                /* ?????? This is a fallback one */
#define KviOption_stringPartMessage 8                                /* ircengine::defaultmessages */
#define KviOption_stringQuitMessage 9                                /* ircengine::defaultmessages */
#define KviOption_stringUrlHttpCommand 10                            /* urlhandlers */
#define KviOption_stringExtendedPrivmsgPrefix 11                     /* ircengine::outputcontrol::privmsg */
#define KviOption_stringExtendedPrivmsgPostfix 12                    /* ircengine::outputcontrol::privmsg */
#define KviOption_stringCtcpVersionPostfix 13                        /* ircengine::ctcp::replies */
#define KviOption_stringCtcpUserInfoOther 14                         /* identity */
#define KviOption_stringCtcpSourcePostfix 15                         /* ircengine::ctcp::replies */
#define KviOption_stringUrlFtpCommand 16                             /* urlhandlers */
#define KviOption_stringUrlHttpsCommand 17                           /* urlhandlers */
#define KviOption_stringUrlMailtoCommand 18                          /* urlhandlers */
#define KviOption_stringPreferredMediaPlayer 19                      /* still missing in options ! */
#define KviOption_stringUrlFileCommand 20                            /* urlhandlers */
#define KviOption_stringNickCompletionPostfix 21
#define KviOption_stringUrlUnknownCommand 22                         /* urlhandlers */
#define KviOption_stringIPv4ConnectionBindAddress 23                 /* connection::transport */
#define KviOption_stringIPv6ConnectionBindAddress 24                 /* connection::transport */
#define KviOption_stringDccVoiceSoundDevice 25                       /* dcc::voice */
#define KviOption_stringDccVoiceMixerDevice 26                       /* dcc::voice */
#define KviOption_stringAwayMessage 27                               /* irc::away */
#define KviOption_stringIdentdUser 28                                /* connection::ident */
#define KviOption_stringDccListenDefaultInterface 29                 /* dcc::general */
#define KviOption_stringCustomAwayNick 30                            /* ircengine::away */
#define KviOption_stringDefaultDccFakeAddress 31                     /* dcc::general */
#define KviOption_stringCtcpPageReply 32                             /* ircengine::ctcp::replies */
#define KviOption_stringSSLCertificatePath 33                        /* ssl */
#define KviOption_stringSSLCertificatePass 34                        /* ssl */
#define KviOption_stringSSLPrivateKeyPath 35                         /* ssl */
#define KviOption_stringSSLPrivateKeyPass 36                         /* ssl */
#define KviOption_stringSoundSystem 37                               /* sound */
#define KviOption_stringDefaultUserMode 38
#define KviOption_stringLastImageDialogPath 39                       /* internal */
#define KviOption_stringIconThemeSubdir 40
#define KviOption_stringIncomingPath 41                              /* folders */
#define KviOption_stringMyAvatar 42
// empty string means guess from locale
#define KviOption_stringDefaultTextEncoding 43                       /* text encoding */

#define KviOption_stringWinampTextEncoding 44
#define KviOption_stringMp3TagsEncoding 45

#define KviOption_stringCtcpUserInfoAge 46                           /* identity */
#define KviOption_stringCtcpUserInfoGender 47                        /* identity */
#define KviOption_stringCtcpUserInfoLocation 48                      /* identity */
#define KviOption_stringCtcpUserInfoLanguages 49                     /* identity */
#define KviOption_stringWordSplitters 50                             /* highlight */
#define KviOption_stringOnNewQueryOpenedSound 51                     /* sounds */
#define KviOption_stringOnHighlightedMessageSound 52                 /* sounds */
#define KviOption_stringOnMeKickedSound 53                           /* sounds */
#define KviOption_stringOnQueryMessageSound 54                       /* sounds */
#define KviOption_stringIrcViewTimestampFormat 55
#define KviOption_stringPreferredTorrentClient 56                    /* still missing in options ! */
// empty string means guess from locale
#define KviOption_stringDefaultSrvEncoding 57                        /* server encoding */
#define KviOption_stringLogsPath 58                                  /* logfolder */
#define KviOption_stringLogsDynamicPath 59                           /* logfolder */

#define KVI_NUM_STRING_OPTIONS 60



#define KVI_STRINGLIST_OPTIONS_PREFIX "stringlist"
#define KVI_STRINGLIST_OPTIONS_PREFIX_LEN 10

#define KviOption_stringlistHighlightWords 0                        /* ircengine::outputcontrol::highlighting */
#define KviOption_stringlistSpamWords 1                             /* ircengine::outputcontrol::antispam */
#define KviOption_stringlistRecentChannels 2                        /* internal */
#define KviOption_stringlistRecentServers 3                         /* internal */
#define KviOption_stringlistRecentNicknames 4                       /* internal */
#define KviOption_stringlistModuleExtensionToolbars 5               /* internal */
#define KviOption_stringlistImageSearchPaths 6                      /* ??? */
#define KviOption_stringlistRecentIrcUrls 7                      /* internal */

#define KVI_NUM_STRINGLIST_OPTIONS 8



#define KVI_COLOR_OPTIONS_PREFIX "color"
#define KVI_COLOR_OPTIONS_PREFIX_LEN 5

#define KviOption_colorMdiBackground 0                                            /* ?? look & feel : gui elements : mdi manager */
#define KviOption_colorWindowListNormalText 1                                     /* look & feel::interface look::WindowList::classic WindowList */
#define KviOption_colorWindowListMinimizedText 2                                  /* look & feel::interface look::WindowList::classic WindowList */
#define KviOption_colorWindowListHighlight1Text 3                                 /* look & feel::interface look::WindowList::classic WindowList */
#define KviOption_colorWindowListHighlight2Text 4                                 /* look & feel::interface look::WindowList::classic WindowList */
#define KviOption_colorWindowListProgressBar 5                                    /* look & feel::interface look::WindowList::classic WindowList */
#define KviOption_colorUserListViewAwayForeground 6                               /* look & feel::interface look::userlist */
#define KviOption_colorUserListViewOwnForeground 7                                /* look & feel::interface look::userlist */
#define KviOption_colorUserListViewIrcOpForeground 8                              /* look & feel::interface look::userlist */
#define KviOption_colorNotifierBackground 9                                       /* look & feel::interface look::notifier */
#define KviOption_colorNotifierForeground 10                                      /* look & feel::interface look::notifier */
#define KviOption_colorNotifierTitleForeground 11                                 /* look & feel::interface look::notifier */
#define KviOption_colorIrcViewBackground 12                                       /* look & feel::interface look::ircview */
#define KviOption_colorInputBackground 13                                         /* look & feel::interface look::input */
#define KviOption_colorInputSelectionBackground 14                                /* look & feel::interface look::input */
#define KviOption_colorInputSelectionForeground 15                                /* look & feel::interface look::input */
#define KviOption_colorInputCursor 16                                             /* look & feel::interface look::input */
#define KviOption_colorInputControl 17                                            /* look & feel::interface look::input */
#define KviOption_colorInputForeground 18                                         /* look & feel::interface look::input */
#define KviOption_colorUserListViewBackground 19                                  /* look & feel::interface look::userlist */
#define KviOption_colorUserListViewOpForeground 20                                /* look & feel::interface look::userlist */
#define KviOption_colorUserListViewVoiceForeground 21                             /* look & feel::interface look::userlist */
#define KviOption_colorUserListViewNormalForeground 22                            /* look & feel::interface look::userlist */
#define KviOption_colorUserListViewSelectionBackground 23                         /* look & feel::interface look::userlist */
#define KviOption_colorUserListViewSelectionForeground 24                         /* look & feel::interface look::userlist */
#define KviOption_colorLabelBackground 25                                         /* look & feel::interface look::topic label */
#define KviOption_colorLabelForeground 26                                         /* look & feel::interface look::topic label */
#define KviOption_colorGlobalTransparencyFade 27                                  /* look & feel::interface look */
#define KviOption_colorIrcToolBarAppletBackground 28                              /* theme::tool bar applets */
#define KviOption_colorIrcToolBarAppletForegroundLowContrast 29                   /* theme::tool bar applets */
#define KviOption_colorIrcToolBarAppletForegroundMidContrast 30                   /* theme::tool bar applets */
#define KviOption_colorIrcToolBarAppletForegroundHighContrastInactive1 31         /* theme::tool bar applets */
#define KviOption_colorIrcToolBarAppletForegroundHighContrastInactive2 32         /* theme::tool bar applets */
#define KviOption_colorIrcToolBarAppletForegroundHighContrastActive1 33           /* theme::tool bar applets */
#define KviOption_colorIrcToolBarAppletForegroundHighContrastActive2 34           /* theme::tool bar applets */
#define KviOption_colorTreeWindowListBackground 35                                /* look & feel::interface look::WindowList::tree WindowList */
#define KviOption_colorTreeWindowListActiveBackground 36                          /* look & feel::interface look::WindowList::tree WindowList */
#define KviOption_colorTreeWindowListForeground 37                                /* look & feel::interface look::WindowList::tree WindowList */
#define KviOption_colorTreeWindowListActiveForeground 38                          /* look & feel::interface look::WindowList::tree WindowList */
#define KviOption_colorTreeWindowListHighlight1Foreground 39                      /* look & feel::interface look::WindowList::tree WindowList */
#define KviOption_colorTreeWindowListHighlight2Foreground 40                      /* look & feel::interface look::WindowList::tree WindowList */
#define KviOption_colorTreeWindowListProgress 41                                  /* look & feel::interface look::WindowList::tree WindowList */
#define KviOption_colorUserListViewHalfOpForeground 42                            /* look & feel::interface look::userlist */
#define KviOption_colorWindowListHighlight3Text 43                                /* look & feel::interface look::WindowList::classic WindowList */
#define KviOption_colorWindowListHighlight4Text 44                                /* look & feel::interface look::WindowList::classic WindowList */
#define KviOption_colorWindowListHighlight5Text 45                                /* look & feel::interface look::WindowList::classic WindowList */
#define KviOption_colorTreeWindowListHighlight3Foreground 46                      /* look & feel::interface look::WindowList::tree WindowList */
#define KviOption_colorTreeWindowListHighlight4Foreground 47                      /* look & feel::interface look::WindowList::tree WindowList */
#define KviOption_colorTreeWindowListHighlight5Foreground 48                      /* look & feel::interface look::WindowList::tree WindowList */
#define KviOption_colorUserListViewChanAdminForeground 49                         /* look & feel::interface look::userlist */
#define KviOption_colorUserListViewUserOpForeground 50                            /* look & feel::interface look::userlist */
#define KviOption_colorUserListViewGrid 51                                        /* look & feel::interface look::userlist */
#define KviOption_colorUserListViewChanOwnerForeground 52                         /* look & feel::interface look::userlist */
#define KviOption_colorIrcViewMarkLine 53                                         /* look & feel::interface look::ircvew */

#define KVI_NUM_COLOR_OPTIONS 54



#define KVI_PIXMAP_OPTIONS_PREFIX "pixmap"
#define KVI_PIXMAP_OPTIONS_PREFIX_LEN 6

#define KviOption_pixmapMdiBackground 0
#define KviOption_pixmapIrcViewBackground 1                                    /* interface::look::components::ircview */
#define KviOption_pixmapInputBackground 2                                      /* interface::look::components::input */
#define KviOption_pixmapUserListViewBackground 3                               /* look & feel::interface look::userlist */
#define KviOption_pixmapLabelBackground 4                                      /* look & feel::interface look::topic label */
#define KviOption_pixmapMyAvatar 5                                             /* identity */
#define KviOption_pixmapIrcToolBarAppletBackground 6                           /* theme::tool bar applets */
#define KviOption_pixmapTreeWindowListBackground 7                             /* look & feel::interface look::WindowList::tree WindowList */
#define KviOption_pixmapGlobalTransparencyBackground 8                         /* theme::transparency */
#define KviOption_pixmapNotifierBackground 9                                   /* interface::look::components::notifier */

#define KVI_NUM_PIXMAP_OPTIONS 10




#define KVI_UINT_OPTIONS_PREFIX "uint"
#define KVI_UINT_OPTIONS_PREFIX_LEN 4

#define KviOption_uintIrcSocketTimeout 0                             /* connection::transport */
#define KviOption_uintModuleCleanupTimerInterval 1
#define KviOption_uintModuleCleanupTimeout 2
#define KviOption_uintMaxCtcpRequests 3                              /* ircengine::ctcp */
#define KviOption_uintCtcpFloodCheckInterval 4                       /* ircengine::ctcp */
#define KviOption_uintTotalConnectionTime 5
#define KviOption_uintDccSocketTimeout 6                             /* dcc::general */
#define KviOption_uintDccSendIdleStepInMSec 7                        /* dcc::send */
#define KviOption_uintDccSendPacketSize 8                            /* dcc::send */
#define KviOption_uintGlobalTransparencyParentFadeFactor 9           /* interface::look::general */
#define KviOption_uintGlobalTransparencyChildFadeFactor 10           /* interface::look::general */
#define KviOption_uintNotifyListCheckTimeInSecs 11                   /* notify */
#define KviOption_uintSocketQueueFlushTimeout 12                     /* connection::transport */
#define KviOption_uintOutgoingTrafficLimitUSeconds 13                /* connection::transport */
#define KviOption_uintNotifyListIsOnDelayTimeInSecs 14               /* notify */
#define KviOption_uintNotifyListUserhostDelayTimeInSecs 15           /* notify */
#define KviOption_uintTreeWindowListMinimumWidth 16           /* ?? interface::general ?? */
#define KviOption_uintAvatarOfferTimeoutInSecs 17                    /* irc::ctcp::avatar */
#define KviOption_uintIrcViewMaxBufferSize 18                        /* interface::features::components::ircview */
#define KviOption_uintIrcViewToolTipTimeoutInMsec 19                 /* interface::features::components::ircview */
#define KviOption_uintIrcViewToolTipHideTimeoutInMsec 20             /* interface::features::components::ircview */
#define KviOption_uintDccVoicePreBufferSize	21                       /* dcc::voice */
#define KviOption_uintIdentdPort 22                                  /* connection::ident */
#define KviOption_uintAvatarScaleWidth 23                            /* ctcp::avatar */
#define KviOption_uintAvatarScaleHeight 24                           /* ctcp::avatar */
#define KviOption_uintDccMinPort 25                                  /* dcc::general */
#define KviOption_uintDccMaxPort 26                                  /* dcc::general */
#define KviOption_uintSoundOfferTimeoutInSecs 27
#define KviOption_uintMaximumRequestedAvatarSize 28                  /* ctcp::avatar */
#define KviOption_uintGlobalWindowOpacityPercent 29
#define KviOption_uintPasteDelay 30
// 0 = unlimited!
#define KviOption_uintMaxDccSendTransfers 31
// 0 = unlimited slots = windows
#define KviOption_uintMaxDccSlots 32
#define KviOption_uintTileMethod 33        /* internal ? */
#define KviOption_uintLagMeterHeartbeat 34
#define KviOption_uintLagAlarmTime 35
// KVI_TEXT_ENCODING_UNKNOWN = KviLocale::defaultTextEncoding() <-- guessed from locale
//#define KviOption_uintDefaultTextEncoding 36                          /* textencoding */
#define KviOption_uintWindowListButtonMinWidth 37                             /* interface::look::WindowList::classic WindowList */
// follows the verbosity constants defined below
#define KviOption_uintOutputVerbosityLevel 38
#define KviOption_uintIrcContextAppletWidth 39                       /* internal */
#define KviOption_uintMinHighlightLevel 40                                /* look::interface::features::output */
#define KviOption_uintUserListViewGridType 41                        /* look&feel::ircview */
#define KviOption_uintWindowListMaximumRows 42                          /* interface::look::WindowList::classic WindowList */
// 0 = unlimited attempts
#define KviOption_uintMaxAutoReconnectAttempts 43                    /* connection */
#define KviOption_uintAutoReconnectDelay 44                          /* connection */
#define KviOption_uintMaxDccSendSpeed 45                             /* dcc::file transfers */
#define KviOption_uintMaxDccRecvSpeed 46                             /* dcc::file transfers */
#define KviOption_uintTimeStampForeground 47
#define KviOption_uintTimeStampBackground 48
#define KviOption_uintUserExperienceLevel 49
#define KviOption_uintClassicWindowListMaximumButtonWidth 50
#define KviOption_uintDefaultBanType 51
#define KviOption_uintIrcViewPixmapAlign 52
#define KviOption_uintUserListPixmapAlign 53
#define KviOption_uintToolBarAppletPixmapAlign 54
#define KviOption_uintTreeWindowListPixmapAlign 55
#define KviOption_uintInputPixmapAlign 56
#define KviOption_uintAutoFlushLogs 57
#define KviOption_uintMinTrayLowLevelMessage 58
#define KviOption_uintMinTrayHighLevelMessage 59
#define KviOption_uintIdentdOutputMode 60
#define KviOption_uintScaleAvatarsOnLoadHeight 61
#define KviOption_uintScaleAvatarsOnLoadWidth 62
#define KviOption_uintNotifierActiveTransparency 63
#define KviOption_uintNotifierInactiveTransparency 64
#define KviOption_uintIrcViewMarkerStyle 65
#define KviOption_uintIrcViewMarkerSize 66
#define KviOption_uintUrlMouseClickNum 67
#define KviOption_uintNotifierAutoHideTime 68
#define KviOption_uintLinesToPasteOnChannelJoin 69                /* channel */
#define KviOption_uintDaysIntervalToPasteOnChannelJoin 70         /* channel */
#define KviOption_uintLinesToPasteOnQueryJoin 71                  /* query */
#define KviOption_uintDaysIntervalToPasteOnQueryJoin 72           /* query */
#define KviOption_uintSpacesToExpandTabulationInput 73           /* number of spaces used in tabulation expansion in the inputbar */
#define KviOption_uintUserIrcViewOwnForeground 74                /* look & feel::ircview */
#define KviOption_uintUserIrcViewOwnBackground 75                /* look & feel::ircview */
#define KviOption_uintNotifierPixmapAlign 76
#define KviOption_uintOutputDatetimeFormat 77                    /* irc::verbosity */
#define KviOption_uintOnJoinRequestsDelay 78
#define KviOption_uintToolBarIconSize 79
#define KviOption_uintToolBarButtonStyle 80

#define KVI_NUM_UINT_OPTIONS 81

namespace KviIdentdOutputMode {
	enum Mode {
		Quiet = 0,
		ToConsole = 1,
		ToActiveWindow = 2
	};
}


#define KVI_FONT_OPTIONS_PREFIX "font"
#define KVI_FONT_OPTIONS_PREFIX_LEN 4

#define KviOption_fontIrcView 0                                      /* interface::look::components::ircview */
#define KviOption_fontInput 1                                        /* interface::look::components::input */
#define KviOption_fontUserListView 2                                 /* look & feel::interface look::userlist */
#define KviOption_fontLabel 3                                        /* look & feel::interface look::topic label */
#define KviOption_fontApplication 4                                  /* interface::look::global */
#define KviOption_fontIrcToolBarApplet 5                             /* theme::tool bar applet */
#define KviOption_fontWindowList 6                                   /* interface::look::WindowList::classic WindowList */
#define KviOption_fontTreeWindowList 7                               /* interface::look::WindowList::tree WindowList */
#define KviOption_fontNotifier 8                                     /* interface::look::components::notifier */
#define KviOption_fontNotifierTitle 9                                /* interface::look::components::notifier */

#define KVI_NUM_FONT_OPTIONS 10




#define KVI_ICCOLOR_OPTIONS_PREFIX "iccolor"
#define KVI_ICCOLOR_OPTIONS_PREFIX_LEN 7

#define KVI_NUM_ICCOLOR_OPTIONS 8


// indices declared in kvi_out.h

#define KVI_MSGTYPE_OPTIONS_PREFIX "msgtype"
#define KVI_MSGTYPE_OPTIONS_PREFIX_LEN 7

#define KVI_NUM_MSGTYPE_OPTIONS 144



#define KVI_MIRCCOLOR_OPTIONS_PREFIX "mirccolor"
#define KVI_MIRCCOLOR_OPTIONS_PREFIX_LEN 9

#define KVI_NUM_MIRCCOLOR_OPTIONS 16


// external declaration of the tables
extern KVIRC_API KviBoolOption       g_boolOptionsTable[KVI_NUM_BOOL_OPTIONS];
extern KVIRC_API KviRectOption       g_rectOptionsTable[KVI_NUM_RECT_OPTIONS];
extern KVIRC_API KviStringOption     g_stringOptionsTable[KVI_NUM_STRING_OPTIONS];
extern KVIRC_API KviColorOption      g_colorOptionsTable[KVI_NUM_COLOR_OPTIONS];
extern KVIRC_API KviPixmapOption     g_pixmapOptionsTable[KVI_NUM_PIXMAP_OPTIONS];
//	extern KviIntOption        g_intOptionsTable[KVI_NUM_INT_OPTIONS];
extern KVIRC_API KviUIntOption       g_uintOptionsTable[KVI_NUM_UINT_OPTIONS];
extern KVIRC_API KviFontOption       g_fontOptionsTable[KVI_NUM_FONT_OPTIONS];
extern KVIRC_API KviMessageTypeSettingsOption    g_msgtypeOptionsTable[KVI_NUM_MSGTYPE_OPTIONS];
extern KVIRC_API KviColorOption      g_mirccolorOptionsTable[KVI_NUM_MIRCCOLOR_OPTIONS];
extern KVIRC_API KviColorOption      g_iccolorOptionsTable[KVI_NUM_ICCOLOR_OPTIONS];
extern KVIRC_API KviStringListOption g_stringlistOptionsTable[KVI_NUM_STRINGLIST_OPTIONS];

// __this is how we access the options from outside
#define KVI_OPTION_BOOL(_idx)        g_boolOptionsTable[_idx].option
#define KVI_OPTION_RECT(_idx)        g_rectOptionsTable[_idx].option
#define KVI_OPTION_STRING(_idx)      g_stringOptionsTable[_idx].option
#define KVI_OPTION_COLOR(_idx)       g_colorOptionsTable[_idx].option
#define KVI_OPTION_PIXMAP(_idx)      g_pixmapOptionsTable[_idx].option
#define KVI_OPTION_UINT(_idx)        g_uintOptionsTable[_idx].option
#define KVI_OPTION_FONT(_idx)        g_fontOptionsTable[_idx].option
#define KVI_OPTION_MSGTYPE(_idx)     g_msgtypeOptionsTable[_idx].option
#define KVI_OPTION_MIRCCOLOR(_idx)   g_mirccolorOptionsTable[_idx].option
#define KVI_OPTION_STRINGLIST(_idx)  g_stringlistOptionsTable[_idx].option
#define KVI_OPTION_ICCOLOR(_idx)     g_iccolorOptionsTable[_idx].option


// Verbosity constants

#define KVI_VERBOSITY_LEVEL_MUTE 0
#define KVI_VERBOSITY_LEVEL_QUIET 1
#define KVI_VERBOSITY_LEVEL_NORMAL 2
#define KVI_VERBOSITY_LEVEL_VERBOSE 3
#define KVI_VERBOSITY_LEVEL_PARANOIC 4

#define _OUTPUT_PARANOIC (KVI_OPTION_UINT(KviOption_uintOutputVerbosityLevel) >= KVI_VERBOSITY_LEVEL_PARANOIC)
#define _OUTPUT_VERBOSE (KVI_OPTION_UINT(KviOption_uintOutputVerbosityLevel) >= KVI_VERBOSITY_LEVEL_VERBOSE)
#define _OUTPUT_QUIET (KVI_OPTION_UINT(KviOption_uintOutputVerbosityLevel) <= KVI_VERBOSITY_LEVEL_QUIET)
#define _OUTPUT_MUTE (KVI_OPTION_UINT(KviOption_uintOutputVerbosityLevel) <= KVI_VERBOSITY_LEVEL_MUTE)

	#define START_TABLE_BOLD_ROW "<tr><td style=\"background-color: rgb(0,0,0); font-weight: bold; color: rgb(200,200,255); text-indent: 5px;\"><nobr>"
	#define END_TABLE_BOLD_ROW "</nobr></td></tr>"

	#define START_TABLE_NORMAL_ROW "<tr><td>"

	#define SET_ANTI_ALIASING(p) (p).setRenderHint(QPainter::TextAntialiasing);


#define KVI_RECENT_CHANNELS_SEPARATOR ":"

#if defined(_KVI_OPTIONS_CPP_) || defined(_WANT_OPTION_FLAGS_)

	// flag definitions

	// section of the config file (rightmost 8 bits)
	#define KviOption_sectFlagNone 0
	#define KviOption_sectFlagFrame 1
	#define KviOption_sectFlagMdi 2
	#define KviOption_sectFlagWindowList 3
	#define KviOption_sectFlagWindows 4
	#define KviOption_sectFlagIrcView 5
	#define KviOption_sectFlagMsgType 6
	#define KviOption_sectFlagMircColor 7
	#define KviOption_sectFlagInput 8
	#define KviOption_sectFlagUserParser 9
	#define KviOption_sectFlagConnection 10
	#define KviOption_sectFlagIrcSocket 11
	#define KviOption_sectFlagUser 12
	#define KviOption_sectFlagUserListView 13
	#define KviOption_sectFlagLabel 14
	#define KviOption_sectFlagModules 15
	#define KviOption_sectFlagCtcp 16
	#define KviOption_sectFlagDcc 17
	#define KviOption_sectFlagGui 18
	#define KviOption_sectFlagIrcToolBar 19
	#define KviOption_sectFlagLogging 20
	#define KviOption_sectFlagAntiSpam 21
	#define KviOption_sectFlagAvatar 22
	#define KviOption_sectFlagUrl 23
	#define KviOption_sectFlagRecent 24
	#define KviOption_sectFlagGeometry 25
	#define KviOption_sectFlagNotifier 26

	#define KVI_NUM_OPTION_SECT_FLAGS 27

	#define KviOption_sectMask 255

	// other logical groupings of options
	#define KviOption_groupTheme (1 << 8)

	#define KviOption_groupMask (255 << 8)



	// actions to take when an option is set
	#define KviOption_resetNone 0
	#define KviOption_resetUpdateNotifier (1 << 16)
	#define KviOption_resetUpdateGui (1 << 17)
	#define KviOption_resetUpdateWindowList (1 << 18)
	#define KviOption_resetUpdateAppFont (1 << 19)
	#define KviOption_resetUpdatePseudoTransparency (1 << 20)
	#define KviOption_resetRestartNotifyList (1 << 21)
	#define KviOption_resetRestartIdentd (1 << 22)
	#define KviOption_resetReloadImages (1 << 23)
	#define KviOption_resetRestartLagMeter (1 << 24)
	#define KviOption_resetRecentChannels (1 << 25)

	#define KviOption_resetMask (~(KviOption_sectMask | KviOption_groupMask))

	//for file pathes
	#define KviOption_encodePath (1 << 25)

#endif

#endif //_KVI_OPTIONS_H_
