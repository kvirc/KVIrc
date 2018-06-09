//=============================================================================
//
//   File : KviError.cpp
//   Creation date : Sun Jul 02 2000 18:37:02 by Szymon Stefanek
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

#define _KVI_ERROR_CPP_

#include "KviError.h"
#include "KviLocale.h"
#include "kvi_settings.h"

#include <QString>

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
#include <winsock2.h> // for the WSAE* error codes
#endif

#include <cerrno>

#ifdef HAVE_STRERROR
#include <string.h> // for strerror()
#endif

// FIXME: This stuff should basically die and be eventually replaced with some
//        helper functions for handling ONLY system errors.
//
// WARNING: getDescription() is not even thread safe... it will die in the near future

const char * g_errorTable[KviError::ErrorCount] = {
	__tr_no_lookup("Success"),                                                     // 000: success
	__tr_no_lookup("Unknown error"),                                               // 001: unkonwnError
	__tr_no_lookup("Internal error"),                                              // 002: internalError
	__tr_no_lookup("Unknown command"),                                             // 003: unknownCommand
	__tr_no_lookup("Missing closing brace"),                                       // 004: missingClosingBrace
	__tr_no_lookup("Unexpected end of command in string"),                         // 005: unexpectedEndInString
	__tr_no_lookup("Unexpected end of command in dictionary key"),                 // 006: unexpectedEndInDictionaryKey
	__tr_no_lookup("Switch dash without switch letter"),                           // 007: switchDashWithoutSwitchLetter
	__tr_no_lookup("Unknown function"),                                            // 008: unknownFunction
	__tr_no_lookup("Unexpected end of command in parenthesis"),                    // 009: unexpectedEndInParenthesis
	__tr_no_lookup("Unexpected end of command in function parameters"),            // 010: unexpectedEndInFunctionParams
	__tr_no_lookup("Missing variable name"),                                       // 011: missingVariableName
	__tr_no_lookup("Variable or identifier expected"),                             // 012: variableOrIdentifierExpected
	__tr_no_lookup("Left operand is not a number"),                                // 013: leftOperandIsNotANumber
	__tr_no_lookup("Multiple operations not supported for numeric operators"),     // 014: multipleOpsNotSupportedForOperator
	__tr_no_lookup("Division by zero"),                                            // 015: divisionByZero
	__tr_no_lookup("Modulo by zero"),                                              // 016: moduloByZero
	__tr_no_lookup("Right operand is not a number"),                               // 017: rightOperandIsNotANumber
	__tr_no_lookup("Unterminated expression (missing ')' ?)"),                     // 018: unterminatedExpression
	__tr_no_lookup("Unterminated subexpression (Parenthesis mismatch)"),           // 019: unterminatedSubexpression
	__tr_no_lookup("Unexpected character"),                                        // 020: unexpectedCharacter
	__tr_no_lookup("Unknown operator"),                                            // 021: unknownOperator
	__tr_no_lookup("No host to resolve"),                                          // 022
	__tr_no_lookup("(DNS internal) Unsupported address family"),                   // 023
	__tr_no_lookup("Valid name but the host has no IP address"),                   // 024
	__tr_no_lookup("Unrecoverable nameserver error (crashed?)"),                   // 025
	__tr_no_lookup("DNS temporary fault (try again)"),                             // 026
	__tr_no_lookup("(DNS internal) Bad flags"),                                    // 027
	__tr_no_lookup("(DNS internal) Out of memory"),                                // 028
	__tr_no_lookup("(DNS internal) Service not supported"),                        // 029
	__tr_no_lookup("Unknown node (host not found)"),                               // 030
	__tr_no_lookup("(DNS internal) Unsupported socket type"),                      // 031
	__tr_no_lookup("DNS query failed"),                                            // 032
	__tr_no_lookup("This KVIrc executable has no IPv6 support"),                   // 033
	__tr_no_lookup("Host not found"),                                              // 034
	__tr_no_lookup("(DNS internal) IPC failure (slave data corrupted)"),           // 035
	__tr_no_lookup("Another connection is in progress"),                           // 036
	__tr_no_lookup("Invalid IP address"),                                          // 037
	__tr_no_lookup("Socket creation failed"),                                      // 038
	__tr_no_lookup("Failed to put the socket in non blocking mode"),               // 039
	__tr_no_lookup("Bad file descriptor"),                                         // 040
	__tr_no_lookup("Out of address space"),                                        // 041
	__tr_no_lookup("Connection refused"),                                          // 042
	__tr_no_lookup("Kernel networking panic"),                                     // 043
	__tr_no_lookup("Connection timed out"),                                        // 044
	__tr_no_lookup("Network is unreachable"),                                      // 045
	__tr_no_lookup("Broken pipe"),                                                 // 046
	__tr_no_lookup("Invalid proxy address"),                                       // 047
	__tr_no_lookup("Remote end has closed the connection"),                        // 048
	__tr_no_lookup("Invalid IRC context ID"),                                      // 049
	__tr_no_lookup("Error loading module"),                                        // 050
	__tr_no_lookup("No such module command"),                                      // 051
	__tr_no_lookup("No such module function"),                                     // 052
	__tr_no_lookup("Left operand is not a dictionary reference"),                  // 053
	__tr_no_lookup("Right operand is not a dictionary reference"),                 // 054
	__tr_no_lookup("Missing object class name"),                                   // 055
	__tr_no_lookup("No such object class"),                                        // 056
	__tr_no_lookup("No such object"),                                              // 057
	__tr_no_lookup("No such object function"),                                     // 058
	__tr_no_lookup("Invalid left operand"),                                        // 059
	__tr_no_lookup("Not enough parameters"),                                       // 060
	__tr_no_lookup("Integer parameter expected"),                                  // 061
	__tr_no_lookup("Invalid parameter"),                                           // 062
	__tr_no_lookup("No such file"),                                                // 063
	__tr_no_lookup("Open parenthesis expected"),                                   // 064
	__tr_no_lookup("Open brace expected"),                                         // 065
	__tr_no_lookup("Can't kill a builtin class"),                                  // 066
	__tr_no_lookup("The SOCKSv4 protocol lacks IPv6 support"),                     // 067
	__tr_no_lookup("Unrecognized proxy reply"),                                    // 068
	__tr_no_lookup("Proxy response: auth failed: access denied"),                  // 069
	__tr_no_lookup("Proxy response: no acceptable auth method: request rejected"), // 070
	__tr_no_lookup("Proxy response: request failed"),                              // 071
	__tr_no_lookup("Proxy response: Ident failed"),                                // 072
	__tr_no_lookup("Proxy response: Ident not matching"),                          // 073
	__tr_no_lookup("Proxy response: general SOCKS failure"),                       // 074
	__tr_no_lookup("Proxy response: connection not allowed"),                      // 075
	__tr_no_lookup("Proxy response: network unreachable"),                         // 076
	__tr_no_lookup("Proxy response: host unreachable"),                            // 077
	__tr_no_lookup("Proxy response: connection refused"),                          // 078
	__tr_no_lookup("Proxy response: TTL expired"),                                 // 079
	__tr_no_lookup("Proxy response: command not supported"),                       // 080
	__tr_no_lookup("Proxy response: address type not supported"),                  // 081
	__tr_no_lookup("Proxy response: invalid address"),                             // 082
	__tr_no_lookup("Invalid port number"),                                         // 083
	__tr_no_lookup("Socket not connected"),                                        // 084
	__tr_no_lookup("Insufficient resources to complete the operation"),            // 085
	__tr_no_lookup("Can't setup a listening socket : bind failed"),                // 086
	__tr_no_lookup("Can't resolve the localhost name"),                            // 087
	__tr_no_lookup("Unsupported image format"),                                    // 088
	__tr_no_lookup("Can't open file for appending"),                               // 089
	__tr_no_lookup("Can't open file for writing"),                                 // 090
	__tr_no_lookup("File I/O error"),                                              // 091
	__tr_no_lookup("Acknowledge error"),                                           // 092
	__tr_no_lookup("Can't open file for reading"),                                 // 093
	__tr_no_lookup("Can't send a zero-size file"),                                 // 094
	__tr_no_lookup("Missing popup name"),                                          // 095
	__tr_no_lookup("'item', 'popup', 'label' or 'separator' keyword expected"),    // 096
	__tr_no_lookup("Self modification not allowed"),                               // 097
	__tr_no_lookup("Feature not available"),                                       // 098
	__tr_no_lookup("Unexpected characters in array index"),                        // 099
	__tr_no_lookup("Unexpected end in expression"),                                // 100
	__tr_no_lookup("Unexpected end in array index"),                               // 101
	__tr_no_lookup("Connection through HTTP proxy failed"),                        // 102
	__tr_no_lookup("Case, match, regexp, default or break keyword expected"),      // 103
	__tr_no_lookup("Access denied"),                                               // 104
	__tr_no_lookup("Address already in use"),                                      // 105
	__tr_no_lookup("Can't assign the requested address"),                          // 106
	__tr_no_lookup("Connection reset by peer"),                                    // 107
	__tr_no_lookup("Host unreachable (no route to host)"),                         // 108
	__tr_no_lookup("Variable expected"),                                           // 109
	__tr_no_lookup("Invalid array index: positive integer expected"),              // 110
	__tr_no_lookup("listen() call failed"),                                        // 111
	__tr_no_lookup("This executable has been compiled without SSL support"),       // 112
	__tr_no_lookup("Secure Socket Layer error"),                                   // 113
	__tr_no_lookup("Slash (/) character expected"),                                // 114
	__tr_no_lookup("Unknown string manipulation operation"),                       // 115
	__tr_no_lookup("Operation aborted"),                                           // 116
	__tr_no_lookup("Unexpected token"),                                            // 117
	__tr_no_lookup("Scope object already defined (unexpected @)"),                 // 118
	__tr_no_lookup("There is no $this pointer in this scope (unexpected @)")       // 119
};

