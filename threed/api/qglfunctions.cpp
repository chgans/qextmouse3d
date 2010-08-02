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

#include "qglfunctions.h"
#include <QtOpenGL/private/qgl_p.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLFunctions
    \brief The QGLFunctions class provides cross-platform access to the OpenGL/ES 2.0 API.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::enablers
*/

#if QT_VERSION >= 0x040800
Q_GLOBAL_STATIC(QGLContextGroupResource<QGLFunctionsPrivate>, qt_gl_functions_resource)
#else
static void qt_gl_functions_free(void *data)
{
    delete reinterpret_cast<QGLFunctionsPrivate *>(data);
}

Q_GLOBAL_STATIC_WITH_ARGS(QGLContextResource, qt_gl_functions_resource, (qt_gl_functions_free))
#endif
static QGLFunctionsPrivate *qt_gl_functions(const QGLContext *context = 0)
{
    if (!context)
        context = QGLContext::currentContext();
    Q_ASSERT(context);
    QGLFunctionsPrivate *funcs =
        reinterpret_cast<QGLFunctionsPrivate *>
            (qt_gl_functions_resource()->value(context));
#if QT_VERSION < 0x040800
    if (!funcs) {
        funcs = new QGLFunctionsPrivate();
        qt_gl_functions_resource()->insert(context, funcs);
    }
#endif
    return funcs;
}

/*!
    Constructs a function resolver for the current QGLContext.
*/
QGLFunctions::QGLFunctions()
    : d_ptr(qt_gl_functions())
{
}

/*!
    Constructs a function resolver for \a context.  If \a context,
    is null, then the resolver will be created for the current QGLContext.
*/
QGLFunctions::QGLFunctions(const QGLContext *context)
    : d_ptr(qt_gl_functions(context))
{
}

/*!
    \fn QGLFunctions::~QGLFunctions()

    Destroys this function resolver.
*/

/*!
    \fn void QGLFunctions::activeTexture(GLenum texture)

    Convenience function that calls glActiveTexture(\a texture).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glActiveTexture.xml}{glActiveTexture()}.
*/

/*!
    \fn void QGLFunctions::attachShader(GLuint program, GLuint shader)

    Convenience function that calls glAttachShader(\a program, \a shader).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glAttachShader.xml}{glAttachShader()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::bindAttribLocation(GLuint program, GLuint index, const char* name)

    Convenience function that calls glBindAttribLocation(\a program, \a index, \a name).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glBindAttribLocation.xml}{glBindAttribLocation()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::bindBuffer(GLenum target, GLuint buffer)

    Convenience function that calls glBindBuffer(\a target, \a buffer).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glBindBuffer.xml}{glBindBuffer()}.
*/

/*!
    \fn void QGLFunctions::bindFramebuffer(GLenum target, GLuint framebuffer)

    Convenience function that calls glBindFramebuffer(\a target, \a framebuffer).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glBindFramebuffer.xml}{glBindFramebuffer()}.
*/

/*!
    \fn void QGLFunctions::bindRenderbuffer(GLenum target, GLuint renderbuffer)

    Convenience function that calls glBindRenderbuffer(\a target, \a renderbuffer).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glBindRenderbuffer.xml}{glBindRenderbuffer()}.
*/

/*!
    \fn void QGLFunctions::bindTexture(GLenum target, GLuint texture)

    Convenience function that calls glBindTexture(\a target, \a texture).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glBindTexture.xml}{glBindTexture()}.
*/

/*!
    \fn void QGLFunctions::blendColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)

    Convenience function that calls glBlendColor(\a red, \a green, \a blue, \a alpha).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glBlendColor.xml}{glBlendColor()}.
*/

/*!
    \fn void QGLFunctions::blendEquation(GLenum mode)

    Convenience function that calls glBlendEquation(\a mode).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glBlendEquation.xml}{glBlendEquation()}.
*/

/*!
    \fn void QGLFunctions::blendEquationSeparate(GLenum modeRGB, GLenum modeAlpha)

    Convenience function that calls glBlendEquationSeparate(\a modeRGB, \a modeAlpha).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glBlendEquationSeparate.xml}{glBlendEquationSeparate()}.
*/

/*!
    \fn void QGLFunctions::blendFunc(GLenum sfactor, GLenum dfactor)

    Convenience function that calls glBlendFunc(\a sfactor, \a dfactor).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glBlendFunc.xml}{glBlendFunc()}.
*/

/*!
    \fn void QGLFunctions::blendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)

    Convenience function that calls glBlendFuncSeparate(\a srcRGB, \a dstRGB, \a srcAlpha, \a dstAlpha).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glBlendFuncSeparate.xml}{glBlendFuncSeparate()}.
*/

/*!
    \fn void QGLFunctions::bufferData(GLenum target, qgl_GLsizeiptr size, const void* data, GLenum usage)

    Convenience function that calls glBufferData(\a target, \a size, \a data, \a usage).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glBufferData.xml}{glBufferData()}.
*/

/*!
    \fn void QGLFunctions::bufferSubData(GLenum target, qgl_GLintptr offset, qgl_GLsizeiptr size, const void* data)

    Convenience function that calls glBufferSubData(\a target, \a offset, \a size, \a data).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glBufferSubData.xml}{glBufferSubData()}.
*/

/*!
    \fn GLenum QGLFunctions::checkFramebufferStatus(GLenum target)

    Convenience function that calls glCheckFramebufferStatus(\a target).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glCheckFramebufferStatus.xml}{glCheckFramebufferStatus()}.
*/

/*!
    \fn void QGLFunctions::clear(GLbitfield mask)

    Convenience function that calls glClear(\a mask).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glClear.xml}{glClear()}.
*/

/*!
    \fn void QGLFunctions::clearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)

    Convenience function that calls glClearColor(\a red, \a green, \a blue, \a alpha).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glClearColor.xml}{glClearColor()}.
*/

/*!
    \fn void QGLFunctions::clearDepth(GLclampf depth)

    Convenience function that calls glClearDepth(\a depth) on
    desktop OpenGL systems and glClearDepthf(\a depth) on
    embedded OpenGL/ES systems.

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glClearDepthf.xml}{glClearDepthf()}.
*/

/*!
    \fn void QGLFunctions::clearStencil(GLint s)

    Convenience function that calls glClearStencil(\a s).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glClearStencil.xml}{glClearStencil()}.
*/

/*!
    \fn void QGLFunctions::colorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)

    Convenience function that calls glColorMask(\a red, \a green, \a blue, \a alpha).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glColorMask.xml}{glColorMask()}.
*/

/*!
    \fn void QGLFunctions::compileShader(GLuint shader)

    Convenience function that calls glCompileShader(\a shader).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glCompileShader.xml}{glCompileShader()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::compressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data)

    Convenience function that calls glCompressedTexImage2D(\a target, \a level, \a internalformat, \a width, \a height, \a border, \a imageSize, \a data).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glCompressedTexImage2D.xml}{glCompressedTexImage2D()}.
*/

/*!
    \fn void QGLFunctions::compressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data)

    Convenience function that calls glCompressedTexSubImage2D(\a target, \a level, \a xoffset, \a yoffset, \a width, \a height, \a format, \a imageSize, \a data).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glCompressedTexSubImage2D.xml}{glCompressedTexSubImage2D()}.
*/

/*!
    \fn void QGLFunctions::copyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)

    Convenience function that calls glCopyTexImage2D(\a target, \a level, \a internalformat, \a x, \a y, \a width, \a height, \a border).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glCopyTexImage2D.xml}{glCopyTexImage2D()}.
*/

/*!
    \fn void QGLFunctions::copyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)

    Convenience function that calls glCopyTexSubImage2D(\a target, \a level, \a xoffset, \a yoffset, \a x, \a y, \a width, \a height).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glCopyTexSubImage2D.xml}{glCopyTexSubImage2D()}.
*/

/*!
    \fn GLuint QGLFunctions::createProgram()

    Convenience function that calls glCreateProgram().

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glCreateProgram.xml}{glCreateProgram()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn GLuint QGLFunctions::createShader(GLenum type)

    Convenience function that calls glCreateShader(\a type).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glCreateShader.xml}{glCreateShader()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::cullFace(GLenum mode)

    Convenience function that calls glCullFace(\a mode).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glCullFace.xml}{glCullFace()}.
*/

/*!
    \fn void QGLFunctions::deleteBuffers(GLsizei n, const GLuint* buffers)

    Convenience function that calls glDeleteBuffers(\a n, \a buffers).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glDeleteBuffers.xml}{glDeleteBuffers()}.
*/

/*!
    \fn void QGLFunctions::deleteFramebuffers(GLsizei n, const GLuint* framebuffers)

    Convenience function that calls glDeleteFramebuffers(\a n, \a framebuffers).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glDeleteFramebuffers.xml}{glDeleteFramebuffers()}.
*/

/*!
    \fn void QGLFunctions::deleteProgram(GLuint program)

    Convenience function that calls glDeleteProgram(\a program).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glDeleteProgram.xml}{glDeleteProgram()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::deleteRenderbuffers(GLsizei n, const GLuint* renderbuffers)

    Convenience function that calls glDeleteRenderbuffers(\a n, \a renderbuffers).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glDeleteRenderbuffers.xml}{glDeleteRenderbuffers()}.
*/

/*!
    \fn void QGLFunctions::deleteShader(GLuint shader)

    Convenience function that calls glDeleteShader(\a shader).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glDeleteShader.xml}{glDeleteShader()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::deleteTextures(GLsizei n, const GLuint* textures)

    Convenience function that calls glDeleteTextures(\a n, \a textures).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glDeleteTextures.xml}{glDeleteTextures()}.
*/

/*!
    \fn void QGLFunctions::depthFunc(GLenum func)

    Convenience function that calls glDepthFunc(\a func).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glDepthFunc.xml}{glDepthFunc()}.
*/

/*!
    \fn void QGLFunctions::depthMask(GLboolean flag)

    Convenience function that calls glDepthMask(\a flag).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glDepthMask.xml}{glDepthMask()}.
*/

/*!
    \fn void QGLFunctions::depthRange(GLclampf zNear, GLclampf zFar)

    Convenience function that calls glDepthRange(\a zNear, \a zFar) on
    desktop OpenGL systems and glDepthRangef(\a zNear, \a zFar) on
    embedded OpenGL/ES systems.

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glDepthRangef.xml}{glDepthRangef()}.
*/

/*!
    \fn void QGLFunctions::detachShader(GLuint program, GLuint shader)

    Convenience function that calls glDetachShader(\a program, \a shader).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glDetachShader.xml}{glDetachShader()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::disable(GLenum cap)

    Convenience function that calls glDisable(\a cap).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glDisable.xml}{glDisable()}.
*/

/*!
    \fn void QGLFunctions::disableVertexAttribArray(GLuint index)

    Convenience function that calls glDisableVertexAttribArray(\a index).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glDisableVertexAttribArray.xml}{glDisableVertexAttribArray()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::drawArrays(GLenum mode, GLint first, GLsizei count)

    Convenience function that calls glDrawArrays(\a mode, \a first, \a count).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glDrawArrays.xml}{glDrawArrays()}.
*/

/*!
    \fn void QGLFunctions::drawElements(GLenum mode, GLsizei count, GLenum type, const void* indices)

    Convenience function that calls glDrawElements(\a mode, \a count, \a type, \a indices).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glDrawElements.xml}{glDrawElements()}.
*/

/*!
    \fn void QGLFunctions::enable(GLenum cap)

    Convenience function that calls glEnable(\a cap).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glEnable.xml}{glEnable()}.
*/

/*!
    \fn void QGLFunctions::enableVertexAttribArray(GLuint index)

    Convenience function that calls glEnableVertexAttribArray(\a index).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glEnableVertexAttribArray.xml}{glEnableVertexAttribArray()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::finish()

    Convenience function that calls glFinish().

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glFinish.xml}{glFinish()}.
*/

/*!
    \fn void QGLFunctions::flush()

    Convenience function that calls glFlush().

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glFlush.xml}{glFlush()}.
*/

/*!
    \fn void QGLFunctions::framebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)

    Convenience function that calls glFramebufferRenderbuffer(\a target, \a attachment, \a renderbuffertarget, \a renderbuffer).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glFramebufferRenderbuffer.xml}{glFramebufferRenderbuffer()}.
*/

/*!
    \fn void QGLFunctions::framebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)

    Convenience function that calls glFramebufferTexture2D(\a target, \a attachment, \a textarget, \a texture, \a level).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glFramebufferTexture2D.xml}{glFramebufferTexture2D()}.
*/

/*!
    \fn void QGLFunctions::frontFace(GLenum mode)

    Convenience function that calls glFrontFace(\a mode).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glFrontFace.xml}{glFrontFace()}.
*/

/*!
    \fn void QGLFunctions::genBuffers(GLsizei n, GLuint* buffers)

    Convenience function that calls glGenBuffers(\a n, \a buffers).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glGenBuffers.xml}{glGenBuffers()}.
*/

/*!
    \fn void QGLFunctions::generateMipmap(GLenum target)

    Convenience function that calls glGenerateMipmap(\a target).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glGenerateMipmap.xml}{glGenerateMipmap()}.
*/

/*!
    \fn void QGLFunctions::genFramebuffers(GLsizei n, GLuint* framebuffers)

    Convenience function that calls glGenFramebuffers(\a n, \a framebuffers).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glGenFramebuffers.xml}{glGenFramebuffers()}.
*/

/*!
    \fn void QGLFunctions::genRenderbuffers(GLsizei n, GLuint* renderbuffers)

    Convenience function that calls glGenRenderbuffers(\a n, \a renderbuffers).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glGenRenderbuffers.xml}{glGenRenderbuffers()}.
*/

/*!
    \fn void QGLFunctions::genTextures(GLsizei n, GLuint* textures)

    Convenience function that calls glGenTextures(\a n, \a textures).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glGenTextures.xml}{glGenTextures()}.
*/

/*!
    \fn void QGLFunctions::getActiveAttrib(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name)

    Convenience function that calls glGetActiveAttrib(\a program, \a index, \a bufsize, \a length, \a size, \a type, \a name).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glGetActiveAttrib.xml}{glGetActiveAttrib()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::getActiveUniform(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name)

    Convenience function that calls glGetActiveUniform(\a program, \a index, \a bufsize, \a length, \a size, \a type, \a name).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glGetActiveUniform.xml}{glGetActiveUniform()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::getAttachedShaders(GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders)

    Convenience function that calls glGetAttachedShaders(\a program, \a maxcount, \a count, \a shaders).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glGetAttachedShaders.xml}{glGetAttachedShaders()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn int QGLFunctions::getAttribLocation(GLuint program, const char* name)

    Convenience function that calls glGetAttribLocation(\a program, \a name).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glGetAttribLocation.xml}{glGetAttribLocation()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::getBooleanv(GLenum pname, GLboolean* params)

    Convenience function that calls glGetBooleanv(\a pname, \a params).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glGetBooleanv.xml}{glGetBooleanv()}.
*/

/*!
    \fn void QGLFunctions::getBufferParameteriv(GLenum target, GLenum pname, GLint* params)

    Convenience function that calls glGetBufferParameteriv(\a target, \a pname, \a params).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glGetBufferParameteriv.xml}{glGetBufferParameteriv()}.
*/

/*!
    \fn GLenum QGLFunctions::getError()

    Convenience function that calls glGetError().

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glGetError.xml}{glGetError()}.
*/

