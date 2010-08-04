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

#ifndef QGLFUNCTIONS_H
#define QGLFUNCTIONS_H

#include <QtOpenGL/qgl.h>
#include "qt3dglobal.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

// Types that aren't defined in all system's gl.h files.
typedef ptrdiff_t qgl_GLintptr;
typedef ptrdiff_t qgl_GLsizeiptr;

#ifndef Q_WS_MAC
# ifndef QGLF_APIENTRYP
#   ifdef QGLF_APIENTRY
#     define QGLF_APIENTRYP QGLF_APIENTRY *
#   else
#     define QGLF_APIENTRY
#     define QGLF_APIENTRYP *
#   endif
# endif
#else
# define QGLF_APIENTRY
# define QGLF_APIENTRYP *
#endif

struct QGLFunctionsPrivate;

// Undefine any macros from GLEW, qglextensions_p.h, etc that
// may interfere with the definition of QGLFunctions.
#undef glActiveTexture
#undef glAttachShader
#undef glBindAttribLocation
#undef glBindBuffer
#undef glBindFramebuffer
#undef glBindRenderbuffer
#undef glBlendColor
#undef glBlendEquation
#undef glBlendEquationSeparate
#undef glBlendFuncSeparate
#undef glBufferData
#undef glBufferSubData
#undef glCheckFramebufferStatus
#undef glClearDepthf
#undef glCompileShader
#undef glCompressedTexImage2D
#undef glCompressedTexSubImage2D
#undef glCreateProgram
#undef glCreateShader
#undef glDeleteBuffers
#undef glDeleteFramebuffers
#undef glDeleteProgram
#undef glDeleteRenderbuffers
#undef glDeleteShader
#undef glDepthRangef
#undef glDetachShader
#undef glDisableVertexAttribArray
#undef glEnableVertexAttribArray
#undef glFramebufferRenderbuffer
#undef glFramebufferTexture2D
#undef glGenBuffers
#undef glGenerateMipmap
#undef glGenFramebuffers
#undef glGenRenderbuffers
#undef glGetActiveAttrib
#undef glGetActiveUniform
#undef glGetAttachedShaders
#undef glGetAttribLocation
#undef glGetBufferParameteriv
#undef glGetFramebufferAttachmentParameteriv
#undef glGetProgramiv
#undef glGetProgramInfoLog
#undef glGetRenderbufferParameteriv
#undef glGetShaderiv
#undef glGetShaderInfoLog
#undef glGetShaderPrecisionFormat
#undef glGetShaderSource
#undef glGetUniformfv
#undef glGetUniformiv
#undef glGetUniformLocation
#undef glGetVertexAttribfv
#undef glGetVertexAttribiv
#undef glGetVertexAttribPointerv
#undef glIsBuffer
#undef glIsFramebuffer
#undef glIsProgram
#undef glIsRenderbuffer
#undef glIsShader
#undef glLinkProgram
#undef glReleaseShaderCompiler
#undef glRenderbufferStorage
#undef glSampleCoverage
#undef glShaderBinary
#undef glShaderSource
#undef glStencilFuncSeparate
#undef glStencilMaskSeparate
#undef glStencilOpSeparate
#undef glUniform1f
#undef glUniform1fv
#undef glUniform1i
#undef glUniform1iv
#undef glUniform2f
#undef glUniform2fv
#undef glUniform2i
#undef glUniform2iv
#undef glUniform3f
#undef glUniform3fv
#undef glUniform3i
#undef glUniform3iv
#undef glUniform4f
#undef glUniform4fv
#undef glUniform4i
#undef glUniform4iv
#undef glUniformMatrix2fv
#undef glUniformMatrix3fv
#undef glUniformMatrix4fv
#undef glUseProgram
#undef glValidateProgram
#undef glVertexAttrib1f
#undef glVertexAttrib1fv
#undef glVertexAttrib2f
#undef glVertexAttrib2fv
#undef glVertexAttrib3f
#undef glVertexAttrib3fv
#undef glVertexAttrib4f
#undef glVertexAttrib4fv
#undef glVertexAttribPointer

class Q_QT3D_EXPORT QGLFunctions
{
public:
    QGLFunctions();
    explicit QGLFunctions(const QGLContext *context);
    ~QGLFunctions() {}

    void initializeGLFunctions(const QGLContext *context = 0);

