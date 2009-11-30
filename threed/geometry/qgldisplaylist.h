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

#ifndef QGLDISPLAYLIST_H
#define QGLDISPLAYLIST_H

#include <QtCore/qobject.h>
#include <QtCore/qvector.h>
#include <QtCore/qlist.h>
#include <QtGui/qvector3d.h>
#include <QtOpenGL/qgl.h>

#include "qgltexturespecifier.h"
#include "qglgeometry.h"
#include "qglnamespace.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class QGLSection;
class QGLMaterialCollection;
class QGLDisplayListPrivate;
class QGLPainter;

class Q_QT3D_EXPORT QGLDisplayList : public QGLGeometry
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGLDisplayList);
public:
    // useful type definitions
    typedef QVector<QVector3D> VectorArray;
    typedef QVector<QVector2D> TexCoordArray;
    typedef QVector<QColor4b> ColorArray;
    typedef QVector<int> IndexArray;

    QGLDisplayList(QObject *parent = 0, QGLMaterialCollection *materials = 0);
    virtual ~QGLDisplayList();

    // drawing
    void draw(QGLPainter *painter);

    // section management
    QGLSection *newSection(QGL::Smoothing = QGL::Smooth);
    QGLSection *currentSection();
    QList<QGLSection*> &sections();

    // geometry building
    void reserve(int n);
    void addTriangle(const QVector3D &a, const QVector3D &b,
                     const QVector3D &c, const QVector3D &n = QVector3D(),
                     const QGLTextureSpecifier &textureModel = QGLTextureSpecifier(),
                     bool inverted = false);
    void addQuad(const QVector3D &a, const QVector3D &b,
                 const QVector3D &c, const QVector3D &d,
                 const QGLTextureSpecifier &textureModel = QGLTextureSpecifier());
    void addTriangleFan(const QVector3D &center, const VectorArray &edges,
                        const QGLTextureSpecifier &textureModel = QGLTextureSpecifier());
    void addTriangulatedFace(const QVector3D &center, const VectorArray &edges,
                        const QGLTextureSpecifier &textureModel = QGLTextureSpecifier());
    QVector<QVector3D> extrude(const VectorArray &edges,
                               const QVector3D &dir = QVector3D(),
                               const QGLTextureSpecifier &textureModel = QGLTextureSpecifier());

    // data accessors
    inline QGLMaterialCollection *materials() const;
    inline VectorArray vertexArray() const;
    inline VectorArray normalArray() const;
    inline TexCoordArray texCoordArray() const;
    inline ColorArray colorArray() const;
    inline IndexArray indexArray() const;
    int indexOf(const QVector3D &a, QGLSection *section = 0) const;
    void setVertex(int, const QVector3D &v);
    void setNormal(int, const QVector3D &n);
    void setTexCoord(int, const QVector2D &t);
    void setColor(int, const QColor4b &c);

    // state accessors
    inline int count() const;
    inline bool hasColors() const;
    inline bool hasNormals() const;
    inline bool hasTexCoords() const;
protected:
    virtual void appendSmooth(const QVector3D &a, const QVector3D &n,
                              const QVector2D &t = QGLTextureSpecifier::InvalidTexCoord);
    virtual void appendFaceted(const QVector3D &a, const QVector3D &n,
                               const QVector2D &t = QGLTextureSpecifier::InvalidTexCoord);
    virtual void appendColor(const QVector3D &a, const QColor4b &c,
                             const QVector2D &t = QGLTextureSpecifier::InvalidTexCoord);
    virtual int updateTexCoord(int position, const QVector2D &t);
    virtual void append(const QVector3D &a,
                        const QVector2D &t = QGLTextureSpecifier::InvalidTexCoord);

    virtual void finalize();
    virtual void loadArrays(QGLPainter *painter);

    virtual void addSection(QGLSection *section);
    virtual void draw(QGLPainter *painter, int start, int count);
private:
    Q_DISABLE_COPY(QGLDisplayList);

    QGLVertexArray toVertexArray() const;
    friend class QGLSection;

#ifndef QT_NO_DEBUG_STREAM
    friend QDebug operator<<(QDebug, const QGLSection &);
#endif
    bool m_hasColors;
    bool m_hasNormals;
    bool m_hasTexCoords;

    QGLMaterialCollection *m_materials;
    VectorArray m_vertices;
    VectorArray m_normals;
    TexCoordArray m_texCoords;
    ColorArray m_colors;
    IndexArray m_indices;
    QList<QGLSection *> m_sections;
    QGLSection *m_currentSection;
};


inline QGLSection *QGLDisplayList::currentSection()
{
    return m_currentSection;
}

inline QList<QGLSection*> &QGLDisplayList::sections()
{
    return m_sections;
}

inline QGLMaterialCollection *QGLDisplayList::materials() const
{
    return m_materials;
}

inline QGLDisplayList::VectorArray QGLDisplayList::vertexArray() const
{
    return m_vertices;
}

inline QGLDisplayList::VectorArray QGLDisplayList::normalArray() const
{
    return m_normals;
}

inline QGLDisplayList::TexCoordArray QGLDisplayList::texCoordArray() const
{
    return m_texCoords;
}

inline QGLDisplayList::ColorArray QGLDisplayList::colorArray() const
{
    return m_colors;
}

inline QGLDisplayList::IndexArray QGLDisplayList::indexArray() const
{
    return m_indices;
}

inline int QGLDisplayList::count() const
{
    return m_indices.count();
}

inline bool QGLDisplayList::hasColors() const
{
    return m_hasColors;
}

inline bool QGLDisplayList::hasNormals() const
{
    return m_hasNormals;
}

inline bool QGLDisplayList::hasTexCoords() const
{
    return m_hasTexCoords;
}

inline void QGLDisplayList::draw(QGLPainter *painter, int start, int count)
{
    QGLGeometry::draw(painter, start, count);
}


QT_END_NAMESPACE

QT_END_HEADER

#endif // QGLDISPLAYLIST_H