/*!
    \fn void QGLFunctions::getFloatv(GLenum pname, GLfloat* params)

    Convenience function that calls glGetFloatv(\a pname, \a params).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glGetFloatv.xml}{glGetFloatv()}.
*/

/*!
    \fn void QGLFunctions::getFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint* params)

    Convenience function that calls glGetFramebufferAttachmentParameteriv(\a target, \a attachment, \a pname, \a params).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glGetFramebufferAttachmentParameteriv.xml}{glGetFramebufferAttachmentParameteriv()}.
*/

/*!
    \fn void QGLFunctions::getIntegerv(GLenum pname, GLint* params)

    Convenience function that calls glGetIntegerv(\a pname, \a params).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glGetIntegerv.xml}{glGetIntegerv()}.
*/

/*!
    \fn void QGLFunctions::getProgramiv(GLuint program, GLenum pname, GLint* params)

    Convenience function that calls glGetProgramiv(\a program, \a pname, \a params).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glGetProgramiv.xml}{glGetProgramiv()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::getProgramInfoLog(GLuint program, GLsizei bufsize, GLsizei* length, char* infolog)

    Convenience function that calls glGetProgramInfoLog(\a program, \a bufsize, \a length, \a infolog).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glGetProgramInfoLog.xml}{glGetProgramInfoLog()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::getRenderbufferParameteriv(GLenum target, GLenum pname, GLint* params)

    Convenience function that calls glGetRenderbufferParameteriv(\a target, \a pname, \a params).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glGetRenderbufferParameteriv.xml}{glGetRenderbufferParameteriv()}.
*/

/*!
    \fn void QGLFunctions::getShaderiv(GLuint shader, GLenum pname, GLint* params)

    Convenience function that calls glGetShaderiv(\a shader, \a pname, \a params).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glGetShaderiv.xml}{glGetShaderiv()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::getShaderInfoLog(GLuint shader, GLsizei bufsize, GLsizei* length, char* infolog)

    Convenience function that calls glGetShaderInfoLog(\a shader, \a bufsize, \a length, \a infolog).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glGetShaderInfoLog.xml}{glGetShaderInfoLog()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::getShaderPrecisionFormat(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision)

    Convenience function that calls glGetShaderPrecisionFormat(\a shadertype, \a precisiontype, \a range, \a precision).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glGetShaderPrecisionFormat.xml}{glGetShaderPrecisionFormat()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::getShaderSource(GLuint shader, GLsizei bufsize, GLsizei* length, char* source)

    Convenience function that calls glGetShaderSource(\a shader, \a bufsize, \a length, \a source).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glGetShaderSource.xml}{glGetShaderSource()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn const GLubyte* QGLFunctions::getString(GLenum name)

    Convenience function that calls glGetString(\a name).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glGetString.xml}{glGetString()}.
*/

/*!
    \fn void QGLFunctions::getTexParameterfv(GLenum target, GLenum pname, GLfloat* params)

    Convenience function that calls glGetTexParameterfv(\a target, \a pname, \a params).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glGetTexParameterfv.xml}{glGetTexParameterfv()}.
*/

/*!
    \fn void QGLFunctions::getTexParameteriv(GLenum target, GLenum pname, GLint* params)

    Convenience function that calls glGetTexParameteriv(\a target, \a pname, \a params).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glGetTexParameteriv.xml}{glGetTexParameteriv()}.
*/

/*!
    \fn void QGLFunctions::getUniformfv(GLuint program, GLint location, GLfloat* params)

    Convenience function that calls glGetUniformfv(\a program, \a location, \a params).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glGetUniformfv.xml}{glGetUniformfv()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::getUniformiv(GLuint program, GLint location, GLint* params)

    Convenience function that calls glGetUniformiv(\a program, \a location, \a params).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glGetUniformiv.xml}{glGetUniformiv()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn int QGLFunctions::getUniformLocation(GLuint program, const char* name)

    Convenience function that calls glGetUniformLocation(\a program, \a name).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glGetUniformLocation.xml}{glGetUniformLocation()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::getVertexAttribfv(GLuint index, GLenum pname, GLfloat* params)

    Convenience function that calls glGetVertexAttribfv(\a index, \a pname, \a params).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glGetVertexAttribfv.xml}{glGetVertexAttribfv()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::getVertexAttribiv(GLuint index, GLenum pname, GLint* params)

    Convenience function that calls glGetVertexAttribiv(\a index, \a pname, \a params).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glGetVertexAttribiv.xml}{glGetVertexAttribiv()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::getVertexAttribPointerv(GLuint index, GLenum pname, void** pointer)

    Convenience function that calls glGetVertexAttribPointerv(\a index, \a pname, \a pointer).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glGetVertexAttribPointerv.xml}{glGetVertexAttribPointerv()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::hint(GLenum target, GLenum mode)

    Convenience function that calls glHint(\a target, \a mode).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glHint.xml}{glHint()}.
*/

/*!
    \fn GLboolean QGLFunctions::isBuffer(GLuint buffer)

    Convenience function that calls glIsBuffer(\a buffer).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glIsBuffer.xml}{glIsBuffer()}.
*/

/*!
    \fn GLboolean QGLFunctions::isEnabled(GLenum cap)

    Convenience function that calls glIsEnabled(\a cap).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glIsEnabled.xml}{glIsEnabled()}.
*/

/*!
    \fn GLboolean QGLFunctions::isFramebuffer(GLuint framebuffer)

    Convenience function that calls glIsFramebuffer(\a framebuffer).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glIsFramebuffer.xml}{glIsFramebuffer()}.
*/

/*!
    \fn GLboolean QGLFunctions::isProgram(GLuint program)

    Convenience function that calls glIsProgram(\a program).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glIsProgram.xml}{glIsProgram()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn GLboolean QGLFunctions::isRenderbuffer(GLuint renderbuffer)

    Convenience function that calls glIsRenderbuffer(\a renderbuffer).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glIsRenderbuffer.xml}{glIsRenderbuffer()}.
*/

/*!
    \fn GLboolean QGLFunctions::isShader(GLuint shader)

    Convenience function that calls glIsShader(\a shader).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glIsShader.xml}{glIsShader()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn GLboolean QGLFunctions::isTexture(GLuint texture)

    Convenience function that calls glIsTexture(\a texture).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glIsTexture.xml}{glIsTexture()}.
*/

/*!
    \fn void QGLFunctions::lineWidth(GLfloat width)

    Convenience function that calls glLineWidth(\a width).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glLineWidth.xml}{glLineWidth()}.
*/

/*!
    \fn void QGLFunctions::linkProgram(GLuint program)

    Convenience function that calls glLinkProgram(\a program).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glLinkProgram.xml}{glLinkProgram()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::pixelStorei(GLenum pname, GLint param)

    Convenience function that calls glPixelStorei(\a pname, \a param).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glPixelStorei.xml}{glPixelStorei()}.
*/

/*!
    \fn void QGLFunctions::polygonOffset(GLfloat factor, GLfloat units)

    Convenience function that calls glPolygonOffset(\a factor, \a units).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glPolygonOffset.xml}{glPolygonOffset()}.
*/

/*!
    \fn void QGLFunctions::readPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void* pixels)

    Convenience function that calls glReadPixels(\a x, \a y, \a width, \a height, \a format, \a type, \a pixels).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glReadPixels.xml}{glReadPixels()}.
*/

/*!
    \fn void QGLFunctions::releaseShaderCompiler()

    Convenience function that calls glReleaseShaderCompiler().

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glReleaseShaderCompiler.xml}{glReleaseShaderCompiler()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::renderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)

    Convenience function that calls glRenderbufferStorage(\a target, \a internalformat, \a width, \a height).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glRenderbufferStorage.xml}{glRenderbufferStorage()}.
*/

/*!
    \fn void QGLFunctions::sampleCoverage(GLclampf value, GLboolean invert)

    Convenience function that calls glSampleCoverage(\a value, \a invert).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glSampleCoverage.xml}{glSampleCoverage()}.
*/

/*!
    \fn void QGLFunctions::scissor(GLint x, GLint y, GLsizei width, GLsizei height)

    Convenience function that calls glScissor(\a x, \a y, \a width, \a height).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glScissor.xml}{glScissor()}.
*/

/*!
    \fn void QGLFunctions::shaderBinary(GLint n, const GLuint* shaders, GLenum binaryformat, const void* binary, GLint length)

    Convenience function that calls glShaderBinary(\a n, \a shaders, \a binaryformat, \a binary, \a length).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glShaderBinary.xml}{glShaderBinary()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::shaderSource(GLuint shader, GLsizei count, const char** string, const GLint* length)

    Convenience function that calls glShaderSource(\a shader, \a count, \a string, \a length).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glShaderSource.xml}{glShaderSource()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::stencilFunc(GLenum func, GLint ref, GLuint mask)

    Convenience function that calls glStencilFunc(\a func, \a ref, \a mask).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glStencilFunc.xml}{glStencilFunc()}.
*/

/*!
    \fn void QGLFunctions::stencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask)

    Convenience function that calls glStencilFuncSeparate(\a face, \a func, \a ref, \a mask).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glStencilFuncSeparate.xml}{glStencilFuncSeparate()}.
*/

/*!
    \fn void QGLFunctions::stencilMask(GLuint mask)

    Convenience function that calls glStencilMask(\a mask).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glStencilMask.xml}{glStencilMask()}.
*/

/*!
    \fn void QGLFunctions::stencilMaskSeparate(GLenum face, GLuint mask)

    Convenience function that calls glStencilMaskSeparate(\a face, \a mask).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glStencilMaskSeparate.xml}{glStencilMaskSeparate()}.
*/

/*!
    \fn void QGLFunctions::stencilOp(GLenum fail, GLenum zfail, GLenum zpass)

    Convenience function that calls glStencilOp(\a fail, \a zfail, \a zpass).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glStencilOp.xml}{glStencilOp()}.
*/

/*!
    \fn void QGLFunctions::stencilOpSeparate(GLenum face, GLenum fail, GLenum zfail, GLenum zpass)

    Convenience function that calls glStencilOpSeparate(\a face, \a fail, \a zfail, \a zpass).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glStencilOpSeparate.xml}{glStencilOpSeparate()}.
*/

/*!
    \fn void QGLFunctions::texImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels)

    Convenience function that calls glTexImage2D(\a target, \a level, \a internalformat, \a width, \a height, \a border, \a format, \a type, \a pixels).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glTexImage2D.xml}{glTexImage2D()}.
*/

/*!
    \fn void QGLFunctions::texParameterf(GLenum target, GLenum pname, GLfloat param)

    Convenience function that calls glTexParameterf(\a target, \a pname, \a param).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glTexParameterf.xml}{glTexParameterf()}.
*/

/*!
    \fn void QGLFunctions::texParameterfv(GLenum target, GLenum pname, const GLfloat* params)

    Convenience function that calls glTexParameterfv(\a target, \a pname, \a params).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glTexParameterfv.xml}{glTexParameterfv()}.
*/

/*!
    \fn void QGLFunctions::texParameteri(GLenum target, GLenum pname, GLint param)

    Convenience function that calls glTexParameteri(\a target, \a pname, \a param).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glTexParameteri.xml}{glTexParameteri()}.
*/

/*!
    \fn void QGLFunctions::texParameteriv(GLenum target, GLenum pname, const GLint* params)

    Convenience function that calls glTexParameteriv(\a target, \a pname, \a params).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glTexParameteriv.xml}{glTexParameteriv()}.
*/

/*!
    \fn void QGLFunctions::texSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)

    Convenience function that calls glTexSubImage2D(\a target, \a level, \a xoffset, \a yoffset, \a width, \a height, \a format, \a type, \a pixels).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glTexSubImage2D.xml}{glTexSubImage2D()}.
*/

/*!
    \fn void QGLFunctions::uniform1f(GLint location, GLfloat x)

    Convenience function that calls glUniform1f(\a location, \a x).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glUniform1f.xml}{glUniform1f()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::uniform1fv(GLint location, GLsizei count, const GLfloat* v)

    Convenience function that calls glUniform1fv(\a location, \a count, \a v).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glUniform1fv.xml}{glUniform1fv()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::uniform1i(GLint location, GLint x)

    Convenience function that calls glUniform1i(\a location, \a x).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glUniform1i.xml}{glUniform1i()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::uniform1iv(GLint location, GLsizei count, const GLint* v)

    Convenience function that calls glUniform1iv(\a location, \a count, \a v).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glUniform1iv.xml}{glUniform1iv()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::uniform2f(GLint location, GLfloat x, GLfloat y)

    Convenience function that calls glUniform2f(\a location, \a x, \a y).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glUniform2f.xml}{glUniform2f()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::uniform2fv(GLint location, GLsizei count, const GLfloat* v)

    Convenience function that calls glUniform2fv(\a location, \a count, \a v).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glUniform2fv.xml}{glUniform2fv()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::uniform2i(GLint location, GLint x, GLint y)

    Convenience function that calls glUniform2i(\a location, \a x, \a y).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glUniform2i.xml}{glUniform2i()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::uniform2iv(GLint location, GLsizei count, const GLint* v)

    Convenience function that calls glUniform2iv(\a location, \a count, \a v).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glUniform2iv.xml}{glUniform2iv()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::uniform3f(GLint location, GLfloat x, GLfloat y, GLfloat z)

    Convenience function that calls glUniform3f(\a location, \a x, \a y, \a z).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glUniform3f.xml}{glUniform3f()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::uniform3fv(GLint location, GLsizei count, const GLfloat* v)

    Convenience function that calls glUniform3fv(\a location, \a count, \a v).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glUniform3fv.xml}{glUniform3fv()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::uniform3i(GLint location, GLint x, GLint y, GLint z)

    Convenience function that calls glUniform3i(\a location, \a x, \a y, \a z).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glUniform3i.xml}{glUniform3i()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::uniform3iv(GLint location, GLsizei count, const GLint* v)

    Convenience function that calls glUniform3iv(\a location, \a count, \a v).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glUniform3iv.xml}{glUniform3iv()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::uniform4f(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w)

    Convenience function that calls glUniform4f(\a location, \a x, \a y, \a z, \a w).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glUniform4f.xml}{glUniform4f()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::uniform4fv(GLint location, GLsizei count, const GLfloat* v)

    Convenience function that calls glUniform4fv(\a location, \a count, \a v).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glUniform4fv.xml}{glUniform4fv()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::uniform4i(GLint location, GLint x, GLint y, GLint z, GLint w)

    Convenience function that calls glUniform4i(\a location, \a x, \a y, \a z, \a w).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glUniform4i.xml}{glUniform4i()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::uniform4iv(GLint location, GLsizei count, const GLint* v)

    Convenience function that calls glUniform4iv(\a location, \a count, \a v).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glUniform4iv.xml}{glUniform4iv()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::uniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)

    Convenience function that calls glUniformMatrix2fv(\a location, \a count, \a transpose, \a value).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glUniformMatrix2fv.xml}{glUniformMatrix2fv()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::uniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)

    Convenience function that calls glUniformMatrix3fv(\a location, \a count, \a transpose, \a value).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glUniformMatrix3fv.xml}{glUniformMatrix3fv()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::uniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)

    Convenience function that calls glUniformMatrix4fv(\a location, \a count, \a transpose, \a value).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glUniformMatrix4fv.xml}{glUniformMatrix4fv()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::useProgram(GLuint program)

    Convenience function that calls glUseProgram(\a program).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glUseProgram.xml}{glUseProgram()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::validateProgram(GLuint program)

    Convenience function that calls glValidateProgram(\a program).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glValidateProgram.xml}{glValidateProgram()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::vertexAttrib1f(GLuint indx, GLfloat x)

    Convenience function that calls glVertexAttrib1f(\a indx, \a x).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glVertexAttrib1f.xml}{glVertexAttrib1f()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::vertexAttrib1fv(GLuint indx, const GLfloat* values)

    Convenience function that calls glVertexAttrib1fv(\a indx, \a values).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glVertexAttrib1fv.xml}{glVertexAttrib1fv()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::vertexAttrib2f(GLuint indx, GLfloat x, GLfloat y)

    Convenience function that calls glVertexAttrib2f(\a indx, \a x, \a y).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glVertexAttrib2f.xml}{glVertexAttrib2f()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::vertexAttrib2fv(GLuint indx, const GLfloat* values)

    Convenience function that calls glVertexAttrib2fv(\a indx, \a values).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glVertexAttrib2fv.xml}{glVertexAttrib2fv()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::vertexAttrib3f(GLuint indx, GLfloat x, GLfloat y, GLfloat z)

    Convenience function that calls glVertexAttrib3f(\a indx, \a x, \a y, \a z).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glVertexAttrib3f.xml}{glVertexAttrib3f()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::vertexAttrib3fv(GLuint indx, const GLfloat* values)

    Convenience function that calls glVertexAttrib3fv(\a indx, \a values).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glVertexAttrib3fv.xml}{glVertexAttrib3fv()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::vertexAttrib4f(GLuint indx, GLfloat x, GLfloat y, GLfloat z, GLfloat w)

    Convenience function that calls glVertexAttrib4f(\a indx, \a x, \a y, \a z, \a w).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glVertexAttrib4f.xml}{glVertexAttrib4f()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::vertexAttrib4fv(GLuint indx, const GLfloat* values)

    Convenience function that calls glVertexAttrib4fv(\a indx, \a values).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glVertexAttrib4fv.xml}{glVertexAttrib4fv()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::vertexAttribPointer(GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* ptr)

    Convenience function that calls glVertexAttribPointer(\a indx, \a size, \a type, \a normalized, \a stride, \a ptr).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glVertexAttribPointer.xml}{glVertexAttribPointer()}.

    This convenience function is not present on OpenGL/ES 1.x systems.
*/