    void glActiveTexture(GLenum texture);
    void glAttachShader(GLuint program, GLuint shader);
    void glBindAttribLocation(GLuint program, GLuint index, const char* name);
    void glBindBuffer(GLenum target, GLuint buffer);
    void glBindFramebuffer(GLenum target, GLuint framebuffer);
    void glBindRenderbuffer(GLenum target, GLuint renderbuffer);
    void glBlendColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
    void glBlendEquation(GLenum mode);
    void glBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha);
    void glBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
    void glBufferData(GLenum target, qgl_GLsizeiptr size, const void* data, GLenum usage);
    void glBufferSubData(GLenum target, qgl_GLintptr offset, qgl_GLsizeiptr size, const void* data);
    GLenum glCheckFramebufferStatus(GLenum target);
    void glClearDepthf(GLclampf depth);
    void glCompileShader(GLuint shader);
    void glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data);
    void glCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data);
    GLuint glCreateProgram();
    GLuint glCreateShader(GLenum type);
    void glDeleteBuffers(GLsizei n, const GLuint* buffers);
    void glDeleteFramebuffers(GLsizei n, const GLuint* framebuffers);
    void glDeleteProgram(GLuint program);
    void glDeleteRenderbuffers(GLsizei n, const GLuint* renderbuffers);
    void glDeleteShader(GLuint shader);
    void glDepthRangef(GLclampf zNear, GLclampf zFar);
    void glDetachShader(GLuint program, GLuint shader);
    void glDisableVertexAttribArray(GLuint index);
    void glEnableVertexAttribArray(GLuint index);
    void glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
    void glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
    void glGenBuffers(GLsizei n, GLuint* buffers);
    void glGenerateMipmap(GLenum target);
    void glGenFramebuffers(GLsizei n, GLuint* framebuffers);
    void glGenRenderbuffers(GLsizei n, GLuint* renderbuffers);
    void glGetActiveAttrib(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name);
    void glGetActiveUniform(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name);
    void glGetAttachedShaders(GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders);
    int glGetAttribLocation(GLuint program, const char* name);
    void glGetBufferParameteriv(GLenum target, GLenum pname, GLint* params);
    void glGetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint* params);
    void glGetProgramiv(GLuint program, GLenum pname, GLint* params);
    void glGetProgramInfoLog(GLuint program, GLsizei bufsize, GLsizei* length, char* infolog);
    void glGetRenderbufferParameteriv(GLenum target, GLenum pname, GLint* params);
    void glGetShaderiv(GLuint shader, GLenum pname, GLint* params);
    void glGetShaderInfoLog(GLuint shader, GLsizei bufsize, GLsizei* length, char* infolog);
    void glGetShaderPrecisionFormat(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision);
    void glGetShaderSource(GLuint shader, GLsizei bufsize, GLsizei* length, char* source);
    void glGetUniformfv(GLuint program, GLint location, GLfloat* params);
    void glGetUniformiv(GLuint program, GLint location, GLint* params);
    int glGetUniformLocation(GLuint program, const char* name);
    void glGetVertexAttribfv(GLuint index, GLenum pname, GLfloat* params);
    void glGetVertexAttribiv(GLuint index, GLenum pname, GLint* params);
    void glGetVertexAttribPointerv(GLuint index, GLenum pname, void** pointer);
    GLboolean glIsBuffer(GLuint buffer);
    GLboolean glIsFramebuffer(GLuint framebuffer);
    GLboolean glIsProgram(GLuint program);
    GLboolean glIsRenderbuffer(GLuint renderbuffer);
    GLboolean glIsShader(GLuint shader);
    void glLinkProgram(GLuint program);
    void glReleaseShaderCompiler();
    void glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
    void glSampleCoverage(GLclampf value, GLboolean invert);
    void glShaderBinary(GLint n, const GLuint* shaders, GLenum binaryformat, const void* binary, GLint length);
    void glShaderSource(GLuint shader, GLsizei count, const char** string, const GLint* length);
    void glStencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask);
    void glStencilMaskSeparate(GLenum face, GLuint mask);
    void glStencilOpSeparate(GLenum face, GLenum fail, GLenum zfail, GLenum zpass);
    void glUniform1f(GLint location, GLfloat x);
    void glUniform1fv(GLint location, GLsizei count, const GLfloat* v);
    void glUniform1i(GLint location, GLint x);
    void glUniform1iv(GLint location, GLsizei count, const GLint* v);
    void glUniform2f(GLint location, GLfloat x, GLfloat y);
    void glUniform2fv(GLint location, GLsizei count, const GLfloat* v);
    void glUniform2i(GLint location, GLint x, GLint y);
    void glUniform2iv(GLint location, GLsizei count, const GLint* v);
    void glUniform3f(GLint location, GLfloat x, GLfloat y, GLfloat z);
    void glUniform3fv(GLint location, GLsizei count, const GLfloat* v);
    void glUniform3i(GLint location, GLint x, GLint y, GLint z);
    void glUniform3iv(GLint location, GLsizei count, const GLint* v);
    void glUniform4f(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
    void glUniform4fv(GLint location, GLsizei count, const GLfloat* v);
    void glUniform4i(GLint location, GLint x, GLint y, GLint z, GLint w);
    void glUniform4iv(GLint location, GLsizei count, const GLint* v);
    void glUniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
    void glUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
    void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
    void glUseProgram(GLuint program);
    void glValidateProgram(GLuint program);
    void glVertexAttrib1f(GLuint indx, GLfloat x);
    void glVertexAttrib1fv(GLuint indx, const GLfloat* values);
    void glVertexAttrib2f(GLuint indx, GLfloat x, GLfloat y);
    void glVertexAttrib2fv(GLuint indx, const GLfloat* values);
    void glVertexAttrib3f(GLuint indx, GLfloat x, GLfloat y, GLfloat z);
    void glVertexAttrib3fv(GLuint indx, const GLfloat* values);
    void glVertexAttrib4f(GLuint indx, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
    void glVertexAttrib4fv(GLuint indx, const GLfloat* values);
    void glVertexAttribPointer(GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* ptr);

private:
    QGLFunctionsPrivate *d_ptr;
    static bool isInitialized(const QGLFunctionsPrivate *d) { return d != 0; }
};

struct QGLFunctionsPrivate
{
    QGLFunctionsPrivate(const QGLContext *context = 0);

#ifndef QT_OPENGL_ES_2
    void (QGLF_APIENTRYP activeTexture)(GLenum texture);
    void (QGLF_APIENTRYP attachShader)(GLuint program, GLuint shader);
    void (QGLF_APIENTRYP bindAttribLocation)(GLuint program, GLuint index, const char* name);
    void (QGLF_APIENTRYP bindBuffer)(GLenum target, GLuint buffer);
    void (QGLF_APIENTRYP bindFramebuffer)(GLenum target, GLuint framebuffer);
    void (QGLF_APIENTRYP bindRenderbuffer)(GLenum target, GLuint renderbuffer);
    void (QGLF_APIENTRYP blendColor)(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
    void (QGLF_APIENTRYP blendEquation)(GLenum mode);
    void (QGLF_APIENTRYP blendEquationSeparate)(GLenum modeRGB, GLenum modeAlpha);
    void (QGLF_APIENTRYP blendFuncSeparate)(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
    void (QGLF_APIENTRYP bufferData)(GLenum target, qgl_GLsizeiptr size, const void* data, GLenum usage);
    void (QGLF_APIENTRYP bufferSubData)(GLenum target, qgl_GLintptr offset, qgl_GLsizeiptr size, const void* data);
    GLenum (QGLF_APIENTRYP checkFramebufferStatus)(GLenum target);
    void (QGLF_APIENTRYP compileShader)(GLuint shader);
    void (QGLF_APIENTRYP compressedTexImage2D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data);
    void (QGLF_APIENTRYP compressedTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data);
    GLuint (QGLF_APIENTRYP createProgram)();
    GLuint (QGLF_APIENTRYP createShader)(GLenum type);
    void (QGLF_APIENTRYP deleteBuffers)(GLsizei n, const GLuint* buffers);
    void (QGLF_APIENTRYP deleteFramebuffers)(GLsizei n, const GLuint* framebuffers);
    void (QGLF_APIENTRYP deleteProgram)(GLuint program);
    void (QGLF_APIENTRYP deleteRenderbuffers)(GLsizei n, const GLuint* renderbuffers);
    void (QGLF_APIENTRYP deleteShader)(GLuint shader);
    void (QGLF_APIENTRYP detachShader)(GLuint program, GLuint shader);
    void (QGLF_APIENTRYP disableVertexAttribArray)(GLuint index);
    void (QGLF_APIENTRYP enableVertexAttribArray)(GLuint index);
    void (QGLF_APIENTRYP framebufferRenderbuffer)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
    void (QGLF_APIENTRYP framebufferTexture2D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
    void (QGLF_APIENTRYP genBuffers)(GLsizei n, GLuint* buffers);
    void (QGLF_APIENTRYP generateMipmap)(GLenum target);
    void (QGLF_APIENTRYP genFramebuffers)(GLsizei n, GLuint* framebuffers);
    void (QGLF_APIENTRYP genRenderbuffers)(GLsizei n, GLuint* renderbuffers);
    void (QGLF_APIENTRYP getActiveAttrib)(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name);
    void (QGLF_APIENTRYP getActiveUniform)(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name);
    void (QGLF_APIENTRYP getAttachedShaders)(GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders);
    int (QGLF_APIENTRYP getAttribLocation)(GLuint program, const char* name);
    void (QGLF_APIENTRYP getBufferParameteriv)(GLenum target, GLenum pname, GLint* params);
    void (QGLF_APIENTRYP getFramebufferAttachmentParameteriv)(GLenum target, GLenum attachment, GLenum pname, GLint* params);
    void (QGLF_APIENTRYP getProgramiv)(GLuint program, GLenum pname, GLint* params);
    void (QGLF_APIENTRYP getProgramInfoLog)(GLuint program, GLsizei bufsize, GLsizei* length, char* infolog);
    void (QGLF_APIENTRYP getRenderbufferParameteriv)(GLenum target, GLenum pname, GLint* params);
    void (QGLF_APIENTRYP getShaderiv)(GLuint shader, GLenum pname, GLint* params);
    void (QGLF_APIENTRYP getShaderInfoLog)(GLuint shader, GLsizei bufsize, GLsizei* length, char* infolog);
    void (QGLF_APIENTRYP getShaderPrecisionFormat)(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision);
    void (QGLF_APIENTRYP getShaderSource)(GLuint shader, GLsizei bufsize, GLsizei* length, char* source);
    void (QGLF_APIENTRYP getUniformfv)(GLuint program, GLint location, GLfloat* params);
    void (QGLF_APIENTRYP getUniformiv)(GLuint program, GLint location, GLint* params);
    int (QGLF_APIENTRYP getUniformLocation)(GLuint program, const char* name);
    void (QGLF_APIENTRYP getVertexAttribfv)(GLuint index, GLenum pname, GLfloat* params);
    void (QGLF_APIENTRYP getVertexAttribiv)(GLuint index, GLenum pname, GLint* params);
    void (QGLF_APIENTRYP getVertexAttribPointerv)(GLuint index, GLenum pname, void** pointer);
    GLboolean (QGLF_APIENTRYP isBuffer)(GLuint buffer);
    GLboolean (QGLF_APIENTRYP isFramebuffer)(GLuint framebuffer);
    GLboolean (QGLF_APIENTRYP isProgram)(GLuint program);
    GLboolean (QGLF_APIENTRYP isRenderbuffer)(GLuint renderbuffer);
    GLboolean (QGLF_APIENTRYP isShader)(GLuint shader);
    void (QGLF_APIENTRYP linkProgram)(GLuint program);
    void (QGLF_APIENTRYP releaseShaderCompiler)();
    void (QGLF_APIENTRYP renderbufferStorage)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
    void (QGLF_APIENTRYP sampleCoverage)(GLclampf value, GLboolean invert);
    void (QGLF_APIENTRYP shaderBinary)(GLint n, const GLuint* shaders, GLenum binaryformat, const void* binary, GLint length);
    void (QGLF_APIENTRYP shaderSource)(GLuint shader, GLsizei count, const char** string, const GLint* length);
    void (QGLF_APIENTRYP stencilFuncSeparate)(GLenum face, GLenum func, GLint ref, GLuint mask);
    void (QGLF_APIENTRYP stencilMaskSeparate)(GLenum face, GLuint mask);
    void (QGLF_APIENTRYP stencilOpSeparate)(GLenum face, GLenum fail, GLenum zfail, GLenum zpass);
    void (QGLF_APIENTRYP uniform1f)(GLint location, GLfloat x);
    void (QGLF_APIENTRYP uniform1fv)(GLint location, GLsizei count, const GLfloat* v);
    void (QGLF_APIENTRYP uniform1i)(GLint location, GLint x);
    void (QGLF_APIENTRYP uniform1iv)(GLint location, GLsizei count, const GLint* v);
    void (QGLF_APIENTRYP uniform2f)(GLint location, GLfloat x, GLfloat y);
    void (QGLF_APIENTRYP uniform2fv)(GLint location, GLsizei count, const GLfloat* v);
    void (QGLF_APIENTRYP uniform2i)(GLint location, GLint x, GLint y);
    void (QGLF_APIENTRYP uniform2iv)(GLint location, GLsizei count, const GLint* v);
    void (QGLF_APIENTRYP uniform3f)(GLint location, GLfloat x, GLfloat y, GLfloat z);
    void (QGLF_APIENTRYP uniform3fv)(GLint location, GLsizei count, const GLfloat* v);
    void (QGLF_APIENTRYP uniform3i)(GLint location, GLint x, GLint y, GLint z);
    void (QGLF_APIENTRYP uniform3iv)(GLint location, GLsizei count, const GLint* v);
    void (QGLF_APIENTRYP uniform4f)(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
    void (QGLF_APIENTRYP uniform4fv)(GLint location, GLsizei count, const GLfloat* v);
    void (QGLF_APIENTRYP uniform4i)(GLint location, GLint x, GLint y, GLint z, GLint w);
    void (QGLF_APIENTRYP uniform4iv)(GLint location, GLsizei count, const GLint* v);
    void (QGLF_APIENTRYP uniformMatrix2fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
    void (QGLF_APIENTRYP uniformMatrix3fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
    void (QGLF_APIENTRYP uniformMatrix4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
    void (QGLF_APIENTRYP useProgram)(GLuint program);
    void (QGLF_APIENTRYP validateProgram)(GLuint program);
    void (QGLF_APIENTRYP vertexAttrib1f)(GLuint indx, GLfloat x);
    void (QGLF_APIENTRYP vertexAttrib1fv)(GLuint indx, const GLfloat* values);
    void (QGLF_APIENTRYP vertexAttrib2f)(GLuint indx, GLfloat x, GLfloat y);
    void (QGLF_APIENTRYP vertexAttrib2fv)(GLuint indx, const GLfloat* values);
    void (QGLF_APIENTRYP vertexAttrib3f)(GLuint indx, GLfloat x, GLfloat y, GLfloat z);
    void (QGLF_APIENTRYP vertexAttrib3fv)(GLuint indx, const GLfloat* values);
    void (QGLF_APIENTRYP vertexAttrib4f)(GLuint indx, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
    void (QGLF_APIENTRYP vertexAttrib4fv)(GLuint indx, const GLfloat* values);
    void (QGLF_APIENTRYP vertexAttribPointer)(GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* ptr);
#endif
};

inline void QGLFunctions::glActiveTexture(GLenum texture)
{
#if defined(QT_OPENGL_ES_1) || defined(QT_OPENGL_ES_2)
    ::glActiveTexture(texture);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->activeTexture(texture);
#endif
}

inline void QGLFunctions::glAttachShader(GLuint program, GLuint shader)
{
#if defined(QT_OPENGL_ES_2)
    ::glAttachShader(program, shader);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->attachShader(program, shader);
#endif
}

inline void QGLFunctions::glBindAttribLocation(GLuint program, GLuint index, const char* name)
{
#if defined(QT_OPENGL_ES_2)
    ::glBindAttribLocation(program, index, name);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->bindAttribLocation(program, index, name);
#endif
}

inline void QGLFunctions::glBindBuffer(GLenum target, GLuint buffer)
{
#if defined(QT_OPENGL_ES_1) || defined(QT_OPENGL_ES_2)
    ::glBindBuffer(target, buffer);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->bindBuffer(target, buffer);
#endif
}

inline void QGLFunctions::glBindFramebuffer(GLenum target, GLuint framebuffer)
{
#if defined(QT_OPENGL_ES_2)
    ::glBindFramebuffer(target, framebuffer);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->bindFramebuffer(target, framebuffer);
#endif
}

inline void QGLFunctions::glBindRenderbuffer(GLenum target, GLuint renderbuffer)
{
#if defined(QT_OPENGL_ES_2)
    ::glBindRenderbuffer(target, renderbuffer);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->bindRenderbuffer(target, renderbuffer);
#endif
}

inline void QGLFunctions::glBlendColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
#if defined(QT_OPENGL_ES_2)
    ::glBlendColor(red, green, blue, alpha);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->blendColor(red, green, blue, alpha);
#endif
}

inline void QGLFunctions::glBlendEquation(GLenum mode)
{
#if defined(QT_OPENGL_ES_2)
    ::glBlendEquation(mode);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->blendEquation(mode);
#endif
}

inline void QGLFunctions::glBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha)
{
#if defined(QT_OPENGL_ES_2)
    ::glBlendEquationSeparate(modeRGB, modeAlpha);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->blendEquationSeparate(modeRGB, modeAlpha);
#endif
}

inline void QGLFunctions::glBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
{
#if defined(QT_OPENGL_ES_2)
    ::glBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->blendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
#endif
}

inline void QGLFunctions::glBufferData(GLenum target, qgl_GLsizeiptr size, const void* data, GLenum usage)
{
#if defined(QT_OPENGL_ES_1) || defined(QT_OPENGL_ES_2)
    ::glBufferData(target, size, data, usage);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->bufferData(target, size, data, usage);
#endif
}

inline void QGLFunctions::glBufferSubData(GLenum target, qgl_GLintptr offset, qgl_GLsizeiptr size, const void* data)
{
#if defined(QT_OPENGL_ES_1) || defined(QT_OPENGL_ES_2)
    ::glBufferSubData(target, offset, size, data);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->bufferSubData(target, offset, size, data);
#endif
}

inline GLenum QGLFunctions::glCheckFramebufferStatus(GLenum target)
{
#if defined(QT_OPENGL_ES_2)
    return ::glCheckFramebufferStatus(target);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    return d_ptr->checkFramebufferStatus(target);
#endif
}

inline void QGLFunctions::glClearDepthf(GLclampf depth)
{
#ifndef QT_OPENGL_ES
    ::glClearDepth(depth);
#else
    ::glClearDepthf(depth);
#endif
}

inline void QGLFunctions::glCompileShader(GLuint shader)
{
#if defined(QT_OPENGL_ES_2)
    ::glCompileShader(shader);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->compileShader(shader);
#endif
}

inline void QGLFunctions::glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data)
{
#if defined(QT_OPENGL_ES_1) || defined(QT_OPENGL_ES_2)
    ::glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->compressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
#endif
}

inline void QGLFunctions::glCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data)
{
#if defined(QT_OPENGL_ES_1) || defined(QT_OPENGL_ES_2)
    ::glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->compressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data);
#endif
}

inline GLuint QGLFunctions::glCreateProgram()
{
#if defined(QT_OPENGL_ES_2)
    return ::glCreateProgram();
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    return d_ptr->createProgram();
#endif
}

inline GLuint QGLFunctions::glCreateShader(GLenum type)
{
#if defined(QT_OPENGL_ES_2)
    return ::glCreateShader(type);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    return d_ptr->createShader(type);
#endif
}

inline void QGLFunctions::glDeleteBuffers(GLsizei n, const GLuint* buffers)
{
#if defined(QT_OPENGL_ES_1) || defined(QT_OPENGL_ES_2)
    ::glDeleteBuffers(n, buffers);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->deleteBuffers(n, buffers);
#endif
}

inline void QGLFunctions::glDeleteFramebuffers(GLsizei n, const GLuint* framebuffers)
{
#if defined(QT_OPENGL_ES_2)
    ::glDeleteFramebuffers(n, framebuffers);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->deleteFramebuffers(n, framebuffers);
#endif
}

inline void QGLFunctions::glDeleteProgram(GLuint program)
{
#if defined(QT_OPENGL_ES_2)
    ::glDeleteProgram(program);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->deleteProgram(program);
#endif
}

inline void QGLFunctions::glDeleteRenderbuffers(GLsizei n, const GLuint* renderbuffers)
{
#if defined(QT_OPENGL_ES_2)
    ::glDeleteRenderbuffers(n, renderbuffers);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->deleteRenderbuffers(n, renderbuffers);
#endif
}

inline void QGLFunctions::glDeleteShader(GLuint shader)
{
#if defined(QT_OPENGL_ES_2)
    ::glDeleteShader(shader);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->deleteShader(shader);
#endif
}

inline void QGLFunctions::glDepthRangef(GLclampf zNear, GLclampf zFar)
{
#ifndef QT_OPENGL_ES
    ::glDepthRange(zNear, zFar);
#else
    ::glDepthRangef(zNear, zFar);
#endif
}

inline void QGLFunctions::glDetachShader(GLuint program, GLuint shader)
{
#if defined(QT_OPENGL_ES_2)
    ::glDetachShader(program, shader);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->detachShader(program, shader);
#endif
}

inline void QGLFunctions::glDisableVertexAttribArray(GLuint index)
{
#if defined(QT_OPENGL_ES_2)
    ::glDisableVertexAttribArray(index);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->disableVertexAttribArray(index);
#endif
}

inline void QGLFunctions::glEnableVertexAttribArray(GLuint index)
{
#if defined(QT_OPENGL_ES_2)
    ::glEnableVertexAttribArray(index);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->enableVertexAttribArray(index);
#endif
}

inline void QGLFunctions::glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
#if defined(QT_OPENGL_ES_2)
    ::glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->framebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
#endif
}

inline void QGLFunctions::glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
#if defined(QT_OPENGL_ES_2)
    ::glFramebufferTexture2D(target, attachment, textarget, texture, level);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->framebufferTexture2D(target, attachment, textarget, texture, level);
#endif
}

inline void QGLFunctions::glGenBuffers(GLsizei n, GLuint* buffers)
{
#if defined(QT_OPENGL_ES_1) || defined(QT_OPENGL_ES_2)
    ::glGenBuffers(n, buffers);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->genBuffers(n, buffers);
#endif
}

inline void QGLFunctions::glGenerateMipmap(GLenum target)
{
#if defined(QT_OPENGL_ES_2)
    ::glGenerateMipmap(target);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->generateMipmap(target);
#endif
}

inline void QGLFunctions::glGenFramebuffers(GLsizei n, GLuint* framebuffers)
{
#if defined(QT_OPENGL_ES_2)
    ::glGenFramebuffers(n, framebuffers);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->genFramebuffers(n, framebuffers);
#endif
}

inline void QGLFunctions::glGenRenderbuffers(GLsizei n, GLuint* renderbuffers)
{
#if defined(QT_OPENGL_ES_2)
    ::glGenRenderbuffers(n, renderbuffers);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->genRenderbuffers(n, renderbuffers);
#endif
}

inline void QGLFunctions::glGetActiveAttrib(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name)
{
#if defined(QT_OPENGL_ES_2)
    ::glGetActiveAttrib(program, index, bufsize, length, size, type, name);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->getActiveAttrib(program, index, bufsize, length, size, type, name);
#endif
}

inline void QGLFunctions::glGetActiveUniform(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name)
{
#if defined(QT_OPENGL_ES_2)
    ::glGetActiveUniform(program, index, bufsize, length, size, type, name);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->getActiveUniform(program, index, bufsize, length, size, type, name);
#endif
}

inline void QGLFunctions::glGetAttachedShaders(GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders)
{
#if defined(QT_OPENGL_ES_2)
    ::glGetAttachedShaders(program, maxcount, count, shaders);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->getAttachedShaders(program, maxcount, count, shaders);
#endif
}

inline int QGLFunctions::glGetAttribLocation(GLuint program, const char* name)
{
#if defined(QT_OPENGL_ES_2)
    return ::glGetAttribLocation(program, name);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    return d_ptr->getAttribLocation(program, name);
#endif
}

inline void QGLFunctions::glGetBufferParameteriv(GLenum target, GLenum pname, GLint* params)
{
#if defined(QT_OPENGL_ES_2)
    ::glGetBufferParameteriv(target, pname, params);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->getBufferParameteriv(target, pname, params);
#endif
}

inline void QGLFunctions::glGetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint* params)
{
#if defined(QT_OPENGL_ES_2)
    ::glGetFramebufferAttachmentParameteriv(target, attachment, pname, params);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->getFramebufferAttachmentParameteriv(target, attachment, pname, params);
#endif
}

inline void QGLFunctions::glGetProgramiv(GLuint program, GLenum pname, GLint* params)
{
#if defined(QT_OPENGL_ES_2)
    ::glGetProgramiv(program, pname, params);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->getProgramiv(program, pname, params);
#endif
}

inline void QGLFunctions::glGetProgramInfoLog(GLuint program, GLsizei bufsize, GLsizei* length, char* infolog)
{
#if defined(QT_OPENGL_ES_2)
    ::glGetProgramInfoLog(program, bufsize, length, infolog);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->getProgramInfoLog(program, bufsize, length, infolog);
#endif
}

inline void QGLFunctions::glGetRenderbufferParameteriv(GLenum target, GLenum pname, GLint* params)
{
#if defined(QT_OPENGL_ES_2)
    ::glGetRenderbufferParameteriv(target, pname, params);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->getRenderbufferParameteriv(target, pname, params);
#endif
}

inline void QGLFunctions::glGetShaderiv(GLuint shader, GLenum pname, GLint* params)
{
#if defined(QT_OPENGL_ES_2)
    ::glGetShaderiv(shader, pname, params);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->getShaderiv(shader, pname, params);
#endif
}

inline void QGLFunctions::glGetShaderInfoLog(GLuint shader, GLsizei bufsize, GLsizei* length, char* infolog)
{
#if defined(QT_OPENGL_ES_2)
    ::glGetShaderInfoLog(shader, bufsize, length, infolog);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->getShaderInfoLog(shader, bufsize, length, infolog);
#endif
}

inline void QGLFunctions::glGetShaderPrecisionFormat(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision)
{
#if defined(QT_OPENGL_ES_2)
    ::glGetShaderPrecisionFormat(shadertype, precisiontype, range, precision);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->getShaderPrecisionFormat(shadertype, precisiontype, range, precision);
#endif
}

inline void QGLFunctions::glGetShaderSource(GLuint shader, GLsizei bufsize, GLsizei* length, char* source)
{
#if defined(QT_OPENGL_ES_2)
    ::glGetShaderSource(shader, bufsize, length, source);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->getShaderSource(shader, bufsize, length, source);
#endif
}

inline void QGLFunctions::glGetUniformfv(GLuint program, GLint location, GLfloat* params)
{
#if defined(QT_OPENGL_ES_2)
    ::glGetUniformfv(program, location, params);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->getUniformfv(program, location, params);
#endif
}

inline void QGLFunctions::glGetUniformiv(GLuint program, GLint location, GLint* params)
{
#if defined(QT_OPENGL_ES_2)
    ::glGetUniformiv(program, location, params);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->getUniformiv(program, location, params);
#endif
}

inline int QGLFunctions::glGetUniformLocation(GLuint program, const char* name)
{
#if defined(QT_OPENGL_ES_2)
    return ::glGetUniformLocation(program, name);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    return d_ptr->getUniformLocation(program, name);
#endif
}

inline void QGLFunctions::glGetVertexAttribfv(GLuint index, GLenum pname, GLfloat* params)
{
#if defined(QT_OPENGL_ES_2)
    ::glGetVertexAttribfv(index, pname, params);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->getVertexAttribfv(index, pname, params);
#endif
}

inline void QGLFunctions::glGetVertexAttribiv(GLuint index, GLenum pname, GLint* params)
{
#if defined(QT_OPENGL_ES_2)
    ::glGetVertexAttribiv(index, pname, params);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->getVertexAttribiv(index, pname, params);
#endif
}

inline void QGLFunctions::glGetVertexAttribPointerv(GLuint index, GLenum pname, void** pointer)
{
#if defined(QT_OPENGL_ES_2)
    ::glGetVertexAttribPointerv(index, pname, pointer);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->getVertexAttribPointerv(index, pname, pointer);
#endif
}

inline GLboolean QGLFunctions::glIsBuffer(GLuint buffer)
{
#if defined(QT_OPENGL_ES_1) || defined(QT_OPENGL_ES_2)
    return ::glIsBuffer(buffer);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    return d_ptr->isBuffer(buffer);
#endif
}

inline GLboolean QGLFunctions::glIsFramebuffer(GLuint framebuffer)
{
#if defined(QT_OPENGL_ES_2)
    return ::glIsFramebuffer(framebuffer);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    return d_ptr->isFramebuffer(framebuffer);
#endif
}

inline GLboolean QGLFunctions::glIsProgram(GLuint program)
{
#if defined(QT_OPENGL_ES_2)
    return ::glIsProgram(program);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    return d_ptr->isProgram(program);
#endif
}

inline GLboolean QGLFunctions::glIsRenderbuffer(GLuint renderbuffer)
{
#if defined(QT_OPENGL_ES_2)
    return ::glIsRenderbuffer(renderbuffer);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    return d_ptr->isRenderbuffer(renderbuffer);
#endif
}

inline GLboolean QGLFunctions::glIsShader(GLuint shader)
{
#if defined(QT_OPENGL_ES_2)
    return ::glIsShader(shader);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    return d_ptr->isShader(shader);
#endif
}

inline void QGLFunctions::glLinkProgram(GLuint program)
{
#if defined(QT_OPENGL_ES_2)
    ::glLinkProgram(program);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->linkProgram(program);
#endif
}

inline void QGLFunctions::glReleaseShaderCompiler()
{
#if defined(QT_OPENGL_ES_2)
    ::glReleaseShaderCompiler();
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->releaseShaderCompiler();
#endif
}

inline void QGLFunctions::glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
#if defined(QT_OPENGL_ES_2)
    ::glRenderbufferStorage(target, internalformat, width, height);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->renderbufferStorage(target, internalformat, width, height);
#endif
}

