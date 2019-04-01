//=============================================================================
//
//   File : KviOptions.cpp
//   Creation date : Tue Jun 20 2000 11:42:00 by Szymon Stefanek
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

#define _KVI_OPTIONS_CPP_

#include "KviOptions.h"
#include "kvi_confignames.h"
#include "kvi_defaults.h"
#include "kvi_out.h"
#include "kvi_settings.h"
#include "KviApplication.h"
#include "KviConfigurationFile.h"
#include "KviControlCodes.h"
#include "KviFileUtils.h"
#include "KviIconManager.h"
#include "KviInternalCommand.h"
#include "KviLocale.h"
#include "KviMainWindow.h"
#include "KviStringConversion.h"
#include "KviTheme.h"
#include "KviWindow.h"

#include <QDir>
#include <QMessageBox>
#include <QStringList>

// KviApplication.cpp
extern KVIRC_API int g_iIdentDaemonRunningUsers;

#define RECT_OPTION(_txt, _val, _flags) KviRectOption(KVI_RECT_OPTIONS_PREFIX _txt, _val, _flags)

KviRectOption g_rectOptionsTable[KVI_NUM_RECT_OPTIONS] = {
	RECT_OPTION("FrameGeometry", QRect(0, 0, 10, 10), KviOption_sectFlagGeometry),
	RECT_OPTION("GeneralOptionsDialogGeometry", QRect(50, 50, 600, 450), KviOption_sectFlagGeometry),
	RECT_OPTION("RegisteredUsersDialogGeometry", QRect(50, 50, 600, 450), KviOption_sectFlagGeometry)
};

#define BOOL_OPTION(_txt, _val, _flags) KviBoolOption(KVI_BOOL_OPTIONS_PREFIX _txt, _val, _flags)

