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

class Q_QT3D_EXPORT QGLFunctions
{
public:
    QGLFunctions();
    explicit QGLFunctions(const QGLContext *context);
    ~QGLFunctions() {}

    void activeTexture(GLenum texture);
#ifndef QT_OPENGL_ES_1
    void attachShader(GLuint program, GLuint shader);
    void bindAttribLocation(GLuint program, GLuint index, const char* name);
#endif
    void bindBuffer(GLenum target, GLuint buffer);
    void bindFramebuffer(GLenum target, GLuint framebuffer);
    void bindRenderbuffer(GLenum target, GLuint renderbuffer);
    void bindTexture(GLenum target, GLuint texture);
    void blendColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
    void blendEquation(GLenum mode);
    void blendEquationSeparate(GLenum modeRGB, GLenum modeAlpha);
    void blendFunc(GLenum sfactor, GLenum dfactor);
    void blendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
    void bufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
    void bufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void* data);
    GLenum checkFramebufferStatus(GLenum target);
    void clear(GLbitfield mask);
    void clearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
    void clearDepth(GLclampf depth);
    void clearStencil(GLint s);
    void colorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
#ifndef QT_OPENGL_ES_1
    void compileShader(GLuint shader);
#endif
    void compressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data);
    void compressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data);
    void copyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
    void copyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
#ifndef QT_OPENGL_ES_1
    GLuint createProgram();
    GLuint createShader(GLenum type);
#endif
    void cullFace(GLenum mode);
    void deleteBuffers(GLsizei n, const GLuint* buffers);
    void deleteFramebuffers(GLsizei n, const GLuint* framebuffers);
#ifndef QT_OPENGL_ES_1
    void deleteProgram(GLuint program);
#endif
    void deleteRenderbuffers(GLsizei n, const GLuint* renderbuffers);
#ifndef QT_OPENGL_ES_1
    void deleteShader(GLuint shader);
#endif
    void deleteTextures(GLsizei n, const GLuint* textures);
    void depthFunc(GLenum func);
    void depthMask(GLboolean flag);
    void depthRange(GLclampf zNear, GLclampf zFar);
#ifndef QT_OPENGL_ES_1
    void detachShader(GLuint program, GLuint shader);
#endif
    void disable(GLenum cap);
#ifndef QT_OPENGL_ES_1
    void disableVertexAttribArray(GLuint index);
#endif
    void drawArrays(GLenum mode, GLint first, GLsizei count);
    void drawElements(GLenum mode, GLsizei count, GLenum type, const void* indices);
    void enable(GLenum cap);
#ifndef QT_OPENGL_ES_1
    void enableVertexAttribArray(GLuint index);
#endif
    void finish();
    void flush();
    void framebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
    void framebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
    void frontFace(GLenum mode);
    void genBuffers(GLsizei n, GLuint* buffers);
    void generateMipmap(GLenum target);
    void genFramebuffers(GLsizei n, GLuint* framebuffers);
    void genRenderbuffers(GLsizei n, GLuint* renderbuffers);
    void genTextures(GLsizei n, GLuint* textures);
#ifndef QT_OPENGL_ES_1
    void getActiveAttrib(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name);
    void getActiveUniform(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name);
    void getAttachedShaders(GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders);
    int getAttribLocation(GLuint program, const char* name);
#endif
    void getBooleanv(GLenum pname, GLboolean* params);
    void getBufferParameteriv(GLenum target, GLenum pname, GLint* params);
    GLenum getError();
    void getFloatv(GLenum pname, GLfloat* params);
    void getFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint* params);
    void getIntegerv(GLenum pname, GLint* params);
#ifndef QT_OPENGL_ES_1
    void getProgramiv(GLuint program, GLenum pname, GLint* params);
    void getProgramInfoLog(GLuint program, GLsizei bufsize, GLsizei* length, char* infolog);
#endif
    void getRenderbufferParameteriv(GLenum target, GLenum pname, GLint* params);
#ifndef QT_OPENGL_ES_1
    void getShaderiv(GLuint shader, GLenum pname, GLint* params);
    void getShaderInfoLog(GLuint shader, GLsizei bufsize, GLsizei* length, char* infolog);
    void getShaderPrecisionFormat(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision);
    void getShaderSource(GLuint shader, GLsizei bufsize, GLsizei* length, char* source);
#endif
    const GLubyte* getString(GLenum name);
    void getTexParameterfv(GLenum target, GLenum pname, GLfloat* params);
    void getTexParameteriv(GLenum target, GLenum pname, GLint* params);
#ifndef QT_OPENGL_ES_1
    void getUniformfv(GLuint program, GLint location, GLfloat* params);
    void getUniformiv(GLuint program, GLint location, GLint* params);
    int getUniformLocation(GLuint program, const char* name);
    void getVertexAttribfv(GLuint index, GLenum pname, GLfloat* params);
    void getVertexAttribiv(GLuint index, GLenum pname, GLint* params);
    void getVertexAttribPointerv(GLuint index, GLenum pname, void** pointer);
#endif
    void hint(GLenum target, GLenum mode);
    GLboolean isBuffer(GLuint buffer);
    GLboolean isEnabled(GLenum cap);
    GLboolean isFramebuffer(GLuint framebuffer);
#ifndef QT_OPENGL_ES_1
    GLboolean isProgram(GLuint program);
#endif
    GLboolean isRenderbuffer(GLuint renderbuffer);
#ifndef QT_OPENGL_ES_1
    GLboolean isShader(GLuint shader);
#endif
    GLboolean isTexture(GLuint texture);
    void lineWidth(GLfloat width);
#ifndef QT_OPENGL_ES_1
    void linkProgram(GLuint program);
#endif
    void pixelStorei(GLenum pname, GLint param);
    void polygonOffset(GLfloat factor, GLfloat units);
    void readPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void* pixels);
#ifndef QT_OPENGL_ES_1
    void releaseShaderCompiler();