inline void QGLFunctions::glSampleCoverage(GLclampf value, GLboolean invert)
{
#if defined(QT_OPENGL_ES_1) || defined(QT_OPENGL_ES_2)
    ::glSampleCoverage(value, invert);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->sampleCoverage(value, invert);
#endif
}

inline void QGLFunctions::glShaderBinary(GLint n, const GLuint* shaders, GLenum binaryformat, const void* binary, GLint length)
{
#if defined(QT_OPENGL_ES_2)
    ::glShaderBinary(n, shaders, binaryformat, binary, length);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->shaderBinary(n, shaders, binaryformat, binary, length);
#endif
}

inline void QGLFunctions::glShaderSource(GLuint shader, GLsizei count, const char** string, const GLint* length)
{
#if defined(QT_OPENGL_ES_2)
    ::glShaderSource(shader, count, string, length);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->shaderSource(shader, count, string, length);
#endif
}

inline void QGLFunctions::glStencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask)
{
#if defined(QT_OPENGL_ES_2)
    ::glStencilFuncSeparate(face, func, ref, mask);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->stencilFuncSeparate(face, func, ref, mask);
#endif
}

inline void QGLFunctions::glStencilMaskSeparate(GLenum face, GLuint mask)
{
#if defined(QT_OPENGL_ES_2)
    ::glStencilMaskSeparate(face, mask);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->stencilMaskSeparate(face, mask);
#endif
}

