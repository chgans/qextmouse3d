/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QGLVERTEXDESCRIPTION_H
#define QGLVERTEXDESCRIPTION_H

#include <QtCore/qvector.h>
#include "qt3dglobal.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

namespace QGL
{
    enum VertexAttribute {
        Position,
        Normal,
        Color,
        TextureCoord0,
        TextureCoord1,
        TextureCoord2,
        TextureCoord3,
        TextureCoord4,
        TextureCoord5,
        TextureCoord6,
        TextureCoord7,
        CustomVertex0,
        CustomVertex1,
        CustomVertex2,
        CustomVertex3,
        CustomVertex4,
        CustomVertex5,
        CustomVertex6,
        CustomVertex7
    };
};

class Q_QT3D_EXPORT QGLVertexDescription
{
public:
    QGLVertexDescription();
    QGLVertexDescription(const QGLVertexDescription& other);

    int fieldCount() const;
    int stride() const;

    void addField(QGL::VertexAttribute attr, int size);

    void addFields(const QGLVertexDescription& other);

    int indexOf(QGL::VertexAttribute attr) const;

    QGL::VertexAttribute fieldAttribute(int index) const;
    int fieldSize(int index) const;
    int fieldOffset(int index) const;

    bool operator==(const QGLVertexDescription& other) const;
    bool operator!=(const QGLVertexDescription& other) const;

private:
    struct Field
    {
        QGL::VertexAttribute attr;
        int size;
        int offset;
        int reserved;

        bool operator==(const Field& field) const
        {
            return attr == field.attr && size == field.size &&
                   offset == field.offset && reserved == field.reserved;
        }
    };
    QVector<Field> m_fields;
    int m_stride;

    friend class QGLVertexArray;
};

inline QGLVertexDescription::QGLVertexDescription()
    : m_stride(0)
{
}

inline QGLVertexDescription::QGLVertexDescription
        (const QGLVertexDescription& other)
    : m_fields(other.m_fields), m_stride(other.m_stride)
{
}

inline int QGLVertexDescription::fieldCount() const
{
    return m_fields.count();
}

inline int QGLVertexDescription::stride() const
{
    return m_stride;
}

inline QGL::VertexAttribute QGLVertexDescription::fieldAttribute(int index) const
{
    if (index >= 0 && index < m_fields.count())
        return m_fields[index].attr;
    else
        return QGL::Position;
}

inline int QGLVertexDescription::fieldSize(int index) const
{
    if (index >= 0 && index < m_fields.count())
        return m_fields[index].size;
    else
        return 0;
}

inline int QGLVertexDescription::fieldOffset(int index) const
{
    if (index >= 0 && index < m_fields.count())
        return m_fields[index].offset;
    else
        return 0;
}

inline bool QGLVertexDescription::operator==(const QGLVertexDescription& other) const
{
    return m_fields == other.m_fields && m_stride == other.m_stride;
}

inline bool QGLVertexDescription::operator!=(const QGLVertexDescription& other) const
{
    return !(m_fields == other.m_fields) || m_stride != other.m_stride;
}

#ifndef QT_NO_DEBUG_STREAM
Q_QT3D_EXPORT QDebug operator<<(QDebug dbg, const QGLVertexDescription &description);
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif
