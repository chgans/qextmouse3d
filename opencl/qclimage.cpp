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

#include "qclimage.h"
#include "qclcontext.h"

QT_BEGIN_NAMESPACE

/*!
    \class QCLImage2D
    \brief The QCLImage2D class represents a 2D OpenCL image object.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::opencl
*/

/*!
    \fn QCLImage2D::QCLImage2D()

    Constructs a null 2D OpenCL image object.
*/

/*!
    \fn QCLImage2D::QCLImage2D(QCLContext *context, cl_mem id)

    Constructs a 2D OpenCL image object that is initialized with the
    native OpenCL identifier \a id, and associates it with \a context.
    This class will take over ownership of \a id and will release
    it in the destructor.
*/

/*!
    \fn QCLImage2D::QCLImage2D(const QCLImage2D& other)

    Constructs a copy of \a other.
*/

/*!
    \fn QCLImage2D& QCLImage2D::operator=(const QCLImage2D& other)

    Assigns \a other to this object.
*/

/*!
    Returns the format descriptor for this OpenCL image.
*/
QCLImageFormat QCLImage2D::format() const
{
    cl_image_format iformat;
    if (clGetImageInfo(id(), CL_IMAGE_FORMAT, sizeof(iformat), &iformat, 0)
            != CL_SUCCESS)
        return QCLImageFormat();
    else
        return QCLImageFormat
            (QCLImageFormat::ChannelOrder(iformat.image_channel_order),
             QCLImageFormat::ChannelType(iformat.image_channel_data_type));
}

static int qt_cl_imageParam(cl_mem id, cl_image_info name)
{
    size_t value = 0;
    if (clGetImageInfo(id, name, sizeof(value), &value, 0) != CL_SUCCESS)
        return 0;
    else
        return int(value);
}

/*!
    Returns the width of this OpenCL image.

    \sa height(), bytesPerLine()
*/
int QCLImage2D::width() const
{
    return qt_cl_imageParam(id(), CL_IMAGE_WIDTH);
}

/*!
    Returns the height of this OpenCL image.

    \sa width()
*/
int QCLImage2D::height() const
{
    return qt_cl_imageParam(id(), CL_IMAGE_HEIGHT);
}

/*!
    Returns the number of bytes per element in this OpenCL image.

    \sa bytesPerLine()
*/
int QCLImage2D::bytesPerElement() const
{
    return qt_cl_imageParam(id(), CL_IMAGE_ELEMENT_SIZE);
}

/*!
    Returns the number of bytes per line in this OpenCL image.

    \sa bytesPerElement()
*/
int QCLImage2D::bytesPerLine() const
{
    return qt_cl_imageParam(id(), CL_IMAGE_ROW_PITCH);
}

/*!
    Reads the contents of \a rect from within this image into \a data.
    Returns true if the read was successful; false otherwise.
    If \a bytesPerLine is not zero, it indicates the number of bytes
    between lines in \a data.

    This function will block until the request completes.
    The request is executed on the active command queue for context().

    \sa readAsync(), write()
*/
bool QCLImage2D::read(void *data, const QRect& rect, int bytesPerLine)
{
    size_t origin[3] = {rect.x(), rect.y(), 0};
    size_t region[3] = {rect.width(), rect.height(), 1};
    cl_int error = clEnqueueReadImage
        (context()->activeQueue(), id(), CL_TRUE,
         origin, region, bytesPerLine, 0, data, 0, 0, 0);
    context()->reportError("QCLImage2D::read:", error);
    return error == CL_SUCCESS;
}

/*!
    \overload

    Reads the contents of \a rect from within this image into \a data.
    Returns true if the read was successful; false otherwise.
    If \a bytesPerLine is not zero, it indicates the number of bytes
    between lines in \a data.

    The request will not start until all of the events in \a after
    have been signalled as completed, and then this function will
    block until the request completes.  The request is executed on
    the active command queue for context().

    \sa readAsync(), write()
*/
bool QCLImage2D::read
    (void *data, const QRect& rect,
     const QVector<QCLEvent>& after, int bytesPerLine)
{
    size_t origin[3] = {rect.x(), rect.y(), 0};
    size_t region[3] = {rect.width(), rect.height(), 1};
    cl_int error = clEnqueueReadImage
        (context()->activeQueue(), id(), CL_TRUE,
         origin, region, bytesPerLine, 0, data, after.size(),
         reinterpret_cast<const cl_event *>(after.constData()), 0);
    context()->reportError("QCLImage2D::read(after):", error);
    return error == CL_SUCCESS;
}

