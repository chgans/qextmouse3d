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

#include "qglvertexbuffer.h"
#include "qglvertexbuffer_p.h"
#include "qglabstracteffect.h"
#include <QtCore/qlist.h>
#include <QtCore/qatomic.h>
#include <QtOpenGL/qglshaderprogram.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLVertexBuffer
    \brief The QGLVertexBuffer class manages uploading of vertex attribute arrays into a GL server.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::enablers
*/

void QGLVertexBufferFloatAttribute::replace
    (int index, int count, const QGLAttributeValue& value)
{
    Q_ASSERT(value.type() == GL_FLOAT);
    Q_ASSERT(value.tupleSize() == 1);
    Q_ASSERT(value.stride() == 0 || value.stride() == sizeof(float));
    Q_ASSERT(index >= 0 && count >= 0 &&
             (index + count) <= floatArray.size());
    floatArray.replace(index, value.floatData(), count);
}

void QGLVertexBufferVector2DAttribute::replace
    (int index, int count, const QGLAttributeValue& value)
{
    Q_ASSERT(value.type() == GL_FLOAT);
    Q_ASSERT(value.tupleSize() == 2);
    Q_ASSERT(value.stride() == 0 || value.stride() == (sizeof(float) * 2));
    Q_ASSERT(index >= 0 && count >= 0 &&
             (index + count) <= vector2DArray.size());
    vector2DArray.replace
        (index, reinterpret_cast<const QVector2D *>(value.floatData()), count);
}

void QGLVertexBufferVector3DAttribute::replace
    (int index, int count, const QGLAttributeValue& value)
{
    Q_ASSERT(value.type() == GL_FLOAT);
    Q_ASSERT(value.tupleSize() == 3);
    Q_ASSERT(value.stride() == 0 || value.stride() == (sizeof(float) * 3));
    Q_ASSERT(index >= 0 && count >= 0 &&
             (index + count) <= vector3DArray.size());
    vector3DArray.replace
        (index, reinterpret_cast<const QVector3D *>(value.floatData()), count);
}

void QGLVertexBufferVector4DAttribute::replace
    (int index, int count, const QGLAttributeValue& value)
{
    Q_ASSERT(value.type() == GL_FLOAT);
    Q_ASSERT(value.tupleSize() == 4);
    Q_ASSERT(value.stride() == 0 || value.stride() == (sizeof(float) * 4));
    Q_ASSERT(index >= 0 && count >= 0 &&
             (index + count) <= vector4DArray.size());
    vector4DArray.replace
        (index, reinterpret_cast<const QVector4D *>(value.floatData()), count);
}

void QGLVertexBufferColorAttribute::replace
    (int index, int count, const QGLAttributeValue& value)
{
    Q_ASSERT(value.type() == GL_UNSIGNED_BYTE);
    Q_ASSERT(value.tupleSize() == 4);
    Q_ASSERT(value.stride() == 0 || value.stride() == sizeof(QColor4ub));
    Q_ASSERT(index >= 0 && count >= 0 &&
             (index + count) <= colorArray.size());
    colorArray.replace
        (index, reinterpret_cast<const QColor4ub *>(value.data()), count);
}