/*!
    \fn void QGLFunctions::viewport(GLint x, GLint y, GLsizei width, GLsizei height)

    Convenience function that calls glViewport(\a x, \a y, \a width, \a height).

    For more information, see the OpenGL/ES 2.0 documentation for
    \l{http://www.khronos.org/opengles/sdk/docs/man/glViewport.xml}{glViewport()}.
*/

#ifndef QT_OPENGL_ES_2

#ifndef QT_OPENGL_ES_1

static void qglfResolveActiveTexture(GLenum texture)
{
    typedef void (QGLF_APIENTRYP type_glActiveTexture)(GLenum texture);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->activeTexture = (type_glActiveTexture)
        context->getProcAddress(QLatin1String("glActiveTexture"));
    if (funcs->activeTexture) {
        funcs->activeTexture = (type_glActiveTexture)
            context->getProcAddress(QLatin1String("glActiveTextureARB"));
    }

    if (funcs->activeTexture)
        funcs->activeTexture(texture);
    else
        funcs->activeTexture = qglfResolveActiveTexture;
}

static void qglfResolveAttachShader(GLuint program, GLuint shader)
{
    typedef void (QGLF_APIENTRYP type_glAttachShader)(GLuint program, GLuint shader);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->attachShader = (type_glAttachShader)
        context->getProcAddress(QLatin1String("glAttachShader"));
    if (funcs->attachShader) {
        funcs->attachShader = (type_glAttachShader)
            context->getProcAddress(QLatin1String("glAttachObjectARB"));
    }

    if (funcs->attachShader)
        funcs->attachShader(program, shader);
    else
        funcs->attachShader = qglfResolveAttachShader;
}

static void qglfResolveBindAttribLocation(GLuint program, GLuint index, const char* name)
{
    typedef void (QGLF_APIENTRYP type_glBindAttribLocation)(GLuint program, GLuint index, const char* name);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->bindAttribLocation = (type_glBindAttribLocation)
        context->getProcAddress(QLatin1String("glBindAttribLocation"));
    if (funcs->bindAttribLocation) {
        funcs->bindAttribLocation = (type_glBindAttribLocation)
            context->getProcAddress(QLatin1String("glBindAttribLocationARB"));
    }

    if (funcs->bindAttribLocation)
        funcs->bindAttribLocation(program, index, name);
    else
        funcs->bindAttribLocation = qglfResolveBindAttribLocation;
}

static void qglfResolveBindBuffer(GLenum target, GLuint buffer)
{
    typedef void (QGLF_APIENTRYP type_glBindBuffer)(GLenum target, GLuint buffer);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->bindBuffer = (type_glBindBuffer)
        context->getProcAddress(QLatin1String("glBindBuffer"));
#ifdef QT_OPENGL_ES
    if (funcs->bindBuffer) {
        funcs->bindBuffer = (type_glBindBuffer)
            context->getProcAddress(QLatin1String("glBindBufferOES"));
    }
#endif
    if (funcs->bindBuffer) {
        funcs->bindBuffer = (type_glBindBuffer)
            context->getProcAddress(QLatin1String("glBindBufferEXT"));
    }
    if (funcs->bindBuffer) {
        funcs->bindBuffer = (type_glBindBuffer)
            context->getProcAddress(QLatin1String("glBindBufferARB"));
    }

    if (funcs->bindBuffer)
        funcs->bindBuffer(target, buffer);
    else
        funcs->bindBuffer = qglfResolveBindBuffer;
}

#endif

static void qglfResolveBindFramebuffer(GLenum target, GLuint framebuffer)
{
    typedef void (QGLF_APIENTRYP type_glBindFramebuffer)(GLenum target, GLuint framebuffer);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->bindFramebuffer = (type_glBindFramebuffer)
        context->getProcAddress(QLatin1String("glBindFramebuffer"));
#ifdef QT_OPENGL_ES
    if (funcs->bindFramebuffer) {
        funcs->bindFramebuffer = (type_glBindFramebuffer)
            context->getProcAddress(QLatin1String("glBindFramebufferOES"));
    }
#endif
    if (funcs->bindFramebuffer) {
        funcs->bindFramebuffer = (type_glBindFramebuffer)
            context->getProcAddress(QLatin1String("glBindFramebufferEXT"));
    }
    if (funcs->bindFramebuffer) {
        funcs->bindFramebuffer = (type_glBindFramebuffer)
            context->getProcAddress(QLatin1String("glBindFramebufferARB"));
    }

    if (funcs->bindFramebuffer)
        funcs->bindFramebuffer(target, framebuffer);
    else
        funcs->bindFramebuffer = qglfResolveBindFramebuffer;
}

static void qglfResolveBindRenderbuffer(GLenum target, GLuint renderbuffer)
{
    typedef void (QGLF_APIENTRYP type_glBindRenderbuffer)(GLenum target, GLuint renderbuffer);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->bindRenderbuffer = (type_glBindRenderbuffer)
        context->getProcAddress(QLatin1String("glBindRenderbuffer"));
#ifdef QT_OPENGL_ES
    if (funcs->bindRenderbuffer) {
        funcs->bindRenderbuffer = (type_glBindRenderbuffer)
            context->getProcAddress(QLatin1String("glBindRenderbufferOES"));
    }
#endif
    if (funcs->bindRenderbuffer) {
        funcs->bindRenderbuffer = (type_glBindRenderbuffer)
            context->getProcAddress(QLatin1String("glBindRenderbufferEXT"));
    }
    if (funcs->bindRenderbuffer) {
        funcs->bindRenderbuffer = (type_glBindRenderbuffer)
            context->getProcAddress(QLatin1String("glBindRenderbufferARB"));
    }

    if (funcs->bindRenderbuffer)
        funcs->bindRenderbuffer(target, renderbuffer);
    else
        funcs->bindRenderbuffer = qglfResolveBindRenderbuffer;
}

static void qglfResolveBlendColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    typedef void (QGLF_APIENTRYP type_glBlendColor)(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->blendColor = (type_glBlendColor)
        context->getProcAddress(QLatin1String("glBlendColor"));
#ifdef QT_OPENGL_ES
    if (funcs->blendColor) {
        funcs->blendColor = (type_glBlendColor)
            context->getProcAddress(QLatin1String("glBlendColorOES"));
    }
#endif
    if (funcs->blendColor) {
        funcs->blendColor = (type_glBlendColor)
            context->getProcAddress(QLatin1String("glBlendColorEXT"));
    }
    if (funcs->blendColor) {
        funcs->blendColor = (type_glBlendColor)
            context->getProcAddress(QLatin1String("glBlendColorARB"));
    }

    if (funcs->blendColor)
        funcs->blendColor(red, green, blue, alpha);
    else
        funcs->blendColor = qglfResolveBlendColor;
}

static void qglfResolveBlendEquation(GLenum mode)
{
    typedef void (QGLF_APIENTRYP type_glBlendEquation)(GLenum mode);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->blendEquation = (type_glBlendEquation)
        context->getProcAddress(QLatin1String("glBlendEquation"));
#ifdef QT_OPENGL_ES
    if (funcs->blendEquation) {
        funcs->blendEquation = (type_glBlendEquation)
            context->getProcAddress(QLatin1String("glBlendEquationOES"));
    }
#endif
    if (funcs->blendEquation) {
        funcs->blendEquation = (type_glBlendEquation)
            context->getProcAddress(QLatin1String("glBlendEquationEXT"));
    }
    if (funcs->blendEquation) {
        funcs->blendEquation = (type_glBlendEquation)
            context->getProcAddress(QLatin1String("glBlendEquationARB"));
    }

    if (funcs->blendEquation)
        funcs->blendEquation(mode);
    else
        funcs->blendEquation = qglfResolveBlendEquation;
}

static void qglfSpecialBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha)
{
    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);
    Q_UNUSED(modeAlpha);
    funcs->blendEquation(modeRGB);
}

static void qglfResolveBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha)
{
    typedef void (QGLF_APIENTRYP type_glBlendEquationSeparate)(GLenum modeRGB, GLenum modeAlpha);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->blendEquationSeparate = (type_glBlendEquationSeparate)
        context->getProcAddress(QLatin1String("glBlendEquationSeparate"));
#ifdef QT_OPENGL_ES
    if (funcs->blendEquationSeparate) {
        funcs->blendEquationSeparate = (type_glBlendEquationSeparate)
            context->getProcAddress(QLatin1String("glBlendEquationSeparateOES"));
    }
#endif
    if (funcs->blendEquationSeparate) {
        funcs->blendEquationSeparate = (type_glBlendEquationSeparate)
            context->getProcAddress(QLatin1String("glBlendEquationSeparateEXT"));
    }
    if (funcs->blendEquationSeparate) {
        funcs->blendEquationSeparate = (type_glBlendEquationSeparate)
            context->getProcAddress(QLatin1String("glBlendEquationSeparateARB"));
    }

    if (!funcs->blendEquationSeparate)
        funcs->blendEquationSeparate = qglfSpecialBlendEquationSeparate;

    funcs->blendEquationSeparate(modeRGB, modeAlpha);
}

static void qglfSpecialBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
{
    Q_UNUSED(srcAlpha);
    Q_UNUSED(dstAlpha);
    ::glBlendFunc(srcRGB, dstRGB);
}

static void qglfResolveBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
{
    typedef void (QGLF_APIENTRYP type_glBlendFuncSeparate)(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->blendFuncSeparate = (type_glBlendFuncSeparate)
        context->getProcAddress(QLatin1String("glBlendFuncSeparate"));
#ifdef QT_OPENGL_ES
    if (funcs->blendFuncSeparate) {
        funcs->blendFuncSeparate = (type_glBlendFuncSeparate)
            context->getProcAddress(QLatin1String("glBlendFuncSeparateOES"));
    }
#endif
    if (funcs->blendFuncSeparate) {
        funcs->blendFuncSeparate = (type_glBlendFuncSeparate)
            context->getProcAddress(QLatin1String("glBlendFuncSeparateEXT"));
    }
    if (funcs->blendFuncSeparate) {
        funcs->blendFuncSeparate = (type_glBlendFuncSeparate)
            context->getProcAddress(QLatin1String("glBlendFuncSeparateARB"));
    }

    if (!funcs->blendFuncSeparate)
        funcs->blendFuncSeparate = qglfSpecialBlendFuncSeparate;

    funcs->blendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
}

#ifndef QT_OPENGL_ES_1

static void qglfResolveBufferData(GLenum target, qgl_GLsizeiptr size, const void* data, GLenum usage)
{
    typedef void (QGLF_APIENTRYP type_glBufferData)(GLenum target, qgl_GLsizeiptr size, const void* data, GLenum usage);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->bufferData = (type_glBufferData)
        context->getProcAddress(QLatin1String("glBufferData"));
#ifdef QT_OPENGL_ES
    if (funcs->bufferData) {
        funcs->bufferData = (type_glBufferData)
            context->getProcAddress(QLatin1String("glBufferDataOES"));
    }
#endif
    if (funcs->bufferData) {
        funcs->bufferData = (type_glBufferData)
            context->getProcAddress(QLatin1String("glBufferDataEXT"));
    }
    if (funcs->bufferData) {
        funcs->bufferData = (type_glBufferData)
            context->getProcAddress(QLatin1String("glBufferDataARB"));
    }

    if (funcs->bufferData)
        funcs->bufferData(target, size, data, usage);
    else
        funcs->bufferData = qglfResolveBufferData;
}

static void qglfResolveBufferSubData(GLenum target, qgl_GLintptr offset, qgl_GLsizeiptr size, const void* data)
{
    typedef void (QGLF_APIENTRYP type_glBufferSubData)(GLenum target, qgl_GLintptr offset, qgl_GLsizeiptr size, const void* data);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->bufferSubData = (type_glBufferSubData)
        context->getProcAddress(QLatin1String("glBufferSubData"));
#ifdef QT_OPENGL_ES
    if (funcs->bufferSubData) {
        funcs->bufferSubData = (type_glBufferSubData)
            context->getProcAddress(QLatin1String("glBufferSubDataOES"));
    }
#endif
    if (funcs->bufferSubData) {
        funcs->bufferSubData = (type_glBufferSubData)
            context->getProcAddress(QLatin1String("glBufferSubDataEXT"));
    }
    if (funcs->bufferSubData) {
        funcs->bufferSubData = (type_glBufferSubData)
            context->getProcAddress(QLatin1String("glBufferSubDataARB"));
    }

    if (funcs->bufferSubData)
        funcs->bufferSubData(target, offset, size, data);
    else
        funcs->bufferSubData = qglfResolveBufferSubData;
}

#endif

static GLenum qglfResolveCheckFramebufferStatus(GLenum target)
{
    typedef GLenum (QGLF_APIENTRYP type_glCheckFramebufferStatus)(GLenum target);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->checkFramebufferStatus = (type_glCheckFramebufferStatus)
        context->getProcAddress(QLatin1String("glCheckFramebufferStatus"));
#ifdef QT_OPENGL_ES
    if (funcs->checkFramebufferStatus) {
        funcs->checkFramebufferStatus = (type_glCheckFramebufferStatus)
            context->getProcAddress(QLatin1String("glCheckFramebufferStatusOES"));
    }
#endif
    if (funcs->checkFramebufferStatus) {
        funcs->checkFramebufferStatus = (type_glCheckFramebufferStatus)
            context->getProcAddress(QLatin1String("glCheckFramebufferStatusEXT"));
    }
    if (funcs->checkFramebufferStatus) {
        funcs->checkFramebufferStatus = (type_glCheckFramebufferStatus)
            context->getProcAddress(QLatin1String("glCheckFramebufferStatusARB"));
    }

    if (funcs->checkFramebufferStatus)
        return funcs->checkFramebufferStatus(target);
    funcs->checkFramebufferStatus = qglfResolveCheckFramebufferStatus;
    return GLenum(0);
}

#ifndef QT_OPENGL_ES_1

static void qglfResolveCompileShader(GLuint shader)
{
    typedef void (QGLF_APIENTRYP type_glCompileShader)(GLuint shader);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->compileShader = (type_glCompileShader)
        context->getProcAddress(QLatin1String("glCompileShader"));
    if (funcs->compileShader) {
        funcs->compileShader = (type_glCompileShader)
            context->getProcAddress(QLatin1String("glCompileShader"));
    }

    if (funcs->compileShader)
        funcs->compileShader(shader);
    else
        funcs->compileShader = qglfResolveCompileShader;
}

static void qglfResolveCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data)
{
    typedef void (QGLF_APIENTRYP type_glCompressedTexImage2D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->compressedTexImage2D = (type_glCompressedTexImage2D)
        context->getProcAddress(QLatin1String("glCompressedTexImage2D"));
#ifdef QT_OPENGL_ES
    if (funcs->compressedTexImage2D) {
        funcs->compressedTexImage2D = (type_glCompressedTexImage2D)
            context->getProcAddress(QLatin1String("glCompressedTexImage2DOES"));
    }
#endif
    if (funcs->compressedTexImage2D) {
        funcs->compressedTexImage2D = (type_glCompressedTexImage2D)
            context->getProcAddress(QLatin1String("glCompressedTexImage2DEXT"));
    }
    if (funcs->compressedTexImage2D) {
        funcs->compressedTexImage2D = (type_glCompressedTexImage2D)
            context->getProcAddress(QLatin1String("glCompressedTexImage2DARB"));
    }

    if (funcs->compressedTexImage2D)
        funcs->compressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
    else
        funcs->compressedTexImage2D = qglfResolveCompressedTexImage2D;
}

static void qglfResolveCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data)
{
    typedef void (QGLF_APIENTRYP type_glCompressedTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->compressedTexSubImage2D = (type_glCompressedTexSubImage2D)
        context->getProcAddress(QLatin1String("glCompressedTexSubImage2D"));
#ifdef QT_OPENGL_ES
    if (funcs->compressedTexSubImage2D) {
        funcs->compressedTexSubImage2D = (type_glCompressedTexSubImage2D)
            context->getProcAddress(QLatin1String("glCompressedTexSubImage2DOES"));
    }
#endif
    if (funcs->compressedTexSubImage2D) {
        funcs->compressedTexSubImage2D = (type_glCompressedTexSubImage2D)
            context->getProcAddress(QLatin1String("glCompressedTexSubImage2DEXT"));
    }
    if (funcs->compressedTexSubImage2D) {
        funcs->compressedTexSubImage2D = (type_glCompressedTexSubImage2D)
            context->getProcAddress(QLatin1String("glCompressedTexSubImage2DARB"));
    }

    if (funcs->compressedTexSubImage2D)
        funcs->compressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data);
    else
        funcs->compressedTexSubImage2D = qglfResolveCompressedTexSubImage2D;
}