/*!
    Reads the contents of \a rect from within this image into \a data.
    Returns true if the read was successful; false otherwise.
    If \a bytesPerLine is not zero, it indicates the number of bytes
    between lines in \a data.

    This function will queue the request and return immediately.
    Returns an event object that can be used to wait for the
    request to complete.  The request is executed on the active
    command queue for context().

    \sa read(), writeAsync()
*/
QCLEvent QCLImage2D::readAsync
    (void *data, const QRect& rect, int bytesPerLine)
{
    size_t origin[3] = {rect.x(), rect.y(), 0};
    size_t region[3] = {rect.width(), rect.height(), 1};
    cl_event event;
    cl_int error = clEnqueueReadImage
        (context()->activeQueue(), id(), CL_FALSE,
         origin, region, bytesPerLine, 0, data, 0, 0, &event);
    context()->reportError("QCLImage2D::readAsync:", error);
    if (error == CL_SUCCESS)
        return QCLEvent(event);
    else
        return QCLEvent();
}

/*!
    \overload

    Reads the contents of \a rect from within this image into \a data.
    Returns true if the read was successful; false otherwise.
    If \a bytesPerLine is not zero, it indicates the number of bytes
    between lines in \a data.

    This function will queue the request and return immediately.
    Returns an event object that can be used to wait for the
    request to complete.

    The request will not start until all of the events in \a after
    have been signalled as completed.  The request is executed on
    the active command queue for context().

    \sa read(), writeAsync()
*/
QCLEvent QCLImage2D::readAsync
    (void *data, const QRect& rect,
     const QVector<QCLEvent>& after, int bytesPerLine)
{
    size_t origin[3] = {rect.x(), rect.y(), 0};
    size_t region[3] = {rect.width(), rect.height(), 1};
    cl_event event;
    cl_int error = clEnqueueReadImage
        (context()->activeQueue(), id(), CL_FALSE,
         origin, region, bytesPerLine, 0, data, after.size(),
         reinterpret_cast<const cl_event *>(after.constData()), &event);
    context()->reportError("QCLImage2D::readAsync(after):", error);
    if (error == CL_SUCCESS)
        return QCLEvent(event);
    else
        return QCLEvent();
}

/*!
    Writes the contents \a data to \a rect within this image.
    Returns true if the write was successful; false otherwise.
    If \a bytesPerLine is not zero, it indicates the number of bytes
    between lines in \a data.

    This function will block until the request completes.
    The request is executed on the active command queue for context().

    \sa writeAsync(), read()
*/
bool QCLImage2D::write(const void *data, const QRect& rect, int bytesPerLine)
{
    size_t origin[3] = {rect.x(), rect.y(), 0};
    size_t region[3] = {rect.width(), rect.height(), 1};
    cl_int error = clEnqueueWriteImage
        (context()->activeQueue(), id(), CL_TRUE,
         origin, region, bytesPerLine, 0, data, 0, 0, 0);
    context()->reportError("QCLImage2D::write:", error);
    return error == CL_SUCCESS;
}

/*!
    \overload

    Writes the contents of \a data into \a rect within this image.
    Returns true if the write was successful; false otherwise.
    If \a bytesPerLine is not zero, it indicates the number of bytes
    between lines in \a data.

    The request will not start until all of the events in \a after
    have been signalled as completed, and then this function will
    block until the request completes.  The request is executed on
    the active command queue for context().

    \sa writeAsync(), read()
*/
bool QCLImage2D::write
    (const void *data, const QRect& rect,
     const QVector<QCLEvent>& after, int bytesPerLine)
{
    size_t origin[3] = {rect.x(), rect.y(), 0};
    size_t region[3] = {rect.width(), rect.height(), 1};
    cl_int error = clEnqueueWriteImage
        (context()->activeQueue(), id(), CL_TRUE,
         origin, region, bytesPerLine, 0, data, after.size(),
         reinterpret_cast<const cl_event *>(after.constData()), 0);
    context()->reportError("QCLImage2D::write(after):", error);
    return error == CL_SUCCESS;
}

/*!
    Writes the contents of \a data into \a rect within this image.
    Returns true if the write was successful; false otherwise.
    If \a bytesPerLine is not zero, it indicates the number of bytes
    between lines in \a data.

    This function will queue the request and return immediately.
    Returns an event object that can be used to wait for the
    request to complete.  The request is executed on the active
    command queue for context().

    \sa write(), readAsync()
*/
QCLEvent QCLImage2D::writeAsync
    (const void *data, const QRect& rect, int bytesPerLine)
{
    size_t origin[3] = {rect.x(), rect.y(), 0};
    size_t region[3] = {rect.width(), rect.height(), 1};
    cl_event event;
    cl_int error = clEnqueueWriteImage
        (context()->activeQueue(), id(), CL_FALSE,
         origin, region, bytesPerLine, 0, data, 0, 0, &event);
    context()->reportError("QCLImage2D::writeAsync:", error);
    if (error == CL_SUCCESS)
        return QCLEvent(event);
    else
        return QCLEvent();
}