inline void QGLFunctions::glStencilOpSeparate(GLenum face, GLenum fail, GLenum zfail, GLenum zpass)
{
#if defined(QT_OPENGL_ES_2)
    ::glStencilOpSeparate(face, fail, zfail, zpass);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->stencilOpSeparate(face, fail, zfail, zpass);
#endif
}

inline void QGLFunctions::glUniform1f(GLint location, GLfloat x)
{
#if defined(QT_OPENGL_ES_2)
    ::glUniform1f(location, x);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->uniform1f(location, x);
#endif
}

inline void QGLFunctions::glUniform1fv(GLint location, GLsizei count, const GLfloat* v)
{
#if defined(QT_OPENGL_ES_2)
    ::glUniform1fv(location, count, v);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->uniform1fv(location, count, v);
#endif
}

inline void QGLFunctions::glUniform1i(GLint location, GLint x)
{
#if defined(QT_OPENGL_ES_2)
    ::glUniform1i(location, x);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->uniform1i(location, x);
#endif
}

inline void QGLFunctions::glUniform1iv(GLint location, GLsizei count, const GLint* v)
{
#if defined(QT_OPENGL_ES_2)
    ::glUniform1iv(location, count, v);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->uniform1iv(location, count, v);
#endif
}

inline void QGLFunctions::glUniform2f(GLint location, GLfloat x, GLfloat y)
{
#if defined(QT_OPENGL_ES_2)
    ::glUniform2f(location, x, y);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->uniform2f(location, x, y);
#endif
}

