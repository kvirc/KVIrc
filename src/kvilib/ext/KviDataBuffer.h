#ifndef _KVI_DATABUFFER_H_
#define _KVI_DATABUFFER_H_
//=============================================================================
//
//   File : KviDataBuffer.h
//   Creation date : Thu Aug 23 17:04:25 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)
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
#include "KviHeapObject.h"

class KVILIB_API KviDataBuffer : public KviHeapObject
{
public:
	// uSize MUST be greater than 0
	// if data is non-zero, it MUST point to a buffer at least uSize bytes long
	// and the data is COPIED from that buffer!
	KviDataBuffer(int uSize, const unsigned char * data = nullptr);
	KviDataBuffer();
	~KviDataBuffer();

private:
	int m_uSize;
	unsigned char * m_pData;

public:
	int size() const { return m_uSize; };
	unsigned char * data() const { return m_pData; };
	// uSize MUST be smaller or equal to size()
	// consumes data!
	void remove(int uSize);
	void clear()
	{
		if(m_uSize > 0)
			remove(m_uSize);
	};
	// uSize MUST be greater than 0
	void resize(int uSize);
	void addSize(int uSize) { resize(m_uSize + uSize); };
	void append(const unsigned char * data, int uSize);
	void append(const KviDataBuffer & b) { append(b.data(), b.size()); };
	int find(unsigned char c);
	int find(const unsigned char * block, int uSize);
};

#endif //_KVI_DATABUFFER_H_