// NOTICE: REUSE EQUIVALENT UNUSED KviOption_bool in KviOptions.h ENTRIES BEFORE ADDING NEW ENTRIES BELOW
KviBoolOption g_boolOptionsTable[KVI_NUM_BOOL_OPTIONS] = {
	BOOL_OPTION("WindowsRememberProperties", true, KviOption_sectFlagWindows),
	BOOL_OPTION("IrcViewShowImages", false, KviOption_sectFlagIrcView | KviOption_resetUpdateGui | KviOption_groupTheme),
	BOOL_OPTION("IrcViewTimestamp", true, KviOption_sectFlagIrcView),
	BOOL_OPTION("IrcViewUrlHighlighting", true, KviOption_sectFlagIrcView | KviOption_groupTheme),
	BOOL_OPTION("IrcViewWrapMargin", true, KviOption_sectFlagIrcView | KviOption_resetUpdateGui | KviOption_groupTheme),
	BOOL_OPTION("InputHistoryCursorAtEnd", true, KviOption_sectFlagInput),
	BOOL_OPTION("AvoidParserWarnings", false, KviOption_sectFlagUserParser),
	BOOL_OPTION("UseProxyHost", false, KviOption_sectFlagConnection),
	BOOL_OPTION("IgnoreSpecialCharactersInNickCompletion", false, KviOption_sectFlagInput),
	BOOL_OPTION("ForceBrutalQuit", false, KviOption_sectFlagIrcSocket),
	BOOL_OPTION("ShowPingPong", true, KviOption_sectFlagConnection),
	BOOL_OPTION("PopupNotifierOnNewQueryMessages", true, KviOption_sectFlagFrame),
	BOOL_OPTION("ShowExtendedServerInfo", false, KviOption_sectFlagConnection),
	BOOL_OPTION("EchoNumericTopic", true, KviOption_sectFlagConnection),
	BOOL_OPTION("ShowOwnParts", true, KviOption_sectFlagConnection),
	BOOL_OPTION("UseAntiSpamOnPrivmsg", false, KviOption_sectFlagAntiSpam),
	BOOL_OPTION("UseExtendedPrivmsgView", false, KviOption_sectFlagIrcView | KviOption_groupTheme),
	BOOL_OPTION("ShowUserAndHostInPrivmsgView", false, KviOption_sectFlagIrcView | KviOption_groupTheme),
	BOOL_OPTION("UseWordHighlighting", true, KviOption_sectFlagIrcView), /* _ALL_ newbie users, with who i was taling asks me where can they switch on */
	BOOL_OPTION("CleanupUnusedModules", true, KviOption_sectFlagModules),
	BOOL_OPTION("IgnoreCtcpPing", false, KviOption_sectFlagCtcp),
	BOOL_OPTION("IgnoreCtcpVersion", false, KviOption_sectFlagCtcp),
	BOOL_OPTION("UseCtcpFloodProtection", true, KviOption_sectFlagCtcp),
	BOOL_OPTION("DisableBrokenEventHandlers", true, KviOption_sectFlagUserParser),
	BOOL_OPTION("IgnoreCtcpClientinfo", false, KviOption_sectFlagCtcp),
	BOOL_OPTION("IgnoreCtcpUserinfo", false, KviOption_sectFlagCtcp),
	BOOL_OPTION("IgnoreCtcpFinger", true, KviOption_sectFlagCtcp),
	BOOL_OPTION("IgnoreCtcpSource", false, KviOption_sectFlagCtcp),
	BOOL_OPTION("IgnoreCtcpTime", false, KviOption_sectFlagCtcp),
	BOOL_OPTION("RequestMissingAvatars", false, KviOption_sectFlagAvatar),
	BOOL_OPTION("ShowCompactModeChanges", true, KviOption_sectFlagConnection),
	BOOL_OPTION("IgnoreCtcpDcc", false, KviOption_sectFlagDcc),
	BOOL_OPTION("AutoAcceptDccChat", false, KviOption_sectFlagDcc),
	BOOL_OPTION("AutoAcceptDccSend", false, KviOption_sectFlagDcc),
	BOOL_OPTION("SendZeroAckInDccRecv", false, KviOption_sectFlagDcc),
	BOOL_OPTION("UseFastDccSend", true, KviOption_sectFlagDcc),
	BOOL_OPTION("UseCompositingForTransparency", false, KviOption_sectFlagGui | KviOption_resetUpdatePseudoTransparency | KviOption_groupTheme),
	BOOL_OPTION("UseWindowListIrcContextIndicator", false, KviOption_sectFlagWindowList | KviOption_resetUpdateGui | KviOption_groupTheme),
	BOOL_OPTION("UseGlobalApplicationFont", false, KviOption_sectFlagGui | KviOption_resetUpdateAppFont | KviOption_groupTheme),
	BOOL_OPTION("AlwaysQuitKVIrcClosingLastConsole", false, KviOption_sectFlagFrame),
	BOOL_OPTION("AlwaysDisconnectClosingConnectedConsole", false, KviOption_sectFlagFrame),
	BOOL_OPTION("FlashWindowOnHighlightedMessages", true, KviOption_sectFlagFrame),
	BOOL_OPTION("PopupNotifierOnHighlightedMessages", true, KviOption_sectFlagFrame),
	BOOL_OPTION("FlashWindowOnNotifyOnLine", true, KviOption_sectFlagConnection),
	BOOL_OPTION("IrcViewTimestampUTC", false, KviOption_sectFlagIrcView),
	BOOL_OPTION("UseGlobalPseudoTransparency", false, KviOption_sectFlagGui | KviOption_resetUpdatePseudoTransparency | KviOption_groupTheme),
	BOOL_OPTION("UseNotifyList", false, KviOption_sectFlagConnection | KviOption_resetRestartNotifyList),
	BOOL_OPTION("NotifyListChangesToActiveWindow", true, KviOption_sectFlagConnection),
	BOOL_OPTION("AutoResumeDccSendWhenAutoAccepted", false, KviOption_sectFlagDcc),
	BOOL_OPTION("LimitOutgoingTraffic", false, KviOption_sectFlagIrcSocket),
	BOOL_OPTION("SendUnknownCommandsAsRaw", true, KviOption_sectFlagUserParser),
	BOOL_OPTION("UserListViewDrawGrid", true, KviOption_sectFlagUserListView | KviOption_resetUpdateGui | KviOption_groupTheme),
	BOOL_OPTION("ShowChannelSyncTime", true, KviOption_sectFlagConnection),
	BOOL_OPTION("RejoinChannelOnKick", false, KviOption_sectFlagConnection),
	BOOL_OPTION("KeepChannelOpenOnKick", true, KviOption_sectFlagConnection),
	BOOL_OPTION("AutoCloseDccSendOnSuccess", false, KviOption_sectFlagDcc),
	BOOL_OPTION("NotifyDccSendSuccessInConsole", false, KviOption_sectFlagDcc),
	BOOL_OPTION("CreateMinimizedDccSend", false, KviOption_sectFlagDcc),
	BOOL_OPTION("CreateMinimizedDccChat", false, KviOption_sectFlagDcc),
	BOOL_OPTION("AutoAcceptIncomingAvatars", false, KviOption_sectFlagDcc),
	BOOL_OPTION("UseNickCompletionPostfixForFirstWordOnly", true, KviOption_sectFlagInput),
	BOOL_OPTION("UseWindowListIcons", true, KviOption_sectFlagWindowList | KviOption_resetUpdateGui | KviOption_groupTheme),
	BOOL_OPTION("CreateMinimizedDccSendWhenAutoAccepted", true, KviOption_sectFlagDcc),
	BOOL_OPTION("CreateMinimizedDccChatWhenAutoAccepted", true, KviOption_sectFlagDcc),
	BOOL_OPTION("DccGuessIpFromServerWhenLocalIsUnroutable", true, KviOption_sectFlagDcc),
	BOOL_OPTION("ColorNicksWithBackground", false, KviOption_sectFlagIrcView | KviOption_groupTheme),
	BOOL_OPTION("AutoLogQueries", true, KviOption_sectFlagLogging), /* this options enabled by default in mIRC,XChat and irssi. People are confused while they want to see logs, but see empty dir*/
	BOOL_OPTION("AutoLogChannels", true, KviOption_sectFlagLogging),
	BOOL_OPTION("AutoLogDccChat", false, KviOption_sectFlagLogging),
	BOOL_OPTION("AutoLogConsole", false, KviOption_sectFlagLogging),
	BOOL_OPTION("SilentAntiSpam", false, KviOption_sectFlagAntiSpam),
	BOOL_OPTION("UseIncomingDccMediaTypeSavePath", true, KviOption_sectFlagDcc),
	BOOL_OPTION("NotifyListSendUserhostForOnlineUsers", true, KviOption_sectFlagConnection),
	BOOL_OPTION("UseIntelligentNotifyListManager", true, KviOption_sectFlagConnection | KviOption_resetRestartNotifyList),
	BOOL_OPTION("UseWatchListIfAvailable", true, KviOption_sectFlagConnection),
	BOOL_OPTION("UseTreeWindowList", true, KviOption_sectFlagWindowList | KviOption_resetUpdateWindowList | KviOption_groupTheme),
	BOOL_OPTION("BindIrcIPv4ConnectionsToSpecifiedAddress", false, KviOption_sectFlagConnection),
	BOOL_OPTION("BindIrcIPv6ConnectionsToSpecifiedAddress", false, KviOption_sectFlagConnection),
	BOOL_OPTION("UseAntiSpamOnNotice", false, KviOption_sectFlagAntiSpam),
	BOOL_OPTION("SetLastAvatarAsDefaultForRegisteredUsers", true, KviOption_sectFlagAvatar),
	BOOL_OPTION("CantAcceptIncomingDccConnections", false, KviOption_sectFlagDcc),
	BOOL_OPTION("IgnoreCtcpAvatar", false, KviOption_sectFlagCtcp),
	BOOL_OPTION("CtcpRepliesToActiveWindow", true, KviOption_sectFlagCtcp),
	BOOL_OPTION("AutoAcceptDccCanvas", false, KviOption_sectFlagDcc),
	BOOL_OPTION("NotifyDccSendSuccessInNotifier", true, KviOption_sectFlagDcc),
	BOOL_OPTION("KillBrokenTimers", true, KviOption_sectFlagUserParser),
	BOOL_OPTION("DccVoiceVolumeSliderControlsPCM", true, KviOption_sectFlagDcc),
	BOOL_OPTION("DccVoiceForceHalfDuplex", false, KviOption_sectFlagDcc),
	BOOL_OPTION("AutoAcceptDccVoice", false, KviOption_sectFlagDcc),
	BOOL_OPTION("CreateMinimizedDccVoice", false, KviOption_sectFlagDcc),
	BOOL_OPTION("CreateMinimizedDccVoiceWhenAutoAccepted", true, KviOption_sectFlagDcc),
	BOOL_OPTION("UseIdentService", false, KviOption_sectFlagConnection | KviOption_resetRestartIdentd),
	BOOL_OPTION("ShowTipAtStartup", false, KviOption_sectFlagFrame),
	BOOL_OPTION("IdentdEnableIPv6", false, KviOption_sectFlagConnection | KviOption_resetRestartIdentd),
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	BOOL_OPTION("IdentdIPv6ContainsIPv4", false, KviOption_sectFlagConnection | KviOption_resetRestartIdentd),
#else
	BOOL_OPTION("IdentdIPv6ContainsIPv4", true, KviOption_sectFlagConnection | KviOption_resetRestartIdentd),
#endif
	BOOL_OPTION("ScaleAvatars", true, KviOption_sectFlagAvatar | KviOption_resetUpdateGui | KviOption_groupTheme),
	BOOL_OPTION("ColorNicks", false, KviOption_sectFlagIrcView | KviOption_groupTheme),
	BOOL_OPTION("BoldedNicks", true, KviOption_sectFlagIrcView | KviOption_groupTheme),
	BOOL_OPTION("DccListenOnSpecifiedInterfaceByDefault", false, KviOption_sectFlagDcc),
	BOOL_OPTION("ChangeNickAway", false, KviOption_sectFlagConnection),
	BOOL_OPTION("AutoGeneratedAwayNick", false, KviOption_sectFlagConnection), /* only ircnet uses 9-symbol nicks */
	BOOL_OPTION("ExitAwayOnInput", false, KviOption_sectFlagConnection),
	BOOL_OPTION("AlwaysHighlightNick", true, KviOption_sectFlagIrcView),
	BOOL_OPTION("ShowChannelsJoinOnIrc", true, KviOption_sectFlagFrame),
	BOOL_OPTION("ShowChannelsJoinDialogAsToplevel", true, KviOption_sectFlagFrame), //UNUSED
	BOOL_OPTION("UserDefinedPortRange", false, KviOption_sectFlagDcc),
	BOOL_OPTION("CreateQueryOnPrivmsg", true, KviOption_sectFlagConnection),
	BOOL_OPTION("CreateQueryOnNotice", false, KviOption_sectFlagConnection),
	BOOL_OPTION("CreateIncomingQueriesAsMinimized", true, KviOption_sectFlagConnection),
	BOOL_OPTION("AutoJoinOnInvite", false, KviOption_sectFlagConnection),
	BOOL_OPTION("ShowServersConnectDialogOnStart", true, KviOption_sectFlagFrame),
	BOOL_OPTION("ShowServersConnectDialogAsToplevel", true, KviOption_sectFlagFrame),  //UNUSED
	BOOL_OPTION("AcceptBrokenFileNameDccResumeRequests", true, KviOption_sectFlagDcc),
	BOOL_OPTION("AutoReconnectOnUnexpectedDisconnect", true, KviOption_sectFlagFrame),
	BOOL_OPTION("RejoinChannelsAfterReconnect", true, KviOption_sectFlagFrame),
	BOOL_OPTION("ReopenQueriesAfterReconnect", true, KviOption_sectFlagFrame),
	BOOL_OPTION("EnableQueryTracing", true, KviOption_sectFlagConnection),
	BOOL_OPTION("OperatorMessagesToActiveWindow", false, KviOption_sectFlagConnection),
	BOOL_OPTION("InvitesToActiveWindow", true, KviOption_sectFlagConnection),
	BOOL_OPTION("ShowChannelUserFlagInPrivmsgView", true, KviOption_sectFlagConnection),
	BOOL_OPTION("DccSendFakeAddressByDefault", false, KviOption_sectFlagDcc),
	BOOL_OPTION("UseWindowListActivityMeter", false, KviOption_sectFlagWindowList | KviOption_resetUpdateGui | KviOption_groupTheme),
	BOOL_OPTION("CloseServerWidgetAfterConnect", false, KviOption_sectFlagFrame),
	BOOL_OPTION("PrioritizeLastActionTime", false, KviOption_sectFlagInput),
	BOOL_OPTION("ShowUserChannelIcons", true, KviOption_sectFlagUserListView | KviOption_resetUpdateGui | KviOption_groupTheme),
	BOOL_OPTION("ShowUserChannelState", false, KviOption_sectFlagUserListView | KviOption_resetUpdateGui | KviOption_groupTheme),
	BOOL_OPTION("EnableIgnoreOnPrivMsg", true, KviOption_sectFlagConnection),
	BOOL_OPTION("VerboseIgnore", false, KviOption_sectFlagConnection),
	BOOL_OPTION("EnableIgnoreOnNotice", true, KviOption_sectFlagConnection),
	BOOL_OPTION("NotifyFailedDccHandshakes", true, KviOption_sectFlagDcc),
	BOOL_OPTION("DrawEmoticons", true, KviOption_sectFlagIrcView | KviOption_groupTheme),
	BOOL_OPTION("IgnoreCtcpPage", false, KviOption_sectFlagCtcp),
	BOOL_OPTION("ShowDialogOnCtcpPage", true, KviOption_sectFlagCtcp),
	BOOL_OPTION("BashLikeNickCompletion", false, KviOption_sectFlagInput),
	BOOL_OPTION("UseSSLCertificate", false, KviOption_sectFlagConnection),
	BOOL_OPTION("UseSSLPrivateKey", false, KviOption_sectFlagConnection),
	BOOL_OPTION("WarnAboutPastingMultipleLines", true, KviOption_sectFlagFrame),
	BOOL_OPTION("KeepChannelsOpenOnUnexpectedDisconnect", true, KviOption_sectFlagFrame),
	BOOL_OPTION("KeepQueriesOpenOnUnexpectedDisconnect", true, KviOption_sectFlagFrame),
	BOOL_OPTION("UseLagMeterEngine", true, KviOption_sectFlagConnection | KviOption_resetRestartLagMeter),
	BOOL_OPTION("ShowLagOnContextDisplay", true, KviOption_sectFlagFrame),
	BOOL_OPTION("DisableBanListRequestOnJoin", false, KviOption_sectFlagConnection),
	BOOL_OPTION("DisableBanExceptionListRequestOnJoin", true, KviOption_sectFlagConnection),
	BOOL_OPTION("DisableInviteListRequestOnJoin", true, KviOption_sectFlagConnection),
	BOOL_OPTION("DisableWhoRequestOnJoin", false, KviOption_sectFlagConnection),
	BOOL_OPTION("ShowDockExtension", true, KviOption_sectFlagFrame),
	BOOL_OPTION("DccAddressByIrcDns", false, KviOption_sectFlagFrame),
	BOOL_OPTION("HighlightOnlyNormalMsg", false, KviOption_sectFlagFrame),
	BOOL_OPTION("HighlightOnlyNormalMsgQueryToo", false, KviOption_sectFlagFrame),
	BOOL_OPTION("HighlightOnlyNormalMsgHighlightInChanToo", false, KviOption_sectFlagFrame),
	BOOL_OPTION("HighlightOnlyAtCustomHighlightLevel", false, KviOption_sectFlagFrame),
	BOOL_OPTION("DccBrokenBouncerHack", false, KviOption_sectFlagDcc),
	BOOL_OPTION("ExternalMessagesToActiveWindow", true, KviOption_sectFlagConnection),
	BOOL_OPTION("IgnoreChannelAvatarRequestsWhenNoAvatarSet", true, KviOption_sectFlagAvatar),
	BOOL_OPTION("FlashQueryWindowOnNewMessages", true, KviOption_sectFlagFrame),
	BOOL_OPTION("ConfirmCloseWhenThereAreConnections", true, KviOption_sectFlagFrame),
	BOOL_OPTION("UseWindowsDesktopForTransparency", false, KviOption_sectFlagGui | KviOption_resetUpdatePseudoTransparency | KviOption_groupTheme),
	BOOL_OPTION("CtcpRequestsToActiveWindow", false, KviOption_sectFlagConnection),
	BOOL_OPTION("ServerRepliesToActiveWindow", true, KviOption_sectFlagConnection),
	BOOL_OPTION("ServerNoticesToActiveWindow", false, KviOption_sectFlagConnection),
	BOOL_OPTION("ServicesNoticesToActiveWindow", false, KviOption_sectFlagConnection),
	BOOL_OPTION("SortWindowListItemsByName", true, KviOption_sectFlagConnection | KviOption_groupTheme),
	BOOL_OPTION("ShowWindowListToolTips", true, KviOption_sectFlagConnection | KviOption_groupTheme),
	BOOL_OPTION("ShowUserListViewToolTips", true, KviOption_sectFlagConnection | KviOption_groupTheme),
	BOOL_OPTION("WhoisRepliesToActiveWindow", true, KviOption_sectFlagConnection),
	BOOL_OPTION("PopupNotifierOnNotifyOnLine", true, KviOption_sectFlagConnection),
	BOOL_OPTION("EnableNotifier", true, KviOption_sectFlagConnection),
	BOOL_OPTION("DccSendForceIdleStep", false, KviOption_sectFlagDcc),
	BOOL_OPTION("StatusBarVisible", true, KviOption_sectFlagFrame),
	BOOL_OPTION("TrackLastReadTextViewLine", true, KviOption_sectFlagFrame),
	BOOL_OPTION("KeepChannelOpenOnPart", false, KviOption_sectFlagFrame),
	BOOL_OPTION("StripMircColorsInUserMessages", false, KviOption_sectFlagFrame | KviOption_groupTheme),
	BOOL_OPTION("DoNotSuggestRestoreDefaultScript", false, KviOption_sectFlagFrame),
	BOOL_OPTION("LimitDccSendSpeed", false, KviOption_sectFlagDcc),
	BOOL_OPTION("LimitDccRecvSpeed", false, KviOption_sectFlagDcc),
	BOOL_OPTION("IgnoreModuleVersions", false, KviOption_sectFlagFrame),
	BOOL_OPTION("EnableInputHistory", true, KviOption_sectFlagInput | KviOption_resetUpdateGui),
	BOOL_OPTION("UseSpecialColorForTimestamp", true, KviOption_sectFlagIrcView | KviOption_resetUpdateGui | KviOption_groupTheme),
	BOOL_OPTION("EnableAwayListUpdates", true, KviOption_sectFlagFrame),
	BOOL_OPTION("ShowAvatarsInUserlist", true, KviOption_sectFlagIrcView | KviOption_resetUpdateGui | KviOption_groupTheme),
	BOOL_OPTION("ShowUserListStatisticLabel", true, KviOption_sectFlagIrcView | KviOption_resetUpdateGui | KviOption_groupTheme),
#ifdef COMPILE_ON_MAC
	BOOL_OPTION("ShowIconsInPopupMenus", false, KviOption_sectFlagIrcView | KviOption_resetUpdateGui | KviOption_groupTheme | KviOption_resetReloadImages),
#else
	BOOL_OPTION("ShowIconsInPopupMenus", true, KviOption_sectFlagIrcView | KviOption_resetUpdateGui | KviOption_groupTheme | KviOption_resetReloadImages),
#endif
	BOOL_OPTION("ScriptErrorsToDebugWindow", false, KviOption_sectFlagFrame),
	BOOL_OPTION("ShowMinimizedDebugWindow", true, KviOption_sectFlagFrame),
	BOOL_OPTION("ShowExtendedInfoInQueryLabel", true, KviOption_resetUpdateGui),
	BOOL_OPTION("UseUserListColorsAsNickColors", true, KviOption_sectFlagIrcView | KviOption_groupTheme),
	BOOL_OPTION("GzipLogs", false, KviOption_sectFlagLogging),
	BOOL_OPTION("UseFullWordHighlighting", false, KviOption_sectFlagIrcView),
	BOOL_OPTION("NotifierFlashing", true, KviOption_sectFlagFrame),
	BOOL_OPTION("CommandlineInUserFriendlyModeByDefault", true, KviOption_sectFlagFrame),
	BOOL_OPTION("EnableVisualEffects", true, KviOption_resetUpdateGui),
	BOOL_OPTION("DCCFileTransferReplaceOutgoingSpacesWithUnderscores", true, KviOption_sectFlagDcc),
	BOOL_OPTION("DoNotUpscaleAvatars", true, KviOption_sectFlagIrcView | KviOption_resetUpdateGui),
	BOOL_OPTION("UseDifferentColorForOwnNick", false, KviOption_sectFlagIrcView | KviOption_resetUpdateGui | KviOption_groupTheme),
	BOOL_OPTION("CloseInTray", false, KviOption_sectFlagFrame | KviOption_resetUpdateGui),
	BOOL_OPTION("StartupMinimized", false, KviOption_sectFlagFrame),
	BOOL_OPTION("SortReceivedByDccFilesByNicks", false, KviOption_sectFlagDcc),
	BOOL_OPTION("LogChannelHistory", true, KviOption_sectFlagConnection),
	BOOL_OPTION("ClassicWindowListSetMaximumButtonWidth", true, KviOption_groupTheme | KviOption_sectFlagGeometry | KviOption_resetUpdateGui),
	BOOL_OPTION("KeepChannelsOpenOnDisconnect", true, KviOption_sectFlagFrame),
	BOOL_OPTION("KeepQueriesOpenOnDisconnect", true, KviOption_sectFlagFrame),
	BOOL_OPTION("HideWindowToolButtons", true, KviOption_sectFlagFrame),
	BOOL_OPTION("HideInputToolButtons", true, KviOption_sectFlagFrame),
	BOOL_OPTION("StripControlCodesInLogs", false, KviOption_sectFlagLogging),
	BOOL_OPTION("EnableTrayIconFlashing", true, KviOption_sectFlagFrame),
	BOOL_OPTION("UseFlatClassicWindowListButtons", false, KviOption_sectFlagWindowList | KviOption_resetUpdateGui | KviOption_groupTheme),
	BOOL_OPTION("StripMsgTypeInLogs", false, KviOption_sectFlagLogging),
	BOOL_OPTION("UseLevelBasedTrayNotification", false, KviOption_sectFlagFrame),
	BOOL_OPTION("MuteAllSounds", false, KviOption_sectFlagFrame),
	BOOL_OPTION("CreateMinimizedChannels", false, KviOption_sectFlagFrame),
	BOOL_OPTION("ShowNetworkNameForConsoleWindowListEntry", true, KviOption_sectFlagFrame | KviOption_resetUpdateGui),
	BOOL_OPTION("DrawGenderIcons", true, KviOption_sectFlagFrame | KviOption_resetUpdateGui | KviOption_groupTheme),
	BOOL_OPTION("PrependGenderInfoToRealname", true, KviOption_sectFlagConnection),
	BOOL_OPTION("UseIdentServiceOnlyOnConnect", true, KviOption_sectFlagConnection | KviOption_resetRestartIdentd),
	BOOL_OPTION("UseSystemUrlHandlers", true, KviOption_sectFlagUrl),
	BOOL_OPTION("ScaleAvatarsOnLoad", true, KviOption_sectFlagAvatar),
	BOOL_OPTION("NotifierFading", true, KviOption_sectFlagFrame),
	BOOL_OPTION("PickRandomIpAddressForRoundRobinServers", true, KviOption_sectFlagConnection),
	BOOL_OPTION("PrependAvatarInfoToRealname", true, KviOption_sectFlagConnection),
	BOOL_OPTION("EnableAnimatedAvatars", true, KviOption_sectFlagGui | KviOption_resetUpdateGui),
	BOOL_OPTION("EnableAnimatedSmiles", true, KviOption_sectFlagGui | KviOption_resetUpdateGui),
	BOOL_OPTION("PlaceNickWithNonAlphaCharsAtEnd", true, KviOption_sectFlagGui | KviOption_resetUpdateGui),
	BOOL_OPTION("PasteLastLogOnChannelJoin", false, KviOption_sectFlagLogging),
	BOOL_OPTION("PasteLastLogOnQueryJoin", false, KviOption_sectFlagLogging),
	BOOL_OPTION("UseSpecifiedSmartColorForOwnNick", false, KviOption_sectFlagIrcView | KviOption_resetUpdateGui | KviOption_groupTheme),
	BOOL_OPTION("ZshLikeNickCompletion", false, KviOption_sectFlagInput),
	BOOL_OPTION("AutoAcceptDccVideo", false, KviOption_sectFlagDcc),
	BOOL_OPTION("CreateMinimizedDccVideo", false, KviOption_sectFlagDcc),
	BOOL_OPTION("CreateMinimizedDccVideoWhenAutoAccepted", true, KviOption_sectFlagDcc),
	BOOL_OPTION("ShowTaskBarButton", true, KviOption_sectFlagFrame),
	BOOL_OPTION("FlashDccChatWindowOnNewMessages", true, KviOption_sectFlagFrame),
	BOOL_OPTION("PopupNotifierOnNewDccChatMessages", true, KviOption_sectFlagFrame),
	BOOL_OPTION("UseAwayMessage", true, KviOption_sectFlagConnection),
	BOOL_OPTION("DisableQuietBanListRequestOnJoin", true, KviOption_sectFlagConnection),
	BOOL_OPTION("UseWindowListCloseButton", false, KviOption_sectFlagWindowList | KviOption_resetUpdateGui | KviOption_groupTheme),
	BOOL_OPTION("FrameIsMaximized", false, KviOption_sectFlagGeometry),
	BOOL_OPTION("PrependNickColorInfoToRealname", true, KviOption_sectFlagConnection),
	BOOL_OPTION("DontShowNotifierIfActiveWindowIsFullScreen", true, KviOption_sectFlagFrame),
	BOOL_OPTION("WheelScrollsWindowsList", true, KviOption_sectFlagWindowList),
	BOOL_OPTION("AcceptMismatchedPortDccResumeRequests", false, KviOption_sectFlagDcc),
	BOOL_OPTION("EnableUrlLinkToolTip", true, KviOption_sectFlagGui),
	BOOL_OPTION("EnableHostLinkToolTip", true, KviOption_sectFlagGui),
	BOOL_OPTION("EnableServerLinkToolTip", true, KviOption_sectFlagGui),
	BOOL_OPTION("EnableModeLinkToolTip", true, KviOption_sectFlagGui),
	BOOL_OPTION("EnableNickLinkToolTip", true, KviOption_sectFlagGui),
	BOOL_OPTION("EnableChannelLinkToolTip", true, KviOption_sectFlagGui),
	BOOL_OPTION("EnableEscapeLinkToolTip", true, KviOption_sectFlagGui),
	BOOL_OPTION("UseDBusNotifier", false, KviOption_sectFlagConnection),
	BOOL_OPTION("UseKDENotifier", false, KviOption_sectFlagConnection),
	BOOL_OPTION("CaseSensitiveHighlighting", false, KviOption_sectFlagIrcView),
	BOOL_OPTION("MinimizeInTray", false, KviOption_sectFlagFrame | KviOption_resetUpdateGui),
	BOOL_OPTION("DisplayNotifierOnPrimaryScreen", true, KviOption_sectFlagFrame),
	BOOL_OPTION("ShowDialogOnChannelCtcpPage", false, KviOption_sectFlagCtcp),
	BOOL_OPTION("PopupNotifierOnNewNotices", true, KviOption_sectFlagFrame),
	BOOL_OPTION("UserListViewUseAwayColor", true, KviOption_sectFlagUserListView | KviOption_resetUpdateGui),
	BOOL_OPTION("ShowUserFlagForChannelsInWindowList", true, KviOption_sectFlagWindowList | KviOption_resetUpdateGui | KviOption_groupTheme),
	BOOL_OPTION("EnableCustomCursorWidth", false, KviOption_sectFlagGui | KviOption_resetUpdateGui),
	BOOL_OPTION("ShowFavoriteServersOnly", false, KviOption_sectFlagFrame),
	BOOL_OPTION("RequireControlToCopy", false, KviOption_sectFlagIrcView),
	BOOL_OPTION("Send64BitAckInDccRecv", false, KviOption_sectFlagDcc),
	BOOL_OPTION("ShowTreeWindowListHandle", true, KviOption_sectFlagWindowList | KviOption_resetUpdateGui | KviOption_groupTheme),
	BOOL_OPTION("MenuBarVisible", true, KviOption_sectFlagFrame | KviOption_resetUpdateGui),
	BOOL_OPTION("WarnAboutHidingMenuBar", true, KviOption_sectFlagFrame),
	BOOL_OPTION("WhoRepliesToActiveWindow", false, KviOption_sectFlagConnection),
	BOOL_OPTION("DropConnectionOnSaslFailure", false, KviOption_sectFlagConnection)
};