inline void QGLFunctions::glUniform2fv(GLint location, GLsizei count, const GLfloat* v)
{
#if defined(QT_OPENGL_ES_2)
    ::glUniform2fv(location, count, v);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->uniform2fv(location, count, v);
#endif
}

inline void QGLFunctions::glUniform2i(GLint location, GLint x, GLint y)
{
#if defined(QT_OPENGL_ES_2)
    ::glUniform2i(location, x, y);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->uniform2i(location, x, y);
#endif
}

inline void QGLFunctions::glUniform2iv(GLint location, GLsizei count, const GLint* v)
{
#if defined(QT_OPENGL_ES_2)
    ::glUniform2iv(location, count, v);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->uniform2iv(location, count, v);
#endif
}

inline void QGLFunctions::glUniform3f(GLint location, GLfloat x, GLfloat y, GLfloat z)
{
#if defined(QT_OPENGL_ES_2)
    ::glUniform3f(location, x, y, z);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->uniform3f(location, x, y, z);
#endif
}

inline void QGLFunctions::glUniform3fv(GLint location, GLsizei count, const GLfloat* v)
{
#if defined(QT_OPENGL_ES_2)
    ::glUniform3fv(location, count, v);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->uniform3fv(location, count, v);
#endif
}

inline void QGLFunctions::glUniform3i(GLint location, GLint x, GLint y, GLint z)
{
#if defined(QT_OPENGL_ES_2)
    ::glUniform3i(location, x, y, z);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->uniform3i(location, x, y, z);
#endif
}

