#ifndef _KVI_OPTIONS_H_
#define _KVI_OPTIONS_H_
//=============================================================================
//
//   File : KviOptions.h
//   Creation date : Tue Jun 20 2000 12:42:07 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2014 OmegaPhil (OmegaPhil at startmail dot com)
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

#include "kvi_settings.h"
#include "KviCString.h"
#include "KviPixmap.h"
#include "KviMessageTypeSettings.h"
#include "KviControlCodes.h"

#include <QRect>
#include <QPixmap>
#include <QColor>
#include <QFont>
#include <QStringList>

// option structures
#define DECLARE_OPTION_STRUCT(_cname, _type)      \
	class _cname                                  \
	{                                             \
	public:                                       \
		QString name;                             \
		_type option;                             \
		int flags;                                \
                                                  \
	public:                                       \
		_cname(const QString & n, _type o, int f) \
		    : name(n), option(o), flags(f){}      \
		~_cname() = default;                      \
	};

DECLARE_OPTION_STRUCT(KviBoolOption, bool)
DECLARE_OPTION_STRUCT(KviRectOption, QRect)
DECLARE_OPTION_STRUCT(KviStringOption, QString)
DECLARE_OPTION_STRUCT(KviColorOption, QColor)
DECLARE_OPTION_STRUCT(KviFontOption, QFont)
DECLARE_OPTION_STRUCT(KviPixmapOption, KviPixmap)
//DECLARE_OPTION_STRUCT(KviIntOption,int)
DECLARE_OPTION_STRUCT(KviUIntOption, unsigned int)
DECLARE_OPTION_STRUCT(KviMessageTypeSettingsOption, KviMessageTypeSettings)
DECLARE_OPTION_STRUCT(KviStringListOption, QStringList)

#define KVI_COLOR_EXT_USER_OP 150
#define KVI_COLOR_EXT_USER_HALFOP 151
#define KVI_COLOR_EXT_USER_ADMIN 152
#define KVI_COLOR_EXT_USER_OWNER 153
#define KVI_COLOR_EXT_USER_VOICE 154
#define KVI_COLOR_EXT_USER_USEROP 155
#define KVI_COLOR_EXT_USER_NORMAL 156
#define KVI_COLOR_CUSTOM 255
#define KVI_COLOR_OWN 254

// option entry indexes
#define KVI_RECT_OPTIONS_PREFIX "rect"
#define KVI_RECT_OPTIONS_PREFIX_LEN 4

#define KviOption_rectFrameGeometry 0 /* internal */
/* FIXME: this should be in the options module (local config) ? */
#define KviOption_rectGeneralOptionsDialogGeometry 1 /* internal */
/* FIXME: this should be in the regusers module (local config) ? */
#define KviOption_rectRegisteredUsersDialogGeometry 2 /* internal */

#define KVI_NUM_RECT_OPTIONS 3

#define KVI_BOOL_OPTIONS_PREFIX "bool"
#define KVI_BOOL_OPTIONS_PREFIX_LEN 4

// NOTICE: REUSE EQUIVALENT UNUSED BOOL_OPTION in KviOptions.cpp ENTRIES BEFORE ADDING NEW ENTRIES BELOW

