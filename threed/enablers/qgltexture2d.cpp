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

#include "qgltexture2d.h"
#include "qgltexture2d_p.h"
#include "qglpainter_p.h"

#include <QtCore/qfile.h>
#include <QtCore/qfileinfo.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLTexture2D
    \brief The QGLTexture2D class represents a 2D texture object for GL painting operations.
    \ingroup qt3d
    \ingroup qt3d::enablers

    QGLTexture2D contains a QImage and settings for texture filters,
    wrap modes, and mipmap generation.  When bind() is called, this
    information is uploaded to the GL server if it has changed since
    the last time bind() was called.

    Once a QGLTexture2D object is created, it can be bound to multiple
    GL contexts.  Internally, a separate texture identifier is created
    for each context.  This makes QGLTexture2D easier to use than
    raw GL texture identifiers because the application does not need
    to be as concerned with whether the texture identifier is valid
    in the current context.  The application merely calls bind() and
    QGLTexture2D will create a new texture identifier for the context
    if necessary.

    QGLTexture2D internally points to a reference-counted object that
    represents the current texture state.  If the QGLTexture2D is copied,
    the internal pointer is the same.  Modifications to one QGLTexture2D
    copy will affect all of the other copies in the system.

    The texture identifiers will be destroyed when the last QGLTexture2D
    reference is destroyed, or when a context is destroyed that contained a
    texture identifier that was created by QGLTexture2D.

    QGLTexture2D can also be used for uploading 1D textures into the
    GL server by specifying an image() with a height of 1.

    \sa QGLTextureCube
*/

QGLTexture2DPrivate::QGLTexture2DPrivate()
{
    ddsPixels = 0;
    ddsHeader = 0;
    minifyFilter = QGL::LinearMipmapLinear;
    magnifyFilter = QGL::Linear;
    horizontalWrap = QGL::Repeat;
    verticalWrap = QGL::Repeat;
    generateMipmap = true;
#if !defined(QT_OPENGL_ES)
    mipmapSupported = false;
    mipmapSupportedKnown = false;
#endif
    imageGeneration = 0;
    parameterGeneration = 0;
    infos = 0;
    flipped = false;
}

QGLTexture2DPrivate::~QGLTexture2DPrivate()
{
    free(ddsPixels);
    free(ddsHeader);
    // Destroy the texture id's in the GL server in their original contexts.
    QGLTexture2DTextureInfo *current = infos;
    QGLTexture2DTextureInfo *next;
    const QGLContext *ctx = QGLContext::currentContext();
    const QGLContext *activectx = ctx;
    while (current != 0) {
        next = current->next;
        if (!current->isLiteral) {
            if (current->context != activectx) {
                activectx = current->context;
                const_cast<QGLContext *>(activectx)->makeCurrent();
            }
            glDeleteTextures(1, &(current->textureId));
        }
        delete current;
        current = next;
    }
    if (activectx != ctx) {
        if (ctx)
            const_cast<QGLContext *>(ctx)->makeCurrent();
        else
            const_cast<QGLContext *>(activectx)->doneCurrent();
    }
}

void QGLTexture2DPrivate::destroyedContext(const QGLContext *context)
{
    // Scan the texture information to remove the block for "context".
    // We assume that the texture identifier was destroyed by GL itself.
    QGLTexture2DTextureInfo *current = infos;
    QGLTexture2DTextureInfo *prev = 0;
    QGLTexture2DTextureInfo *next;
    while (current != 0) {
        if (current->context == context) {
            next = current->next;
            delete current;
            current = next;
            if (prev)
                prev->next = next;
            else
                infos = next;
        } else {
            prev = current;
            current = current->next;
        }
    }
}

/*!
    Constructs a null texture object.

    \sa isNull()
*/
QGLTexture2D::QGLTexture2D()
    : d_ptr(new QGLTexture2DPrivate())
{
}

/*!
    Destroys this texture object.  If this object is the last
    reference to the underlying GL texture, then the underlying
    GL texture will also be deleted.
*/
QGLTexture2D::~QGLTexture2D()
{
}

/*!
    Returns true if this texture object is null; that is, image()
    is null and textureId() is zero.
*/
bool QGLTexture2D::isNull() const
{
    Q_D(const QGLTexture2D);
    return d->image.isNull() && !d->infos;
}