#endif
    void renderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
    void sampleCoverage(GLclampf value, GLboolean invert);
    void scissor(GLint x, GLint y, GLsizei width, GLsizei height);
#ifndef QT_OPENGL_ES_1
    void shaderBinary(GLint n, const GLuint* shaders, GLenum binaryformat, const void* binary, GLint length);
    void shaderSource(GLuint shader, GLsizei count, const char** string, const GLint* length);
#endif
    void stencilFunc(GLenum func, GLint ref, GLuint mask);
    void stencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask);
    void stencilMask(GLuint mask);
    void stencilMaskSeparate(GLenum face, GLuint mask);
    void stencilOp(GLenum fail, GLenum zfail, GLenum zpass);
    void stencilOpSeparate(GLenum face, GLenum fail, GLenum zfail, GLenum zpass);
    void texImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels);
    void texParameterf(GLenum target, GLenum pname, GLfloat param);
    void texParameterfv(GLenum target, GLenum pname, const GLfloat* params);
    void texParameteri(GLenum target, GLenum pname, GLint param);
    void texParameteriv(GLenum target, GLenum pname, const GLint* params);
    void texSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
#ifndef QT_OPENGL_ES_1
    void uniform1f(GLint location, GLfloat x);
    void uniform1fv(GLint location, GLsizei count, const GLfloat* v);
    void uniform1i(GLint location, GLint x);
    void uniform1iv(GLint location, GLsizei count, const GLint* v);
    void uniform2f(GLint location, GLfloat x, GLfloat y);
    void uniform2fv(GLint location, GLsizei count, const GLfloat* v);
    void uniform2i(GLint location, GLint x, GLint y);
    void uniform2iv(GLint location, GLsizei count, const GLint* v);
    void uniform3f(GLint location, GLfloat x, GLfloat y, GLfloat z);
    void uniform3fv(GLint location, GLsizei count, const GLfloat* v);
    void uniform3i(GLint location, GLint x, GLint y, GLint z);
    void uniform3iv(GLint location, GLsizei count, const GLint* v);
    void uniform4f(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
    void uniform4fv(GLint location, GLsizei count, const GLfloat* v);
    void uniform4i(GLint location, GLint x, GLint y, GLint z, GLint w);
    void uniform4iv(GLint location, GLsizei count, const GLint* v);
    void uniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
    void uniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
    void uniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
    void useProgram(GLuint program);
    void validateProgram(GLuint program);
    void vertexAttrib1f(GLuint indx, GLfloat x);
    void vertexAttrib1fv(GLuint indx, const GLfloat* values);
    void vertexAttrib2f(GLuint indx, GLfloat x, GLfloat y);
    void vertexAttrib2fv(GLuint indx, const GLfloat* values);
    void vertexAttrib3f(GLuint indx, GLfloat x, GLfloat y, GLfloat z);
    void vertexAttrib3fv(GLuint indx, const GLfloat* values);
    void vertexAttrib4f(GLuint indx, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
    void vertexAttrib4fv(GLuint indx, const GLfloat* values);
    void vertexAttribPointer(GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* ptr);
#endif
    void viewport(GLint x, GLint y, GLsizei width, GLsizei height);

private:
    QGLFunctionsPrivate *d_ptr;
};

struct QGLFunctionsPrivate
{
    QGLFunctionsPrivate(const QGLContext * = 0);

#ifndef QT_OPENGL_ES_2
    void (QGLF_APIENTRYP activeTexture)(GLenum texture);
#ifndef QT_OPENGL_ES_1
    void (QGLF_APIENTRYP attachShader)(GLuint program, GLuint shader);
    void (QGLF_APIENTRYP bindAttribLocation)(GLuint program, GLuint index, const char* name);
#endif
    void (QGLF_APIENTRYP bindBuffer)(GLenum target, GLuint buffer);
    void (QGLF_APIENTRYP bindFramebuffer)(GLenum target, GLuint framebuffer);
    void (QGLF_APIENTRYP bindRenderbuffer)(GLenum target, GLuint renderbuffer);
    void (QGLF_APIENTRYP blendColor)(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
    void (QGLF_APIENTRYP blendEquation)(GLenum mode);
    void (QGLF_APIENTRYP blendEquationSeparate)(GLenum modeRGB, GLenum modeAlpha);
    void (QGLF_APIENTRYP blendFuncSeparate)(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
    void (QGLF_APIENTRYP bufferData)(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
    void (QGLF_APIENTRYP bufferSubData)(GLenum target, GLintptr offset, GLsizeiptr size, const void* data);
    GLenum (QGLF_APIENTRYP checkFramebufferStatus)(GLenum target);
#ifndef QT_OPENGL_ES_1
    void (QGLF_APIENTRYP compileShader)(GLuint shader);
#endif
    void (QGLF_APIENTRYP compressedTexImage2D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data);
    void (QGLF_APIENTRYP compressedTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data);
#ifndef QT_OPENGL_ES_1
    GLuint (QGLF_APIENTRYP createProgram)();
    GLuint (QGLF_APIENTRYP createShader)(GLenum type);
#endif
    void (QGLF_APIENTRYP deleteBuffers)(GLsizei n, const GLuint* buffers);
    void (QGLF_APIENTRYP deleteFramebuffers)(GLsizei n, const GLuint* framebuffers);
#ifndef QT_OPENGL_ES_1
    void (QGLF_APIENTRYP deleteProgram)(GLuint program);
#endif
    void (QGLF_APIENTRYP deleteRenderbuffers)(GLsizei n, const GLuint* renderbuffers);
#ifndef QT_OPENGL_ES_1
    void (QGLF_APIENTRYP deleteShader)(GLuint shader);
    void (QGLF_APIENTRYP detachShader)(GLuint program, GLuint shader);
    void (QGLF_APIENTRYP disableVertexAttribArray)(GLuint index);
    void (QGLF_APIENTRYP enableVertexAttribArray)(GLuint index);
#endif
    void (QGLF_APIENTRYP framebufferRenderbuffer)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
    void (QGLF_APIENTRYP framebufferTexture2D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
    void (QGLF_APIENTRYP genBuffers)(GLsizei n, GLuint* buffers);
    void (QGLF_APIENTRYP generateMipmap)(GLenum target);
    void (QGLF_APIENTRYP genFramebuffers)(GLsizei n, GLuint* framebuffers);
    void (QGLF_APIENTRYP genRenderbuffers)(GLsizei n, GLuint* renderbuffers);
#ifndef QT_OPENGL_ES_1
    void (QGLF_APIENTRYP getActiveAttrib)(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name);
    void (QGLF_APIENTRYP getActiveUniform)(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name);
    void (QGLF_APIENTRYP getAttachedShaders)(GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders);
    int (QGLF_APIENTRYP getAttribLocation)(GLuint program, const char* name);
#endif
    void (QGLF_APIENTRYP getBufferParameteriv)(GLenum target, GLenum pname, GLint* params);
    void (QGLF_APIENTRYP getFramebufferAttachmentParameteriv)(GLenum target, GLenum attachment, GLenum pname, GLint* params);
#ifndef QT_OPENGL_ES_1
    void (QGLF_APIENTRYP getProgramiv)(GLuint program, GLenum pname, GLint* params);
    void (QGLF_APIENTRYP getProgramInfoLog)(GLuint program, GLsizei bufsize, GLsizei* length, char* infolog);
#endif
    void (QGLF_APIENTRYP getRenderbufferParameteriv)(GLenum target, GLenum pname, GLint* params);
#ifndef QT_OPENGL_ES_1
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
#endif
    GLboolean (QGLF_APIENTRYP isBuffer)(GLuint buffer);
    GLboolean (QGLF_APIENTRYP isFramebuffer)(GLuint framebuffer);
#ifndef QT_OPENGL_ES_1
    GLboolean (QGLF_APIENTRYP isProgram)(GLuint program);
#endif
    GLboolean (QGLF_APIENTRYP isRenderbuffer)(GLuint renderbuffer);
#ifndef QT_OPENGL_ES_1
    GLboolean (QGLF_APIENTRYP isShader)(GLuint shader);
    void (QGLF_APIENTRYP linkProgram)(GLuint program);
    void (QGLF_APIENTRYP releaseShaderCompiler)();
#endif
    void (QGLF_APIENTRYP renderbufferStorage)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
    void (QGLF_APIENTRYP sampleCoverage)(GLclampf value, GLboolean invert);
#ifndef QT_OPENGL_ES_1
    void (QGLF_APIENTRYP shaderBinary)(GLint n, const GLuint* shaders, GLenum binaryformat, const void* binary, GLint length);
    void (QGLF_APIENTRYP shaderSource)(GLuint shader, GLsizei count, const char** string, const GLint* length);
#endif
    void (QGLF_APIENTRYP stencilFuncSeparate)(GLenum face, GLenum func, GLint ref, GLuint mask);
    void (QGLF_APIENTRYP stencilMaskSeparate)(GLenum face, GLuint mask);
    void (QGLF_APIENTRYP stencilOpSeparate)(GLenum face, GLenum fail, GLenum zfail, GLenum zpass);
#ifndef QT_OPENGL_ES_1
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
#endif
};

inline void QGLFunctions::activeTexture(GLenum texture)
{
#if defined(QT_OPENGL_ES_1) || defined(QT_OPENGL_ES_2)
    ::glActiveTexture(texture);
#else
    d_ptr->activeTexture(texture);
#endif
}

#ifndef QT_OPENGL_ES_1

inline void QGLFunctions::attachShader(GLuint program, GLuint shader)
{
#if defined(QT_OPENGL_ES_2)
    ::glAttachShader(program, shader);
#else
    d_ptr->attachShader(program, shader);
#endif
}

inline void QGLFunctions::bindAttribLocation(GLuint program, GLuint index, const char* name)
{
#if defined(QT_OPENGL_ES_2)
    ::glBindAttribLocation(program, index, name);
#else
    d_ptr->bindAttribLocation(program, index, name);
#endif
}

#endif

inline void QGLFunctions::bindBuffer(GLenum target, GLuint buffer)
{
#if defined(QT_OPENGL_ES_1) || defined(QT_OPENGL_ES_2)
    ::glBindBuffer(target, buffer);
#else
    d_ptr->bindBuffer(target, buffer);
#endif
}

inline void QGLFunctions::bindFramebuffer(GLenum target, GLuint framebuffer)
{
#if defined(QT_OPENGL_ES_2)
    ::glBindFramebuffer(target, framebuffer);
#else
    d_ptr->bindFramebuffer(target, framebuffer);
#endif
}

inline void QGLFunctions::bindRenderbuffer(GLenum target, GLuint renderbuffer)
{
#if defined(QT_OPENGL_ES_2)
    ::glBindRenderbuffer(target, renderbuffer);
#else
    d_ptr->bindRenderbuffer(target, renderbuffer);
#endif
}

inline void QGLFunctions::bindTexture(GLenum target, GLuint texture)
{
    ::glBindTexture(target, texture);
}

inline void QGLFunctions::blendColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
#if defined(QT_OPENGL_ES_2)
    ::glBlendColor(red, green, blue, alpha);
#else
    d_ptr->blendColor(red, green, blue, alpha);
#endif
}

inline void QGLFunctions::blendEquation(GLenum mode)
{
#if defined(QT_OPENGL_ES_2)
    ::glBlendEquation(mode);
#else
    d_ptr->blendEquation(mode);
#endif
}

inline void QGLFunctions::blendEquationSeparate(GLenum modeRGB, GLenum modeAlpha)
{
#if defined(QT_OPENGL_ES_2)
    ::glBlendEquationSeparate(modeRGB, modeAlpha);
#else
    d_ptr->blendEquationSeparate(modeRGB, modeAlpha);
#endif
}

inline void QGLFunctions::blendFunc(GLenum sfactor, GLenum dfactor)
{
    ::glBlendFunc(sfactor, dfactor);
}

inline void QGLFunctions::blendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
{
#if defined(QT_OPENGL_ES_2)
    ::glBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
#else
    d_ptr->blendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
#endif
}

inline void QGLFunctions::bufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage)
{
#if defined(QT_OPENGL_ES_1) || defined(QT_OPENGL_ES_2)
    ::glBufferData(target, size, data, usage);
#else
    d_ptr->bufferData(target, size, data, usage);
#endif
}

inline void QGLFunctions::bufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void* data)
{
#if defined(QT_OPENGL_ES_1) || defined(QT_OPENGL_ES_2)
    ::glBufferSubData(target, offset, size, data);
#else
    d_ptr->bufferSubData(target, offset, size, data);
#endif
}

inline GLenum QGLFunctions::checkFramebufferStatus(GLenum target)
{
#if defined(QT_OPENGL_ES_2)
    return ::glCheckFramebufferStatus(target);
#else
    return d_ptr->checkFramebufferStatus(target);
#endif
}

inline void QGLFunctions::clear(GLbitfield mask)
{
    ::glClear(mask);
}

inline void QGLFunctions::clearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    ::glClearColor(red, green, blue, alpha);
}

inline void QGLFunctions::clearDepth(GLclampf depth)
{
#ifndef QT_OPENGL_ES
    ::glClearDepth(depth);
#else
    ::glClearDepthf(depth);
#endif
}

inline void QGLFunctions::clearStencil(GLint s)
{
    ::glClearStencil(s);
}

inline void QGLFunctions::colorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
    ::glColorMask(red, green, blue, alpha);
}

