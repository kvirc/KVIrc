//=============================================================================
//
//   File : kvi_error.cpp
//   Creation date : Sun Jul 02 2000 18:37:02 by Szymon Stefanek
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


#define _KVI_ERROR_CPP_

#include "kvi_locale.h"
#include "kvi_error.h"
#include "kvi_settings.h"


#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	#include <winsock2.h> // for the WSAE* error codes
#endif

#include <errno.h>

#ifdef HAVE_STRERROR
	#include <string.h> // for strerror()
#endif

// FIXME: This stuff should basically die and be eventually replaced with some
//        helper functions for handling ONLY system errors.
//
// WARNING: getDescription() is not even thread safe... it will die in the near future

const char * g_errorTable[KVI_NUM_ERRORS]=
{
	__tr_no_lookup("Success"),                                                  // 000: success
	__tr_no_lookup("Unknown error"),                                            // 001: unkonwnError
	__tr_no_lookup("Internal error"),                                           // 002: internalError
	__tr_no_lookup("Unknown command"),                                          // 003: unknownCommand
	__tr_no_lookup("Missing closing brace"),                                    // 004: missingClosingBrace
	__tr_no_lookup("Unexpected end of command in string"),                      // 005: unexpectedEndInString
	__tr_no_lookup("Unexpected end of command in dictionary key"),              // 006: unexpectedEndInDictionaryKey
	__tr_no_lookup("Switch dash without switch letter"),                        // 007: switchDashWithoutSwitchLetter
	__tr_no_lookup("Unknown function"),                                         // 008: unknownFunction
	__tr_no_lookup("Unexpected end of command in parenthesis"),                 // 009: unexpectedEndInParenthesis
	__tr_no_lookup("Unexpected end of command in function parameters"),         // 010: unexpectedEndInFunctionParams
	__tr_no_lookup("Missing variable name"),                                    // 011: missingVariableName
	__tr_no_lookup("Variable or identifier expected"),                          // 012: variableOrIdentifierExpected
	__tr_no_lookup("Left operand is not a number"),                             // 013: leftOperandIsNotANumber
	__tr_no_lookup("Multiple operations not supported for numeric operators"),  // 014: multipleOpsNotSupportedForOperator
	__tr_no_lookup("Division by zero"),                                         // 015: divisionByZero
	__tr_no_lookup("Modulo by zero"),                                           // 016: moduloByZero
	__tr_no_lookup("Right operand is not a number"),                            // 017: rightOperandIsNotANumber
	__tr_no_lookup("Unterminated expression (missing ')' ?)"),                  // 018: unterminatedExpression
	__tr_no_lookup("Unterminated subexpression (Parenthesis mismatch)"),        // 019: unterminatedSubexpression
	__tr_no_lookup("Unexpected character"),                                     // 020: unexpectedCharacter
	__tr_no_lookup("Unknown operator"),                                         // 021: unknownOperator
	__tr_no_lookup("No host to resolve"),                                       // 022
	__tr_no_lookup("(DNS Internal) Unsupported address family"),                // 023
	__tr_no_lookup("Valid name but the host has no IP address"),                // 024
	__tr_no_lookup("Unrecoverable nameserver error (crashed ?)"),               // 025
	__tr_no_lookup("Dns temporaneous fault (try again)"),                       // 026
	__tr_no_lookup("(DNS Internal) Bad flags"),                                 // 027
	__tr_no_lookup("(DNS Internal) Out of memory"),                             // 028
	__tr_no_lookup("(DNS Internal) Service not supported"),                     // 029
	__tr_no_lookup("Unknown node (host not found)"),                            // 030
	__tr_no_lookup("(DNS Internal) Unsupported socket type"),                   // 031
	__tr_no_lookup("Dns query failed"),                                         // 032
	__tr_no_lookup("This KVIrc executable has no IPV6 support"),                // 033
	__tr_no_lookup("Host not found"),                                           // 034
	__tr_no_lookup("(DNS Internal) IPC failure (slave data corrupted)"),        // 035
	__tr_no_lookup("Another connection in progress"),                           // 036
	__tr_no_lookup("Invalid IP address"),                                       // 037
	__tr_no_lookup("Socket creation failed"),                                   // 038
	__tr_no_lookup("Failed to put the socket in non blocking mode"),            // 039
	__tr_no_lookup("Bad file descriptor"),                                      // 040
	__tr_no_lookup("Out of address space"),                                     // 041
	__tr_no_lookup("Connection refused"),                                       // 042
	__tr_no_lookup("Kernel networking panic"),                                  // 043
	__tr_no_lookup("Connection timed out"),                                     // 044
	__tr_no_lookup("Network is unreachable"),                                   // 045
	__tr_no_lookup("Broken pipe"),                                              // 046
	__tr_no_lookup("Invalid proxy address"),                                    // 047
	__tr_no_lookup("Remote end has closed the connection"),                     // 048
	__tr_no_lookup("Invalid irc context id"),                                   // 049
	__tr_no_lookup("Error in loading module"),                                  // 050
	__tr_no_lookup("No such module command"),                                   // 051
	__tr_no_lookup("No such module function"),                                  // 052
	__tr_no_lookup("Left operand is not a dictionary reference"),               // 053
	__tr_no_lookup("Right operand is not a dictionary reference"),              // 054
	__tr_no_lookup("Missing object class name"),                                // 055
	__tr_no_lookup("No such object class"),                                     // 056
	__tr_no_lookup("No such object"),                                           // 057
	__tr_no_lookup("No such object function"),                                  // 058
	__tr_no_lookup("Invalid left operand"),                                     // 059
	__tr_no_lookup("Not enough parameters"),                                    // 060
	__tr_no_lookup("Integer parameter expected"),                               // 061
	__tr_no_lookup("Invalid parameter"),                                        // 062
	__tr_no_lookup("No such file"),                                             // 063
	__tr_no_lookup("Open parenthesis expected"),                                // 064
	__tr_no_lookup("Open brace expected"),                                      // 065
	__tr_no_lookup("Can't kill a builtin class"),                               // 066
	__tr_no_lookup("The SOCKSv4 protocol lacks IPv6 support"),                  // 067
	__tr_no_lookup("Unrecognized proxy reply"),                                 // 068
	__tr_no_lookup("Proxy response: auth failed: access denied"),
	__tr_no_lookup("Proxy response: No acceptable auth method: request rejected"),
	__tr_no_lookup("Proxy response: request failed"),
	__tr_no_lookup("Proxy response: ident failed"),
	__tr_no_lookup("Proxy response: ident not matching"),
	__tr_no_lookup("Proxy response: general SOCKS failure"),
	__tr_no_lookup("Proxy response: connection not allowed"),
	__tr_no_lookup("Proxy response: network unreachable"),
	__tr_no_lookup("Proxy response: host unreachable"),
	__tr_no_lookup("Proxy response: connection refused"),
	__tr_no_lookup("Proxy response: TTL expired"),
	__tr_no_lookup("Proxy response: command not supported"),
	__tr_no_lookup("Proxy response: address type not supported"),
	__tr_no_lookup("Proxy response: invalid address"),
	__tr_no_lookup("Invalid port number"),
	__tr_no_lookup("Socket not connected"),
	__tr_no_lookup("Insufficient resources to complete the operation"),
	__tr_no_lookup("Can't setup a listening socket : bind failed"),
	__tr_no_lookup("Can't resolve the localhost name"),
	__tr_no_lookup("Unsupported image format"),
	__tr_no_lookup("Can't open file for appending"),
	__tr_no_lookup("Can't open file for writing"),
	__tr_no_lookup("File I/O error"),
	__tr_no_lookup("Acknowledge error"),
	__tr_no_lookup("Can't open file for reading"),
	__tr_no_lookup("Can't send a zero-size file"),
	__tr_no_lookup("Missing popup name"),
	__tr_no_lookup("'item', 'popup', 'label' or 'separator' keyword expected"),
	__tr_no_lookup("Self modification not allowed"),
	__tr_no_lookup("UNUSED"),
	__tr_no_lookup("Feature not available"),
	__tr_no_lookup("Unexpected characters in array index"),
	__tr_no_lookup("Unexpected end in expression"),
	__tr_no_lookup("Unexpected end in array index"),
	__tr_no_lookup("Connection thru HTTP proxy failed"),
	__tr_no_lookup("Case , match , regexp , default or break keyword expected"),
	__tr_no_lookup("Access denied"),
	__tr_no_lookup("Address already in use"),
	__tr_no_lookup("Can't assign the requested address"),
	__tr_no_lookup("Connection reset by peer"),
	__tr_no_lookup("Host unreachable (no route to host)"),
	__tr_no_lookup("Variable expected"),
	__tr_no_lookup("Invalid array index: positive integer expected"),
	__tr_no_lookup("listen() call failed"),
	__tr_no_lookup("This executable has been compiled without SSL support"),
	__tr_no_lookup("Secure Socket Layer error"),
	__tr_no_lookup("Slash (/) character expected"),
	__tr_no_lookup("Unknown string manipulation operation"),
	__tr_no_lookup("Operation aborted"),
	__tr_no_lookup("Unexpected token"),
	__tr_no_lookup("Scope object already defined (unexpected @)"),
	__tr_no_lookup("There is no $this pointer in this scope (unexpected @)")
};

