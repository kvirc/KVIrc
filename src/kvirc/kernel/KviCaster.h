//=============================================================================
//
//   File : KviCaster.h
//   Creation date : Mon April 18 2016 02:00:36 by Matt Ullman
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2016 Matt Ullman (staticfox at staticfox dot net)
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

#include <typeinfo>

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
#define KVI_CAST_FUNC __FUNCTION__
#else
#define KVI_CAST_FUNC __func__
#endif

#define KVI_DYNAMIC(expr, expr2)          \
	KviCast::dynamic<expr>(expr2,         \
	    __FILE__ ":" STRINGIFY(__LINE__), \
	    KVI_CAST_FUNC,                    \
	    #expr)

namespace KviCast
{
	template <typename T, typename P>
	T dynamic(P p, const char * file, const char * func, const char * expr)
	{
		T newPointer = dynamic_cast<T>(p);

		KVI_ASSERT(newPointer != nullptr);

		if(newPointer == nullptr)
		{
			qDebug("KviCast::dynamic: Conversion from %s to %s failed: %s in %s (%s).",
			    typeid(P).name(), typeid(T).name(), func, file, expr);
		}

		return newPointer;
	}
}
