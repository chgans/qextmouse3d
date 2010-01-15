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

#include "qglvertexbuffer.h"
#include "qglabstracteffect.h"
#include <QtCore/qlist.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLVertexBuffer
    \brief The QGLVertexBuffer class manages uploading of vertex attribute arrays into a GL server.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::enablers
*/

class QGLVertexBufferAttribute
{
public:
    QGLVertexBufferAttribute(QGL::VertexAttribute attr) : attribute(attr) {}

    virtual void clear() = 0;
    virtual QGLAttributeValue uploadValue() = 0;
    virtual int count() = 0;
    virtual int elementSize() = 0;

    QGL::VertexAttribute attribute;
    QGLAttributeValue value;
};

class QGLVertexBufferFloatAttribute : public QGLVertexBufferAttribute
{
public:
    QGLVertexBufferFloatAttribute
            (QGL::VertexAttribute attr, const QDataArray<float>& array)
        : QGLVertexBufferAttribute(attr), floatArray(array)
    {
        value = QGLAttributeValue(floatArray);
    }

    void clear() { floatArray.clear(); }
    QGLAttributeValue uploadValue()
        { return QGLAttributeValue(floatArray); }
    int count() { return floatArray.count(); }
    int elementSize() { return sizeof(float); }

    QDataArray<float> floatArray;
};

class QGLVertexBufferVector2DAttribute : public QGLVertexBufferAttribute
{
public:
    QGLVertexBufferVector2DAttribute
            (QGL::VertexAttribute attr, const QDataArray<QVector2D>& array)
        : QGLVertexBufferAttribute(attr), vector2DArray(array)
    {
        value = QGLAttributeValue(vector2DArray);
    }

    void clear() { vector2DArray.clear(); }
    QGLAttributeValue uploadValue()
        { return QGLAttributeValue(vector2DArray); }
    int count() { return vector2DArray.count(); }
    int elementSize() { return sizeof(QVector2D); }

    QDataArray<QVector2D> vector2DArray;
};

class QGLVertexBufferVector3DAttribute : public QGLVertexBufferAttribute
{
public:
    QGLVertexBufferVector3DAttribute
            (QGL::VertexAttribute attr, const QDataArray<QVector3D>& array)
        : QGLVertexBufferAttribute(attr), vector3DArray(array)
    {
        value = QGLAttributeValue(vector3DArray);
    }

    void clear() { vector3DArray.clear(); }
    QGLAttributeValue uploadValue()
        { return QGLAttributeValue(vector3DArray); }
    int count() { return vector3DArray.count(); }
    int elementSize() { return sizeof(QVector3D); }

    QDataArray<QVector3D> vector3DArray;
};

class QGLVertexBufferVector4DAttribute : public QGLVertexBufferAttribute
{
public:
    QGLVertexBufferVector4DAttribute
            (QGL::VertexAttribute attr, const QDataArray<QVector4D>& array)
        : QGLVertexBufferAttribute(attr), vector4DArray(array)
    {
        value = QGLAttributeValue(vector4DArray);
    }

    void clear() { vector4DArray.clear(); }
    QGLAttributeValue uploadValue()
        { return QGLAttributeValue(vector4DArray); }
    int count() { return vector4DArray.count(); }
    int elementSize() { return sizeof(QVector4D); }

    QDataArray<QVector4D> vector4DArray;
};

class QGLVertexBufferColorAttribute : public QGLVertexBufferAttribute
{
public:
    QGLVertexBufferColorAttribute
            (QGL::VertexAttribute attr, const QDataArray<QColor4b>& array)
        : QGLVertexBufferAttribute(attr), colorArray(array)
    {
        value = QGLAttributeValue(colorArray);
    }

    void clear() { colorArray.clear(); }
    QGLAttributeValue uploadValue()
        { return QGLAttributeValue(colorArray); }
    int count() { return colorArray.count(); }
    int elementSize() { return sizeof(QColor4b); }

    QDataArray<QColor4b> colorArray;
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