static GLuint qglfResolveCreateProgram()
{
    typedef GLuint (QGLF_APIENTRYP type_glCreateProgram)();

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->createProgram = (type_glCreateProgram)
        context->getProcAddress(QLatin1String("glCreateProgram"));
    if (funcs->createProgram) {
        funcs->createProgram = (type_glCreateProgram)
            context->getProcAddress(QLatin1String("glCreateProgramObjectARB"));
    }

    if (funcs->createProgram)
        return funcs->createProgram();
    funcs->createProgram = qglfResolveCreateProgram;
    return GLuint(0);
}

static GLuint qglfResolveCreateShader(GLenum type)
{
    typedef GLuint (QGLF_APIENTRYP type_glCreateShader)(GLenum type);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->createShader = (type_glCreateShader)
        context->getProcAddress(QLatin1String("glCreateShader"));
    if (funcs->createShader) {
        funcs->createShader = (type_glCreateShader)
            context->getProcAddress(QLatin1String("glCreateShaderObjectARB"));
    }

    if (funcs->createShader)
        return funcs->createShader(type);
    funcs->createShader = qglfResolveCreateShader;
    return GLuint(0);
}

static void qglfResolveDeleteBuffers(GLsizei n, const GLuint* buffers)
{
    typedef void (QGLF_APIENTRYP type_glDeleteBuffers)(GLsizei n, const GLuint* buffers);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->deleteBuffers = (type_glDeleteBuffers)
        context->getProcAddress(QLatin1String("glDeleteBuffers"));
#ifdef QT_OPENGL_ES
    if (funcs->deleteBuffers) {
        funcs->deleteBuffers = (type_glDeleteBuffers)
            context->getProcAddress(QLatin1String("glDeleteBuffersOES"));
    }
#endif
    if (funcs->deleteBuffers) {
        funcs->deleteBuffers = (type_glDeleteBuffers)
            context->getProcAddress(QLatin1String("glDeleteBuffersEXT"));
    }
    if (funcs->deleteBuffers) {
        funcs->deleteBuffers = (type_glDeleteBuffers)
            context->getProcAddress(QLatin1String("glDeleteBuffersARB"));
    }

    if (funcs->deleteBuffers)
        funcs->deleteBuffers(n, buffers);
    else
        funcs->deleteBuffers = qglfResolveDeleteBuffers;
}

#endif

static void qglfResolveDeleteFramebuffers(GLsizei n, const GLuint* framebuffers)
{
    typedef void (QGLF_APIENTRYP type_glDeleteFramebuffers)(GLsizei n, const GLuint* framebuffers);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->deleteFramebuffers = (type_glDeleteFramebuffers)
        context->getProcAddress(QLatin1String("glDeleteFramebuffers"));
#ifdef QT_OPENGL_ES
    if (funcs->deleteFramebuffers) {
        funcs->deleteFramebuffers = (type_glDeleteFramebuffers)
            context->getProcAddress(QLatin1String("glDeleteFramebuffersOES"));
    }
#endif
    if (funcs->deleteFramebuffers) {
        funcs->deleteFramebuffers = (type_glDeleteFramebuffers)
            context->getProcAddress(QLatin1String("glDeleteFramebuffersEXT"));
    }
    if (funcs->deleteFramebuffers) {
        funcs->deleteFramebuffers = (type_glDeleteFramebuffers)
            context->getProcAddress(QLatin1String("glDeleteFramebuffersARB"));
    }

    if (funcs->deleteFramebuffers)
        funcs->deleteFramebuffers(n, framebuffers);
    else
        funcs->deleteFramebuffers = qglfResolveDeleteFramebuffers;
}

#ifndef QT_OPENGL_ES_1

static void qglfResolveDeleteProgram(GLuint program)
{
    typedef void (QGLF_APIENTRYP type_glDeleteProgram)(GLuint program);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->deleteProgram = (type_glDeleteProgram)
        context->getProcAddress(QLatin1String("glDeleteProgram"));
    if (funcs->deleteProgram) {
        funcs->deleteProgram = (type_glDeleteProgram)
            context->getProcAddress(QLatin1String("glDeleteObjectARB"));
    }

    if (funcs->deleteProgram)
        funcs->deleteProgram(program);
    else
        funcs->deleteProgram = qglfResolveDeleteProgram;
}

#endif

static void qglfResolveDeleteRenderbuffers(GLsizei n, const GLuint* renderbuffers)
{
    typedef void (QGLF_APIENTRYP type_glDeleteRenderbuffers)(GLsizei n, const GLuint* renderbuffers);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->deleteRenderbuffers = (type_glDeleteRenderbuffers)
        context->getProcAddress(QLatin1String("glDeleteRenderbuffers"));
#ifdef QT_OPENGL_ES
    if (funcs->deleteRenderbuffers) {
        funcs->deleteRenderbuffers = (type_glDeleteRenderbuffers)
            context->getProcAddress(QLatin1String("glDeleteRenderbuffersOES"));
    }
#endif
    if (funcs->deleteRenderbuffers) {
        funcs->deleteRenderbuffers = (type_glDeleteRenderbuffers)
            context->getProcAddress(QLatin1String("glDeleteRenderbuffersEXT"));
    }
    if (funcs->deleteRenderbuffers) {
        funcs->deleteRenderbuffers = (type_glDeleteRenderbuffers)
            context->getProcAddress(QLatin1String("glDeleteRenderbuffersARB"));
    }

    if (funcs->deleteRenderbuffers)
        funcs->deleteRenderbuffers(n, renderbuffers);
    else
        funcs->deleteRenderbuffers = qglfResolveDeleteRenderbuffers;
}

#ifndef QT_OPENGL_ES_1

static void qglfResolveDeleteShader(GLuint shader)
{
    typedef void (QGLF_APIENTRYP type_glDeleteShader)(GLuint shader);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->deleteShader = (type_glDeleteShader)
        context->getProcAddress(QLatin1String("glDeleteShader"));
    if (funcs->deleteShader) {
        funcs->deleteShader = (type_glDeleteShader)
            context->getProcAddress(QLatin1String("glDeleteObjectARB"));
    }

    if (funcs->deleteShader)
        funcs->deleteShader(shader);
    else
        funcs->deleteShader = qglfResolveDeleteShader;
}

static void qglfResolveDetachShader(GLuint program, GLuint shader)
{
    typedef void (QGLF_APIENTRYP type_glDetachShader)(GLuint program, GLuint shader);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->detachShader = (type_glDetachShader)
        context->getProcAddress(QLatin1String("glDetachShader"));
    if (funcs->detachShader) {
        funcs->detachShader = (type_glDetachShader)
            context->getProcAddress(QLatin1String("glDetachObjectARB"));
    }

    if (funcs->detachShader)
        funcs->detachShader(program, shader);
    else
        funcs->detachShader = qglfResolveDetachShader;
}

static void qglfResolveDisableVertexAttribArray(GLuint index)
{
    typedef void (QGLF_APIENTRYP type_glDisableVertexAttribArray)(GLuint index);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->disableVertexAttribArray = (type_glDisableVertexAttribArray)
        context->getProcAddress(QLatin1String("glDisableVertexAttribArray"));
    if (funcs->disableVertexAttribArray) {
        funcs->disableVertexAttribArray = (type_glDisableVertexAttribArray)
            context->getProcAddress(QLatin1String("glDisableVertexAttribArrayARB"));
    }

    if (funcs->disableVertexAttribArray)
        funcs->disableVertexAttribArray(index);
    else
        funcs->disableVertexAttribArray = qglfResolveDisableVertexAttribArray;
}

static void qglfResolveEnableVertexAttribArray(GLuint index)
{
    typedef void (QGLF_APIENTRYP type_glEnableVertexAttribArray)(GLuint index);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->enableVertexAttribArray = (type_glEnableVertexAttribArray)
        context->getProcAddress(QLatin1String("glEnableVertexAttribArray"));
    if (funcs->enableVertexAttribArray) {
        funcs->enableVertexAttribArray = (type_glEnableVertexAttribArray)
            context->getProcAddress(QLatin1String("glEnableVertexAttribArrayARB"));
    }

    if (funcs->enableVertexAttribArray)
        funcs->enableVertexAttribArray(index);
    else
        funcs->enableVertexAttribArray = qglfResolveEnableVertexAttribArray;
}

#endif

static void qglfResolveFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
    typedef void (QGLF_APIENTRYP type_glFramebufferRenderbuffer)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->framebufferRenderbuffer = (type_glFramebufferRenderbuffer)
        context->getProcAddress(QLatin1String("glFramebufferRenderbuffer"));
#ifdef QT_OPENGL_ES
    if (funcs->framebufferRenderbuffer) {
        funcs->framebufferRenderbuffer = (type_glFramebufferRenderbuffer)
            context->getProcAddress(QLatin1String("glFramebufferRenderbufferOES"));
    }
#endif
    if (funcs->framebufferRenderbuffer) {
        funcs->framebufferRenderbuffer = (type_glFramebufferRenderbuffer)
            context->getProcAddress(QLatin1String("glFramebufferRenderbufferEXT"));
    }
    if (funcs->framebufferRenderbuffer) {
        funcs->framebufferRenderbuffer = (type_glFramebufferRenderbuffer)
            context->getProcAddress(QLatin1String("glFramebufferRenderbufferARB"));
    }

    if (funcs->framebufferRenderbuffer)
        funcs->framebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
    else
        funcs->framebufferRenderbuffer = qglfResolveFramebufferRenderbuffer;
}

static void qglfResolveFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
    typedef void (QGLF_APIENTRYP type_glFramebufferTexture2D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->framebufferTexture2D = (type_glFramebufferTexture2D)
        context->getProcAddress(QLatin1String("glFramebufferTexture2D"));
#ifdef QT_OPENGL_ES
    if (funcs->framebufferTexture2D) {
        funcs->framebufferTexture2D = (type_glFramebufferTexture2D)
            context->getProcAddress(QLatin1String("glFramebufferTexture2DOES"));
    }
#endif
    if (funcs->framebufferTexture2D) {
        funcs->framebufferTexture2D = (type_glFramebufferTexture2D)
            context->getProcAddress(QLatin1String("glFramebufferTexture2DEXT"));
    }
    if (funcs->framebufferTexture2D) {
        funcs->framebufferTexture2D = (type_glFramebufferTexture2D)
            context->getProcAddress(QLatin1String("glFramebufferTexture2DARB"));
    }

    if (funcs->framebufferTexture2D)
        funcs->framebufferTexture2D(target, attachment, textarget, texture, level);
    else
        funcs->framebufferTexture2D = qglfResolveFramebufferTexture2D;
}

#ifndef QT_OPENGL_ES_1

static void qglfResolveGenBuffers(GLsizei n, GLuint* buffers)
{
    typedef void (QGLF_APIENTRYP type_glGenBuffers)(GLsizei n, GLuint* buffers);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->genBuffers = (type_glGenBuffers)
        context->getProcAddress(QLatin1String("glGenBuffers"));
#ifdef QT_OPENGL_ES
    if (funcs->genBuffers) {
        funcs->genBuffers = (type_glGenBuffers)
            context->getProcAddress(QLatin1String("glGenBuffersOES"));
    }
#endif
    if (funcs->genBuffers) {
        funcs->genBuffers = (type_glGenBuffers)
            context->getProcAddress(QLatin1String("glGenBuffersEXT"));
    }
    if (funcs->genBuffers) {
        funcs->genBuffers = (type_glGenBuffers)
            context->getProcAddress(QLatin1String("glGenBuffersARB"));
    }

    if (funcs->genBuffers)
        funcs->genBuffers(n, buffers);
    else
        funcs->genBuffers = qglfResolveGenBuffers;
}

#endif