inline void QGLFunctions::glUniform3iv(GLint location, GLsizei count, const GLint* v)
{
#if defined(QT_OPENGL_ES_2)
    ::glUniform3iv(location, count, v);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->uniform3iv(location, count, v);
#endif
}

inline void QGLFunctions::glUniform4f(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
#if defined(QT_OPENGL_ES_2)
    ::glUniform4f(location, x, y, z, w);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->uniform4f(location, x, y, z, w);
#endif
}

inline void QGLFunctions::glUniform4fv(GLint location, GLsizei count, const GLfloat* v)
{
#if defined(QT_OPENGL_ES_2)
    ::glUniform4fv(location, count, v);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->uniform4fv(location, count, v);
#endif
}

inline void QGLFunctions::glUniform4i(GLint location, GLint x, GLint y, GLint z, GLint w)
{
#if defined(QT_OPENGL_ES_2)
    ::glUniform4i(location, x, y, z, w);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->uniform4i(location, x, y, z, w);
#endif
}

inline void QGLFunctions::glUniform4iv(GLint location, GLsizei count, const GLint* v)
{
#if defined(QT_OPENGL_ES_2)
    ::glUniform4iv(location, count, v);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->uniform4iv(location, count, v);
#endif
}

inline void QGLFunctions::glUniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
#if defined(QT_OPENGL_ES_2)
    ::glUniformMatrix2fv(location, count, transpose, value);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->uniformMatrix2fv(location, count, transpose, value);