    QCustomDataArray customArray;
};

class QGLVertexBufferPrivate
{
public:
    QGLVertexBufferPrivate()
        : buffer(QGLBuffer::VertexBuffer),
          isUploaded(false),
          packingPolicy(QGLVertexBuffer::Interleave)
    {
    }
    ~QGLVertexBufferPrivate()
    {
        qDeleteAll(attributes);
    }

    QGLBuffer buffer;
    bool isUploaded;
    QGLVertexBuffer::PackingPolicy packingPolicy;
    QList<QGLVertexBufferAttribute *> attributes;
    QList<QGL::VertexAttribute> attributeNames;
};

/*!
    Constructs a new vertex buffer.
*/
QGLVertexBuffer::QGLVertexBuffer()
    : d_ptr(new QGLVertexBufferPrivate())
{
}

/*!
    Destroys this vertex buffer.
*/
QGLVertexBuffer::~QGLVertexBuffer()
{
}

/*!
    \enum QGLVertexBuffer::PackingPolicy
    This enum defines the policy for packing vertex attribute arrays
    into a QGLVertexBuffer.

    \value Interleave Interleaves the elements of the attribute arrays
           for better locality of reference.  This is the default.
    \value Append Appends the attribute arrays one after the other.
           This may be more efficient to upload into the GL server
           than Interleave, but may cause locality of reference issues
           when the data is used for drawing primitives.
*/

/*!
    Returns the packing policy to use when upload() is called.
    The default value is QGLVertexBuffer::Interleave.

    \sa setPackingPolicy(), upload()
*/
QGLVertexBuffer::PackingPolicy QGLVertexBuffer::packingPolicy() const
{
    Q_D(const QGLVertexBuffer);
    return d->packingPolicy;
}

/*!
    Sets the packing policy, to use when upload() is called, to \a value.
    This function must be called before upload() for the \a value
    to take effect.

    \sa packingPolicy(), upload()
*/
void QGLVertexBuffer::setPackingPolicy(QGLVertexBuffer::PackingPolicy value)
{
    Q_D(QGLVertexBuffer);
    d->packingPolicy = value;
}

/*!
    Returns the usage pattern for this vertex buffer.
    The default value is QGLBuffer::StaticDraw.

    \sa setUsagePattern()
*/
QGLBuffer::UsagePattern QGLVertexBuffer::usagePattern() const
{
    Q_D(const QGLVertexBuffer);
    return d->buffer.usagePattern();
}

/*!
    Sets the usage pattern for this vertex buffer to \a value.
    This function must be called before upload() for the \a value
    to take effect.

    \sa usagePattern(), upload()
*/
void QGLVertexBuffer::setUsagePattern(QGLBuffer::UsagePattern value)
{
    Q_D(QGLVertexBuffer);
    d->buffer.setUsagePattern(value);
}

/*!
    Adds the floating-point array \a value to this vertex buffer as the
    data for \a attribute.

    \sa upload()
*/
void QGLVertexBuffer::addAttribute
    (QGL::VertexAttribute attribute, const QDataArray<float>& value)
{
    Q_D(QGLVertexBuffer);
    if (!d->isUploaded) {
        d->attributes +=
            new QGLVertexBufferFloatAttribute(attribute, value);
        d->attributeNames += attribute;
    }
}

/*!
    Adds the 2D vector array \a value to this vertex buffer as the
    data for \a attribute.

    \sa upload()
*/
void QGLVertexBuffer::addAttribute
    (QGL::VertexAttribute attribute, const QDataArray<QVector2D>& value)
{
    Q_D(QGLVertexBuffer);
    if (!d->isUploaded) {
        d->attributes +=
            new QGLVertexBufferVector2DAttribute(attribute, value);
        d->attributeNames += attribute;
    }
}

/*!
    Adds the 3D vector array \a value to this vertex buffer as the
    data for \a attribute.

    \sa upload()
*/
void QGLVertexBuffer::addAttribute
    (QGL::VertexAttribute attribute, const QDataArray<QVector3D>& value)
{
    Q_D(QGLVertexBuffer);
    if (!d->isUploaded) {
        d->attributes +=
            new QGLVertexBufferVector3DAttribute(attribute, value);
        d->attributeNames += attribute;
    }
}