#ifndef QT_OPENGL_ES_1

inline void QGLFunctions::compileShader(GLuint shader)
{
#if defined(QT_OPENGL_ES_2)
    ::glCompileShader(shader);
#else
    d_ptr->compileShader(shader);
#endif
}

#endif

inline void QGLFunctions::compressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data)
{
#if defined(QT_OPENGL_ES_1) || defined(QT_OPENGL_ES_2)
    ::glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
#else
    d_ptr->compressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
#endif
}

inline void QGLFunctions::compressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data)
{
#if defined(QT_OPENGL_ES_1) || defined(QT_OPENGL_ES_2)
    ::glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data);
#else
    d_ptr->compressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data);
#endif
}

inline void QGLFunctions::copyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
    ::glCopyTexImage2D(target, level, internalformat, x, y, width, height, border);
}

inline void QGLFunctions::copyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
    ::glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
}

#ifndef QT_OPENGL_ES_1

inline GLuint QGLFunctions::createProgram()
{
#if defined(QT_OPENGL_ES_2)
    return ::glCreateProgram();
#else
    return d_ptr->createProgram();
#endif
}

inline GLuint QGLFunctions::createShader(GLenum type)
{
#if defined(QT_OPENGL_ES_2)
    return ::glCreateShader(type);
#else
    return d_ptr->createShader(type);
#endif
}

