//=============================================================================
//
//   File : XmlFunctions.cpp
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

#include "XmlFunctions.h"

#include <QStringList>

// Helper function, get a specific node
QDomNode XmlFunctions::getNode(const QDomNode & rootNode, const QString & path)
{

	QStringList pathItems = path.split("/", QString::SkipEmptyParts);
	QDomNode childNode = rootNode.namedItem(pathItems[0]); // can be a null node

	int i = 1;
	while(i < pathItems.count())
	{
		if(childNode.isNull())
		{
			break;
		}

		childNode = childNode.namedItem(pathItems[i]);
		i++; // not using for loop so i is always correct for kdDebug() below.
	}

	if(childNode.isNull())
	{
		qDebug() << "XmlFunctions::getNode() - Notice: node '" << pathItems[i - 1] << "'"
		         << " does not exist (root=" << rootNode.nodeName() << " path=" << path << ")." << endl;
	}

	return childNode;
}

// Helper function, get the attribute text of a node
QString XmlFunctions::getNodeAttribute(const QDomNode & node, const QString & attribute)
{

	// Writing this is not funny
	return node.attributes().namedItem(attribute).toAttr().value();
	// node.toElement().attribute( attribute );  does not work for const nodes.
}

// Helper function, get a specific child node
QDomNode XmlFunctions::getNodeChildByKey(const QDomNodeList & childNodes, const QString & keyTagName, const QString & keyValue)
{

	for(int i = 0; i < childNodes.count(); i++)
	{
		//    kdDebug() << "node " << childNodes.item(i).nodeName() << "/" << keyTagName
		//              << "="     << childNodes.item(i).namedItem(keyTagName).toElement().text() << " == " << keyValue << "?" << endl;

		// If the node has an childname with a certain value... e.g. <childNodes> <item><name>value</name></item> .. </childNodes>
		if(childNodes.item(i).namedItem(keyTagName).toElement().text() == keyValue)
		{
			// Return the node
			return childNodes.item(i);
		}
	}

	// Return a null node (is there a better way?)
	return childNodes.item(childNodes.count());
}

// Helper function, get the text value of a node
QString XmlFunctions::getNodeValue(const QDomNode & rootNode, const QString & path)
{

	// Added code to avoid more assertion errors, and trace the cause.
	if(rootNode.isNull())
	{
		qWarning() << "XmlFunctions::getNodeValue: attempted to request '" << path << "' on null root node." << endl;
		return QString();
	}

	// Because writing node.namedItem("childItem").namedItem("child2").toElement().text() is not funny.
	return getNode(rootNode, path).toElement().text();
}

// Helper function, get the source XML of a node.
QString XmlFunctions::getSource(const QDomNode & node, int indent)
{
	QString source;
	QTextStream textStream(&source, QIODevice::WriteOnly);
	node.save(textStream, indent);
	return source;
}
