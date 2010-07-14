#ifndef _kvi_debugcontext_h_
#define _kvi_debugcontext_h_
//=============================================================================
//
// File : kvi_debugcontext.h
// Creation date : Tue 13 Jul 2010 23:27:23
// Project : KVIrc Irc client
// Author : Szymon Stefanek <pragma at kvirc dot net>
//
//=============================================================================

#include "kvi_settings.h"
#include "kvi_string.h"

///
/// \class KviDebugContext
/// \brief The KviDebugContext class
///
/// This class...
///
class KVILIB_API KviDebugContext
{
public:

	///
	/// Creates an instance of KviDebugContext
	///
	KviDebugContext(const char * szContext,...);

	///
	/// Destroys the instance of KviDebugContext
	/// and frees all the relevant resources
	///
	~KviDebugContext();

public:
	static void trace(const char * szFmt,...);

private:
	KviStr m_szContext;

private:
	KviDebugContext(const KviDebugContext &){};
	const KviDebugContext & operator = (const KviDebugContext &){ return *this; };

}; // class KviDebugContext


#endif //!_kvi_debugcontext_h_