namespace KviError
{
	const char * getUntranslatedDescription(KviError::Code eError)
	{
		if((eError < ErrorCount) && (eError >= 0))
			return g_errorTable[eError];
#ifdef HAVE_STRERROR
		if(eError < 0)
			return strerror(-eError);
#endif
		return g_errorTable[KviError::UnknownError];
	}

	QString getDescription(KviError::Code eError)
	{
		return __tr2qs_no_xgettext(getUntranslatedDescription(eError));
	}

	KviError::Code translateSystemError(int iErrNo)
	{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		switch(iErrNo)
		{
			case EBADF:
				return KviError::BadFileDescriptor;
				break;
			case WSAEINVAL:
			case WSAEFAULT:
			case EFAULT:
				return KviError::OutOfAddressSpace;
				break;
			case WSAECONNREFUSED:
				return KviError::ConnectionRefused;
				break;
			case WSAENOTSOCK:
				return KviError::KernelNetworkingPanic;
				break;
			case WSAETIMEDOUT:
				return KviError::ConnectionTimedOut;
				break;
			case WSAENETUNREACH:
				return KviError::NetworkUnreachable;
				break;
			case EPIPE:
				return KviError::BrokenPipe;
				break;
			case WSAENOTCONN:
				return KviError::SocketNotConnected;
				break;
			case WSAEACCES:
				return KviError::AccessDenied;
				break;
			case WSAEADDRINUSE:
				return KviError::AddressAlreadyInUse;
				break;
			case WSAEADDRNOTAVAIL:
				return KviError::CantAssignRequestedAddress;
				break;
			case WSAEAFNOSUPPORT:
				return KviError::UnsupportedAddressFamily;
				break;
			case WSAECONNRESET:
				return KviError::ConnectionResetByPeer;
				break;
			case WSAEHOSTUNREACH:
				return KviError::HostUnreachable;
				break;
			// Unhandled error...pass errno to the strerror function
			default:
				return KviError::UnknownError;
				break;
		}
#else
		switch(iErrNo)
		{
			case EBADF:
				return KviError::BadFileDescriptor;
				break;
			case EFAULT:
				return KviError::OutOfAddressSpace;
				break;
			case ECONNREFUSED:
				return KviError::ConnectionRefused;
				break;
			case ENOTSOCK:
				return KviError::KernelNetworkingPanic;
				break;
			case ETIMEDOUT:
				return KviError::ConnectionTimedOut;
				break;
			case ENETUNREACH:
				return KviError::NetworkUnreachable;
				break;
			case EPIPE:
				return KviError::BrokenPipe;
				break;
			case ENOTCONN:
				return KviError::SocketNotConnected;
				break;
			case ENOBUFS:
				return KviError::InsufficientResources;
				break;
			case ECONNRESET:
				return KviError::ConnectionResetByPeer;
				break;
			case EHOSTUNREACH:
				return KviError::HostUnreachable;
				break;
			// Unhandled error...pass errno to the strerror function
			default:
				return KviError::UnknownError;
				break;
		}
#endif
	}
}