#endif

inline void QGLFunctions::cullFace(GLenum mode)
{
    ::glCullFace(mode);
}

inline void QGLFunctions::deleteBuffers(GLsizei n, const GLuint* buffers)
{
#if defined(QT_OPENGL_ES_1) || defined(QT_OPENGL_ES_2)
    ::glDeleteBuffers(n, buffers);
#else
    d_ptr->deleteBuffers(n, buffers);
#endif
}

inline void QGLFunctions::deleteFramebuffers(GLsizei n, const GLuint* framebuffers)
{
#if defined(QT_OPENGL_ES_2)
    ::glDeleteFramebuffers(n, framebuffers);
#else
    d_ptr->deleteFramebuffers(n, framebuffers);
#endif
}

#ifndef QT_OPENGL_ES_1

inline void QGLFunctions::deleteProgram(GLuint program)
{
#if defined(QT_OPENGL_ES_2)
    ::glDeleteProgram(program);
#else
    d_ptr->deleteProgram(program);
#endif
}

#endif

inline void QGLFunctions::deleteRenderbuffers(GLsizei n, const GLuint* renderbuffers)
{
#if defined(QT_OPENGL_ES_2)
    ::glDeleteRenderbuffers(n, renderbuffers);
#else
    d_ptr->deleteRenderbuffers(n, renderbuffers);
#endif
}

#ifndef QT_OPENGL_ES_1

inline void QGLFunctions::deleteShader(GLuint shader)
{
#if defined(QT_OPENGL_ES_2)
    ::glDeleteShader(shader);
#else
    d_ptr->deleteShader(shader);
#endif
}

#endif

inline void QGLFunctions::deleteTextures(GLsizei n, const GLuint* textures)
{
    ::glDeleteTextures(n, textures);
}

inline void QGLFunctions::depthFunc(GLenum func)
{
    ::glDepthFunc(func);
}

inline void QGLFunctions::depthMask(GLboolean flag)
{
    ::glDepthMask(flag);
}

inline void QGLFunctions::depthRange(GLclampf zNear, GLclampf zFar)
{
#ifndef QT_OPENGL_ES
    ::glDepthRange(zNear, zFar);
#else
    ::glDepthRangef(zNear, zFar);
#endif
}

#ifndef QT_OPENGL_ES_1

inline void QGLFunctions::detachShader(GLuint program, GLuint shader)
{
#if defined(QT_OPENGL_ES_2)
    ::glDetachShader(program, shader);
#else
    d_ptr->detachShader(program, shader);
#endif
}

#endif

inline void QGLFunctions::disable(GLenum cap)
{
    ::glDisable(cap);
}

#ifndef QT_OPENGL_ES_1

inline void QGLFunctions::disableVertexAttribArray(GLuint index)
{
#if defined(QT_OPENGL_ES_2)
    ::glDisableVertexAttribArray(index);
#else
    d_ptr->disableVertexAttribArray(index);
#endif
}

#endif

inline void QGLFunctions::drawArrays(GLenum mode, GLint first, GLsizei count)
{
    ::glDrawArrays(mode, first, count);
}