// NOTICE: REUSE EQUIVALENT UNUSED KviOption_bool in KviOptions.h ENTRIES BEFORE ADDING NEW ENTRIES ABOVE

#define STRING_OPTION(_txt, _val, _flags) KviStringOption(KVI_STRING_OPTIONS_PREFIX _txt, _val, _flags)

// FIXME: #warning "Remove the unused string options here!"

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
#define RUN_THE_BROWSER "run cmd.exe /c start \"\" \"$0\""
#else
#ifdef COMPILE_ON_MAC
#define RUN_THE_BROWSER "run open $0"
#else
#define RUN_THE_BROWSER "run xdg-open $0"
#endif
#endif

KviStringOption g_stringOptionsTable[KVI_NUM_STRING_OPTIONS] = {
	STRING_OPTION("Nickname1", "", KviOption_sectFlagUser),
	STRING_OPTION("Nickname2", "", KviOption_sectFlagUser),
	STRING_OPTION("Nickname3", "", KviOption_sectFlagUser),
	STRING_OPTION("Nickname4", "", KviOption_sectFlagUser),
	STRING_OPTION("Username", KVI_DEFAULT_USERNAME, KviOption_sectFlagUser),
	STRING_OPTION("Realname", KVI_DEFAULT_REALNAME, KviOption_sectFlagUser),
	STRING_OPTION("LocalHostIp", "127.0.0.1", KviOption_sectFlagIrcSocket),
	STRING_OPTION("PartMessage", KVI_DEFAULT_PART_MESSAGE, KviOption_sectFlagConnection),
	STRING_OPTION("QuitMessage", KVI_DEFAULT_QUIT_MESSAGE, KviOption_sectFlagConnection),
	STRING_OPTION("UrlHttpCommand", RUN_THE_BROWSER, KviOption_sectFlagUrl),
	STRING_OPTION("ExtendedPrivmsgPrefix", "[", KviOption_sectFlagIrcView | KviOption_groupTheme),
	STRING_OPTION("ExtendedPrivmsgPostfix", "] ", KviOption_sectFlagIrcView | KviOption_groupTheme),
	STRING_OPTION("CtcpVersionPostfix", "", KviOption_sectFlagCtcp),
	STRING_OPTION("CtcpUserInfoOther", KVI_DEFAULT_CTCP_USERINFO_REPLY, KviOption_sectFlagUser),
	STRING_OPTION("CtcpSourcePostfix", "", KviOption_sectFlagCtcp),
	STRING_OPTION("UrlFtpCommand", RUN_THE_BROWSER, KviOption_sectFlagUrl),
	STRING_OPTION("UrlHttpsCommand", RUN_THE_BROWSER, KviOption_sectFlagUrl),
	STRING_OPTION("UrlMailtoCommand", RUN_THE_BROWSER, KviOption_sectFlagUrl),
	STRING_OPTION("PreferredMediaPlayer", "auto", KviOption_sectFlagFrame),
	STRING_OPTION("UrlFileCommand", RUN_THE_BROWSER, KviOption_sectFlagUrl),
	STRING_OPTION("NickCompletionPostfix", ": ", KviOption_sectFlagInput),
	STRING_OPTION("UrlUnknownCommand", RUN_THE_BROWSER, KviOption_sectFlagUrl),
	STRING_OPTION("IPv4ConnectionBindAddress", "", KviOption_sectFlagConnection),
	STRING_OPTION("IPv6ConnectionBindAddress", "", KviOption_sectFlagConnection),
	STRING_OPTION("DccVoiceSoundDevice", "/dev/dsp", KviOption_sectFlagDcc),
	STRING_OPTION("DccVoiceMixerDevice", "/dev/mixer", KviOption_sectFlagDcc),
	STRING_OPTION("AwayMessage", KVI_DEFAULT_AWAY_MESSAGE, KviOption_sectFlagConnection),
	STRING_OPTION("IdentdUser", "kvirc", KviOption_sectFlagConnection | KviOption_resetRestartIdentd),
	STRING_OPTION("DccListenDefaultInterface", "ppp0", KviOption_sectFlagDcc),
	STRING_OPTION("CustomAwayNick", KVI_DEFAULT_AWAY_NICK, KviOption_sectFlagConnection),
	STRING_OPTION("DefaultDccFakeAddress", "", KviOption_sectFlagDcc),
	STRING_OPTION("CtcpPageReply", KVI_DEFAULT_CTCP_PAGE_REPLY, KviOption_sectFlagCtcp),
	STRING_OPTION("SSLCertificatePath", "", KviOption_sectFlagConnection | KviOption_encodePath),
	STRING_OPTION("SSLCertificatePass", "", KviOption_sectFlagConnection),
	STRING_OPTION("SSLPrivateKeyPath", "", KviOption_sectFlagConnection | KviOption_encodePath),
	STRING_OPTION("SSLPrivateKeyPass", "", KviOption_sectFlagConnection),
	STRING_OPTION("SoundSystem", "unknown", KviOption_sectFlagFrame),
	STRING_OPTION("DefaultUserMode", "", KviOption_sectFlagUser),
	STRING_OPTION("LastImageDialogPath", "", KviOption_sectFlagUser | KviOption_encodePath),
	STRING_OPTION("IconThemeSubdir", "", KviOption_sectFlagUser | KviOption_groupTheme | KviOption_resetReloadImages | KviOption_encodePath),
	STRING_OPTION("IncomingPath", "", KviOption_sectFlagUser | KviOption_encodePath),
	STRING_OPTION("MyAvatar", "", KviOption_sectFlagUser),
	STRING_OPTION("DefaultTextEncoding", "", KviOption_sectFlagFrame),
	STRING_OPTION("WinampTextEncoding", "", KviOption_sectFlagFrame),
	STRING_OPTION("Mp3TagsEncoding", "", KviOption_sectFlagFrame),
	STRING_OPTION("CtcpUserInfoAge", "", KviOption_sectFlagUser),
	STRING_OPTION("CtcpUserInfoGender", "", KviOption_sectFlagUser),
	STRING_OPTION("CtcpUserInfoLocation", "", KviOption_sectFlagUser),
	STRING_OPTION("CtcpUserInfoLanguages", "", KviOption_sectFlagUser),
	STRING_OPTION("WordSplitters", ",\"';:|.%^~!\\$#()?", KviOption_sectFlagIrcView),
	STRING_OPTION("OnNewQueryOpenedSound", "", KviOption_sectFlagFrame),
	STRING_OPTION("OnHighlightedMessageSound", "", KviOption_sectFlagFrame),
	STRING_OPTION("OnMeKickedSound", "", KviOption_sectFlagFrame),
	STRING_OPTION("OnQueryMessageSound", "", KviOption_sectFlagFrame),
	STRING_OPTION("IrcViewTimestampFormat", "[hh:mm:ss]", KviOption_sectFlagIrcView),
	STRING_OPTION("PreferredTorrentClient", "auto", KviOption_sectFlagFrame),
	STRING_OPTION("DefaultSrvEncoding", "", KviOption_sectFlagFrame),
	STRING_OPTION("LogsPath", "", KviOption_sectFlagUser | KviOption_encodePath),
	STRING_OPTION("LogsDynamicPath", "", KviOption_sectFlagUser | KviOption_encodePath),
	STRING_OPTION("LogsExportPath", "", KviOption_sectFlagUser | KviOption_encodePath)
};

#define STRINGLIST_OPTION(_txt, _flags) \
	KviStringListOption(KVI_STRINGLIST_OPTIONS_PREFIX _txt, QStringList(), _flags)

#define STRINGLIST_OPTION_WITHDEFAULT(_txt, _flags, _def) \
	KviStringListOption(KVI_STRINGLIST_OPTIONS_PREFIX _txt, QStringList(_def), _flags)

KviStringListOption g_stringlistOptionsTable[KVI_NUM_STRINGLIST_OPTIONS] = {
	STRINGLIST_OPTION("HighlightWords", KviOption_sectFlagIrcView),
	STRINGLIST_OPTION("SpamWords", KviOption_sectFlagAntiSpam),
	STRINGLIST_OPTION_WITHDEFAULT("RecentChannels", KviOption_sectFlagRecent, "#kvirc" KVI_RECENT_CHANNELS_SEPARATOR "freenode"),
	STRINGLIST_OPTION("RecentServers", KviOption_sectFlagRecent),
	STRINGLIST_OPTION("RecentNicknames", KviOption_sectFlagRecent),
	STRINGLIST_OPTION("ModuleExtensionToolbars", KviOption_sectFlagFrame),
	STRINGLIST_OPTION("ImageSearchPaths", KviOption_sectFlagFrame | KviOption_encodePath),
	STRINGLIST_OPTION("RecentIrcUrls", KviOption_sectFlagRecent),
	STRINGLIST_OPTION("SpellCheckerDictionaries", KviOption_sectFlagInput)
};

#define MIRC_COLOR_OPTION(_num, _red, _green, _blue)                               \
	KviColorOption(KVI_MIRCCOLOR_OPTIONS_PREFIX _num, QColor(_red, _green, _blue), \
	    KviOption_sectFlagMircColor | KviOption_resetUpdateGui | KviOption_groupTheme)