namespace KviError
{
	const char * getUntranslatedDescription(int iErrorCode)
	{
		if((iErrorCode < KVI_NUM_ERRORS) && (iErrorCode >= 0))
			return g_errorTable[iErrorCode];
#ifdef HAVE_STRERROR
		if(iErrorCode < 0)return strerror(-iErrorCode);
#endif
		return g_errorTable[KviError_unknownError];
	}

	QString getDescription(int iErrorCode)
	{
		return __tr2qs_no_xgettext(getUntranslatedDescription(iErrorCode));
	}

	int translateSystemError(int iErrNo)
	{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		switch(iErrNo)
		{
			case EBADF:            return KviError_badFileDescriptor;          break;
			case WSAEINVAL:
			case WSAEFAULT:
			case EFAULT:           return KviError_outOfAddressSpace;          break;
			case WSAECONNREFUSED:  return KviError_connectionRefused;          break;
			case WSAENOTSOCK:      return KviError_kernelNetworkingPanic;      break;
			case WSAETIMEDOUT:     return KviError_connectionTimedOut;         break;
			case WSAENETUNREACH:   return KviError_networkUnreachable;         break;
			case EPIPE:            return KviError_brokenPipe;                 break;
			case WSAENOTCONN:      return KviError_socketNotConnected;         break;
			case WSAEACCES:        return KviError_accessDenied;               break;
			case WSAEADDRINUSE:    return KviError_addressAlreadyInUse;        break;
			case WSAEADDRNOTAVAIL: return KviError_cantAssignRequestedAddress; break;
			case WSAEAFNOSUPPORT:  return KviError_unsupportedAddressFamily;   break;
			case WSAECONNRESET:    return KviError_connectionResetByPeer;      break;
			case WSAEHOSTUNREACH:  return KviError_hostUnreachable;            break;

			//case ENOBUFS:      return KviError_insufficientResources; break;
			// Unhandled error...pass errno to the strerror function
			default:              return -iErrNo;                              break;
		}
#else
		switch(iErrNo)
		{
			case EBADF:        return KviError_badFileDescriptor;     break;
			case EFAULT:       return KviError_outOfAddressSpace;     break;
			case ECONNREFUSED: return KviError_connectionRefused;     break;
			case ENOTSOCK:     return KviError_kernelNetworkingPanic; break;
			case ETIMEDOUT:    return KviError_connectionTimedOut;    break;
			case ENETUNREACH:  return KviError_networkUnreachable;    break;
			case EPIPE:        return KviError_brokenPipe;            break;
			case ENOTCONN:     return KviError_socketNotConnected;    break;
			case ENOBUFS:      return KviError_insufficientResources; break;
			case EHOSTUNREACH: return KviError_hostUnreachable;       break;
			// Unhandled error...pass errno to the strerror function
			default:           return -iErrNo;                        break;
		}
#endif
	}
}