inline void QGLFunctions::drawElements(GLenum mode, GLsizei count, GLenum type, const void* indices)
{
    ::glDrawElements(mode, count, type, indices);
}

inline void QGLFunctions::enable(GLenum cap)
{
    ::glEnable(cap);
}

#ifndef QT_OPENGL_ES_1

inline void QGLFunctions::enableVertexAttribArray(GLuint index)
{
#if defined(QT_OPENGL_ES_2)
    ::glEnableVertexAttribArray(index);
#else
    d_ptr->enableVertexAttribArray(index);
#endif
}

#endif

inline void QGLFunctions::finish()
{
    ::glFinish();
}

inline void QGLFunctions::flush()
{
    ::glFlush();
}

inline void QGLFunctions::framebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
#if defined(QT_OPENGL_ES_2)
    ::glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
#else
    d_ptr->framebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
#endif
}

inline void QGLFunctions::framebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
#if defined(QT_OPENGL_ES_2)
    ::glFramebufferTexture2D(target, attachment, textarget, texture, level);
#else
    d_ptr->framebufferTexture2D(target, attachment, textarget, texture, level);
#endif
}

inline void QGLFunctions::frontFace(GLenum mode)
{
    ::glFrontFace(mode);
}

inline void QGLFunctions::genBuffers(GLsizei n, GLuint* buffers)
{
#if defined(QT_OPENGL_ES_1) || defined(QT_OPENGL_ES_2)
    ::glGenBuffers(n, buffers);
#else
    d_ptr->genBuffers(n, buffers);
#endif
}

inline void QGLFunctions::generateMipmap(GLenum target)
{
#if defined(QT_OPENGL_ES_2)
    ::glGenerateMipmap(target);
#else
    d_ptr->generateMipmap(target);
#endif
}

inline void QGLFunctions::genFramebuffers(GLsizei n, GLuint* framebuffers)
{
#if defined(QT_OPENGL_ES_2)
    ::glGenFramebuffers(n, framebuffers);
#else
    d_ptr->genFramebuffers(n, framebuffers);
#endif
}

inline void QGLFunctions::genRenderbuffers(GLsizei n, GLuint* renderbuffers)
{
#if defined(QT_OPENGL_ES_2)
    ::glGenRenderbuffers(n, renderbuffers);
#else
    d_ptr->genRenderbuffers(n, renderbuffers);
#endif
}

inline void QGLFunctions::genTextures(GLsizei n, GLuint* textures)
{
    ::glGenTextures(n, textures);
}

#ifndef QT_OPENGL_ES_1

inline void QGLFunctions::getActiveAttrib(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name)
{
#if defined(QT_OPENGL_ES_2)
    ::glGetActiveAttrib(program, index, bufsize, length, size, type, name);
#else
    d_ptr->getActiveAttrib(program, index, bufsize, length, size, type, name);
#endif
}

inline void QGLFunctions::getActiveUniform(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name)
{
#if defined(QT_OPENGL_ES_2)
    ::glGetActiveUniform(program, index, bufsize, length, size, type, name);
#else
    d_ptr->getActiveUniform(program, index, bufsize, length, size, type, name);
#endif
}

inline void QGLFunctions::getAttachedShaders(GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders)
{
#if defined(QT_OPENGL_ES_2)
    ::glGetAttachedShaders(program, maxcount, count, shaders);
#else
    d_ptr->getAttachedShaders(program, maxcount, count, shaders);
#endif
}

inline int QGLFunctions::getAttribLocation(GLuint program, const char* name)
{
#if defined(QT_OPENGL_ES_2)
    return ::glGetAttribLocation(program, name);
#else
    return d_ptr->getAttribLocation(program, name);
#endif
}

#endif

inline void QGLFunctions::getBooleanv(GLenum pname, GLboolean* params)
{
    ::glGetBooleanv(pname, params);
}

inline void QGLFunctions::getBufferParameteriv(GLenum target, GLenum pname, GLint* params)
{
#if defined(QT_OPENGL_ES_2)
    ::glGetBufferParameteriv(target, pname, params);
#else
    d_ptr->getBufferParameteriv(target, pname, params);
#endif
}

inline GLenum QGLFunctions::getError()
{
    return ::glGetError();
}

inline void QGLFunctions::getFloatv(GLenum pname, GLfloat* params)
{
    ::glGetFloatv(pname, params);
}

inline void QGLFunctions::getFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint* params)
{
#if defined(QT_OPENGL_ES_2)
    ::glGetFramebufferAttachmentParameteriv(target, attachment, pname, params);
#else
    d_ptr->getFramebufferAttachmentParameteriv(target, attachment, pname, params);
#endif
}

inline void QGLFunctions::getIntegerv(GLenum pname, GLint* params)
{
    ::glGetIntegerv(pname, params);
}

#ifndef QT_OPENGL_ES_1

inline void QGLFunctions::getProgramiv(GLuint program, GLenum pname, GLint* params)
{
#if defined(QT_OPENGL_ES_2)
    ::glGetProgramiv(program, pname, params);
#else
    d_ptr->getProgramiv(program, pname, params);
#endif
}

inline void QGLFunctions::getProgramInfoLog(GLuint program, GLsizei bufsize, GLsizei* length, char* infolog)
{
#if defined(QT_OPENGL_ES_2)
    ::glGetProgramInfoLog(program, bufsize, length, infolog);
#else
    d_ptr->getProgramInfoLog(program, bufsize, length, infolog);
#endif
}

#endif

inline void QGLFunctions::getRenderbufferParameteriv(GLenum target, GLenum pname, GLint* params)
{
#if defined(QT_OPENGL_ES_2)
    ::glGetRenderbufferParameteriv(target, pname, params);
#else
    d_ptr->getRenderbufferParameteriv(target, pname, params);
#endif
}

#ifndef QT_OPENGL_ES_1

inline void QGLFunctions::getShaderiv(GLuint shader, GLenum pname, GLint* params)
{
#if defined(QT_OPENGL_ES_2)
    ::glGetShaderiv(shader, pname, params);
#else
    d_ptr->getShaderiv(shader, pname, params);
#endif
}