KviColorOption g_mirccolorOptionsTable[KVI_NUM_MIRCCOLOR_OPTIONS] = {
	MIRC_COLOR_OPTION("0", 255, 255, 255),
	MIRC_COLOR_OPTION("1", 0, 0, 0),
	MIRC_COLOR_OPTION("2", 0, 0, 140),
	MIRC_COLOR_OPTION("3", 0, 100, 0),
	MIRC_COLOR_OPTION("4", 230, 0, 0),
	MIRC_COLOR_OPTION("5", 150, 0, 0),
	MIRC_COLOR_OPTION("6", 80, 0, 80),
	MIRC_COLOR_OPTION("7", 255, 90, 0),
	MIRC_COLOR_OPTION("8", 255, 255, 0),
	MIRC_COLOR_OPTION("9", 0, 255, 0),
	MIRC_COLOR_OPTION("10", 0, 150, 180),
	MIRC_COLOR_OPTION("11", 170, 170, 255),
	MIRC_COLOR_OPTION("12", 15, 15, 255),
	MIRC_COLOR_OPTION("13", 200, 0, 200),
	MIRC_COLOR_OPTION("14", 80, 80, 80),
	MIRC_COLOR_OPTION("15", 170, 170, 170)
};

#define COLOR_OPTION(_name, _red, _green, _blue, _alpha, _flags) \
	KviColorOption(KVI_COLOR_OPTIONS_PREFIX _name,               \
	    QColor(_red, _green, _blue, _alpha),                     \
	    _flags | KviOption_resetUpdateGui | KviOption_groupTheme | KviOption_resetUpdatePseudoTransparency)

KviColorOption g_colorOptionsTable[KVI_NUM_COLOR_OPTIONS] = {
	COLOR_OPTION("WindowListNormalText", 0, 0, 0, 255, KviOption_sectFlagWindowList),
	COLOR_OPTION("WindowListMinimizedText", 60, 60, 60, 255, KviOption_sectFlagWindowList),
	COLOR_OPTION("WindowListHighlight1Text", 0, 16, 144, 255, KviOption_sectFlagWindowList),
	COLOR_OPTION("WindowListHighlight2Text", 32, 0, 255, 255, KviOption_sectFlagWindowList),
	COLOR_OPTION("WindowListProgressBar", 180, 180, 180, 255, KviOption_sectFlagWindowList),
	COLOR_OPTION("UserListViewAwayForeground", 143, 143, 143, 255, KviOption_sectFlagUserListView),
	COLOR_OPTION("UserListViewOwnForeground", 255, 255, 0, 255, KviOption_sectFlagUserListView),
	COLOR_OPTION("UserListViewIrcOpForeground", 200, 0, 0, 255, KviOption_sectFlagUserListView),
	COLOR_OPTION("NotifierBackground", 255, 255, 255, 255, KviOption_sectFlagNotifier),
	COLOR_OPTION("NotifierForeground", 0, 0, 0, 255, KviOption_sectFlagNotifier),
	COLOR_OPTION("NotifierTitleForeground", 255, 255, 255, 255, KviOption_sectFlagNotifier),
	COLOR_OPTION("IrcViewBackground", 255, 255, 255, 255, KviOption_sectFlagIrcView),
	COLOR_OPTION("InputBackground", 255, 255, 255, 255, KviOption_sectFlagInput),
	COLOR_OPTION("InputSelectionBackground", 0, 0, 0, 255, KviOption_sectFlagInput),
	COLOR_OPTION("InputSelectionForeground", 255, 255, 255, 255, KviOption_sectFlagInput),
	COLOR_OPTION("InputCursor", 0, 0, 0, 255, KviOption_sectFlagInput),
	COLOR_OPTION("InputControl", 200, 100, 0, 255, KviOption_sectFlagInput),
	COLOR_OPTION("InputForeground", 0, 0, 0, 255, KviOption_sectFlagInput),
	COLOR_OPTION("UserListViewBackground", 255, 255, 255, 255, KviOption_sectFlagUserListView),
	COLOR_OPTION("UserListViewOpForeground", 200, 0, 0, 255, KviOption_sectFlagUserListView),
	COLOR_OPTION("UserListViewVoiceForeground", 0, 0, 200, 255, KviOption_sectFlagUserListView),
	COLOR_OPTION("UserListViewNormalForeground", 0, 0, 0, 255, KviOption_sectFlagUserListView),
	COLOR_OPTION("UserListViewSelectionBackground", 0, 0, 0, 255, KviOption_sectFlagUserListView),
	COLOR_OPTION("UserListViewSelectionForeground", 255, 255, 255, 255, KviOption_sectFlagUserListView),
	COLOR_OPTION("LabelBackground", 255, 255, 255, 255, KviOption_sectFlagLabel),
	COLOR_OPTION("LabelForeground", 0, 0, 0, 255, KviOption_sectFlagLabel),
	COLOR_OPTION("GlobalTransparencyFade", 255, 255, 255, 255, KviOption_sectFlagGui),
	COLOR_OPTION("IrcToolBarAppletBackground", 255, 255, 255, 255, KviOption_sectFlagIrcToolBar),
	COLOR_OPTION("IrcToolBarAppletBorder", 110, 110, 110, 255, KviOption_sectFlagIrcToolBar),
	COLOR_OPTION("IrcToolBarAppletFont", 0, 0, 0, 255, KviOption_sectFlagIrcToolBar),
	COLOR_OPTION("TreeWindowListBackground", 255, 255, 255, 255, KviOption_sectFlagWindowList),
	COLOR_OPTION("TreeWindowListActiveBackground", 200, 200, 200, 255, KviOption_sectFlagWindowList),
	COLOR_OPTION("TreeWindowListForeground", 0, 0, 0, 255, KviOption_sectFlagWindowList),
	COLOR_OPTION("TreeWindowListActiveForeground", 0, 0, 0, 255, KviOption_sectFlagWindowList),
	COLOR_OPTION("TreeWindowListHighlight1Foreground", 0, 16, 144, 255, KviOption_sectFlagWindowList),
	COLOR_OPTION("TreeWindowListHighlight2Foreground", 32, 0, 255, 255, KviOption_sectFlagWindowList),
	COLOR_OPTION("TreeWindowListProgress", 150, 150, 150, 255, KviOption_sectFlagWindowList),
	COLOR_OPTION("UserListViewHalfOpForeground", 119, 0, 178, 255, KviOption_sectFlagUserListView),
	COLOR_OPTION("WindowListHighlight3Text", 128, 20, 255, 255, KviOption_sectFlagWindowList),
	COLOR_OPTION("WindowListHighlight4Text", 220, 0, 255, 255, KviOption_sectFlagWindowList),
	COLOR_OPTION("WindowListHighlight5Text", 255, 0, 0, 255, KviOption_sectFlagWindowList),
	COLOR_OPTION("TreeWindowListHighlight3Foreground", 128, 0, 255, 255, KviOption_sectFlagWindowList),
	COLOR_OPTION("TreeWindowListHighlight4Foreground", 220, 0, 255, 255, KviOption_sectFlagWindowList),
	COLOR_OPTION("TreeWindowListHighlight5Foreground", 255, 0, 0, 255, KviOption_sectFlagWindowList),
	COLOR_OPTION("UserListViewChanAdminForeground", 83, 54, 5, 255, KviOption_sectFlagUserListView),
	COLOR_OPTION("UserListViewUserOpForeground", 0, 127, 178, 255, KviOption_sectFlagUserListView),
	COLOR_OPTION("UserListViewGrid", 110, 110, 110, 255, KviOption_sectFlagUserListView),
	COLOR_OPTION("UserListViewChanOwnerForeground", 81, 87, 111, 255, KviOption_sectFlagUserListView),
	COLOR_OPTION("IrcViewMarkLine", 40, 40, 40, 255, KviOption_sectFlagIrcView)
};

#define IC_COLOR_OPTION(_num, _red, _green, _blue) \
	KviColorOption(                                \
	    KVI_ICCOLOR_OPTIONS_PREFIX _num,           \
	    QColor(_red, _green, _blue),               \
	    KviOption_sectFlagWindowList | KviOption_resetUpdateGui | KviOption_groupTheme)

KviColorOption g_iccolorOptionsTable[KVI_NUM_ICCOLOR_OPTIONS] = {
	IC_COLOR_OPTION("0", 250, 250, 0),
	IC_COLOR_OPTION("1", 0, 0, 0),
	IC_COLOR_OPTION("2", 0, 0, 200),
	IC_COLOR_OPTION("3", 200, 0, 0),
	IC_COLOR_OPTION("4", 255, 255, 255),
	IC_COLOR_OPTION("5", 0, 150, 190),
	IC_COLOR_OPTION("6", 0, 130, 0),
	IC_COLOR_OPTION("7", 150, 0, 170)
};

#define PIXMAP_OPTION(_name, _flags)     \
	KviPixmapOption(                     \
	    KVI_PIXMAP_OPTIONS_PREFIX _name, \
	    KviPixmap(),                     \
	    _flags | KviOption_resetUpdateGui)

KviPixmapOption g_pixmapOptionsTable[KVI_NUM_PIXMAP_OPTIONS] = {
	PIXMAP_OPTION("IrcViewBackground", KviOption_sectFlagIrcView | KviOption_groupTheme),
	PIXMAP_OPTION("InputBackground", KviOption_sectFlagInput | KviOption_groupTheme),
	PIXMAP_OPTION("UserListViewBackground", KviOption_sectFlagUserListView | KviOption_groupTheme),
	PIXMAP_OPTION("LabelBackground", KviOption_sectFlagLabel | KviOption_groupTheme),
	PIXMAP_OPTION("MyAvatar", KviOption_sectFlagUser),
	PIXMAP_OPTION("IrcToolBarAppletBackground", KviOption_sectFlagIrcToolBar | KviOption_groupTheme),
	PIXMAP_OPTION("TreeWindowListBackground", KviOption_sectFlagWindowList | KviOption_groupTheme),
	PIXMAP_OPTION("GlobalTransparencyBackground", KviOption_sectFlagGui | KviOption_resetUpdatePseudoTransparency | KviOption_groupTheme),
	PIXMAP_OPTION("NotifierBackground", KviOption_sectFlagNotifier | KviOption_groupTheme)
};

#define UINT_OPTION(_name, _value, _flags) \
	KviUIntOption(                         \
	    KVI_UINT_OPTIONS_PREFIX _name,     \
	    _value,                            \
	    _flags)