static void qglfResolveGenerateMipmap(GLenum target)
{
    typedef void (QGLF_APIENTRYP type_glGenerateMipmap)(GLenum target);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->generateMipmap = (type_glGenerateMipmap)
        context->getProcAddress(QLatin1String("glGenerateMipmap"));
#ifdef QT_OPENGL_ES
    if (funcs->generateMipmap) {
        funcs->generateMipmap = (type_glGenerateMipmap)
            context->getProcAddress(QLatin1String("glGenerateMipmapOES"));
    }
#endif
    if (funcs->generateMipmap) {
        funcs->generateMipmap = (type_glGenerateMipmap)
            context->getProcAddress(QLatin1String("glGenerateMipmapEXT"));
    }
    if (funcs->generateMipmap) {
        funcs->generateMipmap = (type_glGenerateMipmap)
            context->getProcAddress(QLatin1String("glGenerateMipmapARB"));
    }

    if (funcs->generateMipmap)
        funcs->generateMipmap(target);
    else
        funcs->generateMipmap = qglfResolveGenerateMipmap;
}

static void qglfResolveGenFramebuffers(GLsizei n, GLuint* framebuffers)
{
    typedef void (QGLF_APIENTRYP type_glGenFramebuffers)(GLsizei n, GLuint* framebuffers);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->genFramebuffers = (type_glGenFramebuffers)
        context->getProcAddress(QLatin1String("glGenFramebuffers"));
#ifdef QT_OPENGL_ES
    if (funcs->genFramebuffers) {
        funcs->genFramebuffers = (type_glGenFramebuffers)
            context->getProcAddress(QLatin1String("glGenFramebuffersOES"));
    }
#endif
    if (funcs->genFramebuffers) {
        funcs->genFramebuffers = (type_glGenFramebuffers)
            context->getProcAddress(QLatin1String("glGenFramebuffersEXT"));
    }
    if (funcs->genFramebuffers) {
        funcs->genFramebuffers = (type_glGenFramebuffers)
            context->getProcAddress(QLatin1String("glGenFramebuffersARB"));
    }

    if (funcs->genFramebuffers)
        funcs->genFramebuffers(n, framebuffers);
    else
        funcs->genFramebuffers = qglfResolveGenFramebuffers;
}

static void qglfResolveGenRenderbuffers(GLsizei n, GLuint* renderbuffers)
{
    typedef void (QGLF_APIENTRYP type_glGenRenderbuffers)(GLsizei n, GLuint* renderbuffers);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->genRenderbuffers = (type_glGenRenderbuffers)
        context->getProcAddress(QLatin1String("glGenRenderbuffers"));
#ifdef QT_OPENGL_ES
    if (funcs->genRenderbuffers) {
        funcs->genRenderbuffers = (type_glGenRenderbuffers)
            context->getProcAddress(QLatin1String("glGenRenderbuffersOES"));
    }
#endif
    if (funcs->genRenderbuffers) {
        funcs->genRenderbuffers = (type_glGenRenderbuffers)
            context->getProcAddress(QLatin1String("glGenRenderbuffersEXT"));
    }
    if (funcs->genRenderbuffers) {
        funcs->genRenderbuffers = (type_glGenRenderbuffers)
            context->getProcAddress(QLatin1String("glGenRenderbuffersARB"));
    }

    if (funcs->genRenderbuffers)
        funcs->genRenderbuffers(n, renderbuffers);
    else
        funcs->genRenderbuffers = qglfResolveGenRenderbuffers;
}

#ifndef QT_OPENGL_ES_1