/*!
    Returns the size of this texture.  If the underlying OpenGL
    implementation requires texture sizes to be a power of two,
    then this function will return the next power of two equal
    to or greater than requestedSize()

    \sa setSize(), requestedSize()
*/
QSize QGLTexture2D::size() const
{
    Q_D(const QGLTexture2D);
    return d->size;
}

/*!
    Sets the size of this texture to \a value.  If the underlying
    OpenGL implementation requires texture sizes to be a power of
    two, then requestedSize() will be set to \a value, and the
    actual size will be set to the next power of two equal
    to or greater than \a value.  Otherwise both size() and
    requestedSize() will be set to \a value.

    \sa size(), requestedSize()
*/
void QGLTexture2D::setSize(const QSize& value)
{
    Q_D(QGLTexture2D);
    if (d->requestedSize == value)
        return;
    // TODO: find out why NPOT textures don't work under ES 2.0.
    if (!(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_2_0))
        //!(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_ES_Version_2_0))
        d->size = QSize(qt_gl_next_power_of_two(value.width()),
                        qt_gl_next_power_of_two(value.height()));
    else
        d->size = value;
    d->requestedSize = value;
    ++(d->imageGeneration);
}

/*!
    Returns the size that was previously set with setSize() before
    it was rounded to a power of two.

    \sa size(), setSize()
*/
QSize QGLTexture2D::requestedSize() const
{
    Q_D(const QGLTexture2D);
    return d->requestedSize;
}

/*!
    Returns the image that is currently associated with this texture.
    The image may not have been uploaded into the GL server yet.
    Uploads occur upon the next call to bind().

    \sa setImage()
*/
QImage QGLTexture2D::image() const
{
    Q_D(const QGLTexture2D);
    return d->image;
}

/*!
    Sets the \a image that is associated with this texture.  The image
    will be uploaded into the GL server the next time bind() is called.

    If setSize() or setImage() has been called previously, then \a image
    will be scaled to size() when it is uploaded.

    If \a image is null, then this function is equivalent to clearImage().

    \sa image(), setSize(), copyImage()
*/
void QGLTexture2D::setImage(const QImage& image)
{
    Q_D(QGLTexture2D);
    if (d->ddsHeader)  // clear any dds image header
    {
        free(d->ddsHeader);
        d->ddsHeader = 0;
    }
    if (d->ddsPixels)  // clear any dds image
    {
        free(d->ddsPixels);
        d->ddsPixels = 0;
    }
    if (image.isNull()) {
        // Don't change the imageGeneration, because we aren't actually
        // changing the image in the GL server, only the client copy.
        d->image = image;
    } else {
        if (!d->size.isValid())
            setSize(image.size());
        d->image = image;
        ++(d->imageGeneration);
    }
}

/*!
    Clears the image() that is associated with this texture, but the
    GL texture will retain its current value.  This can be used to
    release client-side memory that is no longer required once the
    image has been uploaded into the GL server.

    The following code will queue \c image to be uploaded, immediately
    force it to be uploaded into the current GL context, and then
    clear the client copy:

    \code
    texture.setImage(image);
    texture.bind();
    texture.clearImage()
    \endcode

    \sa image(), setImage()
*/
void QGLTexture2D::clearImage()
{
    Q_D(QGLTexture2D);
    d->image = QImage();
}

// DDS stuff - all taken from qgl.cpp
struct DDSFormat {
    quint32 dwSize;
    quint32 dwFlags;
    quint32 dwHeight;
    quint32 dwWidth;
    quint32 dwLinearSize;
    quint32 dummy1;
    quint32 dwMipMapCount;
    quint32 dummy2[11];
    struct {
        quint32 dummy3[2];
        quint32 dwFourCC;
        quint32 dummy4[5];
    } ddsPixelFormat;
};

// compressed texture pixel formats
#define FOURCC_DXT1  0x31545844
#define FOURCC_DXT2  0x32545844
#define FOURCC_DXT3  0x33545844
#define FOURCC_DXT4  0x34545844
#define FOURCC_DXT5  0x35545844

#ifndef GL_COMPRESSED_RGB_S3TC_DXT1_EXT
#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT   0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT  0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT  0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT  0x83F3
#endif

#ifndef GL_GENERATE_MIPMAP_SGIS
#define GL_GENERATE_MIPMAP_SGIS       0x8191
#define GL_GENERATE_MIPMAP_HINT_SGIS  0x8192
#endif