/*!
    \overload

    Writes the contents of \a data into \a rect within this image.
    Returns true if the write was successful; false otherwise.
    If \a bytesPerLine is not zero, it indicates the number of bytes
    between lines in \a data.

    This function will queue the request and return immediately.
    Returns an event object that can be used to wait for the
    request to complete.

    The request will not start until all of the events in \a after
    have been signalled as completed.  The request is executed on
    the active command queue for context().

    \sa write(), readAsync()
*/
QCLEvent QCLImage2D::writeAsync
    (const void *data, const QRect& rect,
     const QVector<QCLEvent>& after, int bytesPerLine)
{
    size_t origin[3] = {rect.x(), rect.y(), 0};
    size_t region[3] = {rect.width(), rect.height(), 1};
    cl_event event;
    cl_int error = clEnqueueWriteImage
        (context()->activeQueue(), id(), CL_FALSE,
         origin, region, bytesPerLine, 0, data, after.size(),
         reinterpret_cast<const cl_event *>(after.constData()), &event);
    context()->reportError("QCLImage2D::writeAsync(after):", error);
    if (error == CL_SUCCESS)
        return QCLEvent(event);
    else
        return QCLEvent();
}

/*!
    \class QCLImage3D
    \brief The QCLImage3D class represents a 3D OpenCL image object.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::opencl
*/

/*!
    \fn QCLImage3D::QCLImage3D()

    Constructs a null 3D OpenCL image object.
*/

/*!
    \fn QCLImage3D::QCLImage3D(QCLContext *context, cl_mem id)

    Constructs a 3D OpenCL image object that is initialized with the
    native OpenCL identifier \a id, and associates it with \a context.
    This class will take over ownership of \a id and will release
    it in the destructor.
*/

/*!
    \fn QCLImage3D::QCLImage3D(const QCLImage3D& other)

    Constructs a copy of \a other.
*/

/*!
    \fn QCLImage3D& QCLImage3D::operator=(const QCLImage3D& other)

    Assigns \a other to this object.
*/

/*!
    Returns the format descriptor for this OpenCL image.
*/
QCLImageFormat QCLImage3D::format() const
{
    cl_image_format iformat;
    if (clGetImageInfo(id(), CL_IMAGE_FORMAT, sizeof(iformat), &iformat, 0)
            != CL_SUCCESS)
        return QCLImageFormat();
    else
        return QCLImageFormat
            (QCLImageFormat::ChannelOrder(iformat.image_channel_order),
             QCLImageFormat::ChannelType(iformat.image_channel_data_type));
}

/*!
    Returns the width of this OpenCL image.

    \sa height(), depth(), bytesPerLine()
*/
int QCLImage3D::width() const
{
    return qt_cl_imageParam(id(), CL_IMAGE_WIDTH);
}

/*!
    Returns the height of this OpenCL image.

    \sa width(), depth()
*/
int QCLImage3D::height() const
{
    return qt_cl_imageParam(id(), CL_IMAGE_HEIGHT);
}

/*!
    Returns the depth of this 3D OpenCL image; 0 if the image is 2D.

    \sa width(), height(), bytesPerSlice()
*/
int QCLImage3D::depth() const
{
    return qt_cl_imageParam(id(), CL_IMAGE_DEPTH);
}

/*!
    Returns the number of bytes per element in this OpenCL image.

    \sa bytesPerLine(), bytesPerSlice()
*/
int QCLImage3D::bytesPerElement() const
{
    return qt_cl_imageParam(id(), CL_IMAGE_ELEMENT_SIZE);
}

/*!
    Returns the number of bytes per line in this OpenCL image.

    \sa bytesPerElement(), bytesPerSlice()
*/
int QCLImage3D::bytesPerLine() const
{
    return qt_cl_imageParam(id(), CL_IMAGE_ROW_PITCH);
}

/*!
    Returns the number of bytes per 2D slice in this 3D OpenCL image;
    0 if the image is 2D.

    \sa bytesPerElement(), bytesPerLine()
*/
int QCLImage3D::bytesPerSlice() const
{
    return qt_cl_imageParam(id(), CL_IMAGE_SLICE_PITCH);
}

QT_END_NAMESPACE
