/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qlogicalvertex.h"

#include <QtCore/qdebug.h>

const qreal QLogicalVertex::InvalidTexValue = -std::numeric_limits<qreal>::max();
const QVector2D QLogicalVertex::InvalidTexCoord = QVector2D(QLogicalVertex::InvalidTexValue,
                                                            QLogicalVertex::InvalidTexValue);

/*!
    \enum QLogicalVertex::Type
    This enumeration specifies which types of OpenGL data is
    utilized in specifying geometry.
    \value None No vertex data is utilized.
    \value Vertex Vertex data is utilized
    \value Normals Normal vector data is utilized
    \value Textures Texture coordinate data is utilized
    \value Color per vertex data is utilized
*/


#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const QLogicalVertex &lv)
{
    dbg.nospace();
    dbg << "QLogicalVertex(";
    if (lv.isNull())
    {
        dbg << " NULL";
    }
    else
    {
        if (lv.hasType(QLogicalVertex::Vertex))
            dbg << lv.vertex();
        else
            dbg << " (No Vertex)";
        if (lv.hasType(QLogicalVertex::Normal))
            dbg << lv.normal();
        else
            dbg << " (No Normal)";
        if (lv.hasType(QLogicalVertex::Texture))
            dbg << lv.texCoord();
        else
            dbg << " (No Texture)";
        if (lv.hasType(QLogicalVertex::Color))
            dbg << lv.color();
        else
            dbg << " (No Color)";
    }
    dbg << " )";
    return dbg.space();
}
#endif