#ifndef APIENTRY
# define APIENTRY
#endif
typedef void (APIENTRY *pfn_glCompressedTexImage2DARB) (GLenum, GLint, GLenum, GLsizei,
                                                        GLsizei, GLint, GLsizei, const GLvoid *);
static pfn_glCompressedTexImage2DARB qt_glCompressedTexImage2DARB = 0;
static bool pfn_initialized = false;

/*!
    Sets a Direct Draw Surface (DDS) image at \a path on this texture.

    The \a path should be a file-system path relative to the current directory
    or an absolute \a path, identifying a DDS image file.

    The DDS image will be uploaded into the GL server the next time bind()
    is called.  Only the DXT1, DXT3 and DXT5 DDS formats are supported.

    Note that this will only work if the implementation supports the
    \c GL_ARB_texture_compression and \c GL_EXT_texture_compression_s3tc
    extensions.

    If the DDS image cannot be loaded then the current image is cleared in the
    client but the GL texture will retain its current value.

    Returns true if the DDS image could be loaded for the texture, and
    otherwise returns false.

    Since DDS is an optimized format for uploading directly to an OpenGL
    texture it is not retained internally as a QImage.

    Subsequent calls to image() will return a null QImage.

    This method sets the flip vertically flag to true, since DDS textures
    are inverted top to bottom with respect to OpenGL.

    \sa setImage(), setSize(), flipVertically()
*/
bool QGLTexture2D::setDdsImage(const QString &path)
{
    Q_D(QGLTexture2D);
    d->image = QImage();

    QFile f(path);
    if (!f.exists())
    {
        qWarning("QGLTexture2D::setDdsImage(%s): File not found",
                 path.toLocal8Bit().constData());
        return false;
    }
    if (!f.open(QIODevice::ReadOnly))
    {
        qWarning("QGLTexture2D::setDdsImage(%s): File could not be read",
                 path.toLocal8Bit().constData());
        return false;
    }

    char tag[4];
    f.read(&tag[0], 4);
    if (strncmp(tag,"DDS ", 4) != 0) {
        qWarning("QGLTexture2D::setDdsImage(%s): not a DDS image file.",
                 path.toLocal8Bit().constData());
        return false;
    }

    if (!d->ddsHeader)
        d->ddsHeader = (DDSFormat *)(malloc(sizeof(DDSFormat)));
    Q_CHECK_PTR(d->ddsHeader);
    if (f.read((char *)d->ddsHeader, sizeof(DDSFormat)) < (int)sizeof(DDSFormat))
    {
        qWarning("QGLTexture2D::setDdsImage(%s): reading failed.",
                 path.toLocal8Bit().constData());
        return false;
    }

    if (!d->ddsHeader->dwLinearSize || !d->ddsHeader->dwMipMapCount) {
        qWarning("QGLTexture2D::setDdsImage(%s): DDS image size or map count zero.",
                 path.toLocal8Bit().constData());
        return false;
    }

    int factor = 4;
    int bufferSize = 0;
    int blockSize = 16;
    switch(d->ddsHeader->ddsPixelFormat.dwFourCC) {
    case FOURCC_DXT1:
        factor = 2;
        blockSize = 8;
        break;
    case FOURCC_DXT3:
        break;
    case FOURCC_DXT5:
        break;
    default:
        qWarning("QGLTexture2D::setDdsImage(%s): DDS image format not supported.",
                 path.toLocal8Bit().constData());
        return 0;
    }

    if (d->ddsHeader->dwMipMapCount > 1)
        bufferSize = d->ddsHeader->dwLinearSize * factor;
    else
        bufferSize = d->ddsHeader->dwLinearSize;

    d->ddsPixels = (GLubyte *)(malloc(bufferSize * sizeof(GLubyte)));
    Q_CHECK_PTR(d->ddsPixels);
    f.seek(d->ddsHeader->dwSize + 4);
    f.read((char *)d->ddsPixels, bufferSize);
    f.close();

    int w = d->ddsHeader->dwWidth;
    int h = d->ddsHeader->dwHeight;

    if (!d->size.isValid())
        setSize(QSize(w, h));
    ++(d->imageGeneration);
    d->flipped = true;

    return true;
}