#define KviOption_boolWindowsRememberProperties 0                              /* interface::features::global */
#define KviOption_boolIrcViewShowImages 1                                      /* interface::features::components::ircview */
#define KviOption_boolIrcViewTimestamp 2                                       /* interface::features::components::ircview */
#define KviOption_boolIrcViewUrlHighlighting 3                                 /* interface::features::components::ircview */
#define KviOption_boolIrcViewWrapMargin 4                                      /* interface::features::components::ircview */
#define KviOption_boolInputHistoryCursorAtEnd 5                                /* interface::features::components::input */
#define KviOption_boolAvoidParserWarnings 6                                    /* ircengine::uparser */
#define KviOption_boolUseProxyHost 7                                           /* transport */
#define KviOption_boolIgnoreSpecialCharactersInNickCompletion 8                /* input */
#define KviOption_boolForceBrutalQuit 9                                        /* irc */
#define KviOption_boolShowPingPong 10                                          /* ircoutput */
#define KviOption_boolPopupNotifierOnNewQueryMessages 11                       /* query */
#define KviOption_boolShowExtendedServerInfo 12                                /* ircoutput */
#define KviOption_boolEchoNumericTopic 13                                      /* channel */
#define KviOption_boolShowOwnParts 14                                          /* ircoutput */
#define KviOption_boolUseAntiSpamOnPrivmsg 15                                  /* ircengine::outputcontrol::antispam */
#define KviOption_boolUseExtendedPrivmsgView 16                                /* ircengine::outputcontrol::privmsg */
#define KviOption_boolShowUserAndHostInPrivmsgView 17                          /* ircengine::outputcontrol::privmsg */
#define KviOption_boolUseWordHighlighting 18                                   /* ircengine::outputcontrol::highlighting */
#define KviOption_boolCleanupUnusedModules 19                                  /* uparser ? */
#define KviOption_boolIgnoreCtcpPing 20                                        /* ctcp */
#define KviOption_boolIgnoreCtcpVersion 21                                     /* ctcp */
#define KviOption_boolUseCtcpFloodProtection 22                                /* ctcp */
#define KviOption_boolDisableBrokenEventHandlers 23                            /* ircengine::uparser */
#define KviOption_boolIgnoreCtcpClientinfo 24                                  /* ctcp */
#define KviOption_boolIgnoreCtcpUserinfo 25                                    /* ctcp */
#define KviOption_boolIgnoreCtcpFinger 26                                      /* ctcp */
#define KviOption_boolIgnoreCtcpSource 27                                      /* ctcp */
#define KviOption_boolIgnoreCtcpTime 28                                        /* ctcp */
#define KviOption_boolRequestMissingAvatars 29                                 /* ctcp::avatar */
#define KviOption_boolShowCompactModeChanges 30                                /* ircoutput */
#define KviOption_boolIgnoreCtcpDcc 31                                         /* ctcp */
#define KviOption_boolAutoAcceptDccChat 32                                     /* dcc::chat */
#define KviOption_boolAutoAcceptDccSend 33                                     /* dcc::send */
#define KviOption_boolSendZeroAckInDccRecv 34                                  /* dcc::send */
#define KviOption_boolUseFastDccSend 35                                        /* dcc::send */
#define KviOption_boolUseCompositingForTransparency 36                         /* interface::look::global */
#define KviOption_boolUseWindowListIrcContextIndicator 37                      /* interface::features::global */
#define KviOption_boolUseGlobalApplicationFont 38                              /* interface::look::global */
#define KviOption_boolAlwaysQuitKVIrcClosingLastConsole 39                     /* internal */
#define KviOption_boolAlwaysDisconnectClosingConnectedConsole 40               /* internal */
#define KviOption_boolFlashWindowOnHighlightedMessages 41                      /* highlighting */
#define KviOption_boolPopupNotifierOnHighlightedMessages 42                    /* highlighting */
#define KviOption_boolFlashWindowOnNotifyOnLine 43                             /* notify */
#define KviOption_boolIrcViewTimestampUTC 44                                   /* interface::features::components::ircview */
#define KviOption_boolUseGlobalPseudoTransparency 45                           /* interface::look::global */
#define KviOption_boolUseNotifyList 46                                         /* ircengine::notifylist */
#define KviOption_boolNotifyListChangesToActiveWindow 47                       /* ircengine::notifylist */
#define KviOption_boolAutoResumeDccSendWhenAutoAccepted 48                     /* dcc::send */
#define KviOption_boolLimitOutgoingTraffic 49                                  /* transport */
#define KviOption_boolSendUnknownCommandsAsRaw 50                              /* ircengine::uparser */
#define KviOption_boolUserListViewDrawGrid 51                                  /* look & feel::interface look::userlist */
#define KviOption_boolShowChannelSyncTime 52                                   /* channel */
#define KviOption_boolRejoinChannelOnKick 53                                   /* channel */
#define KviOption_boolKeepChannelOpenOnKick 54                                 /* channel */
#define KviOption_boolAutoCloseDccSendOnSuccess 55                             /* dcc::send */
#define KviOption_boolNotifyDccSendSuccessInConsole 56                         /* dcc::send */
#define KviOption_boolCreateMinimizedDccSend 57                                /* dcc::send */
#define KviOption_boolCreateMinimizedDccChat 58                                /* dcc::chat */
#define KviOption_boolAutoAcceptIncomingAvatars 59                             /* irc::ctcp::avatar */
#define KviOption_boolUseNickCompletionPostfixForFirstWordOnly 60
#define KviOption_boolUseWindowListIcons 61                                    /* interface::features */
#define KviOption_boolCreateMinimizedDccSendWhenAutoAccepted 62                /* dcc::send */
#define KviOption_boolCreateMinimizedDccChatWhenAutoAccepted 63                /* dcc::chat */
#define KviOption_boolDccGuessIpFromServerWhenLocalIsUnroutable 64             /* dcc */
#define KviOption_boolColorNicksWithBackground 65                              /* interface::features::components::ircview */
#define KviOption_boolAutoLogQueries 66                                        /* ircengine::logging */
#define KviOption_boolAutoLogChannels 67                                       /* ircendine::logging */
#define KviOption_boolAutoLogDccChat 68                                        /* ircengine::logging */
#define KviOption_boolAutoLogConsole 69                                        /* ircengine::logging */
#define KviOption_boolSilentAntiSpam 70                                        /* ircengine::antispam */
#define KviOption_boolUseIncomingDccMediaTypeSavePath 71                       /* dcc::send */
#define KviOption_boolNotifyListSendUserhostForOnlineUsers 72                  /* ircengine::notifylist */
#define KviOption_boolUseIntelligentNotifyListManager 73                       /* ircengine::notifylist */
#define KviOption_boolUseWatchListIfAvailable 74                               /* ircengine::notifylist */
#define KviOption_boolUseTreeWindowList 75                                     /* interface::features::global */
#define KviOption_boolBindIrcIPv4ConnectionsToSpecifiedAddress 76              /* connection::transport */
#define KviOption_boolBindIrcIPv6ConnectionsToSpecifiedAddress 77              /* connection::transport */
#define KviOption_boolUseAntiSpamOnNotice 78                                   /* ircengine::antispam */
#define KviOption_boolSetLastAvatarAsDefaultForRegisteredUsers 79              /* irc::ctcp::avatar */
#define KviOption_boolCantAcceptIncomingDccConnections 80                      /* dcc::general */
#define KviOption_boolIgnoreCtcpAvatar 81                                      /* irc::ctcp::floodprotection */
#define KviOption_boolCtcpRepliesToActiveWindow 82                             /* ctcp */
#define KviOption_boolAutoAcceptDccCanvas 83
#define KviOption_boolNotifyDccSendSuccessInNotifier 84                        /* dcc::send */
#define KviOption_boolKillBrokenTimers 85                                      /* ircengine::uparser */
#define KviOption_boolDccVoiceVolumeSliderControlsPCM 86                       /* dcc::voice */
#define KviOption_boolDccVoiceForceHalfDuplex 87                               /* dcc::voice */
#define KviOption_boolAutoAcceptDccVoice 88                                    /* dcc::voice */
#define KviOption_boolCreateMinimizedDccVoice 89                               /* dcc::voice */
#define KviOption_boolCreateMinimizedDccVoiceWhenAutoAccepted 90               /* dcc::voice */
#define KviOption_boolUseIdentService 91                                       /* connection::ident */
#define KviOption_boolShowTipAtStartup 92                                      /* internal */
#define KviOption_boolIdentdEnableIPv6 93                                      /* connection::ident */
#define KviOption_boolIdentdIPv6ContainsIPv4 94                                /* connection::ident */
#define KviOption_boolScaleAvatars 95                                          /* ctcp::avatar */
#define KviOption_boolColorNicks 96                                            /* interface::features::components::ircview */
#define KviOption_boolBoldedNicks 97                                           /* interface::features::components::ircview */
#define KviOption_boolDccListenOnSpecifiedInterfaceByDefault 98                /* dcc */
#define KviOption_boolChangeNickAway 99                                        /* ircengine::away */
#define KviOption_boolAutoGeneratedAwayNick 100                                /* ircengine::away */
#define KviOption_boolExitAwayOnInput 101                                      /* ircengine::away */
#define KviOption_boolAlwaysHighlightNick 102                                  /* ircengine::outputcontrol::highlighting */
#define KviOption_boolShowChannelsJoinOnIrc 103                                /* internal */
//#define KviOption_boolShowChannelsJoinDialogAsToplevel 104                     /* FIXME: internal ??? */ //UNUSED
#define KviOption_boolUserDefinedPortRange 105                                 /* dcc */
#define KviOption_boolCreateQueryOnPrivmsg 106                                 /* query */
#define KviOption_boolCreateQueryOnNotice 107                                  /* query */
#define KviOption_boolCreateIncomingQueriesAsMinimized 108                     /* query */
#define KviOption_boolAutoJoinOnInvite 109                                     /* channel */
#define KviOption_boolShowServersConnectDialogOnStart 110                      /* connection::ircservers */
//#define KviOption_boolShowServersConnectDialogAsToplevel 111                   /* FIXME: internal ?? */  //UNUSED
#define KviOption_boolAcceptBrokenFileNameDccResumeRequests 112                /* dcc::send */
#define KviOption_boolAutoReconnectOnUnexpectedDisconnect 113                  /* connection */
#define KviOption_boolRejoinChannelsAfterReconnect 114                         /* connection */
#define KviOption_boolReopenQueriesAfterReconnect 115                          /* connection */
#define KviOption_boolEnableQueryTracing 116                                   /* query */
#define KviOption_boolOperatorMessagesToActiveWindow 117                       /* irc::output */
#define KviOption_boolInvitesToActiveWindow 118                                /* irc::output */
#define KviOption_boolShowChannelUserFlagInPrivmsgView 119                     /* ircengine::outputcontrol::privmsg */
#define KviOption_boolDccSendFakeAddressByDefault 120                          /* dcc::general */
#define KviOption_boolUseWindowListActivityMeter 121                           /* irc::output */
#define KviOption_boolCloseServerWidgetAfterConnect 122                        /* IMPLEMENTATION NEEDED !!! */
#define KviOption_boolPrioritizeLastActionTime 123
#define KviOption_boolShowUserChannelIcons 124                                 /* look & feel::interface features::userlist */
#define KviOption_boolShowUserChannelState 125                                 /* look & feel::interface features::userlist */
#define KviOption_boolEnableIgnoreOnPrivMsg 126                                /* irc::ignore */
#define KviOption_boolVerboseIgnore 127                                        /* irc::ignore */
#define KviOption_boolEnableIgnoreOnNotice 128                                 /* irc::ignore */
#define KviOption_boolNotifyFailedDccHandshakes 129
#define KviOption_boolDrawEmoticons 130                                        /* look::interface::features::output */
#define KviOption_boolIgnoreCtcpPage 131                                       /* irc::ctcp::floodprotection */
#define KviOption_boolShowDialogOnCtcpPage 132
#define KviOption_boolBashLikeNickCompletion 133
#define KviOption_boolUseSSLCertificate 134                                    /* ssl */
#define KviOption_boolUseSSLPrivateKey 135                                     /* ssl */
#define KviOption_boolWarnAboutPastingMultipleLines 136                        /* internal */
#define KviOption_boolKeepChannelsOpenOnUnexpectedDisconnect 137               /* connection */
#define KviOption_boolKeepQueriesOpenOnUnexpectedDisconnect 138                /* connection */
#define KviOption_boolUseLagMeterEngine 139                                    /* irc::lag */
#define KviOption_boolShowLagOnContextDisplay 140                              /* irc::lag */
#define KviOption_boolDisableBanListRequestOnJoin 141                          /* channel */
#define KviOption_boolDisableBanExceptionListRequestOnJoin 142                 /* channel */
#define KviOption_boolDisableInviteListRequestOnJoin 143                       /* channel */
#define KviOption_boolDisableWhoRequestOnJoin 144                              /* channel */
#define KviOption_boolShowDockExtension 145                                    /* internal ? */
#define KviOption_boolDccAddressByIrcDns 146                                   /* irc::general */
#define KviOption_boolHighlightOnlyNormalMsg 147                               /* irc::output::highlight */
#define KviOption_boolHighlightOnlyNormalMsgQueryToo 148                       /* irc::output::highlight */
#define KviOption_boolHighlightOnlyNormalMsgHighlightInChanToo 149             /* irc::output::highlight */
#define KviOption_boolHighlightOnlyAtCostumHighlightLevel 150                  /* irc::output::highlight */
#define KviOption_boolDccBrokenBouncerHack 151                                 /* dcc */
#define KviOption_boolExternalMessagesToActiveWindow 152                       /* irc::output */
#define KviOption_boolIgnoreChannelAvatarRequestsWhenNoAvatarSet 153           /* ctcp */
#define KviOption_boolFlashQueryWindowOnNewMessages 154                        /* query */
#define KviOption_boolConfirmCloseWhenThereAreConnections 155                  /* interface features */
#define KviOption_boolUseWindowsDesktopForTransparency 156                     /* interface::features::global */
#define KviOption_boolCtcpRequestsToActiveWindow 157                           /* irc::output */
#define KviOption_boolServerRepliesToActiveWindow 158                          /* irc::output */
#define KviOption_boolServerNoticesToActiveWindow 159                          /* irc::output */
#define KviOption_boolServicesNoticesToActiveWindow 160                        /* irc::output */
#define KviOption_boolSortWindowListItemsByName 161                            /* interface::features::global */
#define KviOption_boolShowWindowListToolTips 162                               /* interface::features::global */
#define KviOption_boolShowUserListViewToolTips 163                             /* interface::features::global */
#define KviOption_boolWhoisRepliesToActiveWindow 164                           /* irc::output */
#define KviOption_boolPopupNotifierOnNotifyOnLine 165                          /* notify */
#define KviOption_boolEnableNotifier 166                                       /* notifier */
#define KviOption_boolDccSendForceIdleStep 167                                 /* dcc */
#define KviOption_boolStatusBarVisible 168                                     /* internal */
#define KviOption_boolTrackLastReadTextViewLine 169                            /* ircview::features */
#define KviOption_boolKeepChannelOpenOnPart 170                                /* channel */
#define KviOption_boolStripMircColorsInUserMessages 171                        /* irc::output */
#define KviOption_boolDoNotSuggestRestoreDefaultScript 172                     /* internal */
#define KviOption_boolLimitDccSendSpeed 173                                    /* dcc::file transfers */
#define KviOption_boolLimitDccRecvSpeed 174                                    /* dcc::file transfers */
#define KviOption_boolIgnoreModuleVersions 175
#define KviOption_boolEnableInputHistory 176                                   /* interface::input */
#define KviOption_boolUseSpecialColorForTimestamp 177                          /* interface::output */
#define KviOption_boolEnableAwayListUpdates 178                                /* channel */
#define KviOption_boolShowAvatarsInUserlist 179                                /* interface::userlist */
#define KviOption_boolShowUserListStatisticLabel 180                           /* interface::userlist */
#define KviOption_boolShowIconsInPopupMenus 181                                /* themes::general */
#define KviOption_boolScriptErrorsToDebugWindow 182                            /* parser */
#define KviOption_boolShowMinimizedDebugWindow 183                             /* parser */
#define KviOption_boolShowExtendedInfoInQueryLabel 184                         /* query */
#define KviOption_boolUseUserListColorsAsNickColors 185                        /* irc::usermessages */
#define KviOption_boolGzipLogs 186                                             /* ircengine::logging */
#define KviOption_boolUseFullWordHighlighting 187                              /* highlight */
#define KviOption_boolNotifierFlashing 188                                     /* interface::notifier */
#define KviOption_boolCommandlineInUserFriendlyModeByDefault 189               /* interface::input */
#define KviOption_boolEnableVisualEffects 190                                  /* interface */
#define KviOption_boolDCCFileTransferReplaceOutgoingSpacesWithUnderscores 191  /* DCC something */
#define KviOption_boolDoNotUpscaleAvatars 192
#define KviOption_boolUseDifferentColorForOwnNick 193                          /* interface */
#define KviOption_boolCloseInTray 194                                          /* interface */
#define KviOption_boolStartupMinimized 195                                     /* interface */
#define KviOption_boolSortReceivedByDccFilesByNicks 196                        /* dcc */
#define KviOption_boolLogChannelHistory 197                                    /* channel */
#define KviOption_boolClassicWindowListSetMaximumButtonWidth 198
#define KviOption_boolKeepChannelsOpenOnDisconnect 199                         /* connection */
#define KviOption_boolKeepQueriesOpenOnDisconnect 200                          /* connection */
#define KviOption_boolHideWindowToolButtons 201                                /* interface */
#define KviOption_boolHideInputToolButtons 202                                 /* interface */
#define KviOption_boolStripControlCodesInLogs 203                              /* log */
#define KviOption_boolEnableTrayIconFlashing 204
#define KviOption_boolUseFlatClassicWindowListButtons 205
#define KviOption_boolStripMsgTypeInLogs 206                                   /* log */
#define KviOption_boolUseLevelBasedTrayNotification 207
#define KviOption_boolMuteAllSounds 208
#define KviOption_boolCreateMinimizedChannels 209
#define KviOption_boolShowNetworkNameForConsoleWindowListEntry 210
#define KviOption_boolDrawGenderIcons 211
#define KviOption_boolPrependGenderInfoToRealname 212
#define KviOption_boolUseIdentServiceOnlyOnConnect 213                         /* connection::ident */
#define KviOption_boolUseSystemUrlHandlers 214                                 /* Windoze only*/
#define KviOption_boolScaleAvatarsOnLoad 215
#define KviOption_boolNotifierFading 216                                       /* interface::notifier */
#define KviOption_boolPickRandomIpAddressForRoundRobinServers 217
#define KviOption_boolPrependAvatarInfoToRealname 218
#define KviOption_boolEnableAnimatedAvatars 219
#define KviOption_boolEnableAnimatedSmiles 220
#define KviOption_boolPlaceNickWithNonAlphaCharsAtEnd 221
#define KviOption_boolPasteLastLogOnChannelJoin 222                            /* channel */
#define KviOption_boolPasteLastLogOnQueryJoin 223                              /* query */
#define KviOption_boolUseSpecifiedSmartColorForOwnNick 224                     /* interface::ircview */
#define KviOption_boolZshLikeNickCompletion 225
#define KviOption_boolAutoAcceptDccVideo 226                                   /* dcc::video */
#define KviOption_boolCreateMinimizedDccVideo 227                              /* dcc::video */
#define KviOption_boolCreateMinimizedDccVideoWhenAutoAccepted 228              /* dcc::video */
#define KviOption_boolShowTaskBarButton 229                                    /* windows only, used in KviApplication::createFrame only */
#define KviOption_boolFlashDccChatWindowOnNewMessages 230                      /* dcc:chat */
#define KviOption_boolPopupNotifierOnNewDccChatMessages 231                    /* dcc:chat */
#define KviOption_boolUseAwayMessage 232                                       /* away */
#define KviOption_boolDisableQuietBanListRequestOnJoin 233                     /* channel */
#define KviOption_boolUseWindowListCloseButton 234                             /* irc::output */
#define KviOption_boolFrameIsMaximized 235                                     /* internal */
#define KviOption_boolPrependNickColorInfoToRealname 236
#define KviOption_boolDontShowNotifierIfActiveWindowIsFullScreen 237           /* notifier */
#define KviOption_boolWheelScrollsWindowsList 238
#define KviOption_boolAcceptMismatchedPortDccResumeRequests 239                /* dcc::send */
#define KviOption_boolEnableUrlLinkToolTip 240                                 /* ircview output */
#define KviOption_boolEnableHostLinkToolTip 241                                /* ircview output */
#define KviOption_boolEnableServerLinkToolTip 242                              /* ircview output */
#define KviOption_boolEnableModeLinkToolTip 243                                /* ircview output */
#define KviOption_boolEnableNickLinkToolTip 244                                /* ircview output */
#define KviOption_boolEnableChannelLinkToolTip 245                             /* ircview output */
#define KviOption_boolEnableEscapeLinkToolTip 246                              /* ircview output */
#define KviOption_boolUseDBusNotifier 247                                      /* notifier */
#define KviOption_boolUseKDENotifier 248                                       /* notifier */
#define KviOption_boolCaseSensitiveHighlighting 249                            /* highlight */
#define KviOption_boolMinimizeInTray 250                                       /* interface */
#define KviOption_boolDisplayNotifierOnPrimaryScreen 251                       /* notifier */
#define KviOption_boolShowDialogOnChannelCtcpPage 252
#define KviOption_boolPopupNotifierOnNewNotices 253                            /* query */
#define KviOption_boolUserListViewUseAwayColor 254                             /* userlist */
#define KviOption_boolShowUserFlagForChannelsInWindowList 255
#define KviOption_boolEnableCustomCursorWidth 256                              /* interface */
#define KviOption_boolShowFavoriteServersOnly 257                              /* connection::ircservers */
#define KviOption_boolRequireControlToCopy 258                                 /* interface::ircview */
#define KviOption_boolSend64BitAckInDccRecv 259                                /* dcc::send */
#define KviOption_boolShowTreeWindowListHandle 260
#define KviOption_boolMenuBarVisible 261
#define KviOption_boolWarnAboutHidingMenuBar 262
#define KviOption_boolWhoRepliesToActiveWindow 263                             /* irc::output */
#define KviOption_boolDropConnectionOnSaslFailure 264                          /* connection::advanced */