inline void QGLFunctions::getShaderInfoLog(GLuint shader, GLsizei bufsize, GLsizei* length, char* infolog)
{
#if defined(QT_OPENGL_ES_2)
    ::glGetShaderInfoLog(shader, bufsize, length, infolog);
#else
    d_ptr->getShaderInfoLog(shader, bufsize, length, infolog);
#endif
}

inline void QGLFunctions::getShaderPrecisionFormat(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision)
{
#if defined(QT_OPENGL_ES_2)
    ::glGetShaderPrecisionFormat(shadertype, precisiontype, range, precision);
#else
    d_ptr->getShaderPrecisionFormat(shadertype, precisiontype, range, precision);
#endif
}

inline void QGLFunctions::getShaderSource(GLuint shader, GLsizei bufsize, GLsizei* length, char* source)
{
#if defined(QT_OPENGL_ES_2)
    ::glGetShaderSource(shader, bufsize, length, source);
#else
    d_ptr->getShaderSource(shader, bufsize, length, source);
#endif
}

#endif

inline const GLubyte* QGLFunctions::getString(GLenum name)
{
    return ::glGetString(name);
}

inline void QGLFunctions::getTexParameterfv(GLenum target, GLenum pname, GLfloat* params)
{
    ::glGetTexParameterfv(target, pname, params);
}

inline void QGLFunctions::getTexParameteriv(GLenum target, GLenum pname, GLint* params)
{
    ::glGetTexParameteriv(target, pname, params);
}

#ifndef QT_OPENGL_ES_1

inline void QGLFunctions::getUniformfv(GLuint program, GLint location, GLfloat* params)
{
#if defined(QT_OPENGL_ES_2)
    ::glGetUniformfv(program, location, params);
#else
    d_ptr->getUniformfv(program, location, params);
#endif
}

inline void QGLFunctions::getUniformiv(GLuint program, GLint location, GLint* params)
{
#if defined(QT_OPENGL_ES_2)
    ::glGetUniformiv(program, location, params);
#else
    d_ptr->getUniformiv(program, location, params);
#endif
}

inline int QGLFunctions::getUniformLocation(GLuint program, const char* name)
{
#if defined(QT_OPENGL_ES_2)
    return ::glGetUniformLocation(program, name);
#else
    return d_ptr->getUniformLocation(program, name);
#endif
}

inline void QGLFunctions::getVertexAttribfv(GLuint index, GLenum pname, GLfloat* params)
{
#if defined(QT_OPENGL_ES_2)
    ::glGetVertexAttribfv(index, pname, params);
#else
    d_ptr->getVertexAttribfv(index, pname, params);
#endif
}

inline void QGLFunctions::getVertexAttribiv(GLuint index, GLenum pname, GLint* params)
{
#if defined(QT_OPENGL_ES_2)
    ::glGetVertexAttribiv(index, pname, params);
#else
    d_ptr->getVertexAttribiv(index, pname, params);
#endif
}

inline void QGLFunctions::getVertexAttribPointerv(GLuint index, GLenum pname, void** pointer)
{
#if defined(QT_OPENGL_ES_2)
    ::glGetVertexAttribPointerv(index, pname, pointer);
#else
    d_ptr->getVertexAttribPointerv(index, pname, pointer);
#endif
}

#endif

inline void QGLFunctions::hint(GLenum target, GLenum mode)
{
    ::glHint(target, mode);
}

inline GLboolean QGLFunctions::isBuffer(GLuint buffer)
{
#if defined(QT_OPENGL_ES_1) || defined(QT_OPENGL_ES_2)
    return ::glIsBuffer(buffer);
#else
    return d_ptr->isBuffer(buffer);
#endif
}

inline GLboolean QGLFunctions::isEnabled(GLenum cap)
{
    return ::glIsEnabled(cap);
}

inline GLboolean QGLFunctions::isFramebuffer(GLuint framebuffer)
{
#if defined(QT_OPENGL_ES_2)
    return ::glIsFramebuffer(framebuffer);
#else
    return d_ptr->isFramebuffer(framebuffer);
#endif
}

#ifndef QT_OPENGL_ES_1

inline GLboolean QGLFunctions::isProgram(GLuint program)
{
#if defined(QT_OPENGL_ES_2)
    return ::glIsProgram(program);
#else
    return d_ptr->isProgram(program);
#endif
}

#endif

inline GLboolean QGLFunctions::isRenderbuffer(GLuint renderbuffer)
{
#if defined(QT_OPENGL_ES_2)
    return ::glIsRenderbuffer(renderbuffer);
#else
    return d_ptr->isRenderbuffer(renderbuffer);
#endif
}

#ifndef QT_OPENGL_ES_1

inline GLboolean QGLFunctions::isShader(GLuint shader)
{
#if defined(QT_OPENGL_ES_2)
    return ::glIsShader(shader);
#else
    return d_ptr->isShader(shader);
#endif
}

#endif

inline GLboolean QGLFunctions::isTexture(GLuint texture)
{
    return ::glIsTexture(texture);
}

inline void QGLFunctions::lineWidth(GLfloat width)
{
    ::glLineWidth(width);
}

#ifndef QT_OPENGL_ES_1

inline void QGLFunctions::linkProgram(GLuint program)
{
#if defined(QT_OPENGL_ES_2)
    ::glLinkProgram(program);
#else
    d_ptr->linkProgram(program);
#endif
}

#endif

inline void QGLFunctions::pixelStorei(GLenum pname, GLint param)
{
    ::glPixelStorei(pname, param);
}

inline void QGLFunctions::polygonOffset(GLfloat factor, GLfloat units)
{
    ::glPolygonOffset(factor, units);
}

inline void QGLFunctions::readPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void* pixels)
{
    ::glReadPixels(x, y, width, height, format, type, pixels);
}

#ifndef QT_OPENGL_ES_1