#endif
}

inline void QGLFunctions::glUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
#if defined(QT_OPENGL_ES_2)
    ::glUniformMatrix3fv(location, count, transpose, value);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->uniformMatrix3fv(location, count, transpose, value);
#endif
}

inline void QGLFunctions::glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
#if defined(QT_OPENGL_ES_2)
    ::glUniformMatrix4fv(location, count, transpose, value);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->uniformMatrix4fv(location, count, transpose, value);
#endif
}

inline void QGLFunctions::glUseProgram(GLuint program)
{
#if defined(QT_OPENGL_ES_2)
    ::glUseProgram(program);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->useProgram(program);
#endif
}

inline void QGLFunctions::glValidateProgram(GLuint program)
{
#if defined(QT_OPENGL_ES_2)
    ::glValidateProgram(program);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->validateProgram(program);
#endif
}

inline void QGLFunctions::glVertexAttrib1f(GLuint indx, GLfloat x)
{
#if defined(QT_OPENGL_ES_2)
    ::glVertexAttrib1f(indx, x);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->vertexAttrib1f(indx, x);
#endif
}

inline void QGLFunctions::glVertexAttrib1fv(GLuint indx, const GLfloat* values)
{
#if defined(QT_OPENGL_ES_2)
    ::glVertexAttrib1fv(indx, values);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->vertexAttrib1fv(indx, values);
#endif
}

