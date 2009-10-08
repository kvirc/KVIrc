#ifndef _KVI_ERROR_H_
#define _KVI_ERROR_H_
//=============================================================================
//
//   File : kvi_error.h
//   Creation date : Sun Jul 02 2000 18:35:56 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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

/**
* \file kvi_error.h
* \author Szymon Stefanek
* \brief Error List
*/

#include "kvi_settings.h"
#include "kvi_qstring.h"

#define KviError_success 0
#define KviError_unknownError 1
#define KviError_internalError 2
#define KviError_unknownCommand 3
#define KviError_missingClosingBrace 4
#define KviError_unexpectedEndInString 5
#define KviError_unexpectedEndInDictionaryKey 6
#define KviError_switchDashWithoutSwitchLetter 7
#define KviError_unknownFunction 8
#define KviError_unexpectedEndInParenthesis 9
#define KviError_unexpectedEndInFunctionParams 10
#define KviError_missingVariableName 11
#define KviError_variableOrIdentifierExpected 12
#define KviError_leftOperandIsNotANumber 13
#define KviError_multipleOpsNotSupportedForOperator 14
#define KviError_divisionByZero 15
#define KviError_moduloByZero 16
#define KviError_rightOperandIsNotANumber 17
#define KviError_unterminatedExpression 18
#define KviError_unterminatedSubexpression 19
#define KviError_unexpectedCharacter 20
#define KviError_unknownOperator 21

#define KviError_noHostToResolve 22
#define KviError_unsupportedAddressFamily 23
#define KviError_validNameButNoIpAddress 24
#define KviError_unrecoverableNameserverError 25
#define KviError_dnsTemporaneousFault 26
#define KviError_dnsInternalErrorBadFlags 27
#define KviError_dnsInternalErrorOutOfMemory 28
#define KviError_dnsInternalErrorServiceNotSupported 29
#define KviError_dnsNoName 30
#define KviError_dnsInternalErrorUnsupportedSocketType 31
#define KviError_dnsQueryFailed 32
#define KviError_noIPv6Support 33
#define KviError_hostNotFound 34
#define KviError_dnsInternalIPCFailure 35

#define KviError_anotherConnectionInProgress 36
#define KviError_invalidIpAddress 37
#define KviError_socketCreationFailed 38
#define KviError_asyncSocketFailed 39
#define KviError_badFileDescriptor 40
#define KviError_outOfAddressSpace 41
#define KviError_connectionRefused 42
#define KviError_kernelNetworkingPanic 43
#define KviError_connectionTimedOut 44
#define KviError_networkUnreachable 45
#define KviError_brokenPipe 46
#define KviError_invalidProxyAddress 47
#define KviError_remoteEndClosedConnection 48

#define KviError_invalidIrcContextId 49
#define KviError_errorInLoadingModule 50
#define KviError_noSuchModuleCommand 51
#define KviError_noSuchModuleFunction 52

#define KviError_leftOperandIsNotADictionaryReference 53
#define KviError_rightOperandIsNotADictionaryReference 54

#define KviError_missingObjectClassName 55
#define KviError_noSuchObjectClass 56
#define KviError_noSuchObject 57
#define KviError_noSuchObjectFunction 58

#define KviError_invalidLeftOperand 59

#define KviError_notEnoughParameters 60
#define KviError_integerParameterExpected 61
#define KviError_invalidParameter 62

#define KviError_noSuchFile 63

#define KviError_openParenthesisExpected 64
#define KviError_openBraceExpected 65

#define KviError_cantKillABuiltinClass 66
#define KviError_socksV4LacksIPv6Support 67
#define KviError_unrecognizedProxyReply 68
#define KviError_proxyAuthFailed 69
#define KviError_proxyNoAcceptableAuthMethod 70

#define KviError_proxyReply91RequestFailed 71
#define KviError_proxyReply92IdentFailed 72
#define KviError_proxyReply93IdentNotMatching 73
#define KviError_proxyReply01GeneralSOCKSFailure 74
#define KviError_proxyReply02ConnectionNotAllowed 75
#define KviError_proxyReply03NetworkUnreachable 76
#define KviError_proxyReply04HostUnreachable 77
#define KviError_proxyReply05ConnectionRefused 78
#define KviError_proxyReply06TTLExpired 79
#define KviError_proxyReply07CommandNotSupported 80
#define KviError_proxyReply08AddressTypeNotSupported 81
#define KviError_proxyReply09InvalidAddress 82

#define KviError_invalidPortNumber 83
#define KviError_socketNotConnected 84
#define KviError_insufficientResources 85
#define KviError_bindFailed 86
#define KviError_cantResolveLocalhost 87

#define KviError_unsupportedImageFormat 88

#define KviError_cantOpenFileForAppending 89
#define KviError_cantOpenFileForWriting 90
#define KviError_fileIOError 91
#define KviError_acknowledgeError 92
#define KviError_cantOpenFileForReading 93
#define KviError_cantSendAZeroSizeFile 94

#define KviError_missingPopupName 95
#define KviError_itemPopupOrSeparatorExpected 96
#define KviError_selfModificationNotAllowed 97

//#define KviError_recursionTooDeep 98
#define KviError_featureNotAvailable 99

#define KviError_unexpectedCharactersInArrayIndex 100
#define KviError_unexpectedEndInExpression 101
#define KviError_unexpectedEndInArrayIndex 102

#define KviError_proxyHttpFailure 103
#define KviError_caseMatchRegexpDefaultOrBreakExpected 104


#define KviError_accessDenied 105
#define KviError_addressAlreadyInUse 106
#define KviError_cantAssignRequestedAddress 107
#define KviError_connectionResetByPeer 108
#define KviError_hostUnreachable 109

#define KviError_variableExpected 110
#define KviError_invalidArrayIndex 111

#define KviError_listenFailed 112

#define KviError_noSSLSupport 113
#define KviError_SSLError 114

#define KviError_slashCharacterExpected 115
#define KviError_unknownStringManipulationOperator 116

#define KviError_operationAborted 117

#define KviError_unexpectedToken 118

#define KviError_scopeObjectAlreadyDefined 119
#define KviError_noThisObject 120

#define KVI_NUM_ERRORS 121

/**
* \namespace KviError
* \brief This namespace handle the error codes
*/
namespace KviError
{
	/**
	* \brief Returns the description of the error
	* \param iErrorCode The code number of the error
	* \return QString
	*/
	KVILIB_API QString getDescription(int iErrorCode);

	/**
	* \brief Returns an untranslated description of the error
	* \param iErrorCode The code number of the error
	* \return const char *
	*/
	KVILIB_API const char * getUntranslatedDescription(int iErrorCode);

	/**
	* \brief Gets system errors and returns their code number
	* \param iErrNo The code number of the system error
	* \return int
	*/
	KVILIB_API int translateSystemError(int iErrNo);
}

#endif //_KVI_ERROR_H_