// NOTICE: REUSE EQUIVALENT UNUSED BOOL_OPTION in KviOptions.cpp ENTRIES BEFORE ADDING NEW ENTRIES ABOVE

#define KVI_NUM_BOOL_OPTIONS 265

#define KVI_STRING_OPTIONS_PREFIX "string"
#define KVI_STRING_OPTIONS_PREFIX_LEN 6

#define KviOption_stringNickname1 0                                            /* identity */
#define KviOption_stringNickname2 1                                            /* identity */
#define KviOption_stringNickname3 2                                            /* identity */
#define KviOption_stringNickname4 3                                            /* identity */
#define KviOption_stringUsername 4                                             /* identity */
#define KviOption_stringRealname 5                                             /* identity */
#define KviOption_stringLocalHostIp 6                                          /* ?????? This is a fallback one */
#define KviOption_stringPartMessage 7                                          /* ircengine::defaultmessages */
#define KviOption_stringQuitMessage 8                                          /* ircengine::defaultmessages */
#define KviOption_stringUrlHttpCommand 9                                       /* urlhandlers */
#define KviOption_stringExtendedPrivmsgPrefix 10                               /* ircengine::outputcontrol::privmsg */
#define KviOption_stringExtendedPrivmsgPostfix 11                              /* ircengine::outputcontrol::privmsg */
#define KviOption_stringCtcpVersionPostfix 12                                  /* ircengine::ctcp::replies */
#define KviOption_stringCtcpUserInfoOther 13                                   /* identity */
#define KviOption_stringCtcpSourcePostfix 14                                   /* ircengine::ctcp::replies */
#define KviOption_stringUrlFtpCommand 15                                       /* urlhandlers */
#define KviOption_stringUrlHttpsCommand 16                                     /* urlhandlers */
#define KviOption_stringUrlMailtoCommand 17                                    /* urlhandlers */
#define KviOption_stringPreferredMediaPlayer 18                                /* still missing in options ! */
#define KviOption_stringUrlFileCommand 19                                      /* urlhandlers */
#define KviOption_stringNickCompletionPostfix 20
#define KviOption_stringUrlUnknownCommand 21                                   /* urlhandlers */
#define KviOption_stringIPv4ConnectionBindAddress 22                           /* connection::transport */
#define KviOption_stringIPv6ConnectionBindAddress 23                           /* connection::transport */
#define KviOption_stringDccVoiceSoundDevice 24                                 /* dcc::voice */
#define KviOption_stringDccVoiceMixerDevice 25                                 /* dcc::voice */
#define KviOption_stringAwayMessage 26                                         /* irc::away */
#define KviOption_stringIdentdUser 27                                          /* connection::ident */
#define KviOption_stringDccListenDefaultInterface 28                           /* dcc::general */
#define KviOption_stringCustomAwayNick 29                                      /* ircengine::away */
#define KviOption_stringDefaultDccFakeAddress 30                               /* dcc::general */
#define KviOption_stringCtcpPageReply 31                                       /* ircengine::ctcp::replies */
#define KviOption_stringSSLCertificatePath 32                                  /* ssl */
#define KviOption_stringSSLCertificatePass 33                                  /* ssl */
#define KviOption_stringSSLPrivateKeyPath 34                                   /* ssl */
#define KviOption_stringSSLPrivateKeyPass 35                                   /* ssl */
#define KviOption_stringSoundSystem 36                                         /* sound */
#define KviOption_stringDefaultUserMode 37
#define KviOption_stringLastImageDialogPath 38                                 /* internal */
#define KviOption_stringIconThemeSubdir 39
#define KviOption_stringIncomingPath 40                                        /* folders */
#define KviOption_stringMyAvatar 41
#define KviOption_stringDefaultTextEncoding 42                                 /* text encoding */ // empty string means guess from locale
#define KviOption_stringWinampTextEncoding 43
#define KviOption_stringMp3TagsEncoding 44
#define KviOption_stringCtcpUserInfoAge 45                                     /* identity */
#define KviOption_stringCtcpUserInfoGender 46                                  /* identity */
#define KviOption_stringCtcpUserInfoLocation 47                                /* identity */
#define KviOption_stringCtcpUserInfoLanguages 48                               /* identity */
#define KviOption_stringWordSplitters 49                                       /* highlight */
#define KviOption_stringOnNewQueryOpenedSound 50                               /* sounds */
#define KviOption_stringOnHighlightedMessageSound 51                           /* sounds */
#define KviOption_stringOnMeKickedSound 52                                     /* sounds */
#define KviOption_stringOnQueryMessageSound 53                                 /* sounds */
#define KviOption_stringIrcViewTimestampFormat 54
#define KviOption_stringPreferredTorrentClient 55                              /* still missing in options ! */
#define KviOption_stringDefaultSrvEncoding 56                                  /* server encoding */ // empty string means guess from locale
#define KviOption_stringLogsPath 57                                            /* logfolder */
#define KviOption_stringLogsDynamicPath 58                                     /* logfolder */
#define KviOption_stringLogsExportPath 59                                      /* logview module log export */