void QGLVertexBufferCustomAttribute::replace
    (int index, int count, const QGLAttributeValue& value)
{
    Q_ASSERT(index >= 0 && count >= 0 &&
             (index + count) <= customArray.size());
    switch (customArray.elementType()) {
    case QCustomDataArray::Float: {
        Q_ASSERT(value.type() == GL_FLOAT);
        Q_ASSERT(value.tupleSize() == 1);
        Q_ASSERT(value.stride() == 0 || value.stride() == sizeof(float));
        customArray.m_array.replace(index, value.floatData(), count);
    }
    break;

    case QCustomDataArray::Vector2D: {
        Q_ASSERT(value.type() == GL_FLOAT);
        Q_ASSERT(value.tupleSize() == 2);
        Q_ASSERT(value.stride() == 0 || value.stride() == (sizeof(float) * 2));
        customArray.m_array.replace(index * 2, value.floatData(), count * 2);
    }
    break;

    case QCustomDataArray::Vector3D: {
        Q_ASSERT(value.type() == GL_FLOAT);
        Q_ASSERT(value.tupleSize() == 3);
        Q_ASSERT(value.stride() == 0 || value.stride() == (sizeof(float) * 3));
        customArray.m_array.replace(index * 3, value.floatData(), count * 3);
    }
    break;

    case QCustomDataArray::Vector4D: {
        Q_ASSERT(value.type() == GL_FLOAT);
        Q_ASSERT(value.tupleSize() == 4);
        Q_ASSERT(value.stride() == 0 || value.stride() == (sizeof(float) * 4));
        customArray.m_array.replace(index * 4, value.floatData(), count * 4);
    }
    break;

    case QCustomDataArray::Color: {
        Q_ASSERT(value.type() == GL_UNSIGNED_BYTE);
        Q_ASSERT(value.tupleSize() == 4);
        Q_ASSERT(value.stride() == 0 || value.stride() == sizeof(QColor4ub));
        customArray.m_array.replace(index, value.floatData(), count);
    }
    break;

    default: break;
    }
}

/*!
    Constructs a new vertex buffer.
*/
QGLVertexBuffer::QGLVertexBuffer()
    : d_ptr(new QGLVertexBufferPrivate())
{
}

/*!
    Creates a copy of \a other.  Note that this just copies a reference
    to the vertex buffer.  Any modifications to the copy will also
    affect the original object.
*/
QGLVertexBuffer::QGLVertexBuffer(const QGLVertexBuffer& other)
    : d_ptr(other.d_ptr)
{
    d_ptr->ref.ref();
}

/*!
    Destroys this vertex buffer if this object is the last reference to it.
*/
QGLVertexBuffer::~QGLVertexBuffer()
{
    if (!d_ptr->ref.deref())
        delete d_ptr;
}

/*!
    Assigns \a other to this object.  Note that this just assigns a
    reference to the \a other vertex buffer.  Any modifications to this
    object will also affect \a other.
*/
QGLVertexBuffer& QGLVertexBuffer::operator=(const QGLVertexBuffer& other)
{
    if (d_ptr != other.d_ptr) {
        if (!d_ptr->ref.deref())
            delete d_ptr;
        d_ptr = other.d_ptr;
        d_ptr->ref.ref();
    }
    return *this;
}

/*!
    \enum QGLVertexBuffer::PackingHint
    This enum defines a hint to QGLVertexBuffer for how the vertex
    attributes should be packed.

    \value Interleave Interleaves the elements of the attribute arrays
           for better locality of reference.  This is the default.
           The Interleave hint will be ignored if
           QGLVertexBuffer::usagePattern() is QGLBuffer::DynamicDraw
           and the buffer cannot be mapped into client-side memory.
           This makes QGLVertexBuffer::replaceAttribute() more efficient
           on systems without \c{glMapBuffer()}.

    \value Append Appends the attribute arrays one after the other.
           This may be more efficient to upload into the GL server
           than Interleave, but may cause locality of reference issues
           when the data is used for drawing primitives.
*/

/*!
    Returns the packing policy to use when upload() is called.
    The default value is QGLVertexBuffer::Interleave.

    \sa setPackingHint(), upload()
*/
QGLVertexBuffer::PackingHint QGLVertexBuffer::packingHint() const
{
    Q_D(const QGLVertexBuffer);
    return d->packingHint;
}