inline void QGLFunctions::releaseShaderCompiler()
{
#if defined(QT_OPENGL_ES_2)
    ::glReleaseShaderCompiler();
#else
    d_ptr->releaseShaderCompiler();
#endif
}

#endif

inline void QGLFunctions::renderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
#if defined(QT_OPENGL_ES_2)
    ::glRenderbufferStorage(target, internalformat, width, height);
#else
    d_ptr->renderbufferStorage(target, internalformat, width, height);
#endif
}

inline void QGLFunctions::sampleCoverage(GLclampf value, GLboolean invert)
{
#if defined(QT_OPENGL_ES_1) || defined(QT_OPENGL_ES_2)
    ::glSampleCoverage(value, invert);
#else
    d_ptr->sampleCoverage(value, invert);
#endif
}

inline void QGLFunctions::scissor(GLint x, GLint y, GLsizei width, GLsizei height)
{
    ::glScissor(x, y, width, height);
}

#ifndef QT_OPENGL_ES_1

inline void QGLFunctions::shaderBinary(GLint n, const GLuint* shaders, GLenum binaryformat, const void* binary, GLint length)
{
#if defined(QT_OPENGL_ES_2)
    ::glShaderBinary(n, shaders, binaryformat, binary, length);
#else
    d_ptr->shaderBinary(n, shaders, binaryformat, binary, length);
#endif
}

inline void QGLFunctions::shaderSource(GLuint shader, GLsizei count, const char** string, const GLint* length)
{
#if defined(QT_OPENGL_ES_2)
    ::glShaderSource(shader, count, string, length);
#else
    d_ptr->shaderSource(shader, count, string, length);
#endif
}

#endif

inline void QGLFunctions::stencilFunc(GLenum func, GLint ref, GLuint mask)
{
    ::glStencilFunc(func, ref, mask);
}

inline void QGLFunctions::stencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask)
{
#if defined(QT_OPENGL_ES_2)
    ::glStencilFuncSeparate(face, func, ref, mask);
#else
    d_ptr->stencilFuncSeparate(face, func, ref, mask);
#endif
}

inline void QGLFunctions::stencilMask(GLuint mask)
{
    ::glStencilMask(mask);
}

inline void QGLFunctions::stencilMaskSeparate(GLenum face, GLuint mask)
{
#if defined(QT_OPENGL_ES_2)
    ::glStencilMaskSeparate(face, mask);
#else
    d_ptr->stencilMaskSeparate(face, mask);
#endif
}

inline void QGLFunctions::stencilOp(GLenum fail, GLenum zfail, GLenum zpass)
{
    ::glStencilOp(fail, zfail, zpass);
}

inline void QGLFunctions::stencilOpSeparate(GLenum face, GLenum fail, GLenum zfail, GLenum zpass)
{
#if defined(QT_OPENGL_ES_2)
    ::glStencilOpSeparate(face, fail, zfail, zpass);
#else
    d_ptr->stencilOpSeparate(face, fail, zfail, zpass);
#endif
}

inline void QGLFunctions::texImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels)
{
    ::glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
}

inline void QGLFunctions::texParameterf(GLenum target, GLenum pname, GLfloat param)
{
    ::glTexParameterf(target, pname, param);
}

inline void QGLFunctions::texParameterfv(GLenum target, GLenum pname, const GLfloat* params)
{
    ::glTexParameterfv(target, pname, params);
}

inline void QGLFunctions::texParameteri(GLenum target, GLenum pname, GLint param)
{
    ::glTexParameteri(target, pname, param);
}

inline void QGLFunctions::texParameteriv(GLenum target, GLenum pname, const GLint* params)
{
    ::glTexParameteriv(target, pname, params);
}

inline void QGLFunctions::texSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
{
    ::glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
}

#ifndef QT_OPENGL_ES_1

inline void QGLFunctions::uniform1f(GLint location, GLfloat x)
{
#if defined(QT_OPENGL_ES_2)
    ::glUniform1f(location, x);
#else
    d_ptr->uniform1f(location, x);
#endif
}

inline void QGLFunctions::uniform1fv(GLint location, GLsizei count, const GLfloat* v)
{
#if defined(QT_OPENGL_ES_2)
    ::glUniform1fv(location, count, v);
#else
    d_ptr->uniform1fv(location, count, v);
#endif
}

inline void QGLFunctions::uniform1i(GLint location, GLint x)
{
#if defined(QT_OPENGL_ES_2)
    ::glUniform1i(location, x);
#else
    d_ptr->uniform1i(location, x);
#endif
}

inline void QGLFunctions::uniform1iv(GLint location, GLsizei count, const GLint* v)
{
#if defined(QT_OPENGL_ES_2)
    ::glUniform1iv(location, count, v);
#else
    d_ptr->uniform1iv(location, count, v);
#endif
}

inline void QGLFunctions::uniform2f(GLint location, GLfloat x, GLfloat y)
{
#if defined(QT_OPENGL_ES_2)
    ::glUniform2f(location, x, y);
#else
    d_ptr->uniform2f(location, x, y);
#endif
}

inline void QGLFunctions::uniform2fv(GLint location, GLsizei count, const GLfloat* v)
{
#if defined(QT_OPENGL_ES_2)
    ::glUniform2fv(location, count, v);
#else
    d_ptr->uniform2fv(location, count, v);
#endif
}

inline void QGLFunctions::uniform2i(GLint location, GLint x, GLint y)
{
#if defined(QT_OPENGL_ES_2)
    ::glUniform2i(location, x, y);
#else
    d_ptr->uniform2i(location, x, y);
#endif
}

inline void QGLFunctions::uniform2iv(GLint location, GLsizei count, const GLint* v)
{
#if defined(QT_OPENGL_ES_2)
    ::glUniform2iv(location, count, v);
#else
    d_ptr->uniform2iv(location, count, v);
#endif
}

inline void QGLFunctions::uniform3f(GLint location, GLfloat x, GLfloat y, GLfloat z)
{
#if defined(QT_OPENGL_ES_2)
    ::glUniform3f(location, x, y, z);
#else
    d_ptr->uniform3f(location, x, y, z);
#endif
}