#define KVI_NUM_STRING_OPTIONS 60

#define KVI_STRINGLIST_OPTIONS_PREFIX "stringlist"
#define KVI_STRINGLIST_OPTIONS_PREFIX_LEN 10

#define KviOption_stringlistHighlightWords 0                                   /* ircengine::outputcontrol::highlighting */
#define KviOption_stringlistSpamWords 1                                        /* ircengine::outputcontrol::antispam */
#define KviOption_stringlistRecentChannels 2                                   /* internal */
#define KviOption_stringlistRecentServers 3                                    /* internal */
#define KviOption_stringlistRecentNicknames 4                                  /* internal */
#define KviOption_stringlistModuleExtensionToolbars 5                          /* internal */
#define KviOption_stringlistImageSearchPaths 6                                 /* ??? */
#define KviOption_stringlistRecentIrcUrls 7                                    /* internal */
#define KviOption_stringlistSpellCheckerDictionaries 8                         /* spellchecker */

#define KVI_NUM_STRINGLIST_OPTIONS 9

#define KVI_COLOR_OPTIONS_PREFIX "color"
#define KVI_COLOR_OPTIONS_PREFIX_LEN 5

#define KviOption_colorWindowListNormalText 0                                  /* look & feel::interface look::WindowList::classic WindowList */
#define KviOption_colorWindowListMinimizedText 1                               /* look & feel::interface look::WindowList::classic WindowList */
#define KviOption_colorWindowListHighlight1Text 2                              /* look & feel::interface look::WindowList::classic WindowList */
#define KviOption_colorWindowListHighlight2Text 3                              /* look & feel::interface look::WindowList::classic WindowList */
#define KviOption_colorWindowListProgressBar 4                                 /* look & feel::interface look::WindowList::classic WindowList */
#define KviOption_colorUserListViewAwayForeground 5                            /* look & feel::interface look::userlist */
#define KviOption_colorUserListViewOwnForeground 6                             /* look & feel::interface look::userlist */
#define KviOption_colorUserListViewIrcOpForeground 7                           /* look & feel::interface look::userlist */
#define KviOption_colorNotifierBackground 8                                    /* look & feel::interface look::notifier */
#define KviOption_colorNotifierForeground 9                                    /* look & feel::interface look::notifier */
#define KviOption_colorNotifierTitleForeground 10                              /* look & feel::interface look::notifier */
#define KviOption_colorIrcViewBackground 11                                    /* look & feel::interface look::ircview */
#define KviOption_colorInputBackground 12                                      /* look & feel::interface look::input */
#define KviOption_colorInputSelectionBackground 13                             /* look & feel::interface look::input */
#define KviOption_colorInputSelectionForeground 14                             /* look & feel::interface look::input */
#define KviOption_colorInputCursor 15                                          /* look & feel::interface look::input */
#define KviOption_colorInputControl 16                                         /* look & feel::interface look::input */
#define KviOption_colorInputForeground 17                                      /* look & feel::interface look::input */
#define KviOption_colorUserListViewBackground 18                               /* look & feel::interface look::userlist */
#define KviOption_colorUserListViewOpForeground 19                             /* look & feel::interface look::userlist */
#define KviOption_colorUserListViewVoiceForeground 20                          /* look & feel::interface look::userlist */
#define KviOption_colorUserListViewNormalForeground 21                         /* look & feel::interface look::userlist */
#define KviOption_colorUserListViewSelectionBackground 22                      /* look & feel::interface look::userlist */
#define KviOption_colorUserListViewSelectionForeground 23                      /* look & feel::interface look::userlist */
#define KviOption_colorLabelBackground 24                                      /* look & feel::interface look::topic label */
#define KviOption_colorLabelForeground 25                                      /* look & feel::interface look::topic label */
#define KviOption_colorGlobalTransparencyFade 26                               /* look & feel::interface look */
#define KviOption_colorIrcToolBarAppletBackground 27                           /* theme::tool bar applets */
#define KviOption_colorIrcToolBarAppletBorder 28                               /* theme::tool bar applets */
#define KviOption_colorIrcToolBarAppletFont 29                                 /* theme::tool bar applets */
#define KviOption_colorTreeWindowListBackground 30                             /* look & feel::interface look::WindowList::tree WindowList */
#define KviOption_colorTreeWindowListActiveBackground 31                       /* look & feel::interface look::WindowList::tree WindowList */
#define KviOption_colorTreeWindowListForeground 32                             /* look & feel::interface look::WindowList::tree WindowList */
#define KviOption_colorTreeWindowListActiveForeground 33                       /* look & feel::interface look::WindowList::tree WindowList */
#define KviOption_colorTreeWindowListHighlight1Foreground 34                   /* look & feel::interface look::WindowList::tree WindowList */
#define KviOption_colorTreeWindowListHighlight2Foreground 35                   /* look & feel::interface look::WindowList::tree WindowList */
#define KviOption_colorTreeWindowListProgress 36                               /* look & feel::interface look::WindowList::tree WindowList */
#define KviOption_colorUserListViewHalfOpForeground 37                         /* look & feel::interface look::userlist */
#define KviOption_colorWindowListHighlight3Text 38                             /* look & feel::interface look::WindowList::classic WindowList */
#define KviOption_colorWindowListHighlight4Text 39                             /* look & feel::interface look::WindowList::classic WindowList */
#define KviOption_colorWindowListHighlight5Text 40                             /* look & feel::interface look::WindowList::classic WindowList */
#define KviOption_colorTreeWindowListHighlight3Foreground 41                   /* look & feel::interface look::WindowList::tree WindowList */
#define KviOption_colorTreeWindowListHighlight4Foreground 42                   /* look & feel::interface look::WindowList::tree WindowList */
#define KviOption_colorTreeWindowListHighlight5Foreground 43                   /* look & feel::interface look::WindowList::tree WindowList */
#define KviOption_colorUserListViewChanAdminForeground 44                      /* look & feel::interface look::userlist */
#define KviOption_colorUserListViewUserOpForeground 45                         /* look & feel::interface look::userlist */
#define KviOption_colorUserListViewGrid 46                                     /* look & feel::interface look::userlist */
#define KviOption_colorUserListViewChanOwnerForeground 47                      /* look & feel::interface look::userlist */
#define KviOption_colorIrcViewMarkLine 48                                      /* look & feel::interface look::ircvew */

