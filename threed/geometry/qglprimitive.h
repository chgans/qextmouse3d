/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt3D module of the Qt Toolkit.
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

#ifndef QGLPRIMITIVE_H
#define QGLPRIMITIVE_H

#include "qglnamespace.h"
#include "qgeometrydata.h"

namespace QGL
{
    enum Operation
    {
        NO_OP,
        TRIANGLE,
        TRIANGLE_STRIP,
        TRIANGLE_FAN,
        TRIANGULATED_FACE,
        QUAD,
        QUAD_STRIP,
        QUADS_ZIPPED
    };

    enum OperationFlag
    {
        NO_FLAG               = 0X00,
        FACE_SENSE_REVERSED   = 0x01,
        NO_CLOSE_PATH         = 0x02,
        NEXT_PRIMITIVE        = 0x04,
        RETAIN_PRIMITIVE      = 0x08,
        USE_VERTEX_0_AS_CTR   = 0x10
    };
    Q_DECLARE_FLAGS(OperationFlags, OperationFlag);
}
Q_DECLARE_OPERATORS_FOR_FLAGS(QGL::OperationFlags);

class QGLPrimitive : public QGeometryData
{
public:
    QGLPrimitive();
    virtual ~QGLPrimitive() {}
    void setFlags(QGL::OperationFlags flags) { m_flags = flags; }
    QGL::OperationFlags flags() const { return m_flags; }
    QGLPrimitive reversed() const;
    QGLPrimitive translated(const QVector3D &) const;
    QGLPrimitive zippedWith(const QGLPrimitive &other) const;
    void generateTextureCoordinates(Qt::Orientation orientation = Qt::Horizontal,
                                    QGL::VertexAttribute attribute = QGL::TextureCoord0);
    QVector3D center() const;
protected:
    QGL::OperationFlags m_flags;
};

#endif // QGLPRIMITIVE_H