KviUIntOption g_uintOptionsTable[KVI_NUM_UINT_OPTIONS] = {
	UINT_OPTION("IrcSocketTimeout", 60, KviOption_sectFlagIrcSocket),
	UINT_OPTION("ModuleCleanupTimerInterval", 90, KviOption_sectFlagModules),
	UINT_OPTION("ModuleCleanupTimeout", 180, KviOption_sectFlagModules),
	UINT_OPTION("MaxCtcpRequests", 5, KviOption_sectFlagCtcp),
	UINT_OPTION("CtcpFloodCheckInterval", 10, KviOption_sectFlagCtcp),
	UINT_OPTION("TotalConnectionTime", 0, 0),
	UINT_OPTION("DccSocketTimeout", 360, KviOption_sectFlagDcc),
	UINT_OPTION("DccSendIdleStepInMSec", 30, KviOption_sectFlagDcc),
	UINT_OPTION("DccSendPacketSize", 16384, KviOption_sectFlagDcc),
	UINT_OPTION("GlobalTransparencyParentFadeFactor", 10, KviOption_sectFlagGui | KviOption_resetUpdatePseudoTransparency | KviOption_groupTheme),
	UINT_OPTION("GlobalTransparencyChildFadeFactor", 50, KviOption_sectFlagGui | KviOption_resetUpdatePseudoTransparency | KviOption_groupTheme),
	UINT_OPTION("NotifyListCheckTimeInSecs", 180, KviOption_sectFlagConnection),
	UINT_OPTION("SocketQueueFlushTimeout", 500, KviOption_sectFlagIrcSocket),
	UINT_OPTION("OutgoingTrafficLimitUSeconds", 2000000, KviOption_sectFlagIrcSocket),
	UINT_OPTION("NotifyListIsOnDelayTimeInSecs", 6, KviOption_sectFlagConnection),
	UINT_OPTION("NotifyListUserhostDelayTimeInSecs", 6, KviOption_sectFlagConnection),
	UINT_OPTION("TreeWindowListMinimumWidth", 24, KviOption_sectFlagWindowList | KviOption_resetUpdateWindowList),
	UINT_OPTION("AvatarOfferTimeoutInSecs", 60, KviOption_sectFlagAvatar),
	UINT_OPTION("IrcViewMaxBufferSize", 1024, KviOption_sectFlagIrcView),
	UINT_OPTION("IrcViewToolTipTimeoutInMsec", 1200, KviOption_sectFlagIrcView),
	UINT_OPTION("IrcViewToolTipHideTimeoutInMsec", 12000, KviOption_sectFlagIrcView),
	UINT_OPTION("DccVoicePreBufferSize", 32000, KviOption_sectFlagDcc),
	UINT_OPTION("IdentdPort", 113, KviOption_sectFlagConnection | KviOption_resetRestartIdentd),
	UINT_OPTION("AvatarScaleWidth", 80, KviOption_sectFlagAvatar | KviOption_resetUpdateGui),
	UINT_OPTION("AvatarScaleHeight", 80, KviOption_sectFlagAvatar | KviOption_resetUpdateGui),
	UINT_OPTION("DccMinPort", 30000, KviOption_sectFlagDcc),
	UINT_OPTION("DccMaxPort", 31000, KviOption_sectFlagDcc),
	UINT_OPTION("SoundOfferTimeoutInSecs", 60, KviOption_sectFlagConnection),
	UINT_OPTION("MaximumRequestedAvatarSize", 524288, KviOption_sectFlagAvatar),
	UINT_OPTION("GlobalWindowOpacityPercent", 100, KviOption_sectFlagGui | KviOption_resetUpdatePseudoTransparency | KviOption_groupTheme),
	UINT_OPTION("PasteDelay", 2000, KviOption_sectFlagInput),
	UINT_OPTION("MaxDccSendTransfers", 10, KviOption_sectFlagDcc),
	UINT_OPTION("MaxDccSlots", 64, KviOption_sectFlagDcc),
	UINT_OPTION("TileMethod", 5, KviOption_sectFlagMdi),
	UINT_OPTION("LagMeterHeartbeat", 5000, KviOption_sectFlagConnection | KviOption_resetRestartLagMeter),
	UINT_OPTION("LagAlarmTime", 120000, KviOption_sectFlagConnection),
	UINT_OPTION("WindowListButtonMinWidth", 100, KviOption_sectFlagWindowList),
	UINT_OPTION("OutputVerbosityLevel", KVI_VERBOSITY_LEVEL_NORMAL, KviOption_sectFlagFrame),
	UINT_OPTION("IrcContextAppletWidth", 160, KviOption_sectFlagFrame),
	UINT_OPTION("MinHighlightLevel", 0, KviOption_uintMinHighlightLevel),
	UINT_OPTION("UserListViewGridType", 3, KviOption_sectFlagUserListView | KviOption_resetUpdateGui | KviOption_groupTheme),
	UINT_OPTION("WindowListMaximumRows", 2, KviOption_sectFlagFrame),
	UINT_OPTION("MaxAutoReconnectAttempts", 15, KviOption_sectFlagFrame),
	UINT_OPTION("AutoReconnectDelay", 10, KviOption_sectFlagFrame),
	UINT_OPTION("MaxDccSendSpeed", 0, KviOption_sectFlagFrame),
	UINT_OPTION("MaxDccRecvSpeed", 0, KviOption_sectFlagFrame),
	UINT_OPTION("TimeStampForeground", KviControlCodes::DarkGray, KviOption_sectFlagIrcView | KviOption_resetUpdateGui | KviOption_groupTheme),
	UINT_OPTION("TimeStampBackground", KviControlCodes::Transparent, KviOption_sectFlagIrcView | KviOption_resetUpdateGui | KviOption_groupTheme),
	UINT_OPTION("UserExperienceLevel", 1, KviOption_sectFlagUser),
	UINT_OPTION("ClassicWindowListMaximumButtonWidth", 100, KviOption_sectFlagGeometry | KviOption_resetUpdateGui | KviOption_groupTheme),
	UINT_OPTION("DefaultBanType", 9, KviOption_sectFlagIrcSocket),
	UINT_OPTION("IrcViewPixmapAlign", 0, KviOption_sectFlagIrcView | KviOption_groupTheme),
	UINT_OPTION("UserListPixmapAlign", 0, KviOption_sectFlagFrame | KviOption_groupTheme),
	UINT_OPTION("ToolBarAppletPixmapAlign", 0, KviOption_sectFlagFrame | KviOption_groupTheme),
	UINT_OPTION("TreeWindowListPixmapAlign", 0, KviOption_sectFlagFrame | KviOption_groupTheme),
	UINT_OPTION("InputPixmapAlign", 0, KviOption_sectFlagFrame | KviOption_groupTheme),
	UINT_OPTION("AutoFlushLogs", 30, KviOption_sectFlagFrame),
	UINT_OPTION("MinTrayLowLevelMessage", 1, KviOption_sectFlagFrame),
	UINT_OPTION("MinTrayHighLevelMessage", 4, KviOption_sectFlagFrame),
	UINT_OPTION("IdentdOutputMode", KviIdentdOutputMode::Quiet, KviOption_sectFlagConnection),
	UINT_OPTION("ScaleAvatarsOnLoadHeight", 640, KviOption_sectFlagAvatar),
	UINT_OPTION("ScaleAvatarsOnLoadWidth", 480, KviOption_sectFlagAvatar),
	UINT_OPTION("NotifierActiveTransparency", 100, KviOption_sectFlagNotifier | KviOption_groupTheme),
	UINT_OPTION("NotifierInactiveTransparency", 80, KviOption_sectFlagNotifier | KviOption_groupTheme), // this may be cool but it confuses the user
	UINT_OPTION("IrcViewMarkerStyle", 0, KviOption_sectFlagIrcView | KviOption_groupTheme),
	UINT_OPTION("IrcViewMarkerSize", 1, KviOption_sectFlagIrcView | KviOption_groupTheme),
	UINT_OPTION("UrlMouseClickNum", 2, KviOption_sectFlagUrl),
	UINT_OPTION("NotifierAutoHideTime", 30, KviOption_sectFlagModules),
	UINT_OPTION("LinesToPasteOnChannelJoin", 10, KviOption_sectFlagLogging),
	UINT_OPTION("DaysIntervalToPasteOnChannelJoin", 10, KviOption_sectFlagLogging),
	UINT_OPTION("LinesToPasteOnQueryJoin", 10, KviOption_sectFlagLogging),
	UINT_OPTION("DaysIntervalToPasteOnQueryJoin", 10, KviOption_sectFlagLogging),
	UINT_OPTION("SpacesToExpandTabulationInput", 4, KviOption_sectFlagInput),
	UINT_OPTION("UserIrcViewOwnForeground", 4, KviOption_sectFlagIrcView | KviOption_resetUpdateGui | KviOption_groupTheme),
	UINT_OPTION("UserIrcViewOwnBackground", 8, KviOption_sectFlagIrcView | KviOption_resetUpdateGui | KviOption_groupTheme),
	UINT_OPTION("NotifierPixmapAlign", 0, KviOption_sectFlagNotifier | KviOption_groupTheme),
	UINT_OPTION("OutputDatetimeFormat", 0, KviOption_sectFlagIrcView),
	UINT_OPTION("OnJoinRequestsDelay", 1, KviOption_sectFlagConnection), // FIXME: Wouldn't this be nicer in msecs defaulting to 100-200 ?
	UINT_OPTION("ToolBarIconSize", 22, KviOption_groupTheme | KviOption_resetReloadImages),
	UINT_OPTION("ToolBarButtonStyle", 0, KviOption_groupTheme), // 0 = Qt::ToolButtonIconOnly
	UINT_OPTION("MaximumBlowFishKeySize", 56, KviOption_sectFlagNone),
	UINT_OPTION("CustomCursorWidth", 1, KviOption_resetUpdateGui),
	UINT_OPTION("UserListMinimumWidth", 100, KviOption_sectFlagUserListView | KviOption_resetUpdateGui | KviOption_groupTheme)
};

#define FONT_OPTION(_name, _face, _size, _flags) \
	KviFontOption(                               \
	    KVI_FONT_OPTIONS_PREFIX _name,           \
	    QFont(_face, _size),                     \
	    _flags | KviOption_groupTheme)

KviFontOption g_fontOptionsTable[KVI_NUM_FONT_OPTIONS] = {
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	FONT_OPTION("IrcView", "Arial", 9, KviOption_sectFlagIrcView | KviOption_resetUpdateGui),
	FONT_OPTION("Input", "Arial", 12, KviOption_sectFlagInput | KviOption_resetUpdateGui),
	FONT_OPTION("UserListView", "Arial", 9, KviOption_sectFlagUserListView | KviOption_resetUpdateGui),
	FONT_OPTION("Label", "Arial", 9, KviOption_sectFlagLabel | KviOption_resetUpdateGui),
	FONT_OPTION("Application", "Arial", 9, KviOption_sectFlagGui | KviOption_resetUpdateAppFont),
	FONT_OPTION("WindowList", "Arial", 9, KviOption_sectFlagWindowList | KviOption_resetUpdateWindowList),
	FONT_OPTION("TreeWindowList", "Arial", 9, KviOption_sectFlagWindowList | KviOption_resetUpdateWindowList),
	FONT_OPTION("Notifier", "Arial", 9, KviOption_sectFlagNotifier | KviOption_resetUpdateGui),
	FONT_OPTION("NotifierTitle", "Arial", 9, KviOption_sectFlagNotifier | KviOption_resetUpdateGui)
#elif defined(COMPILE_ON_MAC)
	FONT_OPTION("IrcView", "Lucida Grande", 12, KviOption_sectFlagIrcView | KviOption_resetUpdateGui),
	FONT_OPTION("Input", "Lucida Grande", 12, KviOption_sectFlagInput | KviOption_resetUpdateGui),
	FONT_OPTION("UserListView", "Lucida Grande", 12, KviOption_sectFlagUserListView | KviOption_resetUpdateGui),
	FONT_OPTION("Label", "Lucida Grande", 12, KviOption_sectFlagLabel | KviOption_resetUpdateGui),
	FONT_OPTION("Application", "Lucida Grande", 12, KviOption_sectFlagGui | KviOption_resetUpdateAppFont),
	FONT_OPTION("WindowList", "Lucida Grande", 12, KviOption_sectFlagWindowList | KviOption_resetUpdateWindowList),
	FONT_OPTION("TreeWindowList", "Lucida Grande", 12, KviOption_sectFlagWindowList | KviOption_resetUpdateWindowList),
	FONT_OPTION("Notifier", "Lucida Grande", 12, KviOption_sectFlagNotifier | KviOption_resetUpdateNotifier),
	FONT_OPTION("NotifierTitle", "Lucida Grande", 12, KviOption_sectFlagNotifier | KviOption_resetUpdateNotifier)
#else
	FONT_OPTION("IrcView", "sans-serif", 10, KviOption_sectFlagIrcView | KviOption_resetUpdateGui),
	FONT_OPTION("Input", "sans-serif", 10, KviOption_sectFlagInput | KviOption_resetUpdateGui),
	FONT_OPTION("UserListView", "sans-serif", 10, KviOption_sectFlagUserListView | KviOption_resetUpdateGui),
	FONT_OPTION("Label", "sans-serif", 10, KviOption_sectFlagLabel | KviOption_resetUpdateGui),
	FONT_OPTION("Application", "sans-serif", 10, KviOption_sectFlagGui | KviOption_resetUpdateAppFont),
	FONT_OPTION("WindowList", "sans-serif", 10, KviOption_sectFlagWindowList | KviOption_resetUpdateWindowList),
	FONT_OPTION("TreeWindowList", "sans-serif", 10, KviOption_sectFlagWindowList | KviOption_resetUpdateWindowList),
	FONT_OPTION("Notifier", "sans-serif", 10, KviOption_sectFlagNotifier | KviOption_resetUpdateNotifier),
	FONT_OPTION("NotifierTitle", "sans-serif", 10, KviOption_sectFlagNotifier | KviOption_resetUpdateNotifier)
#endif
};

#define MSGTYPE_OPTION(_name, _text, _icon, _levl)                                                               \
	KviMessageTypeSettingsOption(                                                                                \
	    KVI_MSGTYPE_OPTIONS_PREFIX _name,                                                                        \
	    KviMessageTypeSettings(_text, _icon, KviControlCodes::Black, KviControlCodes::Transparent, true, _levl), \
	    KviOption_sectFlagMsgType | KviOption_groupTheme)

#define MSGTYPE_OPTION_SPEC(_name, _text, _icon, _fore, _back, _levl)    \
	KviMessageTypeSettingsOption(                                        \
	    KVI_MSGTYPE_OPTIONS_PREFIX _name,                                \
	    KviMessageTypeSettings(_text, _icon, _fore, _back, true, _levl), \
	    KviOption_sectFlagMsgType | KviOption_groupTheme)