#define KVI_NUM_COLOR_OPTIONS 49

#define KVI_PIXMAP_OPTIONS_PREFIX "pixmap"
#define KVI_PIXMAP_OPTIONS_PREFIX_LEN 6

#define KviOption_pixmapIrcViewBackground 0                                    /* interface::look::components::ircview */
#define KviOption_pixmapInputBackground 1                                      /* interface::look::components::input */
#define KviOption_pixmapUserListViewBackground 2                               /* look & feel::interface look::userlist */
#define KviOption_pixmapLabelBackground 3                                      /* look & feel::interface look::topic label */
#define KviOption_pixmapMyAvatar 4                                             /* identity */
#define KviOption_pixmapIrcToolBarAppletBackground 5                           /* theme::tool bar applets */
#define KviOption_pixmapTreeWindowListBackground 6                             /* look & feel::interface look::WindowList::tree WindowList */
#define KviOption_pixmapGlobalTransparencyBackground 7                         /* theme::transparency */
#define KviOption_pixmapNotifierBackground 8                                   /* interface::look::components::notifier */

#define KVI_NUM_PIXMAP_OPTIONS 9

#define KVI_UINT_OPTIONS_PREFIX "uint"
#define KVI_UINT_OPTIONS_PREFIX_LEN 4

#define KviOption_uintIrcSocketTimeout 0                                       /* connection::transport */
#define KviOption_uintModuleCleanupTimerInterval 1
#define KviOption_uintModuleCleanupTimeout 2
#define KviOption_uintMaxCtcpRequests 3                                        /* ircengine::ctcp */
#define KviOption_uintCtcpFloodCheckInterval 4                                 /* ircengine::ctcp */
#define KviOption_uintTotalConnectionTime 5
#define KviOption_uintDccSocketTimeout 6                                       /* dcc::general */
#define KviOption_uintDccSendIdleStepInMSec 7                                  /* dcc::send */
#define KviOption_uintDccSendPacketSize 8                                      /* dcc::send */
#define KviOption_uintGlobalTransparencyParentFadeFactor 9                     /* interface::look::general */
#define KviOption_uintGlobalTransparencyChildFadeFactor 10                     /* interface::look::general */
#define KviOption_uintNotifyListCheckTimeInSecs 11                             /* notify */
#define KviOption_uintSocketQueueFlushTimeout 12                               /* connection::transport */
#define KviOption_uintOutgoingTrafficLimitUSeconds 13                          /* connection::transport */
#define KviOption_uintNotifyListIsOnDelayTimeInSecs 14                         /* notify */
#define KviOption_uintNotifyListUserhostDelayTimeInSecs 15                     /* notify */
#define KviOption_uintTreeWindowListMinimumWidth 16                            /* ?? interface::general ?? */
#define KviOption_uintAvatarOfferTimeoutInSecs 17                              /* irc::ctcp::avatar */
#define KviOption_uintIrcViewMaxBufferSize 18                                  /* interface::features::components::ircview */
#define KviOption_uintIrcViewToolTipTimeoutInMsec 19                           /* interface::features::components::ircview */
#define KviOption_uintIrcViewToolTipHideTimeoutInMsec 20                       /* interface::features::components::ircview */
#define KviOption_uintDccVoicePreBufferSize 21                                 /* dcc::voice */
#define KviOption_uintIdentdPort 22                                            /* connection::ident */
#define KviOption_uintAvatarScaleWidth 23                                      /* ctcp::avatar */
#define KviOption_uintAvatarScaleHeight 24                                     /* ctcp::avatar */
#define KviOption_uintDccMinPort 25                                            /* dcc::general */
#define KviOption_uintDccMaxPort 26                                            /* dcc::general */
#define KviOption_uintSoundOfferTimeoutInSecs 27
#define KviOption_uintMaximumRequestedAvatarSize 28                            /* ctcp::avatar */
#define KviOption_uintGlobalWindowOpacityPercent 29
#define KviOption_uintPasteDelay 30
#define KviOption_uintMaxDccSendTransfers 31                                   /* dcc transfers */ // 0 = unlimited!
#define KviOption_uintMaxDccSlots 32                                           /* dcc transfers */ // 0 = unlimited slots = windows
#define KviOption_uintTileMethod 33                                            /* internal ? */
#define KviOption_uintLagMeterHeartbeat 34
#define KviOption_uintLagAlarmTime 35
#define KviOption_uintWindowListButtonMinWidth 36                              /* interface::look::WindowList::classic WindowList */
#define KviOption_uintOutputVerbosityLevel 37                                  // follows the verbosity constants defined below
#define KviOption_uintIrcContextAppletWidth 38                                 /* internal */
#define KviOption_uintMinHighlightLevel 39                                     /* look::interface::features::output */
#define KviOption_uintUserListViewGridType 40                                  /* look&feel::ircview */
#define KviOption_uintWindowListMaximumRows 41                                 /* interface::look::WindowList::classic WindowList */
#define KviOption_uintMaxAutoReconnectAttempts 42                              /* connection */// 0 = unlimited attempts
#define KviOption_uintAutoReconnectDelay 43                                    /* connection */
#define KviOption_uintMaxDccSendSpeed 44                                       /* dcc::file transfers */
#define KviOption_uintMaxDccRecvSpeed 45                                       /* dcc::file transfers */
#define KviOption_uintTimeStampForeground 46
#define KviOption_uintTimeStampBackground 47
#define KviOption_uintUserExperienceLevel 48
#define KviOption_uintClassicWindowListMaximumButtonWidth 49
#define KviOption_uintDefaultBanType 50
#define KviOption_uintIrcViewPixmapAlign 51
#define KviOption_uintUserListPixmapAlign 52
#define KviOption_uintToolBarAppletPixmapAlign 53
#define KviOption_uintTreeWindowListPixmapAlign 54
#define KviOption_uintInputPixmapAlign 55
#define KviOption_uintAutoFlushLogs 56
#define KviOption_uintMinTrayLowLevelMessage 57
#define KviOption_uintMinTrayHighLevelMessage 58
#define KviOption_uintIdentdOutputMode 59
#define KviOption_uintScaleAvatarsOnLoadHeight 60
#define KviOption_uintScaleAvatarsOnLoadWidth 61
#define KviOption_uintNotifierActiveTransparency 62
#define KviOption_uintNotifierInactiveTransparency 63
#define KviOption_uintIrcViewMarkerStyle 64
#define KviOption_uintIrcViewMarkerSize 65
#define KviOption_uintUrlMouseClickNum 66
#define KviOption_uintNotifierAutoHideTime 67
#define KviOption_uintLinesToPasteOnChannelJoin 68                            /* channel */
#define KviOption_uintDaysIntervalToPasteOnChannelJoin 69                     /* channel */
#define KviOption_uintLinesToPasteOnQueryJoin 70                              /* query */
#define KviOption_uintDaysIntervalToPasteOnQueryJoin 71                       /* query */
#define KviOption_uintSpacesToExpandTabulationInput 72                        /* number of spaces used in tabulation expansion in the inputbar */
#define KviOption_uintUserIrcViewOwnForeground 73                             /* look & feel::ircview */
#define KviOption_uintUserIrcViewOwnBackground 74                             /* look & feel::ircview */
#define KviOption_uintNotifierPixmapAlign 75
#define KviOption_uintOutputDatetimeFormat 76                                 /* irc::verbosity */
#define KviOption_uintOnJoinRequestsDelay 77
#define KviOption_uintToolBarIconSize 78
#define KviOption_uintToolBarButtonStyle 79
#define KviOption_uintMaximumBlowFishKeySize 80
#define KviOption_uintCustomCursorWidth 81                                    /* Interface */
#define KviOption_uintUserListMinimumWidth 82

