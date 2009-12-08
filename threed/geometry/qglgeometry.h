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

#ifndef QGLGEOMETRY_H
#define QGLGEOMETRY_H

#include "qglnamespace.h"
#include "qglindexarray.h"
#include "qglvertexarray.h"
#include "qbox3d.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class QGLPainter;
class QGLGeometryPrivate;
class QGLMaterialCollection;

class Q_QT3D_EXPORT QGLGeometry : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGLGeometry)
public:
    explicit QGLGeometry(QObject *parent = 0);
    virtual ~QGLGeometry();

    QGL::DrawingMode drawingMode() const;
    void setDrawingMode(QGL::DrawingMode value);

    QGLVertexArray vertexArray() const;
    void setVertexArray(const QGLVertexArray& array);

    QGLIndexArray indexArray() const;
    void setIndexArray(const QGLIndexArray& array);

    int bufferThreshold() const;
    void setBufferThreshold(int value);

    QBox3D boundingBox() const;
    void setBoundingBox(const QBox3D& value);

    virtual void draw(QGLPainter *painter);
    virtual void draw(QGLPainter *painter, int start, int count);
    virtual bool upload();

    inline int material() const;
    inline void setMaterial(int material);

    inline QGLMaterialCollection *palette() const;
    inline void setPalette(QGLMaterialCollection *palette);

protected:
    bool isModified() const;

private:
    Q_DISABLE_COPY(QGLGeometry);

    // needed for fast inlined methods
    int mMaterial;
    QGLMaterialCollection *mPalette;
};

inline int QGLGeometry::material() const
{
    return mMaterial;
}

inline void QGLGeometry::setMaterial(int material)
{
    mMaterial = material;
}

inline QGLMaterialCollection *QGLGeometry::palette() const
{
    return mPalette;
}

inline void QGLGeometry::setPalette(QGLMaterialCollection *palette)
{
    mPalette = palette;
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