/*!
    Copies the contents of \a image to \a offset in this texture
    within the current GL context.

    Unlike setImage(), this function copies the image data to the
    GL server immediately using \c{glTexSubImage2D()}.  This is typically
    used to update the contents of a texture after it has been created.

    It is assumed that the application has already called bind() on
    this texture to bind it to the current GL context.

    If the texture has been created in multiple contexts, only the
    texture identifier for the current context will be updated.

    \sa setImage(), bind()
*/
void QGLTexture2D::copyImage(const QImage& image, const QPoint& offset)
{
    QImage img = QGLWidget::convertToGLFormat(image);
    glTexSubImage2D(GL_TEXTURE_2D, 0, offset.x(), offset.y(),
                    img.width(), img.height(), GL_RGBA,
                    GL_UNSIGNED_BYTE, img.bits());
#if defined(QT_OPENGL_ES_2)
    Q_D(QGLTexture2D);
    if (d->generateMipmap)
        glGenerateMipmap(GL_TEXTURE_2D);
#endif
}

/*!
    Returns the texture minifying filter for generating pixels when
    the texture is being mapped to an area of the screen that is
    smaller than the texture itself.  The default value is
    QGL::LinearMipmapLinear.

    \sa setMinifyFilter(), magnifyFilter()
*/
QGL::TextureFilter QGLTexture2D::minifyFilter() const
{
    Q_D(const QGLTexture2D);
    return d->minifyFilter;
}

/*!
    Sets the texture minifying filter to \a value, which indicates
    how to generate pixels when the texture is being mapped to an
    area of the screen that is smaller than the texture itself.

    The \a value will not be applied to the texture in the GL
    server until the next call to bind().

    If generateMipmap() is false and \a value refers to a mipmap
    filtering mode, then the equivalent non-mipmap mode of
    QGL::Linear or QGL::Nearest will be used instead.

    \sa minifyFilter(), setMagnifyFilter()
*/
void QGLTexture2D::setMinifyFilter(QGL::TextureFilter value)
{
    Q_D(QGLTexture2D);
    if (d->minifyFilter != value) {
        d->minifyFilter = value;
        ++(d->parameterGeneration);
    }
}

/*!
    Returns the texture magnifying filter for generating pixels when
    the texture is being mapped to an area of the screen that is
    larger than the texture itself.  The default value is
    QGL::Linear.

    \sa setMagnifyFilter(), minifyFilter()
*/
QGL::TextureFilter QGLTexture2D::magnifyFilter() const
{
    Q_D(const QGLTexture2D);
    return d->magnifyFilter;
}

/*!
    Sets the texture magnifying filter to \a value, which indicates
    how to generate pixels when the texture is being mapped to an
    area of the screen that is larger than the texture itself.
    The only valid values are QGL::Nearest and QGL::Linear.

    The \a value will not be applied to the texture in the GL
    server until the next call to bind().

    \sa magnifyFilter(), setMinifyFilter()
*/
void QGLTexture2D::setMagnifyFilter(QGL::TextureFilter value)
{
    Q_D(QGLTexture2D);
    if (d->magnifyFilter != value) {
        d->magnifyFilter = value;
        ++(d->parameterGeneration);
    }
}

/*!
    Returns the wrapping mode for horizontal texture co-ordinates.
    The default value is QGL::Repeat.

    \sa setHorizontalWrap(), verticalWrap()
*/
QGL::TextureWrap QGLTexture2D::horizontalWrap() const
{
    Q_D(const QGLTexture2D);
    return d->horizontalWrap;
}

QGL::TextureWrap qt_gl_modify_texture_wrap(QGL::TextureWrap value)
{
    switch (value) {
#if defined(QT_OPENGL_ES)
    case QGL::Clamp:
        value = QGL::ClampToEdge;
        break;
#endif
#if !defined(QT_OPENGL_ES)
    case QGL::ClampToBorder:
        if ((QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_1_3)
                == 0)
            value = QGL::Clamp;
        break;
#else
    case QGL::ClampToBorder:
        value = QGL::ClampToEdge;
        break;
#endif
#if !defined(QT_OPENGL_ES)
    case QGL::ClampToEdge:
        if ((QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_1_2)
                == 0)
            value = QGL::Clamp;
        break;
#endif
#if !defined(QT_OPENGL_ES)
    case QGL::MirroredRepeat:
        if ((QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_1_4)
                == 0)
            value = QGL::Repeat;
        break;
#elif !defined(QT_OPENGL_ES_2)
    case QGL::MirroredRepeat:
        value = QGL::Repeat;
        break;
#endif
    default: break;
    }
    return value;
}