/*!
    Adds the 4D vector array \a value to this vertex buffer as the
    data for \a attribute.

    \sa upload()
*/
void QGLVertexBuffer::addAttribute
    (QGL::VertexAttribute attribute, const QDataArray<QVector4D>& value)
{
    Q_D(QGLVertexBuffer);
    if (!d->isUploaded) {
        d->attributes +=
            new QGLVertexBufferVector4DAttribute(attribute, value);
        d->attributeNames += attribute;
    }
}

/*!
    Adds the color array \a value to this vertex buffer as the
    data for \a attribute.

    \sa upload()
*/
void QGLVertexBuffer::addAttribute
    (QGL::VertexAttribute attribute, const QDataArray<QColor4b>& value)
{
    Q_D(QGLVertexBuffer);
    if (!d->isUploaded) {
        d->attributes +=
            new QGLVertexBufferColorAttribute(attribute, value);
        d->attributeNames += attribute;
    }
}

/*!
    Adds the custom data array \a value to this vertex buffer as the
    data for \a attribute.

    \sa upload()
*/
void QGLVertexBuffer::addAttribute
    (QGL::VertexAttribute attribute, const QCustomDataArray& value)
{
    Q_D(QGLVertexBuffer);
    if (!d->isUploaded) {
        d->attributes +=
            new QGLVertexBufferCustomAttribute(attribute, value);
        d->attributeNames += attribute;
    }
}

// Interleave a source array into a destination array.
static void vertexBufferInterleave
    (float *dst, int dstStride, const float *src, int srcStride, int count)
{
    switch (srcStride) {
    case 1:
        while (count-- > 0) {
            dst[0] = src[0];
            ++src;
            dst += dstStride;
        }
        break;
    case 2:
        while (count-- > 0) {
            dst[0] = src[0];
            dst[1] = src[1];
            src += 2;
            dst += dstStride;
        }
        break;
    case 3:
        while (count-- > 0) {
            dst[0] = src[0];
            dst[1] = src[1];
            dst[2] = src[2];
            src += 3;
            dst += dstStride;
        }
        break;
    case 4:
        while (count-- > 0) {
            dst[0] = src[0];
            dst[1] = src[1];
            dst[2] = src[2];
            dst[3] = src[3];
            src += 4;
            dst += dstStride;
        }
        break;
    default:
        while (count-- > 0) {
            for (int component = 0; component < srcStride; ++component)
                dst[component] = src[component];
            src += srcStride;
            dst += dstStride;
        }
        break;
    }
}