static void qglfResolveGetActiveAttrib(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name)
{
    typedef void (QGLF_APIENTRYP type_glGetActiveAttrib)(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->getActiveAttrib = (type_glGetActiveAttrib)
        context->getProcAddress(QLatin1String("glGetActiveAttrib"));
    if (funcs->getActiveAttrib) {
        funcs->getActiveAttrib = (type_glGetActiveAttrib)
            context->getProcAddress(QLatin1String("glGetActiveAttribARB"));
    }

    if (funcs->getActiveAttrib)
        funcs->getActiveAttrib(program, index, bufsize, length, size, type, name);
    else
        funcs->getActiveAttrib = qglfResolveGetActiveAttrib;
}

static void qglfResolveGetActiveUniform(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name)
{
    typedef void (QGLF_APIENTRYP type_glGetActiveUniform)(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->getActiveUniform = (type_glGetActiveUniform)
        context->getProcAddress(QLatin1String("glGetActiveUniform"));
    if (funcs->getActiveUniform) {
        funcs->getActiveUniform = (type_glGetActiveUniform)
            context->getProcAddress(QLatin1String("glGetActiveUniformARB"));
    }

    if (funcs->getActiveUniform)
        funcs->getActiveUniform(program, index, bufsize, length, size, type, name);
    else
        funcs->getActiveUniform = qglfResolveGetActiveUniform;
}

static void qglfResolveGetAttachedShaders(GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders)
{
    typedef void (QGLF_APIENTRYP type_glGetAttachedShaders)(GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->getAttachedShaders = (type_glGetAttachedShaders)
        context->getProcAddress(QLatin1String("glGetAttachedShaders"));
    if (funcs->getAttachedShaders) {
        funcs->getAttachedShaders = (type_glGetAttachedShaders)
            context->getProcAddress(QLatin1String("glGetAttachedObjectsARB"));
    }

    if (funcs->getAttachedShaders)
        funcs->getAttachedShaders(program, maxcount, count, shaders);
    else
        funcs->getAttachedShaders = qglfResolveGetAttachedShaders;
}

static int qglfResolveGetAttribLocation(GLuint program, const char* name)
{
    typedef int (QGLF_APIENTRYP type_glGetAttribLocation)(GLuint program, const char* name);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->getAttribLocation = (type_glGetAttribLocation)
        context->getProcAddress(QLatin1String("glGetAttribLocation"));
    if (funcs->getAttribLocation) {
        funcs->getAttribLocation = (type_glGetAttribLocation)
            context->getProcAddress(QLatin1String("glGetAttribLocationARB"));
    }

    if (funcs->getAttribLocation)
        return funcs->getAttribLocation(program, name);
    funcs->getAttribLocation = qglfResolveGetAttribLocation;
    return int(0);
}

#endif

static void qglfResolveGetBufferParameteriv(GLenum target, GLenum pname, GLint* params)
{
    typedef void (QGLF_APIENTRYP type_glGetBufferParameteriv)(GLenum target, GLenum pname, GLint* params);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->getBufferParameteriv = (type_glGetBufferParameteriv)
        context->getProcAddress(QLatin1String("glGetBufferParameteriv"));
#ifdef QT_OPENGL_ES
    if (funcs->getBufferParameteriv) {
        funcs->getBufferParameteriv = (type_glGetBufferParameteriv)
            context->getProcAddress(QLatin1String("glGetBufferParameterivOES"));
    }
#endif
    if (funcs->getBufferParameteriv) {
        funcs->getBufferParameteriv = (type_glGetBufferParameteriv)
            context->getProcAddress(QLatin1String("glGetBufferParameterivEXT"));
    }
    if (funcs->getBufferParameteriv) {
        funcs->getBufferParameteriv = (type_glGetBufferParameteriv)
            context->getProcAddress(QLatin1String("glGetBufferParameterivARB"));
    }

    if (funcs->getBufferParameteriv)
        funcs->getBufferParameteriv(target, pname, params);
    else
        funcs->getBufferParameteriv = qglfResolveGetBufferParameteriv;
}

static void qglfResolveGetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint* params)
{
    typedef void (QGLF_APIENTRYP type_glGetFramebufferAttachmentParameteriv)(GLenum target, GLenum attachment, GLenum pname, GLint* params);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->getFramebufferAttachmentParameteriv = (type_glGetFramebufferAttachmentParameteriv)
        context->getProcAddress(QLatin1String("glGetFramebufferAttachmentParameteriv"));
#ifdef QT_OPENGL_ES
    if (funcs->getFramebufferAttachmentParameteriv) {
        funcs->getFramebufferAttachmentParameteriv = (type_glGetFramebufferAttachmentParameteriv)
            context->getProcAddress(QLatin1String("glGetFramebufferAttachmentParameterivOES"));
    }
#endif
    if (funcs->getFramebufferAttachmentParameteriv) {
        funcs->getFramebufferAttachmentParameteriv = (type_glGetFramebufferAttachmentParameteriv)
            context->getProcAddress(QLatin1String("glGetFramebufferAttachmentParameterivEXT"));
    }
    if (funcs->getFramebufferAttachmentParameteriv) {
        funcs->getFramebufferAttachmentParameteriv = (type_glGetFramebufferAttachmentParameteriv)
            context->getProcAddress(QLatin1String("glGetFramebufferAttachmentParameterivARB"));
    }

    if (funcs->getFramebufferAttachmentParameteriv)
        funcs->getFramebufferAttachmentParameteriv(target, attachment, pname, params);
    else
        funcs->getFramebufferAttachmentParameteriv = qglfResolveGetFramebufferAttachmentParameteriv;
}

#ifndef QT_OPENGL_ES_1

static void qglfResolveGetProgramiv(GLuint program, GLenum pname, GLint* params)
{
    typedef void (QGLF_APIENTRYP type_glGetProgramiv)(GLuint program, GLenum pname, GLint* params);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->getProgramiv = (type_glGetProgramiv)
        context->getProcAddress(QLatin1String("glGetProgramiv"));
    if (funcs->getProgramiv) {
        funcs->getProgramiv = (type_glGetProgramiv)
            context->getProcAddress(QLatin1String("glGetObjectParameterivARB"));
    }

    if (funcs->getProgramiv)
        funcs->getProgramiv(program, pname, params);
    else
        funcs->getProgramiv = qglfResolveGetProgramiv;
}

static void qglfResolveGetProgramInfoLog(GLuint program, GLsizei bufsize, GLsizei* length, char* infolog)
{
    typedef void (QGLF_APIENTRYP type_glGetProgramInfoLog)(GLuint program, GLsizei bufsize, GLsizei* length, char* infolog);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->getProgramInfoLog = (type_glGetProgramInfoLog)
        context->getProcAddress(QLatin1String("glGetProgramInfoLog"));
    if (funcs->getProgramInfoLog) {
        funcs->getProgramInfoLog = (type_glGetProgramInfoLog)
            context->getProcAddress(QLatin1String("glGetInfoLogARB"));
    }

    if (funcs->getProgramInfoLog)
        funcs->getProgramInfoLog(program, bufsize, length, infolog);
    else
        funcs->getProgramInfoLog = qglfResolveGetProgramInfoLog;
}

#endif

static void qglfResolveGetRenderbufferParameteriv(GLenum target, GLenum pname, GLint* params)
{
    typedef void (QGLF_APIENTRYP type_glGetRenderbufferParameteriv)(GLenum target, GLenum pname, GLint* params);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->getRenderbufferParameteriv = (type_glGetRenderbufferParameteriv)
        context->getProcAddress(QLatin1String("glGetRenderbufferParameteriv"));
#ifdef QT_OPENGL_ES
    if (funcs->getRenderbufferParameteriv) {
        funcs->getRenderbufferParameteriv = (type_glGetRenderbufferParameteriv)
            context->getProcAddress(QLatin1String("glGetRenderbufferParameterivOES"));
    }
#endif
    if (funcs->getRenderbufferParameteriv) {
        funcs->getRenderbufferParameteriv = (type_glGetRenderbufferParameteriv)
            context->getProcAddress(QLatin1String("glGetRenderbufferParameterivEXT"));
    }
    if (funcs->getRenderbufferParameteriv) {
        funcs->getRenderbufferParameteriv = (type_glGetRenderbufferParameteriv)
            context->getProcAddress(QLatin1String("glGetRenderbufferParameterivARB"));
    }

    if (funcs->getRenderbufferParameteriv)
        funcs->getRenderbufferParameteriv(target, pname, params);
    else
        funcs->getRenderbufferParameteriv = qglfResolveGetRenderbufferParameteriv;
}

#ifndef QT_OPENGL_ES_1

static void qglfResolveGetShaderiv(GLuint shader, GLenum pname, GLint* params)
{
    typedef void (QGLF_APIENTRYP type_glGetShaderiv)(GLuint shader, GLenum pname, GLint* params);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->getShaderiv = (type_glGetShaderiv)
        context->getProcAddress(QLatin1String("glGetShaderiv"));
    if (funcs->getShaderiv) {
        funcs->getShaderiv = (type_glGetShaderiv)
            context->getProcAddress(QLatin1String("glGetObjectParameterivARB"));
    }

    if (funcs->getShaderiv)
        funcs->getShaderiv(shader, pname, params);
    else
        funcs->getShaderiv = qglfResolveGetShaderiv;
}

static void qglfResolveGetShaderInfoLog(GLuint shader, GLsizei bufsize, GLsizei* length, char* infolog)
{
    typedef void (QGLF_APIENTRYP type_glGetShaderInfoLog)(GLuint shader, GLsizei bufsize, GLsizei* length, char* infolog);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->getShaderInfoLog = (type_glGetShaderInfoLog)
        context->getProcAddress(QLatin1String("glGetShaderInfoLog"));
    if (funcs->getShaderInfoLog) {
        funcs->getShaderInfoLog = (type_glGetShaderInfoLog)
            context->getProcAddress(QLatin1String("glGetInfoLogARB"));
    }

    if (funcs->getShaderInfoLog)
        funcs->getShaderInfoLog(shader, bufsize, length, infolog);
    else
        funcs->getShaderInfoLog = qglfResolveGetShaderInfoLog;
}

static void qglfSpecialGetShaderPrecisionFormat(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision)
{
    Q_UNUSED(shadertype);
    Q_UNUSED(precisiontype);
    range[0] = range[1] = precision[0] = 0;
}

static void qglfResolveGetShaderPrecisionFormat(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision)
{
    typedef void (QGLF_APIENTRYP type_glGetShaderPrecisionFormat)(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->getShaderPrecisionFormat = (type_glGetShaderPrecisionFormat)
        context->getProcAddress(QLatin1String("glGetShaderPrecisionFormat"));
#ifdef QT_OPENGL_ES
    if (funcs->getShaderPrecisionFormat) {
        funcs->getShaderPrecisionFormat = (type_glGetShaderPrecisionFormat)
            context->getProcAddress(QLatin1String("glGetShaderPrecisionFormatOES"));
    }
#endif
    if (funcs->getShaderPrecisionFormat) {
        funcs->getShaderPrecisionFormat = (type_glGetShaderPrecisionFormat)
            context->getProcAddress(QLatin1String("glGetShaderPrecisionFormatEXT"));
    }
    if (funcs->getShaderPrecisionFormat) {
        funcs->getShaderPrecisionFormat = (type_glGetShaderPrecisionFormat)
            context->getProcAddress(QLatin1String("glGetShaderPrecisionFormatARB"));
    }

    if (!funcs->getShaderPrecisionFormat)
        funcs->getShaderPrecisionFormat = qglfSpecialGetShaderPrecisionFormat;

    funcs->getShaderPrecisionFormat(shadertype, precisiontype, range, precision);
}

static void qglfResolveGetShaderSource(GLuint shader, GLsizei bufsize, GLsizei* length, char* source)
{
    typedef void (QGLF_APIENTRYP type_glGetShaderSource)(GLuint shader, GLsizei bufsize, GLsizei* length, char* source);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->getShaderSource = (type_glGetShaderSource)
        context->getProcAddress(QLatin1String("glGetShaderSource"));
    if (funcs->getShaderSource) {
        funcs->getShaderSource = (type_glGetShaderSource)
            context->getProcAddress(QLatin1String("glGetShaderSourceARB"));
    }

    if (funcs->getShaderSource)
        funcs->getShaderSource(shader, bufsize, length, source);
    else
        funcs->getShaderSource = qglfResolveGetShaderSource;
}

static void qglfResolveGetUniformfv(GLuint program, GLint location, GLfloat* params)
{
    typedef void (QGLF_APIENTRYP type_glGetUniformfv)(GLuint program, GLint location, GLfloat* params);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->getUniformfv = (type_glGetUniformfv)
        context->getProcAddress(QLatin1String("glGetUniformfv"));
    if (funcs->getUniformfv) {
        funcs->getUniformfv = (type_glGetUniformfv)
            context->getProcAddress(QLatin1String("glGetUniformfvARB"));
    }

    if (funcs->getUniformfv)
        funcs->getUniformfv(program, location, params);
    else
        funcs->getUniformfv = qglfResolveGetUniformfv;
}

static void qglfResolveGetUniformiv(GLuint program, GLint location, GLint* params)
{
    typedef void (QGLF_APIENTRYP type_glGetUniformiv)(GLuint program, GLint location, GLint* params);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->getUniformiv = (type_glGetUniformiv)
        context->getProcAddress(QLatin1String("glGetUniformiv"));
    if (funcs->getUniformiv) {
        funcs->getUniformiv = (type_glGetUniformiv)
            context->getProcAddress(QLatin1String("glGetUniformivARB"));
    }

    if (funcs->getUniformiv)
        funcs->getUniformiv(program, location, params);
    else
        funcs->getUniformiv = qglfResolveGetUniformiv;
}

static int qglfResolveGetUniformLocation(GLuint program, const char* name)
{
    typedef int (QGLF_APIENTRYP type_glGetUniformLocation)(GLuint program, const char* name);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->getUniformLocation = (type_glGetUniformLocation)
        context->getProcAddress(QLatin1String("glGetUniformLocation"));
    if (funcs->getUniformLocation) {
        funcs->getUniformLocation = (type_glGetUniformLocation)
            context->getProcAddress(QLatin1String("glGetUniformLocationARB"));
    }

    if (funcs->getUniformLocation)
        return funcs->getUniformLocation(program, name);
    funcs->getUniformLocation = qglfResolveGetUniformLocation;
    return int(0);
}

static void qglfResolveGetVertexAttribfv(GLuint index, GLenum pname, GLfloat* params)
{
    typedef void (QGLF_APIENTRYP type_glGetVertexAttribfv)(GLuint index, GLenum pname, GLfloat* params);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->getVertexAttribfv = (type_glGetVertexAttribfv)
        context->getProcAddress(QLatin1String("glGetVertexAttribfv"));
    if (funcs->getVertexAttribfv) {
        funcs->getVertexAttribfv = (type_glGetVertexAttribfv)
            context->getProcAddress(QLatin1String("glGetVertexAttribfvARB"));
    }

    if (funcs->getVertexAttribfv)
        funcs->getVertexAttribfv(index, pname, params);
    else
        funcs->getVertexAttribfv = qglfResolveGetVertexAttribfv;
}

static void qglfResolveGetVertexAttribiv(GLuint index, GLenum pname, GLint* params)
{
    typedef void (QGLF_APIENTRYP type_glGetVertexAttribiv)(GLuint index, GLenum pname, GLint* params);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->getVertexAttribiv = (type_glGetVertexAttribiv)
        context->getProcAddress(QLatin1String("glGetVertexAttribiv"));
    if (funcs->getVertexAttribiv) {
        funcs->getVertexAttribiv = (type_glGetVertexAttribiv)
            context->getProcAddress(QLatin1String("glGetVertexAttribivARB"));
    }

    if (funcs->getVertexAttribiv)
        funcs->getVertexAttribiv(index, pname, params);
    else
        funcs->getVertexAttribiv = qglfResolveGetVertexAttribiv;
}

static void qglfResolveGetVertexAttribPointerv(GLuint index, GLenum pname, void** pointer)
{
    typedef void (QGLF_APIENTRYP type_glGetVertexAttribPointerv)(GLuint index, GLenum pname, void** pointer);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->getVertexAttribPointerv = (type_glGetVertexAttribPointerv)
        context->getProcAddress(QLatin1String("glGetVertexAttribPointerv"));
    if (funcs->getVertexAttribPointerv) {
        funcs->getVertexAttribPointerv = (type_glGetVertexAttribPointerv)
            context->getProcAddress(QLatin1String("glGetVertexAttribPointervARB"));
    }

    if (funcs->getVertexAttribPointerv)
        funcs->getVertexAttribPointerv(index, pname, pointer);
    else
        funcs->getVertexAttribPointerv = qglfResolveGetVertexAttribPointerv;
}

static GLboolean qglfResolveIsBuffer(GLuint buffer)
{
    typedef GLboolean (QGLF_APIENTRYP type_glIsBuffer)(GLuint buffer);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->isBuffer = (type_glIsBuffer)
        context->getProcAddress(QLatin1String("glIsBuffer"));
#ifdef QT_OPENGL_ES
    if (funcs->isBuffer) {
        funcs->isBuffer = (type_glIsBuffer)
            context->getProcAddress(QLatin1String("glIsBufferOES"));
    }
#endif
    if (funcs->isBuffer) {
        funcs->isBuffer = (type_glIsBuffer)
            context->getProcAddress(QLatin1String("glIsBufferEXT"));
    }
    if (funcs->isBuffer) {
        funcs->isBuffer = (type_glIsBuffer)
            context->getProcAddress(QLatin1String("glIsBufferARB"));
    }

    if (funcs->isBuffer)
        return funcs->isBuffer(buffer);
    funcs->isBuffer = qglfResolveIsBuffer;
    return GLboolean(0);
}

#endif

static GLboolean qglfResolveIsFramebuffer(GLuint framebuffer)
{
    typedef GLboolean (QGLF_APIENTRYP type_glIsFramebuffer)(GLuint framebuffer);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->isFramebuffer = (type_glIsFramebuffer)
        context->getProcAddress(QLatin1String("glIsFramebuffer"));
#ifdef QT_OPENGL_ES
    if (funcs->isFramebuffer) {
        funcs->isFramebuffer = (type_glIsFramebuffer)
            context->getProcAddress(QLatin1String("glIsFramebufferOES"));
    }
#endif
    if (funcs->isFramebuffer) {
        funcs->isFramebuffer = (type_glIsFramebuffer)
            context->getProcAddress(QLatin1String("glIsFramebufferEXT"));
    }
    if (funcs->isFramebuffer) {
        funcs->isFramebuffer = (type_glIsFramebuffer)
            context->getProcAddress(QLatin1String("glIsFramebufferARB"));
    }

    if (funcs->isFramebuffer)
        return funcs->isFramebuffer(framebuffer);
    funcs->isFramebuffer = qglfResolveIsFramebuffer;
    return GLboolean(0);
}

#ifndef QT_OPENGL_ES_1

static GLboolean qglfSpecialIsProgram(GLuint program)
{
    return program != 0;
}

static GLboolean qglfResolveIsProgram(GLuint program)
{
    typedef GLboolean (QGLF_APIENTRYP type_glIsProgram)(GLuint program);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->isProgram = (type_glIsProgram)
        context->getProcAddress(QLatin1String("glIsProgram"));
    if (funcs->isProgram) {
        funcs->isProgram = (type_glIsProgram)
            context->getProcAddress(QLatin1String("glIsProgramARB"));
    }

    if (!funcs->isProgram)
        funcs->isProgram = qglfSpecialIsProgram;

    return funcs->isProgram(program);
}

#endif

static GLboolean qglfResolveIsRenderbuffer(GLuint renderbuffer)
{
    typedef GLboolean (QGLF_APIENTRYP type_glIsRenderbuffer)(GLuint renderbuffer);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->isRenderbuffer = (type_glIsRenderbuffer)
        context->getProcAddress(QLatin1String("glIsRenderbuffer"));
#ifdef QT_OPENGL_ES
    if (funcs->isRenderbuffer) {
        funcs->isRenderbuffer = (type_glIsRenderbuffer)
            context->getProcAddress(QLatin1String("glIsRenderbufferOES"));
    }
#endif
    if (funcs->isRenderbuffer) {
        funcs->isRenderbuffer = (type_glIsRenderbuffer)
            context->getProcAddress(QLatin1String("glIsRenderbufferEXT"));
    }
    if (funcs->isRenderbuffer) {
        funcs->isRenderbuffer = (type_glIsRenderbuffer)
            context->getProcAddress(QLatin1String("glIsRenderbufferARB"));
    }

    if (funcs->isRenderbuffer)
        return funcs->isRenderbuffer(renderbuffer);
    funcs->isRenderbuffer = qglfResolveIsRenderbuffer;
    return GLboolean(0);
}

#ifndef QT_OPENGL_ES_1

static GLboolean qglfSpecialIsShader(GLuint shader)
{
    return shader != 0;
}

static GLboolean qglfResolveIsShader(GLuint shader)
{
    typedef GLboolean (QGLF_APIENTRYP type_glIsShader)(GLuint shader);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->isShader = (type_glIsShader)
        context->getProcAddress(QLatin1String("glIsShader"));
    if (funcs->isShader) {
        funcs->isShader = (type_glIsShader)
            context->getProcAddress(QLatin1String("glIsShaderARB"));
    }

    if (!funcs->isShader)
        funcs->isShader = qglfSpecialIsShader;

    return funcs->isShader(shader);
}

static void qglfResolveLinkProgram(GLuint program)
{
    typedef void (QGLF_APIENTRYP type_glLinkProgram)(GLuint program);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->linkProgram = (type_glLinkProgram)
        context->getProcAddress(QLatin1String("glLinkProgram"));
    if (funcs->linkProgram) {
        funcs->linkProgram = (type_glLinkProgram)
            context->getProcAddress(QLatin1String("glLinkProgramARB"));
    }

    if (funcs->linkProgram)
        funcs->linkProgram(program);
    else
        funcs->linkProgram = qglfResolveLinkProgram;
}

static void qglfSpecialReleaseShaderCompiler()
{
}

static void qglfResolveReleaseShaderCompiler()
{
    typedef void (QGLF_APIENTRYP type_glReleaseShaderCompiler)();

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->releaseShaderCompiler = (type_glReleaseShaderCompiler)
        context->getProcAddress(QLatin1String("glReleaseShaderCompiler"));
    if (funcs->releaseShaderCompiler) {
        funcs->releaseShaderCompiler = (type_glReleaseShaderCompiler)
            context->getProcAddress(QLatin1String("glReleaseShaderCompilerARB"));
    }

    if (!funcs->releaseShaderCompiler)
        funcs->releaseShaderCompiler = qglfSpecialReleaseShaderCompiler;

    funcs->releaseShaderCompiler();
}

#endif

static void qglfResolveRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
    typedef void (QGLF_APIENTRYP type_glRenderbufferStorage)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->renderbufferStorage = (type_glRenderbufferStorage)
        context->getProcAddress(QLatin1String("glRenderbufferStorage"));
#ifdef QT_OPENGL_ES
    if (funcs->renderbufferStorage) {
        funcs->renderbufferStorage = (type_glRenderbufferStorage)
            context->getProcAddress(QLatin1String("glRenderbufferStorageOES"));
    }
#endif
    if (funcs->renderbufferStorage) {
        funcs->renderbufferStorage = (type_glRenderbufferStorage)
            context->getProcAddress(QLatin1String("glRenderbufferStorageEXT"));
    }
    if (funcs->renderbufferStorage) {
        funcs->renderbufferStorage = (type_glRenderbufferStorage)
            context->getProcAddress(QLatin1String("glRenderbufferStorageARB"));
    }

    if (funcs->renderbufferStorage)
        funcs->renderbufferStorage(target, internalformat, width, height);
    else
        funcs->renderbufferStorage = qglfResolveRenderbufferStorage;
}

#ifndef QT_OPENGL_ES_1

static void qglfResolveSampleCoverage(GLclampf value, GLboolean invert)
{
    typedef void (QGLF_APIENTRYP type_glSampleCoverage)(GLclampf value, GLboolean invert);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->sampleCoverage = (type_glSampleCoverage)
        context->getProcAddress(QLatin1String("glSampleCoverage"));
#ifdef QT_OPENGL_ES
    if (funcs->sampleCoverage) {
        funcs->sampleCoverage = (type_glSampleCoverage)
            context->getProcAddress(QLatin1String("glSampleCoverageOES"));
    }
#endif
    if (funcs->sampleCoverage) {
        funcs->sampleCoverage = (type_glSampleCoverage)
            context->getProcAddress(QLatin1String("glSampleCoverageEXT"));
    }
    if (funcs->sampleCoverage) {
        funcs->sampleCoverage = (type_glSampleCoverage)
            context->getProcAddress(QLatin1String("glSampleCoverageARB"));
    }

    if (funcs->sampleCoverage)
        funcs->sampleCoverage(value, invert);
    else
        funcs->sampleCoverage = qglfResolveSampleCoverage;
}

static void qglfSpecialShaderBinary(GLint n, const GLuint* shaders, GLenum binaryformat, const void* binary, GLint length)
{
    Q_UNUSED(n);
    Q_UNUSED(shaders);
    Q_UNUSED(binaryformat);
    Q_UNUSED(binary);
    Q_UNUSED(length);
}

static void qglfResolveShaderBinary(GLint n, const GLuint* shaders, GLenum binaryformat, const void* binary, GLint length)
{
    typedef void (QGLF_APIENTRYP type_glShaderBinary)(GLint n, const GLuint* shaders, GLenum binaryformat, const void* binary, GLint length);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->shaderBinary = (type_glShaderBinary)
        context->getProcAddress(QLatin1String("glShaderBinary"));
    if (funcs->shaderBinary) {
        funcs->shaderBinary = (type_glShaderBinary)
            context->getProcAddress(QLatin1String("glShaderBinaryARB"));
    }

    if (!funcs->shaderBinary)
        funcs->shaderBinary = qglfSpecialShaderBinary;

    funcs->shaderBinary(n, shaders, binaryformat, binary, length);
}

static void qglfResolveShaderSource(GLuint shader, GLsizei count, const char** string, const GLint* length)
{
    typedef void (QGLF_APIENTRYP type_glShaderSource)(GLuint shader, GLsizei count, const char** string, const GLint* length);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->shaderSource = (type_glShaderSource)
        context->getProcAddress(QLatin1String("glShaderSource"));
    if (funcs->shaderSource) {
        funcs->shaderSource = (type_glShaderSource)
            context->getProcAddress(QLatin1String("glShaderSourceARB"));
    }

    if (funcs->shaderSource)
        funcs->shaderSource(shader, count, string, length);
    else
        funcs->shaderSource = qglfResolveShaderSource;
}

#endif

static void qglfSpecialStencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask)
{
    Q_UNUSED(face);
    ::glStencilFunc(func, ref, mask);
}

static void qglfResolveStencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask)
{
    typedef void (QGLF_APIENTRYP type_glStencilFuncSeparate)(GLenum face, GLenum func, GLint ref, GLuint mask);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->stencilFuncSeparate = (type_glStencilFuncSeparate)
        context->getProcAddress(QLatin1String("glStencilFuncSeparate"));
#ifdef QT_OPENGL_ES
    if (funcs->stencilFuncSeparate) {
        funcs->stencilFuncSeparate = (type_glStencilFuncSeparate)
            context->getProcAddress(QLatin1String("glStencilFuncSeparateOES"));
    }
#endif
    if (funcs->stencilFuncSeparate) {
        funcs->stencilFuncSeparate = (type_glStencilFuncSeparate)
            context->getProcAddress(QLatin1String("glStencilFuncSeparateEXT"));
    }
    if (funcs->stencilFuncSeparate) {
        funcs->stencilFuncSeparate = (type_glStencilFuncSeparate)
            context->getProcAddress(QLatin1String("glStencilFuncSeparateARB"));
    }

    if (!funcs->stencilFuncSeparate)
        funcs->stencilFuncSeparate = qglfSpecialStencilFuncSeparate;

    funcs->stencilFuncSeparate(face, func, ref, mask);
}