/*!
    Sets the wrapping mode for horizontal texture co-ordinates to \a value.

    If \a value is not supported by the OpenGL implementation, it will be
    replaced with a value that is supported.  If the application desires a
    very specific \a value, it can call horizontalWrap() to check that
    the specific value was actually set.

    The \a value will not be applied to the texture in the GL
    server until the next call to bind().

    \sa horizontalWrap(), setVerticalWrap()
*/
void QGLTexture2D::setHorizontalWrap(QGL::TextureWrap value)
{
    Q_D(QGLTexture2D);
    value = qt_gl_modify_texture_wrap(value);
    if (d->horizontalWrap != value) {
        d->horizontalWrap = value;
        ++(d->parameterGeneration);
    }
}

/*!
    Returns the wrapping mode for vertical texture co-ordinates.
    The default value is QGL::Repeat.

    \sa setVerticalWrap(), horizontalWrap()
*/
QGL::TextureWrap QGLTexture2D::verticalWrap() const
{
    Q_D(const QGLTexture2D);
    return d->verticalWrap;
}

/*!
    Sets the wrapping mode for vertical texture co-ordinates to \a value.

    If \a value is not supported by the OpenGL implementation, it will be
    replaced with a value that is supported.  If the application desires a
    very specific \a value, it can call verticalWrap() to check that
    the specific value was actually set.

    The \a value will not be applied to the texture in the GL
    server until the next call to bind().

    \sa verticalWrap(), setHorizontalWrap()
*/
void QGLTexture2D::setVerticalWrap(QGL::TextureWrap value)
{
    Q_D(QGLTexture2D);
    value = qt_gl_modify_texture_wrap(value);
    if (d->verticalWrap != value) {
        d->verticalWrap = value;
        ++(d->parameterGeneration);
    }
}

/*!
    Returns true if mipmaps should be generated for this texture
    whenever image() changes; false otherwise.  The default value is true.

    \sa setGenerateMipmap()
*/
bool QGLTexture2D::generateMipmap() const
{
    Q_D(const QGLTexture2D);
    return d->generateMipmap;
}

/*!
    Enables or disables the generation of mipmaps for this
    texture whenever image() changes according to \a value.

    The \a value will not be applied to the texture in the GL
    server until the next call to bind().

    \sa generateMipmap()
*/
void QGLTexture2D::setGenerateMipmap(bool value)
{
    Q_D(QGLTexture2D);
    if (d->generateMipmap != value) {
        d->generateMipmap = value;
        ++(d->parameterGeneration);
    }
}

#if !defined(QT_OPENGL_ES)
#define q_glTexParameteri(target,name,value) \
        glTexParameteri((target), (name), int(value))
#else
#define q_glTexParameteri(target,name,value) \
        glTexParameterf((target), (name), GLfloat(int(value)))
#endif

#ifndef GL_GENERATE_MIPMAP
#define GL_GENERATE_MIPMAP 0x8191
#define GL_GENERATE_MIPMAP_HINT 0x8192
#endif

/*!
    Binds this texture to the 2D texture target.

    If this texture object is not associated with an identifier in
    the current context, then a new identifier will be created,
    and image() uploaded into the GL server.

    If setImage() or setSize() was called since the last upload,
    then image() will be re-uploaded to the GL server.

    Returns false if the texture could not be bound for some reason.

    \sa release(), textureId(), setImage()
*/
bool QGLTexture2D::bind() const
{
    Q_D(const QGLTexture2D);
    return const_cast<QGLTexture2DPrivate *>(d)->bind(GL_TEXTURE_2D);
}