#define KVI_NUM_UINT_OPTIONS 83

namespace KviIdentdOutputMode
{
	enum Mode
	{
		Quiet = 0,
		ToConsole = 1,
		ToActiveWindow = 2
	};
}

#define KVI_FONT_OPTIONS_PREFIX "font"
#define KVI_FONT_OPTIONS_PREFIX_LEN 4

#define KviOption_fontIrcView 0                                              /* interface::look::components::ircview */
#define KviOption_fontInput 1                                                /* interface::look::components::input */
#define KviOption_fontUserListView 2                                         /* look & feel::interface look::userlist */
#define KviOption_fontLabel 3                                                /* look & feel::interface look::topic label */
#define KviOption_fontApplication 4                                          /* interface::look::global */
#define KviOption_fontWindowList 5                                           /* interface::look::WindowList::classic WindowList */
#define KviOption_fontTreeWindowList 6                                       /* interface::look::WindowList::tree WindowList */
#define KviOption_fontNotifier 7                                             /* interface::look::components::notifier */
#define KviOption_fontNotifierTitle 8                                        /* interface::look::components::notifier */

#define KVI_NUM_FONT_OPTIONS 9

#define KVI_ICCOLOR_OPTIONS_PREFIX "iccolor"
#define KVI_ICCOLOR_OPTIONS_PREFIX_LEN 7

