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
    inline QGLOperation(QGLDisplayList *, QGLDisplayList::Operation);
    inline QGLOperation(QGLDisplayList *, QGLDisplayList::Operation,
                        const QVector3D &control);
    inline ~QGLOperation();
    inline void setControl(const QVector3D &);
    inline QVector3D control() const;
    inline void setFlags(QGLDisplayList::OperationFlags);
    inline QGLDisplayList::OperationFlags flags() const;
    inline void addVertex(const QVector3D &);
    inline void addNormal(const QVector3D &);
    inline void addColor(const QColor4b &);
    inline void addTexCoord(const QVector2D &);
    inline void addVertexArray(const QGL::VectorArray &);
    inline void addNormalArray(const QGL::VectorArray &);
    inline void addColorArray(const QGL::ColorArray &);
    inline void addTexCoordArray(const QGL::TexCoordArray &);
    inline void addTextureModel(const QGLTextureModel &);
    inline void addColorModel(const QGLColorModel &);
    inline void end();
    inline QGL::VectorArray endResult();
private:
    QGLDisplayList *m_list;
};

inline QGLOperation::QGLOperation(QGLDisplayList *list, QGLDisplayList::Operation op)
    : m_list(list)
{
    m_list->begin(op);
}

inline QGLOperation::QGLOperation(QGLDisplayList *list, QGLDisplayList::Operation op,
                                  const QVector3D &control)
    : m_list(list)
{
    m_list->begin(op);
    m_list->setControl(control);
}

inline QGLOperation::~QGLOperation()
{
    m_list->end();
}

inline void QGLOperation::setControl(const QVector3D &control)
{
    m_list->setControl(control);
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

inline void QGLOperation::addTexCoord(const QVector2D &texCoord)
{
    m_list->addTexCoord(texCoord);
}

inline void QGLOperation::addVertexArray(const QGL::VectorArray &vertices)
{
    m_list->addVertexArray(vertices);
}

inline void QGLOperation::addNormalArray(const QGL::VectorArray &normals)
{
    m_list->addNormalArray(normals);
}

inline void QGLOperation::addColorArray(const QGL::ColorArray &colors)
{
    m_list->addColorArray(colors);
}

inline void QGLOperation::addTexCoordArray(const QGL::TexCoordArray &texCoords)
{
    m_list->addTexCoordArray(texCoords);
}

inline void QGLOperation::end()
{
    m_list->end();
}

inline QGL::VectorArray QGLOperation::endResult()
{
    return m_list->endResult();
}


inline QVector3D QGLOperation::control() const
{
    return m_list->control();
}

inline void QGLOperation::setFlags(QGLDisplayList::OperationFlags flags)
{
    m_list->setFlags(flags);
}

inline void QGLOperation::addTextureModel(const QGLTextureModel &model)
{
    m_list->addTextureModel(model);
}

inline void QGLOperation::addColorModel(const QGLColorModel &model)
{
    m_list->addColorModel(model);
}

inline QGLDisplayList::OperationFlags QGLOperation::flags() const
{
    return m_list->flags();
}

inline QGLOperation &operator<<(QGLOperation &op, const QGL::VectorArray &ary)
{
    op.addVertexArray(ary);
    return op;
}

inline QGLOperation &operator<<(QGLOperation &op, const QGL::TexCoordArray &ary)
{
    op.addTexCoordArray(ary);
    return op;
}

inline QGLOperation &operator<<(QGLOperation &op, const QGL::ColorArray &ary)
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
    op.addTexCoord(texCoord);
    return op;
}

inline QGLOperation &operator<<(QGLOperation &op, const QColor4b &color)
{
    op.addColor(color);
    return op;
}

inline QGLOperation &operator>>(QGLOperation &op, QGL::VectorArray &ary)
{
    ary += op.endResult();
    return op;
}

inline QGLOperation &operator<<(QGLOperation &op,
                                const QGLDisplayList::OperationFlags &flags)
{
    op.setFlags(flags | op.flags());
    return op;
}

inline QGLOperation &operator>>(QGLOperation &op,
                                QGLDisplayList::OperationFlags &flags)
{
    flags = op.flags();
    return op;
}

inline QGLOperation &operator<<(QGLOperation &op, const QGLTextureModel &model)
{
    op.addTextureModel(model);
    return op;
}

inline QGLOperation &operator<<(QGLOperation &op, const QGLColorModel &model)
{
    op.addColorModel(model);
    return op;
}

#endif // QGLOPERATION_H