bool QGLTexture2DPrivate::bind(GLenum target)
{
    // Get the current context.  If we don't have one, then we
    // cannot bind the texture.
    const QGLContext *ctx = QGLContext::currentContext();
    if (!ctx)
        return false;

    // Find the information block for the context, or create one.
    QGLTexture2DTextureInfo *info = infos;
    QGLTexture2DTextureInfo *prev = 0;
    while (info != 0 && info->context != ctx) {
        if (info->isLiteral)
            return false; // Cannot create extra texture id's for literals.
        prev = info;
        info = info->next;
    }
    bool firstTime = false;
    if (!info) {
        GLuint id = 0;
        glGenTextures(1, &id);
        info = new QGLTexture2DTextureInfo
            (ctx, id, imageGeneration - 1, parameterGeneration - 1);
        if (prev)
            prev->next = info;
        else
            infos = info;
        QObject::connect(QGLPainterPrivateCache::instance(),
                         SIGNAL(destroyedContext(const QGLContext *)),
                         this, SLOT(destroyedContext(const QGLContext *)));
        firstTime = true;
    }


    // Bind the texture to the 2D texture target.
    glBindTexture(target, info->textureId);

    // If the parameter generation has changed, then alter the parameters.
    if (parameterGeneration != info->parameterGeneration) {
        info->parameterGeneration = parameterGeneration;
        bool mipmapEnabled = generateMipmap;
#if !defined(QT_OPENGL_ES)
        if (!mipmapSupportedKnown) {
            if ((QGLFormat::openGLVersionFlags() &
                    QGLFormat::OpenGL_Version_1_4) != 0) {
                // Mipmap generation is standard in OpenGL 1.4 and higher.
                mipmapSupported = true;
            } else {
                // In OpenGL 1.3 and older, see if the extension is present.
                QString extensions =
                    QLatin1String(reinterpret_cast<const char *>
                                    (glGetString(GL_EXTENSIONS)));
                if (extensions.contains(QLatin1String("generate_mipmap")))
                    mipmapSupported = true;
            }
            mipmapSupportedKnown = true;
        }
        if (mipmapSupported) {
            if (generateMipmap)
                glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
            q_glTexParameteri(target, GL_GENERATE_MIPMAP,
                              generateMipmap ? GL_TRUE : GL_FALSE);
        } else {
            mipmapEnabled = false;
        }
#elif !defined(QT_OPENGL_ES_2)
        q_glTexParameteri(target, GL_GENERATE_MIPMAP,
                          generateMipmap ? GL_TRUE : GL_FALSE);
        if (generateMipmap)
            glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
#endif
        QGL::TextureFilter minify = minifyFilter;
        if (!mipmapEnabled) {
            // Don't use the mipmap modes if mipmap creation is disabled.
            if (minify == QGL::NearestMipmapNearest ||
                    minify == QGL::NearestMipmapLinear)
                minify = QGL::Nearest;
            else if (minify == QGL::LinearMipmapNearest ||
                        minify == QGL::LinearMipmapLinear)
                minify = QGL::Linear;
        }
        q_glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minify);
        q_glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magnifyFilter);
        q_glTexParameteri(target, GL_TEXTURE_WRAP_S, horizontalWrap);
        q_glTexParameteri(target, GL_TEXTURE_WRAP_T, verticalWrap);
    }

    // If the image generation has changed, then re-upload the image.
    if (imageGeneration != info->imageGeneration) {
        info->imageGeneration = imageGeneration;
        bindImage(target, firstTime, image);
    }

    // Texture is ready to be used.
    return true;
}

void QGLTexture2DPrivate::bindImage
        (GLenum target, bool firstTime, const QImage& image)
{
    if (ddsPixels)
    {
        const QGLContext *ctx = QGLContext::currentContext();
        if (!pfn_initialized)
            qt_glCompressedTexImage2DARB = (pfn_glCompressedTexImage2DARB)ctx->getProcAddress(
                QLatin1String("glCompressedTexImage2DARB"));
        if (!qt_glCompressedTexImage2DARB)
        {
            qWarning("QGLTexture2D::setDdsImage: The GL implementation does not"
                     "support texture compression extensions");
            return;
        }

        // taken from qlg.cpp: QGLContext::bindTexture(QString)
        int blockSize = 16;
        GLenum format;

        switch(ddsHeader->ddsPixelFormat.dwFourCC)
        {
        case FOURCC_DXT1:
            format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            blockSize = 8;
            break;
        case FOURCC_DXT3:
            format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            break;
        case FOURCC_DXT5:
            format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            break;
        default:
            Q_ASSERT(false);  // this is checked when the file is loaded
        }

        int sz = 0;
        int offset = 0;
        int w = size.width();
        int h = size.height();

        // load mip-maps

        for (quint32 i = 0; i < ddsHeader->dwMipMapCount; ++i)
        {
            if (w == 0)
                w = 1;
            if (h == 0)
                h = 1;

            sz = ((w + 3) / 4) * ((h + 3) / 4) * blockSize;
            qt_glCompressedTexImage2DARB(GL_TEXTURE_2D, i, format, w, h, 0,
                                         sz, ddsPixels + offset);
            offset += sz;

            // half size for each mip-map level
            w = w / 2;
            h = h / 2;
        }
        // note that if we know for sure that the texture is not going to be
        // bind()'ed again then ddsPixels should be free'd here.
        return;
    }
    if (!image.isNull()) {
        QImage img;
        if (image.size() == size)
            img = QGLWidget::convertToGLFormat(image);
        else
            img = QGLWidget::convertToGLFormat(image.scaled(size));
        glTexImage2D(target, 0, GL_RGBA, img.width(),
                     img.height(), 0,  GL_RGBA,
                     GL_UNSIGNED_BYTE, img.bits());
#if defined(QT_OPENGL_ES_2)
        // Cube map mipmaps need to be generated after all faces are uploaded.
        if ((target < GL_TEXTURE_CUBE_MAP_POSITIVE_X ||
             target > GL_TEXTURE_CUBE_MAP_NEGATIVE_Z) &&
                generateMipmap) {
            glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
            glGenerateMipmap(target);
        }
#endif
    } else if (firstTime && size.isValid()) {
        // This is the first time we have created the texture.
        // We have a size but no image, so just create the memory.
        glTexImage2D(target, 0, GL_RGBA, size.width(),
                     size.height(), 0,  GL_RGBA, GL_UNSIGNED_BYTE, 0);
    }
}

