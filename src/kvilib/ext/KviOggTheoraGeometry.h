#ifndef _KVIOGGTHEORAGEOMETRY_H_
#define _KVIOGGTHEORAGEOMETRY_H_
//=============================================================================
//
//   File : KviOggTheoraGeometry.h
//   Creation date : Wed Dec 29 2010 15:51:21 CEST by Elvio Basello
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2010 Elvio Basello (hellvis69 at gmail dot com)
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
* \file KviOggTheoraGeometry.h
* \author Elvio Basello
* \brief This file was originally part of KviTheoraDecoder.h
*/

#ifndef COMPILE_DISABLE_OGG_THEORA

/**
* \typedef KviOggTheoraGeometry
* \brief Contains all the needed geometry information of a theora video stream
*/
struct KviOggTheoraGeometry
{
	int pic_w; /**< width of original picture geometry, chosen by the user */
	int pic_h; /**< height of original picture geometry, chosen by the user */

	int frame_w; /**< width of the picture frame (calculated geometry) */
	int frame_h; /**< height of the picture frame (calculated geometry) */

	int pic_x; /**< x offset of the picture inside the frame (calculated geometry) */
	int pic_y; /**< y offset of the picture inside the frame (calculated geometry) */
};

#endif // COMPILE_DISABLE_OGG_THEORA
#endif // _KVIOGGTHEORAGEOMETRY_H_