inline void QGLFunctions::glVertexAttrib2f(GLuint indx, GLfloat x, GLfloat y)
{
#if defined(QT_OPENGL_ES_2)
    ::glVertexAttrib2f(indx, x, y);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->vertexAttrib2f(indx, x, y);
#endif
}

inline void QGLFunctions::glVertexAttrib2fv(GLuint indx, const GLfloat* values)
{
#if defined(QT_OPENGL_ES_2)
    ::glVertexAttrib2fv(indx, values);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->vertexAttrib2fv(indx, values);
#endif
}

inline void QGLFunctions::glVertexAttrib3f(GLuint indx, GLfloat x, GLfloat y, GLfloat z)
{
#if defined(QT_OPENGL_ES_2)
    ::glVertexAttrib3f(indx, x, y, z);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->vertexAttrib3f(indx, x, y, z);
#endif
}

inline void QGLFunctions::glVertexAttrib3fv(GLuint indx, const GLfloat* values)
{
#if defined(QT_OPENGL_ES_2)
    ::glVertexAttrib3fv(indx, values);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->vertexAttrib3fv(indx, values);
#endif
}

inline void QGLFunctions::glVertexAttrib4f(GLuint indx, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
#if defined(QT_OPENGL_ES_2)
    ::glVertexAttrib4f(indx, x, y, z, w);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->vertexAttrib4f(indx, x, y, z, w);
#endif
}

inline void QGLFunctions::glVertexAttrib4fv(GLuint indx, const GLfloat* values)
{
#if defined(QT_OPENGL_ES_2)
    ::glVertexAttrib4fv(indx, values);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->vertexAttrib4fv(indx, values);
#endif
}

inline void QGLFunctions::glVertexAttribPointer(GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* ptr)
{
#if defined(QT_OPENGL_ES_2)
    ::glVertexAttribPointer(indx, size, type, normalized, stride, ptr);
#else
    Q_ASSERT(QGLFunctions::isInitialized(d_ptr));
    d_ptr->vertexAttribPointer(indx, size, type, normalized, stride, ptr);
#endif
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
