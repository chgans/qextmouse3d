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

#ifndef QGLVERTEXBUFFER_P_H
#define QGLVERTEXBUFFER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qglvertexbuffer.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QGLVertexBufferAttribute
{
public:
    QGLVertexBufferAttribute(QGL::VertexAttribute attr)
        : attribute(attr), index(int(attr)) {}

    virtual void clear() = 0;
    virtual QGLAttributeValue uploadValue() = 0;
    virtual int count() = 0;
    virtual int elementSize() = 0;
    virtual void replace
        (int index, int count, const QGLAttributeValue& value) = 0;

    QGL::VertexAttribute attribute;
    QGLAttributeValue value;
    int index;
};

class QGLVertexBufferFloatAttribute : public QGLVertexBufferAttribute
{
public:
    QGLVertexBufferFloatAttribute
            (QGL::VertexAttribute attr, const QArray<float>& array)
        : QGLVertexBufferAttribute(attr), floatArray(array)
    {
        value = QGLAttributeValue(floatArray);
    }

    void clear() { floatArray.clear(); }
    QGLAttributeValue uploadValue()
        { return QGLAttributeValue(floatArray); }
    int count() { return floatArray.count(); }
    int elementSize() { return sizeof(float); }
    void replace(int index, int count, const QGLAttributeValue& value);

    QArray<float> floatArray;
};

class QGLVertexBufferVector2DAttribute : public QGLVertexBufferAttribute
{
public:
    QGLVertexBufferVector2DAttribute
            (QGL::VertexAttribute attr, const QArray<QVector2D>& array)
        : QGLVertexBufferAttribute(attr), vector2DArray(array)
    {
        value = QGLAttributeValue(vector2DArray);
    }

    void clear() { vector2DArray.clear(); }
    QGLAttributeValue uploadValue()
        { return QGLAttributeValue(vector2DArray); }
    int count() { return vector2DArray.count(); }
    int elementSize() { return sizeof(QVector2D); }
    void replace(int index, int count, const QGLAttributeValue& value);

    QArray<QVector2D> vector2DArray;
};

class QGLVertexBufferVector3DAttribute : public QGLVertexBufferAttribute
{
public:
    QGLVertexBufferVector3DAttribute
            (QGL::VertexAttribute attr, const QArray<QVector3D>& array)
        : QGLVertexBufferAttribute(attr), vector3DArray(array)
    {
        value = QGLAttributeValue(vector3DArray);
    }

    void clear() { vector3DArray.clear(); }
    QGLAttributeValue uploadValue()
        { return QGLAttributeValue(vector3DArray); }
    int count() { return vector3DArray.count(); }
    int elementSize() { return sizeof(QVector3D); }
    void replace(int index, int count, const QGLAttributeValue& value);

    QArray<QVector3D> vector3DArray;
};

class QGLVertexBufferVector4DAttribute : public QGLVertexBufferAttribute
{
public:
    QGLVertexBufferVector4DAttribute
            (QGL::VertexAttribute attr, const QArray<QVector4D>& array)
        : QGLVertexBufferAttribute(attr), vector4DArray(array)
    {
        value = QGLAttributeValue(vector4DArray);
    }

    void clear() { vector4DArray.clear(); }
    QGLAttributeValue uploadValue()
        { return QGLAttributeValue(vector4DArray); }
    int count() { return vector4DArray.count(); }
    int elementSize() { return sizeof(QVector4D); }
    void replace(int index, int count, const QGLAttributeValue& value);

    QArray<QVector4D> vector4DArray;
};

class QGLVertexBufferColorAttribute : public QGLVertexBufferAttribute
{
public:
    QGLVertexBufferColorAttribute
            (QGL::VertexAttribute attr, const QArray<QColor4B>& array)
        : QGLVertexBufferAttribute(attr), colorArray(array)
    {
        value = QGLAttributeValue(colorArray);
    }

    void clear() { colorArray.clear(); }
    QGLAttributeValue uploadValue()
        { return QGLAttributeValue(colorArray); }
    int count() { return colorArray.count(); }
    int elementSize() { return sizeof(QColor4B); }
    void replace(int index, int count, const QGLAttributeValue& value);

    QArray<QColor4B> colorArray;
};

class QGLVertexBufferCustomAttribute : public QGLVertexBufferAttribute
{
public:
    QGLVertexBufferCustomAttribute
            (QGL::VertexAttribute attr, const QCustomDataArray& array)
        : QGLVertexBufferAttribute(attr), customArray(array)
    {
        value = QGLAttributeValue(customArray);
    }

    void clear() { customArray.clear(); }
    QGLAttributeValue uploadValue()
        { return QGLAttributeValue(customArray); }
    int count() { return customArray.count(); }
    int elementSize() { return customArray.elementSize(); }
    void replace(int index, int count, const QGLAttributeValue& value);

    QCustomDataArray customArray;
};

class QGLVertexBufferPrivate
{
public:
    QGLVertexBufferPrivate()
        : buffer(0),
          usagePattern(QGLBuffer::StaticDraw),
          packingHint(QGLVertexBuffer::Interleave),
          actualPackingHint(QGLVertexBuffer::Interleave),
          vertexCount(0)
    {
        ref = 1;
    }
    ~QGLVertexBufferPrivate()
    {
        qDeleteAll(attributes);
        delete buffer;
    }

    QBasicAtomicInt ref;
    QGLBuffer *buffer;
    QGLBuffer::UsagePattern usagePattern;
    QGLVertexBuffer::PackingHint packingHint;
    QGLVertexBuffer::PackingHint actualPackingHint;
    QList<QGLVertexBufferAttribute *> attributes;
    QList<QGL::VertexAttribute> attributeNames;
    int vertexCount;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
