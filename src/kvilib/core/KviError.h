#ifndef _KVI_ERROR_H_
#define _KVI_ERROR_H_
//=============================================================================
//
//   File : KviError.h
//   Creation date : Sun Jul 02 2000 18:35:56 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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
* \file KviError.h
* \author Szymon Stefanek
* \brief Error List
*/

#include "kvi_settings.h"

class QString;

/**
* \namespace KviError
* \brief This namespace handle the error codes
*/
namespace KviError
{
	/**
	* \enum Code
	* \brief Contains all error codes
	*/
	enum Code
	{
		Success = 0,
		UnknownError = 1,
		InternalError = 2,
		UnknownCommand = 3,
		MissingClosingBrace = 4,
		UnexpectedEndInString = 5,
		UnexpectedEndInDictionaryKey = 6,
		SwitchDashWithoutSwitchLetter = 7,
		UnknownFunction = 8,
		UnexpectedEndInParenthesis = 9,
		UnexpectedEndInFunctionParams = 10,
		MissingVariableName = 11,
		VariableOrIdentifierExpected = 12,
		LeftOperandIsNotANumber = 13,
		MultipleOpsNotSupportedForOperator = 14,
		DivisionByZero = 15,
		ModuloByZero = 16,
		RightOperandIsNotANumber = 17,
		UnterminatedExpression = 18,
		UnterminatedSubexpression = 19,
		UnexpectedCharacter = 20,
		UnknownOperator = 21,
		NoHostToResolve = 22,
		UnsupportedAddressFamily = 23,
		ValidNameButNoIpAddress = 24,
		UnrecoverableNameserverError = 25,
		DNSTemporaneousFault = 26,
		DNSInternalErrorBadFlags = 27,
		DNSInternalErrorOutOfMemory = 28,
		DNSInternalErrorServiceNotSupported = 29,
		DNSNoName = 30,
		DNSInternalErrorUnsupportedSocketType = 31,
		DNSQueryFailed = 32,
		NoIPv6Support = 33,
		HostNotFound = 34,
		DNSInternalIPCFailure = 35,
		AnotherConnectionInProgress = 36,
		InvalidIpAddress = 37,
		SocketCreationFailed = 38,
		AsyncSocketFailed = 39,
		BadFileDescriptor = 40,
		OutOfAddressSpace = 41,
		ConnectionRefused = 42,
		KernelNetworkingPanic = 43,
		ConnectionTimedOut = 44,
		NetworkUnreachable = 45,
		BrokenPipe = 46,
		InvalidProxyAddress = 47,
		RemoteEndClosedConnection = 48,
		InvalidIrcContextId = 49,
		ErrorInLoadingModule = 50,
		NoSuchModuleCommand = 51,
		NoSuchModuleFunction = 52,
		LeftOperandIsNotADictionaryReference = 53,
		RightOperandIsNotADictionaryReference = 54,
		MissingObjectClassName = 55,
		NoSuchObjectClass = 56,
		NoSuchObject = 57,
		NoSuchObjectFunction = 58,
		InvalidLeftOperand = 59,
		NotEnoughParameters = 60,
		IntegerParameterExpected = 61,
		InvalidParameter = 62,
		NoSuchFile = 63,
		OpenParenthesisExpected = 64,
		OpenBraceExpected = 65,
		CantKillABuiltinClass = 66,
		SocksV4LacksIPv6Support = 67,
		UnrecognizedProxyReply = 68,
		ProxyAuthFailed = 69,
		ProxyNoAcceptableAuthMethod = 70,
		ProxyReply91RequestFailed = 71,
		ProxyReply92IdentFailed = 72,
		ProxyReply93IdentNotMatching = 73,
		ProxyReply01GeneralSOCKSFailure = 74,
		ProxyReply02ConnectionNotAllowed = 75,
		ProxyReply03NetworkUnreachable = 76,
		ProxyReply04HostUnreachable = 77,
		ProxyReply05ConnectionRefused = 78,
		ProxyReply06TTLExpired = 79,
		ProxyReply07CommandNotSupported = 80,
		ProxyReply08AddressTypeNotSupported = 81,
		ProxyReply09InvalidAddress = 82,
		InvalidPortNumber = 83,
		SocketNotConnected = 84,
		InsufficientResources = 85,
		BindFailed = 86,
		CantResolveLocalhost = 87,
		UnsupportedImageFormat = 88,
		CantOpenFileForAppending = 89,
		CantOpenFileForWriting = 90,
		FileIOError = 91,
		AcknowledgeError = 92,
		CantOpenFileForReading = 93,
		CantSendAZeroSizeFile = 94,
		MissingPopupName = 95,
		ItemPopupOrSeparatorExpected = 96,
		SelfModificationNotAllowed = 97,
		FeatureNotAvailable = 98,
		UnexpectedCharactersInArrayIndex = 99,
		UnexpectedEndInExpression = 100,
		UnexpectedEndInArrayIndex = 101,
		ProxyHttpFailure = 102,
		CaseMatchRegexpDefaultOrBreakExpected = 103,
		AccessDenied = 104,
		AddressAlreadyInUse = 105,
		CantAssignRequestedAddress = 106,
		ConnectionResetByPeer = 107,
		HostUnreachable = 108,
		VariableExpected = 109,
		InvalidArrayIndex = 110,
		ListenFailed = 111,
		NoSSLSupport = 112,
		SSLError = 113,
		SlashCharacterExpected = 114,
		UnknownStringManipulationOperator = 115,
		OperationAborted = 116,
		UnexpectedToken = 117,
		ScopeObjectAlreadyDefined = 118,
		NoThisObject = 119,
		ErrorCount = 120
	};

	/**
	* \brief Returns the description of the error
	* \param eError The code number of the error
	* \return QString
	*/
	KVILIB_API QString getDescription(KviError::Code eError);

	/**
	* \brief Returns an untranslated description of the error
	* \param eError The code number of the error
	* \return const char *
	*/
	KVILIB_API const char * getUntranslatedDescription(KviError::Code eError);

	/**
	* \brief Gets system errors and returns their code number
	* \param iErrNo The code number of the system error
	* \return int
	*/
	KVILIB_API KviError::Code translateSystemError(int iErrNo);
}

#endif //_KVI_ERROR_H_