inline void QGLFunctions::uniform3fv(GLint location, GLsizei count, const GLfloat* v)
{
#if defined(QT_OPENGL_ES_2)
    ::glUniform3fv(location, count, v);
#else
    d_ptr->uniform3fv(location, count, v);
#endif
}

inline void QGLFunctions::uniform3i(GLint location, GLint x, GLint y, GLint z)
{
#if defined(QT_OPENGL_ES_2)
    ::glUniform3i(location, x, y, z);
#else
    d_ptr->uniform3i(location, x, y, z);
#endif
}

inline void QGLFunctions::uniform3iv(GLint location, GLsizei count, const GLint* v)
{
#if defined(QT_OPENGL_ES_2)
    ::glUniform3iv(location, count, v);
#else
    d_ptr->uniform3iv(location, count, v);
#endif
}

inline void QGLFunctions::uniform4f(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
#if defined(QT_OPENGL_ES_2)
    ::glUniform4f(location, x, y, z, w);
#else
    d_ptr->uniform4f(location, x, y, z, w);
#endif
}

inline void QGLFunctions::uniform4fv(GLint location, GLsizei count, const GLfloat* v)
{
#if defined(QT_OPENGL_ES_2)
    ::glUniform4fv(location, count, v);
#else
    d_ptr->uniform4fv(location, count, v);
#endif
}

inline void QGLFunctions::uniform4i(GLint location, GLint x, GLint y, GLint z, GLint w)
{
#if defined(QT_OPENGL_ES_2)
    ::glUniform4i(location, x, y, z, w);
#else
    d_ptr->uniform4i(location, x, y, z, w);
#endif
}

inline void QGLFunctions::uniform4iv(GLint location, GLsizei count, const GLint* v)
{
#if defined(QT_OPENGL_ES_2)
    ::glUniform4iv(location, count, v);
#else
    d_ptr->uniform4iv(location, count, v);
#endif
}

inline void QGLFunctions::uniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
#if defined(QT_OPENGL_ES_2)
    ::glUniformMatrix2fv(location, count, transpose, value);
#else
    d_ptr->uniformMatrix2fv(location, count, transpose, value);
#endif
}

inline void QGLFunctions::uniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
#if defined(QT_OPENGL_ES_2)
    ::glUniformMatrix3fv(location, count, transpose, value);
#else
    d_ptr->uniformMatrix3fv(location, count, transpose, value);
#endif
}

inline void QGLFunctions::uniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
#if defined(QT_OPENGL_ES_2)
    ::glUniformMatrix4fv(location, count, transpose, value);
#else
    d_ptr->uniformMatrix4fv(location, count, transpose, value);
#endif
}

inline void QGLFunctions::useProgram(GLuint program)
{
#if defined(QT_OPENGL_ES_2)
    ::glUseProgram(program);
#else
    d_ptr->useProgram(program);
#endif
}

inline void QGLFunctions::validateProgram(GLuint program)
{
#if defined(QT_OPENGL_ES_2)
    ::glValidateProgram(program);
#else
    d_ptr->validateProgram(program);
#endif
}

inline void QGLFunctions::vertexAttrib1f(GLuint indx, GLfloat x)
{
#if defined(QT_OPENGL_ES_2)
    ::glVertexAttrib1f(indx, x);
#else
    d_ptr->vertexAttrib1f(indx, x);
#endif
}

inline void QGLFunctions::vertexAttrib1fv(GLuint indx, const GLfloat* values)
{
#if defined(QT_OPENGL_ES_2)
    ::glVertexAttrib1fv(indx, values);
#else
    d_ptr->vertexAttrib1fv(indx, values);
#endif
}

inline void QGLFunctions::vertexAttrib2f(GLuint indx, GLfloat x, GLfloat y)
{
#if defined(QT_OPENGL_ES_2)
    ::glVertexAttrib2f(indx, x, y);
#else
    d_ptr->vertexAttrib2f(indx, x, y);
#endif
}

inline void QGLFunctions::vertexAttrib2fv(GLuint indx, const GLfloat* values)
{
#if defined(QT_OPENGL_ES_2)
    ::glVertexAttrib2fv(indx, values);
#else
    d_ptr->vertexAttrib2fv(indx, values);
#endif
}

inline void QGLFunctions::vertexAttrib3f(GLuint indx, GLfloat x, GLfloat y, GLfloat z)
{
#if defined(QT_OPENGL_ES_2)
    ::glVertexAttrib3f(indx, x, y, z);
#else
    d_ptr->vertexAttrib3f(indx, x, y, z);
#endif
}

inline void QGLFunctions::vertexAttrib3fv(GLuint indx, const GLfloat* values)
{
#if defined(QT_OPENGL_ES_2)
    ::glVertexAttrib3fv(indx, values);
#else
    d_ptr->vertexAttrib3fv(indx, values);
#endif
}

inline void QGLFunctions::vertexAttrib4f(GLuint indx, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
#if defined(QT_OPENGL_ES_2)
    ::glVertexAttrib4f(indx, x, y, z, w);
#else
    d_ptr->vertexAttrib4f(indx, x, y, z, w);
#endif
}

inline void QGLFunctions::vertexAttrib4fv(GLuint indx, const GLfloat* values)
{
#if defined(QT_OPENGL_ES_2)
    ::glVertexAttrib4fv(indx, values);
#else
    d_ptr->vertexAttrib4fv(indx, values);
#endif
}

inline void QGLFunctions::vertexAttribPointer(GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* ptr)
{
#if defined(QT_OPENGL_ES_2)
    ::glVertexAttribPointer(indx, size, type, normalized, stride, ptr);
#else
    d_ptr->vertexAttribPointer(indx, size, type, normalized, stride, ptr);
#endif
}

#endif

inline void QGLFunctions::viewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
    ::glViewport(x, y, width, height);
}
QT_END_NAMESPACE

QT_END_HEADER

#endif
