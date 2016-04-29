//=============================================================================
//
//   File : XmlFunctions.h
//   Creation date : Fri Aug 08 18:00:00 2008 GMT by Fabio Bas
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2008 Fabio Bas (ctrlaltca at gmail dot com)
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
//   Original Copyright follows:
//=============================================================================

/***************************************************************************
                          XmlFunctions.cpp -  description
                             -------------------
    begin                : Sun Jul 24 2005
    copyright            : (C) 2005 by Diederik van der Boor
    email                : vdboor --at-- codingdomain.com
 ***************************************************************************/

#ifndef XMLFUNCTIONS_H
#define XMLFUNCTIONS_H

#include <QDomNode>
#include <QDebug>

/**
 * Some helper functions to make the handling of QDom easier.
 *
 * @author Diederik van der Boor
 * @ingroup NetworkExtra
 */
class XmlFunctions
{
public:
	// Helper function, get a specific node
	static QDomNode getNode(const QDomNode & rootNode, const QString & path);
	// Helper function, get the attribute text of a node
	static QString getNodeAttribute(const QDomNode & node, const QString & attribute);
	// Helper function, get a specific child node
	static QDomNode getNodeChildByKey(const QDomNodeList & childNodes,
	    const QString & keyTagName, const QString & keyValue);
	// Helper function, get the text value of a node
	static QString getNodeValue(const QDomNode & rootNode, const QString & path);
	// Helper function, get the source XML of a node.
	static QString getSource(const QDomNode & node, int indent = 0);
};

#endif
