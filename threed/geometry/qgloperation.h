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

#ifndef QGLOPERATION_H
#define QGLOPERATION_H

#include "qgldisplaylist.h"

class QGLOperation
{
public:
    inline QGLOperation(QGLDisplayList *, QGL::Operation);
    inline ~QGLOperation();
    inline void setControl(const QVector3D &);
    inline QVector3D control() const;
    inline void setFlags(QGL::OperationFlags);
    inline QGL::OperationFlags flags() const;

    inline void addVertex(const QVector3D &);
    inline void addNormal(const QVector3D &);
    inline void addColor(const QColor4b &);
    inline void addTexCoord(const QVector2D &, QGL::VertexAttribute);
    inline void addAttribute(const QVector3D &, QGL::VertexAttribute);

    inline void addVertexArray(const QDataArray<QVector3D> &);
    inline void addNormalArray(const QDataArray<QVector3D> &);
    inline void addColorArray(const QDataArray<QColor4b> &);
    inline void addTexCoordArray(const QDataArray<QVector2D> &, QGL::VertexAttribute);
    inline void addAttributeArray(const QDataArray<QVector3D> &, QGL::VertexAttribute);

    inline void end();
private:
    QGLDisplayList *m_list;
};

inline QGLOperation::QGLOperation(QGLDisplayList *list, QGL::Operation op)
    : m_list(list)
{
    m_list->begin(op);
}

inline QGLOperation::~QGLOperation()
{
    m_list->end();
}

inline void QGLOperation::setFlags(QGL::OperationFlags flags)
{
    m_list->setFlags(flags);
}

inline QGL::OperationFlags QGLOperation::flags() const
{
    return m_list->flags();
}

inline void QGLOperation::addVertex(const QVector3D &vertex)
{
    m_list->addVertex(vertex);
}

inline void QGLOperation::addNormal(const QVector3D &normal)
{
    m_list->addNormal(normal);
}

inline void QGLOperation::addColor(const QColor4b &color)
{
    m_list->addColor(color);
}

inline void QGLOperation::addTexCoord(const QVector2D &texCoord,
                                      QGL::VertexAttribute attr)
{
    m_list->addTexCoord(texCoord, attr);
}

inline void QGLOperation::addVertexArray(const QDataArray<QVector3D> &vertices)
{
    m_list->addVertexArray(vertices);
}

inline void QGLOperation::addNormalArray(const QDataArray<QVector3D> &normals)
{
    m_list->addNormalArray(normals);
}

inline void QGLOperation::addColorArray(const QDataArray<QColor4b> &colors)
{
    m_list->addColorArray(colors);
}

inline void QGLOperation::addTexCoordArray(const QDataArray<QVector2D> &texCoords,
                                           QGL::VertexAttribute attr)
{
    m_list->addTexCoordArray(texCoords, attr);
}

inline void QGLOperation::end()
{
    m_list->end();
}

inline QGLOperation &operator<<(QGLOperation &op, const QDataArray<QVector3D> &ary)
{
    op.addVertexArray(ary);
    return op;
}

inline QGLOperation &operator<<(QGLOperation &op, const QDataArray<QVector2D> &ary)
{
    op.addTexCoordArray(ary, QGL::TextureCoord0);
    return op;
}

inline QGLOperation &operator<<(QGLOperation &op, const QDataArray<QColor4b> &ary)
{
    op.addColorArray(ary);
    return op;
}

inline QGLOperation &operator<<(QGLOperation &op, const QVector3D &vertex)
{
    op.addVertex(vertex);
    return op;
}

inline QGLOperation &operator<<(QGLOperation &op, const QVector2D &texCoord)
{
    op.addTexCoord(texCoord, QGL::TextureCoord0);
    return op;
}

inline QGLOperation &operator<<(QGLOperation &op, const QColor4b &color)
{
    op.addColor(color);
    return op;
}

inline QGLOperation &operator<<(QGLOperation &op,
                                const QGL::OperationFlags &flags)
{
    op.setFlags(flags | op.flags());
    return op;
}

inline QGLOperation &operator>>(QGLOperation &op,
                                QGL::OperationFlags &flags)
{
    flags = op.flags();
    return op;
}

#endif // QGLOPERATION_H