#define KVI_NUM_ICCOLOR_OPTIONS 8

// indices declared in kvi_out.h
#define KVI_MSGTYPE_OPTIONS_PREFIX "msgtype"
#define KVI_MSGTYPE_OPTIONS_PREFIX_LEN 7

#define KVI_NUM_MSGTYPE_OPTIONS 149

#define KVI_MIRCCOLOR_OPTIONS_PREFIX "mirccolor"
#define KVI_MIRCCOLOR_OPTIONS_PREFIX_LEN 9

#define KVI_NUM_MIRCCOLOR_OPTIONS (KVI_MIRCCOLOR_MAX+1)

// external declaration of the tables
extern KVIRC_API KviBoolOption g_boolOptionsTable[KVI_NUM_BOOL_OPTIONS];
extern KVIRC_API KviRectOption g_rectOptionsTable[KVI_NUM_RECT_OPTIONS];
extern KVIRC_API KviStringOption g_stringOptionsTable[KVI_NUM_STRING_OPTIONS];
extern KVIRC_API KviColorOption g_colorOptionsTable[KVI_NUM_COLOR_OPTIONS];
extern KVIRC_API KviPixmapOption g_pixmapOptionsTable[KVI_NUM_PIXMAP_OPTIONS];
//	extern KviIntOption        g_intOptionsTable[KVI_NUM_INT_OPTIONS];
extern KVIRC_API KviUIntOption g_uintOptionsTable[KVI_NUM_UINT_OPTIONS];
extern KVIRC_API KviFontOption g_fontOptionsTable[KVI_NUM_FONT_OPTIONS];
extern KVIRC_API KviMessageTypeSettingsOption g_msgtypeOptionsTable[KVI_NUM_MSGTYPE_OPTIONS];
extern KVIRC_API KviColorOption g_mirccolorOptionsTable[KVI_NUM_MIRCCOLOR_OPTIONS];
extern KVIRC_API KviColorOption g_iccolorOptionsTable[KVI_NUM_ICCOLOR_OPTIONS];
extern KVIRC_API KviStringListOption g_stringlistOptionsTable[KVI_NUM_STRINGLIST_OPTIONS];