// FIXME: #warning "FIX THE ICONS HERE!!!"
KviMessageTypeSettingsOption g_msgtypeOptionsTable[KVI_NUM_MSGTYPE_OPTIONS] = {
	MSGTYPE_OPTION("Echo", __tr_no_lookup("Normal text"), KviIconManager::None, KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION_SPEC("Selection", __tr_no_lookup("Selection"), KviIconManager::None, KviControlCodes::White, KviControlCodes::Black, KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION_SPEC("Highlight", __tr_no_lookup("Highlighted text"), KviIconManager::HighlightText, KviControlCodes::White, KviControlCodes::Black, KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION_SPEC("URL", __tr_no_lookup("URL foreground"), KviIconManager::Url, KviControlCodes::Blue, KviControlCodes::Transparent, KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION_SPEC("Link", __tr_no_lookup("Link mouse hover foreground"), KviIconManager::Url, KviControlCodes::Blue, KviControlCodes::Transparent, KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION_SPEC("ParserError", __tr_no_lookup("Parser error"), KviIconManager::ParserError, KviControlCodes::Red, KviControlCodes::Transparent, KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION_SPEC("ParserWarning", __tr_no_lookup("Parser warning"), KviIconManager::ParserWarning, KviControlCodes::Red, KviControlCodes::Transparent, KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("HostLookup", __tr_no_lookup("Host lookup result"), KviIconManager::Server, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("SocketMessage", __tr_no_lookup("Socket message"), KviIconManager::SocketMessage, KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("SocketWarning", __tr_no_lookup("Socket warning"), KviIconManager::SocketWarning, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION_SPEC("SocketError", __tr_no_lookup("Socket error"), KviIconManager::SocketError, KviControlCodes::Red, KviControlCodes::Transparent, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION_SPEC("SystemError", __tr_no_lookup("System error"), KviIconManager::SystemError, KviControlCodes::Red, KviControlCodes::Transparent, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("RAW", __tr_no_lookup("RAW data to server"), KviIconManager::Raw, KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("Connection", __tr_no_lookup("Connection status"), KviIconManager::World, KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("SystemWarning", __tr_no_lookup("System warning"), KviIconManager::SystemWarning, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("SystemMessage", __tr_no_lookup("System message"), KviIconManager::SystemMessage, KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("UnhandledReply", __tr_no_lookup("Unhandled server reply"), KviIconManager::UnHandled, KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("ServerInfo", __tr_no_lookup("Server information"), KviIconManager::ServerInfo, KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("MOTD", __tr_no_lookup("Server message of the day"), KviIconManager::Motd, KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("ServerPing", __tr_no_lookup("Server ping"), KviIconManager::ServerPing, KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("Join", __tr_no_lookup("Join message"), KviIconManager::Join, KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("Part", __tr_no_lookup("Part message"), KviIconManager::Part, KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("Unrecognized", __tr_no_lookup("Unrecognized/broken message"), KviIconManager::Unrecognized, KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("Topic", __tr_no_lookup("Topic message"), KviIconManager::Topic, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("OwnPrivateMessage", __tr_no_lookup("Own private message"), KviIconManager::OwnPrivMsg, KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("ChannelPrivateMessage", __tr_no_lookup("Channel private message"), KviIconManager::ChanPrivMsg, KVI_MSGTYPE_LEVEL_4),
	MSGTYPE_OPTION("QueryPrivateMessage", __tr_no_lookup("Query private message"), KviIconManager::QueryPrivMsg, KVI_MSGTYPE_LEVEL_4),
	MSGTYPE_OPTION("CTCPReply", __tr_no_lookup("CTCP reply"), KviIconManager::CtcpReply, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("CTCPRequestReplied", __tr_no_lookup("CTCP request replied"), KviIconManager::CtcpRequestReplied, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("CTCPRequestIgnored", __tr_no_lookup("CTCP request ignored"), KviIconManager::CtcpRequestIgnored, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("CTCPRequestFlood", __tr_no_lookup("CTCP request flood warning"), KviIconManager::CtcpRequestFlood, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("CTCPRequestUnknown", __tr_no_lookup("CTCP request unknown"), KviIconManager::CtcpRequestUnknown, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("Action", __tr_no_lookup("User action"), KviIconManager::Action, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("AvatarChange", __tr_no_lookup("Avatar change"), KviIconManager::Avatar, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("Quit", __tr_no_lookup("Quit message"), KviIconManager::Quit, KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("Split", __tr_no_lookup("Split message"), KviIconManager::Split, KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("QuitSplit", __tr_no_lookup("Quit on netsplit message"), KviIconManager::QuitSplit, KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("Nick", __tr_no_lookup("Nick changes"), KviIconManager::Nick, KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("Op", __tr_no_lookup("+o mode change"), KviIconManager::Op, KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("Deop", __tr_no_lookup("-o mode change"), KviIconManager::DeOp, KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("Voice", __tr_no_lookup("+v mode change"), KviIconManager::Voice, KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("Devoice", __tr_no_lookup("-v mode change"), KviIconManager::DeVoice, KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("Mode", __tr_no_lookup("Multiple user mode change"), KviIconManager::Mode, KVI_MSGTYPE_LEVEL_2), //This is a generic icon for both user/chan
	MSGTYPE_OPTION("Key", __tr_no_lookup("Channel key change"), KviIconManager::Key, KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("Limit", __tr_no_lookup("Channel limit change"), KviIconManager::Limit, KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("Ban", __tr_no_lookup("+b mode change"), KviIconManager::Ban, KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("Unban", __tr_no_lookup("-b mode change"), KviIconManager::UnBan, KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("BanExcept", __tr_no_lookup("+e mode change"), KviIconManager::BanExcept, KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("BanUnexcept", __tr_no_lookup("-e mode change"), KviIconManager::BanUnExcept, KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("InviteExcept", __tr_no_lookup("+I mode change"), KviIconManager::InviteExcept, KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("InviteUnexcept", __tr_no_lookup("-I mode change"), KviIconManager::InviteUnExcept, KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("ChanMode", __tr_no_lookup("Multiple channel mode change"), KviIconManager::Mode, KVI_MSGTYPE_LEVEL_2), //This is a generic icon for both user/chan
	MSGTYPE_OPTION("WHO", __tr_no_lookup("Who reply"), KviIconManager::Who, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("DCCRequest", __tr_no_lookup("DCC request"), KviIconManager::DccRequest, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("DCCMsg", __tr_no_lookup("DCC message"), KviIconManager::DccMsg, KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("DCCError", __tr_no_lookup("DCC error"), KviIconManager::DccError, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("NicknameProblem", __tr_no_lookup("Nickname problem"), KviIconManager::NickNameProblem, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("WHOISUser", __tr_no_lookup("WHOIS user reply"), KviIconManager::WhoisUser, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("WHOISChannels", __tr_no_lookup("WHOIS channels reply"), KviIconManager::WhoisChannels, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("WHOISIdle", __tr_no_lookup("WHOIS idle reply"), KviIconManager::WhoisIdle, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("WHOISServer", __tr_no_lookup("WHOIS server reply"), KviIconManager::WhoisServer, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("WHOISOther", __tr_no_lookup("WHOIS other reply"), KviIconManager::WhoisOther, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("CreationTime", __tr_no_lookup("Channel creation time reply"), KviIconManager::Time, KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("NotifyOnLine", __tr_no_lookup("Notify list joins"), KviIconManager::NotifyOnLine, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("NotifyOffLine", __tr_no_lookup("Notify list leaves"), KviIconManager::NotifyOffLine, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("OwnPrivmsgCrypted", __tr_no_lookup("Own encrypted private message"), KviIconManager::OwnPrivMsgCrypted, KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("ChanPrivmsgCrypted", __tr_no_lookup("Channel encrypted private message"), KviIconManager::ChanPrivMsgCrypted, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("QueryPrivmsgCrypted", __tr_no_lookup("Query encrypted private message"), KviIconManager::QueryPrivMsgCrypted, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("DCCChatMsg", __tr_no_lookup("DCC chat message"), KviIconManager::DccChatMsg, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("DCCChatMsgCrypted", __tr_no_lookup("Encrypted DCC chat message"), KviIconManager::DccChatMsgCrypted, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("IRC", __tr_no_lookup("Login operations completed"), KviIconManager::Irc, KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("Kick", __tr_no_lookup("Kick action"), KviIconManager::Kick, KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("Links", __tr_no_lookup("Links reply"), KviIconManager::Links, KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("Spam", __tr_no_lookup("Spam report"), KviIconManager::Spam, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("ICQ", __tr_no_lookup("ICQ message"), KviIconManager::Icq, KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("ICQMessage", __tr_no_lookup("ICQ user-message"), KviIconManager::Message, KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("ICQMessageSent", __tr_no_lookup("Outgoing ICQ user-message"), KviIconManager::MessageSent, KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("ChannelNotice", __tr_no_lookup("Channel notice"), KviIconManager::ChannelNotice, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("ChannelNoticeCrypted", __tr_no_lookup("Encrypted channel notice"), KviIconManager::ChannelNoticeCrypted, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("QueryNotice", __tr_no_lookup("Query notice"), KviIconManager::QueryNotice, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("QueryNoticeCrypted", __tr_no_lookup("Encrypted query notice"), KviIconManager::QueryNoticeCrypted, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("ServerNotice", __tr_no_lookup("Server notice"), KviIconManager::ServerNotice, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("HalfOp", __tr_no_lookup("Halfop mode change"), KviIconManager::HalfOp, KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("CTCPReplyUnknown", __tr_no_lookup("Unknown CTCP reply"), KviIconManager::CtcpReplyUnknown, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("NickServ", __tr_no_lookup("NickServ message"), KviIconManager::NickServ, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("ChanServ", __tr_no_lookup("ChanServ message"), KviIconManager::ChanServ, KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("Away", __tr_no_lookup("Away message"), KviIconManager::Away, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("Ident", __tr_no_lookup("Ident message"), KviIconManager::Ident, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("List", __tr_no_lookup("Channel list message"), KviIconManager::List, KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("HalfDeOp", __tr_no_lookup("Half-deop mode change"), KviIconManager::HalfDeOp, KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("Invite", __tr_no_lookup("Invite message"), KviIconManager::Invite, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("Multimedia", __tr_no_lookup("Multimedia message"), KviIconManager::MultiMedia, KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("QueryTrace", __tr_no_lookup("Query trace message"), KviIconManager::QueryTrace, KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("Wallops", __tr_no_lookup("Wallops message"), KviIconManager::WallOps, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("JoinError", __tr_no_lookup("Join error message"), KviIconManager::NoChannel, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("BroadcastPrivmsg", __tr_no_lookup("Broadcast private message"), KviIconManager::BroadcastPrivMsg, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("BroadcastNotice", __tr_no_lookup("Broadcast notice"), KviIconManager::BroadcastNotice, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("MeKick", __tr_no_lookup("Am kicked"), KviIconManager::MeKick, KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("MeOp", __tr_no_lookup("Am op'd"), KviIconManager::MeOp, KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("MeVoice", __tr_no_lookup("Am voiced"), KviIconManager::MeVoice, KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("MeDeOp", __tr_no_lookup("Am deop'd"), KviIconManager::MeDeOp, KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("MeDeVoice", __tr_no_lookup("Am devoiced"), KviIconManager::MeDeVoice, KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("MeHalfOp", __tr_no_lookup("Am halfop'd"), KviIconManager::MeHalfOp, KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("MeDeHalfOp", __tr_no_lookup("Am de-halfop'd"), KviIconManager::MeDeHalfOp, KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("MeBan", __tr_no_lookup("Ban matching my mask"), KviIconManager::MeBan, KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("MeUnban", __tr_no_lookup("Unban matching my mask"), KviIconManager::MeUnBan, KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("MeBanExcept", __tr_no_lookup("Ban exception matching my mask"), KviIconManager::MeBanExcept, KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("MeBanUnExcept", __tr_no_lookup("Ban unexception matching my mask"), KviIconManager::MeBanUnExcept, KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("MeInviteExcept", __tr_no_lookup("Invite exception matching my mask"), KviIconManager::MeInviteExcept, KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("MeInviteUnexcept", __tr_no_lookup("Invite unexception matching my mask"), KviIconManager::MeInviteUnExcept, KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("Ignore", __tr_no_lookup("Ignored user message"), KviIconManager::Ignore, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("Stats", __tr_no_lookup("Server statistics"), KviIconManager::Stats, KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("SSL", __tr_no_lookup("SSL message"), KviIconManager::Ssl, KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("Search", __tr_no_lookup("Search"), KviIconManager::Search, KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("GenericSuccess", __tr_no_lookup("Generic success"), KviIconManager::CyanSquare, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("GenericStatus", __tr_no_lookup("Generic status"), KviIconManager::GreenSquare, KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("GenericVerbose", __tr_no_lookup("Generic verbose message"), KviIconManager::DarkGreenSquare, KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("GenericWarning", __tr_no_lookup("Generic warning"), KviIconManager::YellowSquare, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION_SPEC("GenericError", __tr_no_lookup("Generic error"), KviIconManager::RedSquare, KviControlCodes::Red, KviControlCodes::Transparent, KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION_SPEC("GenericCritical", __tr_no_lookup("Generic critical error"), KviIconManager::BlackSquare, KviControlCodes::Red, KviControlCodes::Transparent, KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("ChanAdmin", __tr_no_lookup("Chan admin status set"), KviIconManager::ChanAdmin, KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("ChanUnAdmin", __tr_no_lookup("Chan admin status unset"), KviIconManager::ChanUnAdmin, KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("MeChanAdmin", __tr_no_lookup("Own chan admin status set"), KviIconManager::MeChanAdmin, KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("MeChanUnAdmin", __tr_no_lookup("Own chan admin status unset"), KviIconManager::MeChanUnAdmin, KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("UserOp", __tr_no_lookup("Userop mode change"), KviIconManager::UserOp, KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("UserDeOp", __tr_no_lookup("User-deop mode change"), KviIconManager::DeUserOp, KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("MeUserOp", __tr_no_lookup("Am userop'd"), KviIconManager::MeUserOp, KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("MeDeUserOp", __tr_no_lookup("Am de-userop'd"), KviIconManager::MeDeUserOp, KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION_SPEC("Verbose", __tr_no_lookup("Verbose/Debug"), KviIconManager::BinaryText, KviControlCodes::DarkGray, KviControlCodes::Transparent, KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("ChanOwner", __tr_no_lookup("Chan owner status set"), KviIconManager::ChanOwner, KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("ChanUnOwner", __tr_no_lookup("Chan owner status unset"), KviIconManager::ChanUnOwner, KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("MeChanOwner", __tr_no_lookup("Own chan owner status set"), KviIconManager::MeChanOwner, KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("MeChanUnOwner", __tr_no_lookup("Own chan owner status unset"), KviIconManager::MeChanUnOwner, KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION_SPEC("ServerError", __tr_no_lookup("Server error"), KviIconManager::ServerError, KviControlCodes::Red, KviControlCodes::Transparent, KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION_SPEC("Help", __tr_no_lookup("Help"), KviIconManager::Help, KviControlCodes::DarkGreen, KviControlCodes::Transparent, KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("TextEncoding", __tr_no_lookup("Text encoding server message"), KviIconManager::TextEncoding, KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("BitTorrent", __tr_no_lookup("BitTorrent message"), KviIconManager::Gnutella, KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("IrcOp", __tr_no_lookup("IRC Op status set"), KviIconManager::IrcOp, KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("DeIrcOp", __tr_no_lookup("IRC Op status unset"), KviIconManager::DeIrcOp, KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("MeIrcOp", __tr_no_lookup("Own IRC Op status set"), KviIconManager::MeIrcOp, KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("MeDeIrcOp", __tr_no_lookup("Own IRC Op status unset"), KviIconManager::MeDeIrcOp, KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("Cap", __tr_no_lookup("Extended capability message"), KviIconManager::ServerInfo, KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("ChanURL", __tr_no_lookup("Channel URL"), KviIconManager::Url, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("MemoServ", __tr_no_lookup("MemoServ message"), KviIconManager::MemoServ, KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("Log", __tr_no_lookup("Log message"), KviIconManager::Log, KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("ActionCrypted", __tr_no_lookup("Encrypted user action"), KviIconManager::ActionCrypted, KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("OwnAction", __tr_no_lookup("Own action"), KviIconManager::OwnAction, KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("OwnActionCrypted", __tr_no_lookup("Own encrypted action"), KviIconManager::OwnActionCrypted, KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("TopicCrypted", __tr_no_lookup("Encrypted topic message"), KviIconManager::TopicCrypted, KVI_MSGTYPE_LEVEL_3),
};

static const char * options_section_table[KVI_NUM_OPTION_SECT_FLAGS] = {
	"None", "Frame", "Mdi", "Windows",
	"WindowList", "IrcView", "MsgType", "MircColor",
	"Input", "UserParser", "Connection", "IrcSocket",
	"User", "UserListView", "Label", "Modules",
	"Ctcp", "Dcc", "Gui", "IrcToolBar",
	"Logging", "AntiSpam", "Avatar", "Url",
	"Recent", "Geometry", "Notifier"
};

static void config_set_section(int flag, KviConfigurationFile * cfg)
{
	int index = flag & KviOption_sectMask;
	if((index < KVI_NUM_OPTION_SECT_FLAGS) && (index >= 0))
		cfg->setGroup(options_section_table[index]);
	else
		cfg->setGroup(""); // Default group
}

void KviApplication::loadOptions()
{
	QString buffer;
	if(getReadOnlyConfigPath(buffer, KVI_CONFIGFILE_MAIN))
	{
		KviConfigurationFile cfg(buffer, KviConfigurationFile::Read);

		int i;

#define READ_OPTIONS(_num, _table, _readFnc)                               \
	for(i = 0; i < _num; i++)                                              \
	{                                                                      \
		config_set_section(_table[i].flags, &cfg);                         \
		_table[i].option = cfg._readFnc(_table[i].name, _table[i].option); \
	}

		READ_OPTIONS(KVI_NUM_RECT_OPTIONS, g_rectOptionsTable, readRectEntry)
		READ_OPTIONS(KVI_NUM_BOOL_OPTIONS, g_boolOptionsTable, readBoolEntry)
		READ_OPTIONS(KVI_NUM_STRING_OPTIONS, g_stringOptionsTable, readEntry)
		READ_OPTIONS(KVI_NUM_COLOR_OPTIONS, g_colorOptionsTable, readColorEntry)
		READ_OPTIONS(KVI_NUM_PIXMAP_OPTIONS, g_pixmapOptionsTable, readPixmapEntry)
		READ_OPTIONS(KVI_NUM_UINT_OPTIONS, g_uintOptionsTable, readUIntEntry)
		READ_OPTIONS(KVI_NUM_FONT_OPTIONS, g_fontOptionsTable, readFontEntry)
		READ_OPTIONS(KVI_NUM_MSGTYPE_OPTIONS, g_msgtypeOptionsTable, readMsgTypeEntry)
		READ_OPTIONS(KVI_NUM_STRINGLIST_OPTIONS, g_stringlistOptionsTable, readStringListEntry)
		READ_OPTIONS(KVI_NUM_MIRCCOLOR_OPTIONS, g_mirccolorOptionsTable, readColorEntry)
		READ_OPTIONS(KVI_NUM_ICCOLOR_OPTIONS, g_iccolorOptionsTable, readColorEntry)

		for(i = 0; i < KVI_NUM_STRING_OPTIONS; i++)
		{
			if(g_stringOptionsTable[i].flags & KviOption_encodePath)
				KviStringConversion::decodePath(g_stringOptionsTable[i].option);
		}

		for(i = 0; i < KVI_NUM_STRINGLIST_OPTIONS; i++)
		{
			if(g_stringlistOptionsTable[i].flags & KviOption_encodePath)
				KviStringConversion::decodePath(g_stringlistOptionsTable[i].option);
		}

#undef READ_OPTIONS
	} // else already have the defaults
}

void KviApplication::saveOptions()
{
	QString buffer;

	saveRecentChannels();

	getLocalKvircDirectory(buffer, Config, KVI_CONFIGFILE_MAIN);

	KviConfigurationFile cfg(buffer, KviConfigurationFile::Write);

	int i;

#define WRITE_OPTIONS(_num, _table)                       \
	for(i = 0; i < _num; i++)                             \
	{                                                     \
		config_set_section(_table[i].flags, &cfg);        \
		cfg.writeEntry(_table[i].name, _table[i].option); \
	}

	WRITE_OPTIONS(KVI_NUM_RECT_OPTIONS, g_rectOptionsTable)
	WRITE_OPTIONS(KVI_NUM_BOOL_OPTIONS, g_boolOptionsTable)

	for(i = 0; i < KVI_NUM_STRING_OPTIONS; i++)
	{
		if(g_stringOptionsTable[i].flags & KviOption_encodePath)
			KviStringConversion::encodePath(g_stringOptionsTable[i].option);
	}

	WRITE_OPTIONS(KVI_NUM_STRING_OPTIONS, g_stringOptionsTable)
	for(i = 0; i < KVI_NUM_STRING_OPTIONS; i++)
	{
		if(g_stringOptionsTable[i].flags & KviOption_encodePath)
			KviStringConversion::decodePath(g_stringOptionsTable[i].option);
	}

	WRITE_OPTIONS(KVI_NUM_COLOR_OPTIONS, g_colorOptionsTable)

	WRITE_OPTIONS(KVI_NUM_PIXMAP_OPTIONS, g_pixmapOptionsTable)
	WRITE_OPTIONS(KVI_NUM_UINT_OPTIONS, g_uintOptionsTable)
	WRITE_OPTIONS(KVI_NUM_FONT_OPTIONS, g_fontOptionsTable)
	WRITE_OPTIONS(KVI_NUM_MSGTYPE_OPTIONS, g_msgtypeOptionsTable)

	for(i = 0; i < KVI_NUM_STRINGLIST_OPTIONS; i++)
	{
		if(g_stringlistOptionsTable[i].flags & KviOption_encodePath)
			KviStringConversion::encodePath(g_stringlistOptionsTable[i].option);
	}

	WRITE_OPTIONS(KVI_NUM_STRINGLIST_OPTIONS, g_stringlistOptionsTable)
	for(i = 0; i < KVI_NUM_STRINGLIST_OPTIONS; i++)
	{
		if(g_stringlistOptionsTable[i].flags & KviOption_encodePath)
			KviStringConversion::decodePath(g_stringlistOptionsTable[i].option);
	}
	WRITE_OPTIONS(KVI_NUM_MIRCCOLOR_OPTIONS, g_mirccolorOptionsTable)
	WRITE_OPTIONS(KVI_NUM_ICCOLOR_OPTIONS, g_iccolorOptionsTable)

	if(!cfg.saveIfDirty())
	{
		QMessageBox::warning(nullptr, __tr2qs("Warning While Writing Configuration - KVIrc"),
		    __tr2qs("I can't write to the main configuration file:\n\t%1\nPlease ensure the directory exists and that you have the proper permissions before continuing, "
		            "or else any custom configuration will be lost.")
		        .arg(buffer));
	}

#undef WRITE_OPTIONS
}

#undef WRITE_OPTIONS

#define WRITE_OPTIONS(_num, _table)                           \
	for(i = 0; i < _num; i++)                                 \
	{                                                         \
		if(_table[i].flags & KviOption_groupTheme)            \
			cfg.writeEntry(_table[i].name, _table[i].option); \
	}

namespace KviTheme
{
	// utility functions for the KviTheme namespace (KviTheme.h)
	// that are implemented here for convenience (in saving the options)
	bool save(KviThemeInfo & options, bool bSaveIcons)
	{
		QString szThemeDirPath = options.directory();

		if(szThemeDirPath.isEmpty())
		{
			options.setLastError(__tr2qs("Missing absolute directory for the theme information"));
			return false;
		}

		if(!KviFileUtils::directoryExists(szThemeDirPath))
		{
			if(!KviFileUtils::makeDir(szThemeDirPath))
			{
				options.setLastError(__tr2qs("Failed to create the theme directory"));
				return false;
			}
		}

		if(!options.save(szThemeDirPath + KVI_THEMEINFO_FILE_NAME))
			return false;

		KviConfigurationFile cfg(szThemeDirPath + KVI_THEMEDATA_FILE_NAME, KviConfigurationFile::Write);

		cfg.setGroup(KVI_THEMEDATA_CONFIG_GROUP);

		int i;

		WRITE_OPTIONS(KVI_NUM_RECT_OPTIONS, g_rectOptionsTable)
		WRITE_OPTIONS(KVI_NUM_BOOL_OPTIONS, g_boolOptionsTable)
		WRITE_OPTIONS(KVI_NUM_STRING_OPTIONS, g_stringOptionsTable)
		WRITE_OPTIONS(KVI_NUM_COLOR_OPTIONS, g_colorOptionsTable)
		WRITE_OPTIONS(KVI_NUM_UINT_OPTIONS, g_uintOptionsTable)
		WRITE_OPTIONS(KVI_NUM_FONT_OPTIONS, g_fontOptionsTable)
		WRITE_OPTIONS(KVI_NUM_MSGTYPE_OPTIONS, g_msgtypeOptionsTable)
		WRITE_OPTIONS(KVI_NUM_STRINGLIST_OPTIONS, g_stringlistOptionsTable)
		WRITE_OPTIONS(KVI_NUM_MIRCCOLOR_OPTIONS, g_mirccolorOptionsTable)
		WRITE_OPTIONS(KVI_NUM_ICCOLOR_OPTIONS, g_iccolorOptionsTable)

#undef WRITE_OPTIONS

		// the pixmap options need special processing
		for(i = 0; i < KVI_NUM_PIXMAP_OPTIONS; i++)
		{
			if(g_pixmapOptionsTable[i].flags & KviOption_groupTheme)
			{
				if(g_pixmapOptionsTable[i].option.pixmap())
				{
					QString szPixPath = szThemeDirPath;
					QString szPixName = g_pixmapOptionsTable[i].name;
					szPixName += ".png";
					szPixPath += szPixName;

					if(g_pixmapOptionsTable[i].option.pixmap()->save(szPixPath, "PNG"))
					{
						cfg.writeEntry(g_pixmapOptionsTable[i].name, szPixName);
					}
					else
					{
						// we ignore this error for now
						qDebug("failed to save %s", szPixPath.toUtf8().data());
						cfg.writeEntry(g_pixmapOptionsTable[i].name, "");
					}
				}
				else
				{
					cfg.writeEntry(g_pixmapOptionsTable[i].name, "");
				}
			}
		}

		cfg.writeEntry("stringIconThemeSubdir", options.subdirectory());

		if(bSaveIcons)
		{

			// find all the "kvi_bigicon" images that we can find in the main pics directory
			QString szPicsPath;

			g_pApp->getGlobalKvircDirectory(szPicsPath, KviApplication::Pics);

			QDir d(szPicsPath);
			QStringList sl = d.entryList(QDir::nameFiltersFromString("kvi_bigicon_*.png"), QDir::Files);

			for(auto & it : sl)
			{
				KviCachedPixmap * p = g_pIconManager->getPixmapWithCache(it);
				if(p)
				{
					QString szPixPath = szThemeDirPath;
					szPixPath += it;

					if(!KviFileUtils::copyFile(p->path(), szPixPath))
					{
						qDebug("Failed to copy file %s into %s", p->path().toUtf8().data(), szPixPath.toUtf8().data());
						options.setLastError(__tr2qs("Failed to save one of the theme images"));
						return false;
					}
				}
			}

			if(!KviFileUtils::makeDir(szThemeDirPath + KVI_SMALLICONS_SUBDIRECTORY))
			{
				options.setLastError(__tr2qs("Failed to create the theme subdirectory"));
				return false;
			}

			// We actually need to *save* the small icons since
			// we have a compatibility mode that can load them from
			// the old format kvi_smallicon_XY.png multiimage libraries.
			for(int j = 0; j < KviIconManager::IconCount; j++)
			{
				QPixmap * pix = g_pIconManager->getSmallIcon(j);

				QString szPixPath = szThemeDirPath;
				szPixPath.append(KVI_SMALLICONS_SUBDIRECTORY);
				szPixPath.append(KVI_PATH_SEPARATOR_CHAR);
				szPixPath.append("kcs_");
				szPixPath.append(g_pIconManager->getSmallIconName(j));
				szPixPath.append(".png");

				if(!pix->isNull() && !pix->save(szPixPath, "PNG", 90))
				{
					qDebug("Failed to save small icon %d into %s", j, szPixPath.toUtf8().data());
					options.setLastError(__tr2qs("Failed to save one of the theme images"));
					return false;
				}
			}
		}

		return true;
	}

	bool apply(const QString & szThemeDir, KviThemeInfo::Location eLocation, KviThemeInfo & buffer)
	{
		//qDebug("Loading and apply theme");
		if(!buffer.load(szThemeDir, eLocation))
		{
			qDebug("data does not exist in theme dir %s at location %d", szThemeDir.toUtf8().data(), eLocation);
			return false; // makes sure that themedata exists too
		}

		QString szThemeDirPath = buffer.directory();

		//qDebug("Ok loading from %s",szThemeDirPath.toUtf8().data());
		// reset the current theme subdir
		KVI_OPTION_STRING(KviOption_stringIconThemeSubdir) = "";

		KviConfigurationFile cfg(szThemeDirPath + KVI_THEMEDATA_FILE_NAME, KviConfigurationFile::Read);
		QString t = szThemeDirPath + KVI_THEMEDATA_FILE_NAME;
		cfg.setGroup(KVI_THEMEDATA_CONFIG_GROUP);

		int i;
		int iResetFlags = 0;
#ifdef COMPILE_ON_MAC
		bool bWerePopupMenuIconsEnabled = KVI_OPTION_BOOL(KviOption_boolShowIconsInPopupMenus);
#endif

#undef READ_OPTIONS

#define READ_OPTIONS(_num, _table, _readFnc)                                       \
	for(i = 0; i < _num; i++)                                                      \
	{                                                                              \
		if(_table[i].flags & KviOption_groupTheme)                                 \
		{                                                                          \
			if(cfg.hasKey(_table[i].name))                                         \
			{                                                                      \
				iResetFlags |= (_table[i].flags & KviOption_resetMask);            \
				_table[i].option = cfg._readFnc(_table[i].name, _table[i].option); \
			}                                                                      \
		}                                                                          \
	}

#define READ_ALL_OPTIONS(_num, _table, _readFnc)                               \
	for(i = 0; i < _num; i++)                                                  \
	{                                                                          \
		if(cfg.hasKey(_table[i].name))                                         \
		{                                                                      \
			iResetFlags |= (_table[i].flags & KviOption_resetMask);            \
			_table[i].option = cfg._readFnc(_table[i].name, _table[i].option); \
		}                                                                      \
	}

		READ_OPTIONS(KVI_NUM_RECT_OPTIONS, g_rectOptionsTable, readRectEntry)
		READ_OPTIONS(KVI_NUM_BOOL_OPTIONS, g_boolOptionsTable, readBoolEntry)
		READ_OPTIONS(KVI_NUM_STRING_OPTIONS, g_stringOptionsTable, readEntry)
		READ_OPTIONS(KVI_NUM_COLOR_OPTIONS, g_colorOptionsTable, readColorEntry)
		READ_OPTIONS(KVI_NUM_UINT_OPTIONS, g_uintOptionsTable, readUIntEntry)
		READ_OPTIONS(KVI_NUM_FONT_OPTIONS, g_fontOptionsTable, readFontEntry)
		READ_ALL_OPTIONS(KVI_NUM_MSGTYPE_OPTIONS, g_msgtypeOptionsTable, readMsgTypeEntry)
		READ_OPTIONS(KVI_NUM_STRINGLIST_OPTIONS, g_stringlistOptionsTable, readStringListEntry)
		READ_OPTIONS(KVI_NUM_MIRCCOLOR_OPTIONS, g_mirccolorOptionsTable, readColorEntry)
		READ_OPTIONS(KVI_NUM_ICCOLOR_OPTIONS, g_iccolorOptionsTable, readColorEntry)

#undef READ_OPTIONS
#undef READ_ALL_OPTIONS

#ifdef COMPILE_ON_MAC
		/* disregard what the theme says and apply no icons on menus if they
			   weren't enabled before */
		if(!bWerePopupMenuIconsEnabled)
		{
			KVI_OPTION_BOOL(KviOption_boolShowIconsInPopupMenus) = false;
			g_boolOptionsTable[KviOption_boolShowIconsInPopupMenus].option = false;
		}
#endif

		KVI_OPTION_STRING(KviOption_stringIconThemeSubdir) = KVI_OPTION_STRING(KviOption_stringIconThemeSubdir).trimmed();

		// the pixmap options need special processing
		for(i = 0; i < KVI_NUM_PIXMAP_OPTIONS; i++)
		{
			if(g_pixmapOptionsTable[i].flags & KviOption_groupTheme)
			{
				if(cfg.hasKey(g_pixmapOptionsTable[i].name))
				{
					QString szVal = cfg.readEntry(g_pixmapOptionsTable[i].name, "").trimmed();
					QString szBuffer;
					if(!szVal.isEmpty())
						g_pApp->findImage(szBuffer, szVal);
					else
						szBuffer = szVal;

					KviStringConversion::fromString(szBuffer, g_pixmapOptionsTable[i].option);

					// reset anyway
					iResetFlags |= g_pixmapOptionsTable[i].flags & KviOption_resetMask;
				}
			}
		}
		// force reloading of images anyway
		g_pApp->optionResetUpdate(iResetFlags | KviOption_resetReloadImages);
		KVI_OPTION_STRING(KviOption_stringIconThemeSubdir) = szThemeDir;
		return true;
	}
};

void KviApplication::listAvailableOptions(KviWindow * wnd)
{
	int i;

#define LIST_OPTIONS(__txt, __num, __table)                                                            \
	wnd->output(KVI_OUT_SYSTEMMESSAGE, "%c%s%c", KviControlCodes::Bold, __txt, KviControlCodes::Bold); \
	for(i = 0; i < __num; i++)                                                                         \
		wnd->outputNoFmt(KVI_OUT_SYSTEMMESSAGE, __table[i].name);

	LIST_OPTIONS(__tr("Boolean options (1/0)"), KVI_NUM_BOOL_OPTIONS, g_boolOptionsTable)
	LIST_OPTIONS(__tr("Rectangle options (x,y,width,height)"), KVI_NUM_RECT_OPTIONS, g_rectOptionsTable)
	LIST_OPTIONS(__tr("String options (string value)"), KVI_NUM_STRING_OPTIONS, g_stringOptionsTable)
	LIST_OPTIONS(__tr("Color options (#RRGGBB)"), KVI_NUM_COLOR_OPTIONS, g_colorOptionsTable)
	LIST_OPTIONS(__tr("Mirccolor options (#RRGGBB)"), KVI_NUM_MIRCCOLOR_OPTIONS, g_mirccolorOptionsTable)
	LIST_OPTIONS(__tr("IRC context color options (#RRGGBB)"), KVI_NUM_ICCOLOR_OPTIONS, g_iccolorOptionsTable)
	LIST_OPTIONS(__tr("Pixmap options (image path)"), KVI_NUM_PIXMAP_OPTIONS, g_pixmapOptionsTable)
	LIST_OPTIONS(__tr("Unsigned integer options (unsigned integer constant)"), KVI_NUM_UINT_OPTIONS, g_uintOptionsTable)
	LIST_OPTIONS(__tr("Font options (family,pointsize,style,charset,weight,flags (biusf))"), KVI_NUM_FONT_OPTIONS, g_fontOptionsTable)
	LIST_OPTIONS(__tr("Message color options (image_id,foreground,background,logFlag (0/1))"), KVI_NUM_MSGTYPE_OPTIONS, g_msgtypeOptionsTable)
	LIST_OPTIONS(__tr("String list options (comma separated list of strings)"), KVI_NUM_STRINGLIST_OPTIONS, g_stringlistOptionsTable)

#undef LIST_OPTIONS
}

//using namespace KviStringConversion;
bool KviApplication::getOptionString(const QString & optName, QString & buffer)
{
#define GET_OPTION_STRING(__numOpt, __table, __prefix, __prefixLen)       \
	if(KviQString::equalCIN(optName, __prefix, __prefixLen))              \
	{                                                                     \
		for(int i = 0; i < __numOpt; i++)                                 \
		{                                                                 \
			if(KviQString::equalCI(optName, __table[i].name))             \
			{                                                             \
				KviStringConversion::toString(__table[i].option, buffer); \
				return true;                                              \
			}                                                             \
		}                                                                 \
		return false;                                                     \
	}

	// WARNING : stringlist prefix must go BEFORE the string prefix (otherwise it will match)
	GET_OPTION_STRING(KVI_NUM_BOOL_OPTIONS, g_boolOptionsTable, KVI_BOOL_OPTIONS_PREFIX, KVI_BOOL_OPTIONS_PREFIX_LEN)
	GET_OPTION_STRING(KVI_NUM_RECT_OPTIONS, g_rectOptionsTable, KVI_BOOL_OPTIONS_PREFIX, KVI_BOOL_OPTIONS_PREFIX_LEN)
	GET_OPTION_STRING(KVI_NUM_STRINGLIST_OPTIONS, g_stringlistOptionsTable, KVI_STRINGLIST_OPTIONS_PREFIX, KVI_STRINGLIST_OPTIONS_PREFIX_LEN)
	GET_OPTION_STRING(KVI_NUM_STRING_OPTIONS, g_stringOptionsTable, KVI_STRING_OPTIONS_PREFIX, KVI_STRING_OPTIONS_PREFIX_LEN)
	GET_OPTION_STRING(KVI_NUM_COLOR_OPTIONS, g_colorOptionsTable, KVI_COLOR_OPTIONS_PREFIX, KVI_COLOR_OPTIONS_PREFIX_LEN)
	GET_OPTION_STRING(KVI_NUM_MIRCCOLOR_OPTIONS, g_mirccolorOptionsTable, KVI_MIRCCOLOR_OPTIONS_PREFIX, KVI_MIRCCOLOR_OPTIONS_PREFIX_LEN)
	GET_OPTION_STRING(KVI_NUM_ICCOLOR_OPTIONS, g_iccolorOptionsTable, KVI_ICCOLOR_OPTIONS_PREFIX, KVI_ICCOLOR_OPTIONS_PREFIX_LEN)
	GET_OPTION_STRING(KVI_NUM_PIXMAP_OPTIONS, g_pixmapOptionsTable, KVI_PIXMAP_OPTIONS_PREFIX, KVI_PIXMAP_OPTIONS_PREFIX_LEN)
	GET_OPTION_STRING(KVI_NUM_UINT_OPTIONS, g_uintOptionsTable, KVI_UINT_OPTIONS_PREFIX, KVI_UINT_OPTIONS_PREFIX_LEN)
	GET_OPTION_STRING(KVI_NUM_FONT_OPTIONS, g_fontOptionsTable, KVI_FONT_OPTIONS_PREFIX, KVI_FONT_OPTIONS_PREFIX_LEN)
	GET_OPTION_STRING(KVI_NUM_MSGTYPE_OPTIONS, g_msgtypeOptionsTable, KVI_MSGTYPE_OPTIONS_PREFIX, KVI_MSGTYPE_OPTIONS_PREFIX_LEN)
	return false;

#undef GET_OPTION_STRING
}

void KviApplication::optionResetUpdate(int flags)
{
	if(flags & KviOption_resetReloadImages)
	{
		g_pIconManager->reloadImages();
		emit reloadImages();
	}

	if(flags & KviOption_resetUpdateAppFont)
		updateApplicationFont();

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	if(flags & KviOption_resetUpdatePseudoTransparency)
		triggerUpdatePseudoTransparency();
#endif

	if(flags & KviOption_resetRestartIdentd)
	{
		if(g_iIdentDaemonRunningUsers)
			g_pMainWindow->executeInternalCommand(KVI_INTERNALCOMMAND_IDENT_STOP);

		if(KVI_OPTION_BOOL(KviOption_boolUseIdentService) && !KVI_OPTION_BOOL(KviOption_boolUseIdentServiceOnlyOnConnect))
			g_pMainWindow->executeInternalCommand(KVI_INTERNALCOMMAND_IDENT_START);
	}

	if(flags & KviOption_resetUpdateGui)
		triggerUpdateGui();

	if(flags & KviOption_resetUpdateWindowList)
		g_pMainWindow->recreateWindowList();

	if(flags & KviOption_resetRestartNotifyList)
		g_pApp->restartNotifyLists();

	if(flags & KviOption_resetRestartLagMeter)
		g_pApp->restartLagMeters();

	if(flags & KviOption_resetRecentChannels)
		g_pApp->buildRecentChannels();

	if(flags & KviOption_resetUpdateNotifier)
		emit updateNotifier();
}

bool KviApplication::setOptionValue(const QString & optName, const QString & value)
{
	if(!setCommonOptionValue(optName, value))
		return false;

	if(KviQString::equalCI(optName, "stringlistRecentChannels"))
	{
		buildRecentChannels();
		return true;
	}

	// The pixmap options have special treating
	if(KviQString::equalCIN(optName, KVI_PIXMAP_OPTIONS_PREFIX, KVI_PIXMAP_OPTIONS_PREFIX_LEN))
	{
		// We lookup the image path (so we allow also relative paths for this option type)
		QString szVal = value.trimmed();
		QString szBuffer;
		if(!szVal.isEmpty())
			findImage(szBuffer, szVal);
		else
			szBuffer = szVal;

		for(auto & i : g_pixmapOptionsTable)
		{
			if(KviQString::equalCI(optName, i.name))
			{
				if(!KviStringConversion::fromString(szBuffer, i.option))
					return false;
				optionResetUpdate(i.flags);
				return true;
			}
		}
		return false;
	}

	return true;
}

bool KviApplication::setCommonOptionValue(const QString & optName, const QString & value)
{

#define SET_OPTION_VALUE(__numOpt, __table, __prefix, __prefixLen)             \
	if(KviQString::equalCIN(optName, __prefix, __prefixLen))                   \
	{                                                                          \
		for(int i = 0; i < __numOpt; i++)                                      \
		{                                                                      \
			if(KviQString::equalCI(optName, __table[i].name))                  \
			{                                                                  \
				if(!KviStringConversion::fromString(value, __table[i].option)) \
					return false;                                              \
				optionResetUpdate(__table[i].flags);                           \
				return true;                                                   \
			}                                                                  \
		}                                                                      \
		return false;                                                          \
	}

	SET_OPTION_VALUE(KVI_NUM_BOOL_OPTIONS, g_boolOptionsTable, KVI_BOOL_OPTIONS_PREFIX, KVI_BOOL_OPTIONS_PREFIX_LEN)
	SET_OPTION_VALUE(KVI_NUM_RECT_OPTIONS, g_rectOptionsTable, KVI_BOOL_OPTIONS_PREFIX, KVI_BOOL_OPTIONS_PREFIX_LEN)
	SET_OPTION_VALUE(KVI_NUM_STRINGLIST_OPTIONS, g_stringlistOptionsTable, KVI_STRINGLIST_OPTIONS_PREFIX, KVI_STRINGLIST_OPTIONS_PREFIX_LEN)
	SET_OPTION_VALUE(KVI_NUM_STRING_OPTIONS, g_stringOptionsTable, KVI_STRING_OPTIONS_PREFIX, KVI_STRING_OPTIONS_PREFIX_LEN)
	SET_OPTION_VALUE(KVI_NUM_COLOR_OPTIONS, g_colorOptionsTable, KVI_COLOR_OPTIONS_PREFIX, KVI_COLOR_OPTIONS_PREFIX_LEN)
	SET_OPTION_VALUE(KVI_NUM_MIRCCOLOR_OPTIONS, g_mirccolorOptionsTable, KVI_MIRCCOLOR_OPTIONS_PREFIX, KVI_MIRCCOLOR_OPTIONS_PREFIX_LEN)
	SET_OPTION_VALUE(KVI_NUM_ICCOLOR_OPTIONS, g_iccolorOptionsTable, KVI_ICCOLOR_OPTIONS_PREFIX, KVI_ICCOLOR_OPTIONS_PREFIX_LEN)
	SET_OPTION_VALUE(KVI_NUM_UINT_OPTIONS, g_uintOptionsTable, KVI_UINT_OPTIONS_PREFIX, KVI_UINT_OPTIONS_PREFIX_LEN)
	SET_OPTION_VALUE(KVI_NUM_FONT_OPTIONS, g_fontOptionsTable, KVI_FONT_OPTIONS_PREFIX, KVI_FONT_OPTIONS_PREFIX_LEN)
	SET_OPTION_VALUE(KVI_NUM_MSGTYPE_OPTIONS, g_msgtypeOptionsTable, KVI_MSGTYPE_OPTIONS_PREFIX, KVI_MSGTYPE_OPTIONS_PREFIX_LEN)

#undef SET_OPTION_VALUE
	return true;
}