static void qglfSpecialStencilMaskSeparate(GLenum face, GLuint mask)
{
    Q_UNUSED(face);
    ::glStencilMask(mask);
}

static void qglfResolveStencilMaskSeparate(GLenum face, GLuint mask)
{
    typedef void (QGLF_APIENTRYP type_glStencilMaskSeparate)(GLenum face, GLuint mask);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->stencilMaskSeparate = (type_glStencilMaskSeparate)
        context->getProcAddress(QLatin1String("glStencilMaskSeparate"));
#ifdef QT_OPENGL_ES
    if (funcs->stencilMaskSeparate) {
        funcs->stencilMaskSeparate = (type_glStencilMaskSeparate)
            context->getProcAddress(QLatin1String("glStencilMaskSeparateOES"));
    }
#endif
    if (funcs->stencilMaskSeparate) {
        funcs->stencilMaskSeparate = (type_glStencilMaskSeparate)
            context->getProcAddress(QLatin1String("glStencilMaskSeparateEXT"));
    }
    if (funcs->stencilMaskSeparate) {
        funcs->stencilMaskSeparate = (type_glStencilMaskSeparate)
            context->getProcAddress(QLatin1String("glStencilMaskSeparateARB"));
    }

    if (!funcs->stencilMaskSeparate)
        funcs->stencilMaskSeparate = qglfSpecialStencilMaskSeparate;

    funcs->stencilMaskSeparate(face, mask);
}

static void qglfSpecialStencilOpSeparate(GLenum face, GLenum fail, GLenum zfail, GLenum zpass)
{
    Q_UNUSED(face);
    ::glStencilOp(fail, zfail, zpass);
}

static void qglfResolveStencilOpSeparate(GLenum face, GLenum fail, GLenum zfail, GLenum zpass)
{
    typedef void (QGLF_APIENTRYP type_glStencilOpSeparate)(GLenum face, GLenum fail, GLenum zfail, GLenum zpass);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->stencilOpSeparate = (type_glStencilOpSeparate)
        context->getProcAddress(QLatin1String("glStencilOpSeparate"));
#ifdef QT_OPENGL_ES
    if (funcs->stencilOpSeparate) {
        funcs->stencilOpSeparate = (type_glStencilOpSeparate)
            context->getProcAddress(QLatin1String("glStencilOpSeparateOES"));
    }
#endif
    if (funcs->stencilOpSeparate) {
        funcs->stencilOpSeparate = (type_glStencilOpSeparate)
            context->getProcAddress(QLatin1String("glStencilOpSeparateEXT"));
    }
    if (funcs->stencilOpSeparate) {
        funcs->stencilOpSeparate = (type_glStencilOpSeparate)
            context->getProcAddress(QLatin1String("glStencilOpSeparateARB"));
    }

    if (!funcs->stencilOpSeparate)
        funcs->stencilOpSeparate = qglfSpecialStencilOpSeparate;

    funcs->stencilOpSeparate(face, fail, zfail, zpass);
}

#ifndef QT_OPENGL_ES_1

static void qglfResolveUniform1f(GLint location, GLfloat x)
{
    typedef void (QGLF_APIENTRYP type_glUniform1f)(GLint location, GLfloat x);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->uniform1f = (type_glUniform1f)
        context->getProcAddress(QLatin1String("glUniform1f"));
    if (funcs->uniform1f) {
        funcs->uniform1f = (type_glUniform1f)
            context->getProcAddress(QLatin1String("glUniform1fARB"));
    }

    if (funcs->uniform1f)
        funcs->uniform1f(location, x);
    else
        funcs->uniform1f = qglfResolveUniform1f;
}

static void qglfResolveUniform1fv(GLint location, GLsizei count, const GLfloat* v)
{
    typedef void (QGLF_APIENTRYP type_glUniform1fv)(GLint location, GLsizei count, const GLfloat* v);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->uniform1fv = (type_glUniform1fv)
        context->getProcAddress(QLatin1String("glUniform1fv"));
    if (funcs->uniform1fv) {
        funcs->uniform1fv = (type_glUniform1fv)
            context->getProcAddress(QLatin1String("glUniform1fvARB"));
    }

    if (funcs->uniform1fv)
        funcs->uniform1fv(location, count, v);
    else
        funcs->uniform1fv = qglfResolveUniform1fv;
}

static void qglfResolveUniform1i(GLint location, GLint x)
{
    typedef void (QGLF_APIENTRYP type_glUniform1i)(GLint location, GLint x);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->uniform1i = (type_glUniform1i)
        context->getProcAddress(QLatin1String("glUniform1i"));
    if (funcs->uniform1i) {
        funcs->uniform1i = (type_glUniform1i)
            context->getProcAddress(QLatin1String("glUniform1iARB"));
    }

    if (funcs->uniform1i)
        funcs->uniform1i(location, x);
    else
        funcs->uniform1i = qglfResolveUniform1i;
}

static void qglfResolveUniform1iv(GLint location, GLsizei count, const GLint* v)
{
    typedef void (QGLF_APIENTRYP type_glUniform1iv)(GLint location, GLsizei count, const GLint* v);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->uniform1iv = (type_glUniform1iv)
        context->getProcAddress(QLatin1String("glUniform1iv"));
    if (funcs->uniform1iv) {
        funcs->uniform1iv = (type_glUniform1iv)
            context->getProcAddress(QLatin1String("glUniform1ivARB"));
    }

    if (funcs->uniform1iv)
        funcs->uniform1iv(location, count, v);
    else
        funcs->uniform1iv = qglfResolveUniform1iv;
}

static void qglfResolveUniform2f(GLint location, GLfloat x, GLfloat y)
{
    typedef void (QGLF_APIENTRYP type_glUniform2f)(GLint location, GLfloat x, GLfloat y);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->uniform2f = (type_glUniform2f)
        context->getProcAddress(QLatin1String("glUniform2f"));
    if (funcs->uniform2f) {
        funcs->uniform2f = (type_glUniform2f)
            context->getProcAddress(QLatin1String("glUniform2fARB"));
    }

    if (funcs->uniform2f)
        funcs->uniform2f(location, x, y);
    else
        funcs->uniform2f = qglfResolveUniform2f;
}

static void qglfResolveUniform2fv(GLint location, GLsizei count, const GLfloat* v)
{
    typedef void (QGLF_APIENTRYP type_glUniform2fv)(GLint location, GLsizei count, const GLfloat* v);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->uniform2fv = (type_glUniform2fv)
        context->getProcAddress(QLatin1String("glUniform2fv"));
    if (funcs->uniform2fv) {
        funcs->uniform2fv = (type_glUniform2fv)
            context->getProcAddress(QLatin1String("glUniform2fvARB"));
    }

    if (funcs->uniform2fv)
        funcs->uniform2fv(location, count, v);
    else
        funcs->uniform2fv = qglfResolveUniform2fv;
}

static void qglfResolveUniform2i(GLint location, GLint x, GLint y)
{
    typedef void (QGLF_APIENTRYP type_glUniform2i)(GLint location, GLint x, GLint y);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->uniform2i = (type_glUniform2i)
        context->getProcAddress(QLatin1String("glUniform2i"));
    if (funcs->uniform2i) {
        funcs->uniform2i = (type_glUniform2i)
            context->getProcAddress(QLatin1String("glUniform2iARB"));
    }

    if (funcs->uniform2i)
        funcs->uniform2i(location, x, y);
    else
        funcs->uniform2i = qglfResolveUniform2i;
}

static void qglfResolveUniform2iv(GLint location, GLsizei count, const GLint* v)
{
    typedef void (QGLF_APIENTRYP type_glUniform2iv)(GLint location, GLsizei count, const GLint* v);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->uniform2iv = (type_glUniform2iv)
        context->getProcAddress(QLatin1String("glUniform2iv"));
    if (funcs->uniform2iv) {
        funcs->uniform2iv = (type_glUniform2iv)
            context->getProcAddress(QLatin1String("glUniform2ivARB"));
    }

    if (funcs->uniform2iv)
        funcs->uniform2iv(location, count, v);
    else
        funcs->uniform2iv = qglfResolveUniform2iv;
}

static void qglfResolveUniform3f(GLint location, GLfloat x, GLfloat y, GLfloat z)
{
    typedef void (QGLF_APIENTRYP type_glUniform3f)(GLint location, GLfloat x, GLfloat y, GLfloat z);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->uniform3f = (type_glUniform3f)
        context->getProcAddress(QLatin1String("glUniform3f"));
    if (funcs->uniform3f) {
        funcs->uniform3f = (type_glUniform3f)
            context->getProcAddress(QLatin1String("glUniform3fARB"));
    }

    if (funcs->uniform3f)
        funcs->uniform3f(location, x, y, z);
    else
        funcs->uniform3f = qglfResolveUniform3f;
}

static void qglfResolveUniform3fv(GLint location, GLsizei count, const GLfloat* v)
{
    typedef void (QGLF_APIENTRYP type_glUniform3fv)(GLint location, GLsizei count, const GLfloat* v);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->uniform3fv = (type_glUniform3fv)
        context->getProcAddress(QLatin1String("glUniform3fv"));
    if (funcs->uniform3fv) {
        funcs->uniform3fv = (type_glUniform3fv)
            context->getProcAddress(QLatin1String("glUniform3fvARB"));
    }

    if (funcs->uniform3fv)
        funcs->uniform3fv(location, count, v);
    else
        funcs->uniform3fv = qglfResolveUniform3fv;
}

static void qglfResolveUniform3i(GLint location, GLint x, GLint y, GLint z)
{
    typedef void (QGLF_APIENTRYP type_glUniform3i)(GLint location, GLint x, GLint y, GLint z);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->uniform3i = (type_glUniform3i)
        context->getProcAddress(QLatin1String("glUniform3i"));
    if (funcs->uniform3i) {
        funcs->uniform3i = (type_glUniform3i)
            context->getProcAddress(QLatin1String("glUniform3iARB"));
    }

    if (funcs->uniform3i)
        funcs->uniform3i(location, x, y, z);
    else
        funcs->uniform3i = qglfResolveUniform3i;
}

static void qglfResolveUniform3iv(GLint location, GLsizei count, const GLint* v)
{
    typedef void (QGLF_APIENTRYP type_glUniform3iv)(GLint location, GLsizei count, const GLint* v);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->uniform3iv = (type_glUniform3iv)
        context->getProcAddress(QLatin1String("glUniform3iv"));
    if (funcs->uniform3iv) {
        funcs->uniform3iv = (type_glUniform3iv)
            context->getProcAddress(QLatin1String("glUniform3ivARB"));
    }

    if (funcs->uniform3iv)
        funcs->uniform3iv(location, count, v);
    else
        funcs->uniform3iv = qglfResolveUniform3iv;
}