/*!
    Uploads the vertex data specified by previous addAttribute()
    calls into the GL server as a vertex buffer object.

    Returns true if the data could be uploaded; false if vertex buffer
    objects are not supported or there is insufficient memory to complete
    the request.  Returns true if the data was already uploaded.

    Once the vertex data has been uploaded, the client-side copies of
    the data arrays will be released.  If the vertex data could not be
    uploaded, then it is retained client-side.  This way, regardless of
    whether the data could be uploaded or not, QGLPainter::setVertexBuffer()
    can be used to support drawing of primitives using this object.

    \sa isUploaded(), addAttribute(), QGLPainter::setVertexBuffer()
*/
bool QGLVertexBuffer::upload()
{
    Q_D(QGLVertexBuffer);
    QGLVertexBufferAttribute *attr;

    // Nothing to do if already uploaded or there are no attributes.
    if (d->isUploaded)
        return true;
    if (d->attributes.isEmpty())
        return false;

    // Create the VBO in the GL server.
    if (!d->buffer.create())
        return false;

    // If there is only one attribute, then realloc and write in one step.
    if (d->attributes.size() == 1) {
        attr = d->attributes[0];
        d->buffer.write(attr->value.data(),
                        attr->count() * attr->elementSize());
        attr->value.setOffset(0);
        attr->clear();
        return true;
    }

    // Calculate the total size of the VBO that we will need,
    // the maximum number of interleaved vertices, and the
    // interleaved stride.
    int size = 0;
    int stride = 0;
    int maxCount = 0;
    for (int index = 0; index < d->attributes.size(); ++index) {
        attr = d->attributes[index];
        int count = attr->count();
        if (count > maxCount)
            maxCount = count;
        int elemSize = attr->elementSize();
        size += count * elemSize;
        stride += elemSize;
    }
    d->buffer.resize(size);
    stride /= sizeof(float);

    // Determine how to upload the data, using a map if possible.
    void *mapped = d->buffer.map(QGLBuffer::WriteOnly);
    int offset = 0;
    if (d->packingPolicy == QGLVertexBuffer::Interleave) {
        // Interleave the data into the final buffer.  We do it in
        // sections so as to keep locality problems to a minimum.
        QDataArray<float> temp;
        float *dst;
        if (mapped)
            dst = reinterpret_cast<float *>(mapped);
        else
            dst = temp.extend(1024);
        int sectionSize = 1024 / stride;
        for (int vertex = 0; vertex < maxCount; vertex += sectionSize) {
            for (int index = 0; index < d->attributes.size(); ++index) {
                attr = d->attributes[index];
                int count = attr->count() - vertex;
                if (count <= 0)
                    continue;
                count = qMin(count, sectionSize);
                vertexBufferInterleave
                    (dst, stride, attr->value.floatData(),
                     attr->elementSize() / sizeof(float), count);
            }
            size = sectionSize * stride;
            if (mapped) {
                dst += size;
            } else {
                size *= sizeof(float);
                d->buffer.write(offset, dst, size);
                offset += size;
            }
        }
        offset = 0;
        for (int index = 0; index < d->attributes.size(); ++index) {
            attr = d->attributes[index];
            attr->value.setOffset(offset);
            offset += attr->elementSize();
            attr->clear();
        }
    } else {
        // Append the arrays to each other and write.
        for (int index = 0; index < d->attributes.size(); ++index) {
            attr = d->attributes[index];
            size = attr->count() * attr->elementSize();
            if (mapped) {
                qMemCopy(reinterpret_cast<char *>(mapped) + offset,
                         attr->value.data(), size);
            } else {
                d->buffer.write(offset, attr->value.data(), size);
            }
            attr->value.setOffset(offset);
            attr->clear();
            offset += size;
        }
    }
    if (mapped)
        d->buffer.unmap();

    // Buffer is uploaded and ready to go.
    d->isUploaded = true;
    return true;
}

/*!
    Returns true if the vertex data specified by previous addAttribute()
    calls has been uploaded into the GL server; false otherwise.

    \sa upload(), addAttribute()
*/
bool QGLVertexBuffer::isUploaded() const
{
    Q_D(const QGLVertexBuffer);
    return d->isUploaded;
}

/*!
    Returns the QGLBuffer in use by this vertex buffer object,
    so that its properties or contents can be modified directly.
*/
QGLBuffer *QGLVertexBuffer::buffer() const
{
    Q_D(const QGLVertexBuffer);
    return const_cast<QGLBuffer *>(&(d->buffer));
}

/*!
    \internal
*/
void QGLVertexBuffer::setOnEffect(QGLAbstractEffect *effect) const
{
    Q_D(const QGLVertexBuffer);
    if (d->isUploaded)
        d->buffer.bind();
    for (int index = 0; index < d->attributes.size(); ++index) {
        QGLVertexBufferAttribute *attr = d->attributes[index];
        effect->setVertexAttribute(attr->attribute, attr->value);
    }
    if (d->isUploaded)
        d->buffer.release();
}

/*!
    \internal
*/
QList<QGL::VertexAttribute> QGLVertexBuffer::attributes() const
{
    Q_D(const QGLVertexBuffer);
    return d->attributeNames;
}

QT_END_NAMESPACE