/*!
    Sets the packing policy, to use when upload() is called, to \a value.
    This function must be called before upload() for the \a value
    to take effect.

    \sa packingHint(), upload()
*/
void QGLVertexBuffer::setPackingHint(QGLVertexBuffer::PackingHint value)
{
    Q_D(QGLVertexBuffer);
    d->packingHint = value;
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
    (QGL::VertexAttribute attribute, const QArray<float>& value)
{
    Q_D(QGLVertexBuffer);
    if (!d->buffer.isCreated()) {
        d->attributes +=
            new QGLVertexBufferFloatAttribute(attribute, value);
        d->attributeNames += attribute;
        d->vertexCount = qMax(d->vertexCount, value.count());
    }
}

/*!
    Adds the 2D vector array \a value to this vertex buffer as the
    data for \a attribute.

    \sa upload()
*/
void QGLVertexBuffer::addAttribute
    (QGL::VertexAttribute attribute, const QArray<QVector2D>& value)
{
    Q_D(QGLVertexBuffer);
    if (!d->buffer.isCreated()) {
        d->attributes +=
            new QGLVertexBufferVector2DAttribute(attribute, value);
        d->attributeNames += attribute;
        d->vertexCount = qMax(d->vertexCount, value.count());
    }
}

/*!
    Adds the 3D vector array \a value to this vertex buffer as the
    data for \a attribute.

    \sa upload()
*/
void QGLVertexBuffer::addAttribute
    (QGL::VertexAttribute attribute, const QArray<QVector3D>& value)
{
    Q_D(QGLVertexBuffer);
    if (!d->buffer.isCreated()) {
        d->attributes +=
            new QGLVertexBufferVector3DAttribute(attribute, value);
        d->attributeNames += attribute;
        d->vertexCount = qMax(d->vertexCount, value.count());
    }
}

/*!
    Adds the 4D vector array \a value to this vertex buffer as the
    data for \a attribute.

    \sa upload()
*/
void QGLVertexBuffer::addAttribute
    (QGL::VertexAttribute attribute, const QArray<QVector4D>& value)
{
    Q_D(QGLVertexBuffer);
    if (!d->buffer.isCreated()) {
        d->attributes +=
            new QGLVertexBufferVector4DAttribute(attribute, value);
        d->attributeNames += attribute;
        d->vertexCount = qMax(d->vertexCount, value.count());
    }
}

/*!
    Adds the color array \a value to this vertex buffer as the
    data for \a attribute.

    \sa upload()
*/
void QGLVertexBuffer::addAttribute
    (QGL::VertexAttribute attribute, const QArray<QColor4ub>& value)
{
    Q_D(QGLVertexBuffer);
    if (!d->buffer.isCreated()) {
        d->attributes +=
            new QGLVertexBufferColorAttribute(attribute, value);
        d->attributeNames += attribute;
        d->vertexCount = qMax(d->vertexCount, value.count());
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
    if (!d->buffer.isCreated()) {
        d->attributes +=
            new QGLVertexBufferCustomAttribute(attribute, value);
        d->attributeNames += attribute;
        d->vertexCount = qMax(d->vertexCount, value.count());
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
    Replaces the \a count elements starting at \a index
    associated with \a attribute with the contents of \a value.

    If this function is called before upload(), or vertex buffers are
    not supported by the GL server, then the client-side copies of
    the attribute data will be updated.  Otherwise the new data will
    be uploaded into the GL server.

    This function will be ignored if usagePattern() is not
    QGLBuffer::DynamicDraw, or \a attribute is not one of the
    attributes that was previously added with addAttribute().

    The data in \a value must be consistent in layout with the original
    data that was written with addAttribute().

    \sa addAttribute()
*/
void QGLVertexBuffer::replaceAttribute
    (QGL::VertexAttribute attribute, int index, int count,
     const QGLAttributeValue& value)
{
    Q_ASSERT(index >= 0 && count >= 0);
    Q_D(QGLVertexBuffer);

    // Check that the the buffer is dynamic.
    if (d->buffer.usagePattern() != QGLBuffer::DynamicDraw) {
        qWarning("QGLVertexBuffer::replaceAttribute: usage pattern is "
                 "not QGLBuffer::DynamicDraw");
        return;
    }

    // Find the existing attribute definition.
    QGLVertexBufferAttribute *attr = 0;
    int attrIndex;
    for (attrIndex = 0; attrIndex < d->attributes.size(); ++attrIndex) {
        attr = d->attributes[attrIndex];
        if (attr->attribute == attribute)
            break;
    }
    if (attrIndex >= d->attributes.size())
        return;

    // Upload the new data or replace the client-side data.
    if (d->buffer.isCreated()) {
        d->buffer.bind();
        int stride = attr->value.stride();
        if (d->actualPackingHint == QGLVertexBuffer::Interleave) {
            void *mapped = d->buffer.map(QGLBuffer::WriteOnly);
            if (!mapped)
                return;
            char *dst = reinterpret_cast<char *>(mapped) +
                        attr->value.offset() + index * stride;
            vertexBufferInterleave
                (reinterpret_cast<float *>(dst), stride / sizeof(float),
                 value.floatData(), attr->elementSize() / sizeof(float),
                 count * value.tupleSize());
            d->buffer.unmap();
        } else {
            Q_ASSERT(value.stride() == 0 || value.stride() == stride);
            d->buffer.write
                (attr->value.offset() + index * stride,
                 value.data(), count * stride);
        }
        d->buffer.release();
    } else {
        attr->replace(index, count, value);
    }
}

/*!
    Returns the raw attribute value associated with \a attribute in
    this vertex buffer; null if \a attribute does not exist in the
    vertex buffer.

    If isUploaded() is true, then the returned value will contain a
    buffer offset to the attribute.  If isUploaded() is false,
    then the returned value will contain a client-side data pointer
    to the attribute.

    \sa addAttribute()
*/
QGLAttributeValue QGLVertexBuffer::attributeValue(QGL::VertexAttribute attribute) const
{
    Q_D(const QGLVertexBuffer);
    QGLVertexBufferAttribute *attr = 0;
    int attrIndex;
    for (attrIndex = 0; attrIndex < d->attributes.size(); ++attrIndex) {
        attr = d->attributes[attrIndex];
        if (attr->attribute == attribute)
            return attr->value;
    }
    return QGLAttributeValue();
}

/*!
    Returns the number of vertices that were defined by previous
    called to addAttribute().

    \sa addAttribute()
*/
int QGLVertexBuffer::vertexCount() const
{
    Q_D(const QGLVertexBuffer);
    return d->vertexCount;
}

/*!
    \fn bool QGLVertexBuffer::isEmpty() const

    Returns true if vertexCount() is zero; false otherwise.
*/

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
    if (d->buffer.isCreated())
        return true;
    if (d->attributes.isEmpty())
        return false;

    // Create the VBO in the GL server and bind it.
    if (!d->buffer.create())
        return false;
    d->buffer.bind();

    // If there is only one attribute, then realloc and write in one step.
    if (d->attributes.size() == 1) {
        attr = d->attributes[0];
        d->buffer.allocate(attr->value.data(),
                           attr->count() * attr->elementSize());
        attr->value.setOffset(0);
        attr->clear();
        d->actualPackingHint = QGLVertexBuffer::Append;
        d->buffer.release();
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
    d->buffer.allocate(size);
    stride /= sizeof(float);

    // Determine how to upload the data, using a map if possible.
    // Interleaving will be ignored if the usage pattern is DynamicDraw
    // and the buffer cannot be mapped into main memory.
    void *mapped = d->buffer.map(QGLBuffer::WriteOnly);
    int offset = 0;
    if (d->packingHint == QGLVertexBuffer::Interleave &&
            (mapped || d->buffer.usagePattern() != QGLBuffer::DynamicDraw)) {
        // Interleave the data into the final buffer.  We do it in
        // sections so as to keep locality problems to a minimum.
        QArray<float> temp;
        float *dst;
        if (mapped)
            dst = reinterpret_cast<float *>(mapped);
        else
            dst = temp.extend(1024);
        int sectionSize = 1024 / stride;
        for (int vertex = 0; vertex < maxCount; vertex += sectionSize) {
            int attrPosn = 0;
            for (int index = 0; index < d->attributes.size(); ++index) {
                attr = d->attributes[index];
                int count = attr->count() - vertex;
                if (count <= 0)
                    continue;
                count = qMin(count, sectionSize);
                int components = attr->elementSize() / sizeof(float);
                vertexBufferInterleave
                    (dst + attrPosn, stride,
                     attr->value.floatData() + vertex * components,
                     components, count);
                attrPosn += attr->elementSize() / sizeof(float);
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
            attr->value.setStride(stride * sizeof(float));
            offset += attr->elementSize();
            attr->clear();
        }
        d->actualPackingHint = QGLVertexBuffer::Interleave;
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
            attr->value.setStride(attr->elementSize());
            attr->clear();
            offset += size;
        }
        d->actualPackingHint = QGLVertexBuffer::Append;
    }
    if (mapped)
        d->buffer.unmap();

    // Buffer is uploaded and ready to go.
    d->buffer.release();
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
    return d->buffer.isCreated();
}

/*!
    Returns the QGLBuffer in use by this vertex buffer object,
    so that its properties or contents can be modified directly.

    \sa isUploaded()
*/
QGLBuffer QGLVertexBuffer::buffer() const
{
    Q_D(const QGLVertexBuffer);
    return d->buffer;
}

/*!
    Binds this vertex buffer to the current GL context.  Returns false if
    binding was not possible, usually because upload() has not been called.

    The buffer must be bound to the same QGLContext current when upload()
    was called, or to another QGLContext that is sharing with it.
    Otherwise, false will be returned from this function.

    \sa release(), upload()
*/
bool QGLVertexBuffer::bind() const
{
    Q_D(const QGLVertexBuffer);
    return d->buffer.bind();
}

/*!
    Releases this vertex buffer from the current GL context.

    This function must be called with the same QGLContext current
    as when bind() was called on the vertex buffer.

    \sa bind()
*/
void QGLVertexBuffer::release() const
{
    Q_D(const QGLVertexBuffer);
    d->buffer.release();
}

// Defined in qglpainter.cpp.  Needs to be moved eventually.
void qt_gl_setVertexAttribute(QGL::VertexAttribute attribute, const QGLAttributeValue& value);

/*!
    Sets the attribute arrays in this vertex buffer on \a program.
    If \a program is null, then the attribute arrays will be
    set on the fixed-function pipeline.

    It is assumed that this vertex buffer and \a program are bound
    to the current GL context.

    \sa bind()
*/
void QGLVertexBuffer::setAttributeArrays(QGLShaderProgram *program)
{
    Q_D(QGLVertexBuffer);
    Q_UNUSED(program);
#if !defined(QT_OPENGL_ES_1)
    if (program) {
        for (int index = 0; index < d->attributes.size(); ++index) {
            QGLVertexBufferAttribute *attr = d->attributes[index];
#if defined(QT_OPENGL_ES_2)
            glVertexAttribPointer(GLuint(attr->attribute),
                                  attr->value.tupleSize(),
                                  attr->value.type(), GL_FALSE,
                                  attr->value.stride(), attr->value.data());
#elif QT_VERSION >= 0x040700
            program->setAttributeArray
                (int(attr->attribute), attr->value.type(), attr->value.data(),
                 attr->value.tupleSize(), attr->value.stride());
#else
            QGLAbstractEffect::setAttributeArray
                (program, int(attr->attribute), attr->value);
#endif
        }
        return;
    }
#endif
#if !defined(QT_OPENGL_ES_2)
    for (int index = 0; index < d->attributes.size(); ++index) {
        QGLVertexBufferAttribute *attr = d->attributes[index];
        qt_gl_setVertexAttribute(attr->attribute, attr->value);
    }
#endif
}

QT_END_NAMESPACE