static void qglfResolveUniform4f(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    typedef void (QGLF_APIENTRYP type_glUniform4f)(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->uniform4f = (type_glUniform4f)
        context->getProcAddress(QLatin1String("glUniform4f"));
    if (funcs->uniform4f) {
        funcs->uniform4f = (type_glUniform4f)
            context->getProcAddress(QLatin1String("glUniform4fARB"));
    }

    if (funcs->uniform4f)
        funcs->uniform4f(location, x, y, z, w);
    else
        funcs->uniform4f = qglfResolveUniform4f;
}

static void qglfResolveUniform4fv(GLint location, GLsizei count, const GLfloat* v)
{
    typedef void (QGLF_APIENTRYP type_glUniform4fv)(GLint location, GLsizei count, const GLfloat* v);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->uniform4fv = (type_glUniform4fv)
        context->getProcAddress(QLatin1String("glUniform4fv"));
    if (funcs->uniform4fv) {
        funcs->uniform4fv = (type_glUniform4fv)
            context->getProcAddress(QLatin1String("glUniform4fvARB"));
    }

    if (funcs->uniform4fv)
        funcs->uniform4fv(location, count, v);
    else
        funcs->uniform4fv = qglfResolveUniform4fv;
}

static void qglfResolveUniform4i(GLint location, GLint x, GLint y, GLint z, GLint w)
{
    typedef void (QGLF_APIENTRYP type_glUniform4i)(GLint location, GLint x, GLint y, GLint z, GLint w);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->uniform4i = (type_glUniform4i)
        context->getProcAddress(QLatin1String("glUniform4i"));
    if (funcs->uniform4i) {
        funcs->uniform4i = (type_glUniform4i)
            context->getProcAddress(QLatin1String("glUniform4iARB"));
    }

    if (funcs->uniform4i)
        funcs->uniform4i(location, x, y, z, w);
    else
        funcs->uniform4i = qglfResolveUniform4i;
}

static void qglfResolveUniform4iv(GLint location, GLsizei count, const GLint* v)
{
    typedef void (QGLF_APIENTRYP type_glUniform4iv)(GLint location, GLsizei count, const GLint* v);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->uniform4iv = (type_glUniform4iv)
        context->getProcAddress(QLatin1String("glUniform4iv"));
    if (funcs->uniform4iv) {
        funcs->uniform4iv = (type_glUniform4iv)
            context->getProcAddress(QLatin1String("glUniform4ivARB"));
    }

    if (funcs->uniform4iv)
        funcs->uniform4iv(location, count, v);
    else
        funcs->uniform4iv = qglfResolveUniform4iv;
}

static void qglfResolveUniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
    typedef void (QGLF_APIENTRYP type_glUniformMatrix2fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->uniformMatrix2fv = (type_glUniformMatrix2fv)
        context->getProcAddress(QLatin1String("glUniformMatrix2fv"));
    if (funcs->uniformMatrix2fv) {
        funcs->uniformMatrix2fv = (type_glUniformMatrix2fv)
            context->getProcAddress(QLatin1String("glUniformMatrix2fvARB"));
    }

    if (funcs->uniformMatrix2fv)
        funcs->uniformMatrix2fv(location, count, transpose, value);
    else
        funcs->uniformMatrix2fv = qglfResolveUniformMatrix2fv;
}

static void qglfResolveUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
    typedef void (QGLF_APIENTRYP type_glUniformMatrix3fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->uniformMatrix3fv = (type_glUniformMatrix3fv)
        context->getProcAddress(QLatin1String("glUniformMatrix3fv"));
    if (funcs->uniformMatrix3fv) {
        funcs->uniformMatrix3fv = (type_glUniformMatrix3fv)
            context->getProcAddress(QLatin1String("glUniformMatrix3fvARB"));
    }

    if (funcs->uniformMatrix3fv)
        funcs->uniformMatrix3fv(location, count, transpose, value);
    else
        funcs->uniformMatrix3fv = qglfResolveUniformMatrix3fv;
}

static void qglfResolveUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
    typedef void (QGLF_APIENTRYP type_glUniformMatrix4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->uniformMatrix4fv = (type_glUniformMatrix4fv)
        context->getProcAddress(QLatin1String("glUniformMatrix4fv"));
    if (funcs->uniformMatrix4fv) {
        funcs->uniformMatrix4fv = (type_glUniformMatrix4fv)
            context->getProcAddress(QLatin1String("glUniformMatrix4fvARB"));
    }

    if (funcs->uniformMatrix4fv)
        funcs->uniformMatrix4fv(location, count, transpose, value);
    else
        funcs->uniformMatrix4fv = qglfResolveUniformMatrix4fv;
}

static void qglfResolveUseProgram(GLuint program)
{
    typedef void (QGLF_APIENTRYP type_glUseProgram)(GLuint program);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->useProgram = (type_glUseProgram)
        context->getProcAddress(QLatin1String("glUseProgram"));
    if (funcs->useProgram) {
        funcs->useProgram = (type_glUseProgram)
            context->getProcAddress(QLatin1String("glUseProgramObjectARB"));
    }

    if (funcs->useProgram)
        funcs->useProgram(program);
    else
        funcs->useProgram = qglfResolveUseProgram;
}

static void qglfResolveValidateProgram(GLuint program)
{
    typedef void (QGLF_APIENTRYP type_glValidateProgram)(GLuint program);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->validateProgram = (type_glValidateProgram)
        context->getProcAddress(QLatin1String("glValidateProgram"));
    if (funcs->validateProgram) {
        funcs->validateProgram = (type_glValidateProgram)
            context->getProcAddress(QLatin1String("glValidateProgramARB"));
    }

    if (funcs->validateProgram)
        funcs->validateProgram(program);
    else
        funcs->validateProgram = qglfResolveValidateProgram;
}

static void qglfResolveVertexAttrib1f(GLuint indx, GLfloat x)
{
    typedef void (QGLF_APIENTRYP type_glVertexAttrib1f)(GLuint indx, GLfloat x);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->vertexAttrib1f = (type_glVertexAttrib1f)
        context->getProcAddress(QLatin1String("glVertexAttrib1f"));
    if (funcs->vertexAttrib1f) {
        funcs->vertexAttrib1f = (type_glVertexAttrib1f)
            context->getProcAddress(QLatin1String("glVertexAttrib1fARB"));
    }

    if (funcs->vertexAttrib1f)
        funcs->vertexAttrib1f(indx, x);
    else
        funcs->vertexAttrib1f = qglfResolveVertexAttrib1f;
}

static void qglfResolveVertexAttrib1fv(GLuint indx, const GLfloat* values)
{
    typedef void (QGLF_APIENTRYP type_glVertexAttrib1fv)(GLuint indx, const GLfloat* values);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->vertexAttrib1fv = (type_glVertexAttrib1fv)
        context->getProcAddress(QLatin1String("glVertexAttrib1fv"));
    if (funcs->vertexAttrib1fv) {
        funcs->vertexAttrib1fv = (type_glVertexAttrib1fv)
            context->getProcAddress(QLatin1String("glVertexAttrib1fvARB"));
    }

    if (funcs->vertexAttrib1fv)
        funcs->vertexAttrib1fv(indx, values);
    else
        funcs->vertexAttrib1fv = qglfResolveVertexAttrib1fv;
}

static void qglfResolveVertexAttrib2f(GLuint indx, GLfloat x, GLfloat y)
{
    typedef void (QGLF_APIENTRYP type_glVertexAttrib2f)(GLuint indx, GLfloat x, GLfloat y);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->vertexAttrib2f = (type_glVertexAttrib2f)
        context->getProcAddress(QLatin1String("glVertexAttrib2f"));
    if (funcs->vertexAttrib2f) {
        funcs->vertexAttrib2f = (type_glVertexAttrib2f)
            context->getProcAddress(QLatin1String("glVertexAttrib2fARB"));
    }

    if (funcs->vertexAttrib2f)
        funcs->vertexAttrib2f(indx, x, y);
    else
        funcs->vertexAttrib2f = qglfResolveVertexAttrib2f;
}

static void qglfResolveVertexAttrib2fv(GLuint indx, const GLfloat* values)
{
    typedef void (QGLF_APIENTRYP type_glVertexAttrib2fv)(GLuint indx, const GLfloat* values);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->vertexAttrib2fv = (type_glVertexAttrib2fv)
        context->getProcAddress(QLatin1String("glVertexAttrib2fv"));
    if (funcs->vertexAttrib2fv) {
        funcs->vertexAttrib2fv = (type_glVertexAttrib2fv)
            context->getProcAddress(QLatin1String("glVertexAttrib2fvARB"));
    }

    if (funcs->vertexAttrib2fv)
        funcs->vertexAttrib2fv(indx, values);
    else
        funcs->vertexAttrib2fv = qglfResolveVertexAttrib2fv;
}

static void qglfResolveVertexAttrib3f(GLuint indx, GLfloat x, GLfloat y, GLfloat z)
{
    typedef void (QGLF_APIENTRYP type_glVertexAttrib3f)(GLuint indx, GLfloat x, GLfloat y, GLfloat z);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->vertexAttrib3f = (type_glVertexAttrib3f)
        context->getProcAddress(QLatin1String("glVertexAttrib3f"));
    if (funcs->vertexAttrib3f) {
        funcs->vertexAttrib3f = (type_glVertexAttrib3f)
            context->getProcAddress(QLatin1String("glVertexAttrib3fARB"));
    }

    if (funcs->vertexAttrib3f)
        funcs->vertexAttrib3f(indx, x, y, z);
    else
        funcs->vertexAttrib3f = qglfResolveVertexAttrib3f;
}

static void qglfResolveVertexAttrib3fv(GLuint indx, const GLfloat* values)
{
    typedef void (QGLF_APIENTRYP type_glVertexAttrib3fv)(GLuint indx, const GLfloat* values);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->vertexAttrib3fv = (type_glVertexAttrib3fv)
        context->getProcAddress(QLatin1String("glVertexAttrib3fv"));
    if (funcs->vertexAttrib3fv) {
        funcs->vertexAttrib3fv = (type_glVertexAttrib3fv)
            context->getProcAddress(QLatin1String("glVertexAttrib3fvARB"));
    }

    if (funcs->vertexAttrib3fv)
        funcs->vertexAttrib3fv(indx, values);
    else
        funcs->vertexAttrib3fv = qglfResolveVertexAttrib3fv;
}

static void qglfResolveVertexAttrib4f(GLuint indx, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    typedef void (QGLF_APIENTRYP type_glVertexAttrib4f)(GLuint indx, GLfloat x, GLfloat y, GLfloat z, GLfloat w);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->vertexAttrib4f = (type_glVertexAttrib4f)
        context->getProcAddress(QLatin1String("glVertexAttrib4f"));
    if (funcs->vertexAttrib4f) {
        funcs->vertexAttrib4f = (type_glVertexAttrib4f)
            context->getProcAddress(QLatin1String("glVertexAttrib4fARB"));
    }

    if (funcs->vertexAttrib4f)
        funcs->vertexAttrib4f(indx, x, y, z, w);
    else
        funcs->vertexAttrib4f = qglfResolveVertexAttrib4f;
}

static void qglfResolveVertexAttrib4fv(GLuint indx, const GLfloat* values)
{
    typedef void (QGLF_APIENTRYP type_glVertexAttrib4fv)(GLuint indx, const GLfloat* values);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->vertexAttrib4fv = (type_glVertexAttrib4fv)
        context->getProcAddress(QLatin1String("glVertexAttrib4fv"));
    if (funcs->vertexAttrib4fv) {
        funcs->vertexAttrib4fv = (type_glVertexAttrib4fv)
            context->getProcAddress(QLatin1String("glVertexAttrib4fvARB"));
    }

    if (funcs->vertexAttrib4fv)
        funcs->vertexAttrib4fv(indx, values);
    else
        funcs->vertexAttrib4fv = qglfResolveVertexAttrib4fv;
}

static void qglfResolveVertexAttribPointer(GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* ptr)
{
    typedef void (QGLF_APIENTRYP type_glVertexAttribPointer)(GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* ptr);

    const QGLContext *context = QGLContext::currentContext();
    QGLFunctionsPrivate *funcs = qt_gl_functions(context);

    funcs->vertexAttribPointer = (type_glVertexAttribPointer)
        context->getProcAddress(QLatin1String("glVertexAttribPointer"));
    if (funcs->vertexAttribPointer) {
        funcs->vertexAttribPointer = (type_glVertexAttribPointer)
            context->getProcAddress(QLatin1String("glVertexAttribPointerARB"));
    }

    if (funcs->vertexAttribPointer)
        funcs->vertexAttribPointer(indx, size, type, normalized, stride, ptr);
    else
        funcs->vertexAttribPointer = qglfResolveVertexAttribPointer;
}

#endif
#endif // !QT_OPENGL_ES_2

QGLFunctionsPrivate::QGLFunctionsPrivate(const QGLContext *)
{
#ifndef QT_OPENGL_ES_2
#ifndef QT_OPENGL_ES_1
    activeTexture = qglfResolveActiveTexture;
    attachShader = qglfResolveAttachShader;
    bindAttribLocation = qglfResolveBindAttribLocation;
    bindBuffer = qglfResolveBindBuffer;
#endif
    bindFramebuffer = qglfResolveBindFramebuffer;
    bindRenderbuffer = qglfResolveBindRenderbuffer;
    blendColor = qglfResolveBlendColor;
    blendEquation = qglfResolveBlendEquation;
    blendEquationSeparate = qglfResolveBlendEquationSeparate;
    blendFuncSeparate = qglfResolveBlendFuncSeparate;
#ifndef QT_OPENGL_ES_1
    bufferData = qglfResolveBufferData;
    bufferSubData = qglfResolveBufferSubData;
#endif
    checkFramebufferStatus = qglfResolveCheckFramebufferStatus;
#ifndef QT_OPENGL_ES_1
    compileShader = qglfResolveCompileShader;
    compressedTexImage2D = qglfResolveCompressedTexImage2D;
    compressedTexSubImage2D = qglfResolveCompressedTexSubImage2D;
    createProgram = qglfResolveCreateProgram;
    createShader = qglfResolveCreateShader;
    deleteBuffers = qglfResolveDeleteBuffers;
#endif
    deleteFramebuffers = qglfResolveDeleteFramebuffers;
#ifndef QT_OPENGL_ES_1
    deleteProgram = qglfResolveDeleteProgram;
#endif
    deleteRenderbuffers = qglfResolveDeleteRenderbuffers;
#ifndef QT_OPENGL_ES_1
    deleteShader = qglfResolveDeleteShader;
    detachShader = qglfResolveDetachShader;
    disableVertexAttribArray = qglfResolveDisableVertexAttribArray;
    enableVertexAttribArray = qglfResolveEnableVertexAttribArray;
#endif
    framebufferRenderbuffer = qglfResolveFramebufferRenderbuffer;
    framebufferTexture2D = qglfResolveFramebufferTexture2D;
#ifndef QT_OPENGL_ES_1
    genBuffers = qglfResolveGenBuffers;
#endif
    generateMipmap = qglfResolveGenerateMipmap;
    genFramebuffers = qglfResolveGenFramebuffers;
    genRenderbuffers = qglfResolveGenRenderbuffers;
#ifndef QT_OPENGL_ES_1
    getActiveAttrib = qglfResolveGetActiveAttrib;
    getActiveUniform = qglfResolveGetActiveUniform;
    getAttachedShaders = qglfResolveGetAttachedShaders;
    getAttribLocation = qglfResolveGetAttribLocation;
#endif
    getBufferParameteriv = qglfResolveGetBufferParameteriv;
    getFramebufferAttachmentParameteriv = qglfResolveGetFramebufferAttachmentParameteriv;
#ifndef QT_OPENGL_ES_1
    getProgramiv = qglfResolveGetProgramiv;
    getProgramInfoLog = qglfResolveGetProgramInfoLog;
#endif
    getRenderbufferParameteriv = qglfResolveGetRenderbufferParameteriv;
#ifndef QT_OPENGL_ES_1
    getShaderiv = qglfResolveGetShaderiv;
    getShaderInfoLog = qglfResolveGetShaderInfoLog;
    getShaderPrecisionFormat = qglfResolveGetShaderPrecisionFormat;
    getShaderSource = qglfResolveGetShaderSource;
    getUniformfv = qglfResolveGetUniformfv;
    getUniformiv = qglfResolveGetUniformiv;
    getUniformLocation = qglfResolveGetUniformLocation;
    getVertexAttribfv = qglfResolveGetVertexAttribfv;
    getVertexAttribiv = qglfResolveGetVertexAttribiv;
    getVertexAttribPointerv = qglfResolveGetVertexAttribPointerv;
    isBuffer = qglfResolveIsBuffer;
#endif
    isFramebuffer = qglfResolveIsFramebuffer;
#ifndef QT_OPENGL_ES_1
    isProgram = qglfResolveIsProgram;
#endif
    isRenderbuffer = qglfResolveIsRenderbuffer;
#ifndef QT_OPENGL_ES_1
    isShader = qglfResolveIsShader;
    linkProgram = qglfResolveLinkProgram;
    releaseShaderCompiler = qglfResolveReleaseShaderCompiler;
#endif
    renderbufferStorage = qglfResolveRenderbufferStorage;
#ifndef QT_OPENGL_ES_1
    sampleCoverage = qglfResolveSampleCoverage;
    shaderBinary = qglfResolveShaderBinary;
    shaderSource = qglfResolveShaderSource;
#endif
    stencilFuncSeparate = qglfResolveStencilFuncSeparate;
    stencilMaskSeparate = qglfResolveStencilMaskSeparate;
    stencilOpSeparate = qglfResolveStencilOpSeparate;
#ifndef QT_OPENGL_ES_1
    uniform1f = qglfResolveUniform1f;
    uniform1fv = qglfResolveUniform1fv;
    uniform1i = qglfResolveUniform1i;
    uniform1iv = qglfResolveUniform1iv;
    uniform2f = qglfResolveUniform2f;
    uniform2fv = qglfResolveUniform2fv;
    uniform2i = qglfResolveUniform2i;
    uniform2iv = qglfResolveUniform2iv;
    uniform3f = qglfResolveUniform3f;
    uniform3fv = qglfResolveUniform3fv;
    uniform3i = qglfResolveUniform3i;
    uniform3iv = qglfResolveUniform3iv;
    uniform4f = qglfResolveUniform4f;
    uniform4fv = qglfResolveUniform4fv;
    uniform4i = qglfResolveUniform4i;
    uniform4iv = qglfResolveUniform4iv;
    uniformMatrix2fv = qglfResolveUniformMatrix2fv;
    uniformMatrix3fv = qglfResolveUniformMatrix3fv;
    uniformMatrix4fv = qglfResolveUniformMatrix4fv;
    useProgram = qglfResolveUseProgram;
    validateProgram = qglfResolveValidateProgram;
    vertexAttrib1f = qglfResolveVertexAttrib1f;
    vertexAttrib1fv = qglfResolveVertexAttrib1fv;
    vertexAttrib2f = qglfResolveVertexAttrib2f;
    vertexAttrib2fv = qglfResolveVertexAttrib2fv;
    vertexAttrib3f = qglfResolveVertexAttrib3f;
    vertexAttrib3fv = qglfResolveVertexAttrib3fv;
    vertexAttrib4f = qglfResolveVertexAttrib4f;
    vertexAttrib4fv = qglfResolveVertexAttrib4fv;
    vertexAttribPointer = qglfResolveVertexAttribPointer;
#endif
#endif // !QT_OPENGL_ES_2
}

QT_END_NAMESPACE