// __this is how we access the options from outside
#define KVI_OPTION_BOOL(_idx) g_boolOptionsTable[_idx].option
#define KVI_OPTION_RECT(_idx) g_rectOptionsTable[_idx].option
#define KVI_OPTION_STRING(_idx) g_stringOptionsTable[_idx].option
#define KVI_OPTION_COLOR(_idx) g_colorOptionsTable[_idx].option
#define KVI_OPTION_PIXMAP(_idx) g_pixmapOptionsTable[_idx].option
#define KVI_OPTION_UINT(_idx) g_uintOptionsTable[_idx].option
#define KVI_OPTION_FONT(_idx) g_fontOptionsTable[_idx].option
#define KVI_OPTION_MSGTYPE(_idx) g_msgtypeOptionsTable[_idx].option
#define KVI_OPTION_MIRCCOLOR(_idx) g_mirccolorOptionsTable[_idx].option
#define KVI_OPTION_STRINGLIST(_idx) g_stringlistOptionsTable[_idx].option
#define KVI_OPTION_ICCOLOR(_idx) g_iccolorOptionsTable[_idx].option

inline QColor getMircColor(unsigned int index)
{
	// Use inline function (instead of macro) to avoid evaluating index more than once.
	if (index <= KVI_MIRCCOLOR_MAX)
		return KVI_OPTION_MIRCCOLOR(index);
	if (index <= KVI_EXTCOLOR_MAX)
		return KviControlCodes::getExtendedColor(index);
	return QColor(); // invalid color (isValid returns false)
}

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

#define START_TABLE_BOLD_ROW "<tr><td style=\"background-color: rgb(48,48,48); font-weight: bold; color: rgb(255,255,255); padding-left: 5px; padding-right: 5px;\">"
#define END_TABLE_BOLD_ROW "</td></tr>"
#define START_TABLE_NORMAL_ROW "<tr><td>"

#define SET_ANTI_ALIASING(p) (p).setRenderHint(QPainter::TextAntialiasing);

#define KVI_RECENT_CHANNELS_SEPARATOR ":"

#if defined(_KVI_OPTIONS_CPP_) || defined(_WANT_OPTION_FLAGS_)
// flag definitions
// section of the config file (rightmost 8 bits)
#define KviOption_sectFlagNone 0
#define KviOption_sectFlagFrame 1
#define KviOption_sectFlagMdi 2
#define KviOption_sectFlagWindows 3
#define KviOption_sectFlagWindowList 4
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

//for file paths
#define KviOption_encodePath (1 << 25)
#endif

#endif //_KVI_OPTIONS_H_