/*!
    Releases the texture associated with the 2D texture target.
    This is equivalent to \c{glBindTexture(GL_TEXTURE_2D, 0)}.

    \sa bind()
*/
void QGLTexture2D::release() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

/*!
    Returns the identifier associated with this texture object in
    the current context.

    Returns zero if the texture has not previously been bound to
    the 2D texture target in the current context with bind().

    \sa bind()
*/
GLuint QGLTexture2D::textureId() const
{
    Q_D(const QGLTexture2D);
    const QGLContext *ctx = QGLContext::currentContext();
    if (!ctx)
        return 0;
    QGLTexture2DTextureInfo *info = d->infos;
    while (info != 0 && info->context != ctx)
        info = info->next;
    return info ? info->textureId : 0;
}

/*!
    Returns true if this texture object is flipped vertically.  This
    is the case for example when DDS textures are imported, since MS
    Direct Draw uses the top-left of the screen as the origin, and
    OpenGL uses the bottom-left.

    This flag simply provides information - code must handle the flipped
    case, by for example using \c{1.0f - texCoord}.

    \sa setFlipVertically(), setDdsImage()
*/
bool QGLTexture2D::flipVertically() const
{
    Q_D(const QGLTexture2D);
    return d->flipped;
}

/*!
    Sets the flip vertically flag for this texture to \a flip.  This
    indicates to client code using this texture that texture coordinates
    will need to be flipped vertically before activating this texture.

    \sa flipVertically(), setDdsImage()
*/
void QGLTexture2D::setFlipVertically(bool flip)
{
    Q_D(QGLTexture2D);
    d->flipped = flip;
}

/*!
    Constructs a QGLTexture2D object that wraps the supplied literal
    texture identifier \a id, with the dimensions specified by \a size.

    The \a id is assumed to have been created by the application in
    the current GL context, and it will be destroyed by the application
    after the returned QGLTexture2D object is destroyed.

    This function is intended for interfacing to existing code that
    uses raw GL texture identifiers.  The returned QGLTexture2D can
    only be used with the current GL context.

    \sa textureId()
*/
QGLTexture2D *QGLTexture2D::fromTextureId(GLuint id, const QSize& size)
{
    const QGLContext *ctx = QGLContext::currentContext();
    if (!id || !ctx)
        return 0;

    QGLTexture2D *texture = new QGLTexture2D();
    if (!size.isNull())
        texture->setSize(size);
    QGLTexture2DTextureInfo *info = new QGLTexture2DTextureInfo
        (ctx, id, texture->d_ptr->imageGeneration,
         texture->d_ptr->parameterGeneration, true);
    texture->d_ptr->infos = info;
    QObject::connect(QGLPainterPrivateCache::instance(),
                     SIGNAL(destroyedContext(const QGLContext *)),
                     texture->d_ptr.data(),
                     SLOT(destroyedContext(const QGLContext *)));
    return texture;
}

QT_END_NAMESPACE
