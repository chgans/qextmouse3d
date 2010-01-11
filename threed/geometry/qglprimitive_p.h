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

#ifndef QGLPRIMITIVE_H
#define QGLPRIMITIVE_H

#include "qglnamespace.h"
#include "qgldisplaylist.h"

class QGLPrimitive
{
public:
    QGLPrimitive(QGLDisplayList *list, const QVector3D &control = QVector3D())
        : m_control(control), m_displayList(list)
    { Q_ASSERT(list); }
    virtual ~QGLPrimitive() {}
    void setControl(const QVector3D &c) { m_control = c; }
    QVector3D control() const { return m_control; }
    void addVertex(const QVector3D &v) { m_vertices.append(v); }
    void addNormal(const QVector3D &n) { m_normals.append(n); }
    void addColor(const QColor4b &c) { m_colors.append(c); }
    void addTexCoord(const QVector2D &t) { m_texCoords.append(t); }
    void addVertexArray(const QGL::VectorArray &va) { m_vertices += va; }
    void addNormalArray(const QGL::VectorArray &na) { m_normals += na; }
    void addColorArray(const QGL::ColorArray &ca) { m_colors += ca; }
    void addTexCoordArray(const QGL::TexCoordArray &ta) { m_texCoords += ta; }
    virtual void finalize() = 0;
    virtual QString toString() = 0;
    virtual QGLDisplayList::Operation type() = 0;
    QGL::VectorArray vertices() const { return m_vertices; }
    QGL::VectorArray normals() const { return m_normals; }
    QGL::TexCoordArray texCoords() const { return m_texCoords; }
    QGL::ColorArray colors() const { return m_colors; }
    inline QGLColorModel colorModel();
    inline void setColorModel(const QGLColorModel &model);
    inline QGLTextureModel textureModel();
    inline void setTextureModel(const QGLTextureModel &model);
    inline void clear();
protected:
    inline QGLColorModel *colorModelPointer();
    inline QGLTextureModel *textureModelPointer();

    QGL::VectorArray m_vertices;
    QGL::VectorArray m_normals;
    QGL::TexCoordArray m_texCoords;
    QGL::ColorArray m_colors;
    QVector3D m_control;
    QGLDisplayList *m_displayList;
    QGLColorModel m_colorModel;
    QGLTextureModel m_textureModel;
};

inline QGLColorModel QGLPrimitive::colorModel()
{
    if (!m_colorModel.isNull())
        return m_colorModel;
    if (m_colors.count() > 0)
    {
        m_colorModel.setBottomLeft(m_colors[0]);
        if (m_colors.count() > 1)
        {
            m_colorModel.setBottomRight(m_colors[1]);
            if (m_colors.count() > 2)
            {
                m_colorModel.setTopRight(m_colors[2]);
                if (m_colors.count() > 3)
                    m_colorModel.setTopLeft(m_colors[3]);
            }
        }
    }
    return m_colorModel;
}

inline void QGLPrimitive::setColorModel(const QGLColorModel &model)
{
    m_colorModel = model;
}

inline QGLTextureModel QGLPrimitive::textureModel()
{
    if (!m_textureModel.isNull())
        return m_textureModel;
    QGLTextureModel tex;
    if (m_texCoords.count() > 0)
    {
        tex.setBottomLeft(m_texCoords[0]);
        if (m_texCoords.count() > 1)
        {
            tex.setBottomRight(m_texCoords[1]);
            if (m_texCoords.count() > 2)
            {
                tex.setTopRight(m_texCoords[2]);
                if (m_texCoords.count() > 3)
                    tex.setTopLeft(m_texCoords[3]);
            }
        }
    }
    return tex;
}

inline void QGLPrimitive::setTextureModel(const QGLTextureModel &model)
{
    m_textureModel = model;
}

inline void QGLPrimitive::clear()
{
    m_vertices.clear();
    m_normals.clear();
    m_texCoords.clear();
    m_colors.clear();
    m_control = QVector3D();
    m_textureModel = QGLTextureModel();
    m_colorModel = QGLColorModel();
}

inline QGLColorModel *QGLPrimitive::colorModelPointer()
{
    return &m_colorModel;
}

inline QGLTextureModel *QGLPrimitive::textureModelPointer()
{
    return &m_textureModel;
}

#endif // QGLPRIMITIVE_H
