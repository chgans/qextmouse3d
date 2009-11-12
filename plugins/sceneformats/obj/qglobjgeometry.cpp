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

#include "qglobjgeometry.h"
#include "qglpainter.h"
#include "qglmaterialcollection.h"

QT_BEGIN_NAMESPACE

QGLObjGeometry::QGLObjGeometry(QObject *parent)
    : QGLGeometry(parent)
{
}

QGLObjGeometry::~QGLObjGeometry()
{
}

QGLIndexArray QGLObjGeometry::materialIndexArray() const
{
    return materialIndex;
}

void QGLObjGeometry::setMaterialIndexArray(const QGLIndexArray& array)
{
    materialIndex = array;
}

void QGLObjGeometry::draw(QGLPainter *painter)
{
    drawRange(painter, 0, indexArray().size());
}

static void clampRange(int *first, int *count, int rangeFirst, int rangeCount)
{
    if (*first < rangeFirst) {
        *count -= (rangeFirst - *first);
        *first = rangeFirst;
        if (*count <= 0)
            return;
    } else if (*first >= (rangeFirst + rangeCount)) {
        *count = -1;
        return;
    }
    if ((*first + *count) > (rangeFirst + rangeCount)) {
        *count = (rangeFirst + rangeCount) - *first;
    }
}

void QGLObjGeometry::drawRange(QGLPainter *painter, int first, int count)
{
    int drawFirst, drawCount;

    // Select an effect to draw the geometry.
    painter->setStandardEffect(QGL::LitMaterial);

    // If there are no material indices, then draw everything.
    if (materialIndex.isEmpty()) {
        setMaterial(-1);
        QGLGeometry::draw(painter, first, count);
        return;
    }

    // Draw the leading region for which there is no specific material.
    int vertIndex = materialIndex[0];
    int matIndex = materialIndex[1];
    if (vertIndex > 0) {
        setMaterial(-1);
        drawFirst = 0;
        drawCount = vertIndex;
        clampRange(&drawFirst, &drawCount, first, count);
        if (drawCount > 0)
            QGLGeometry::draw(painter, drawFirst, drawCount);
    }

    // Draw each of the regions that has a specific material set.
    int posn = 2;
    int nextVert;
    while (posn <= materialIndex.size()) {
        if (posn < materialIndex.size())
            nextVert = materialIndex[posn];
        else
            nextVert = indexArray().size();
        QGLTexture2D *tex = palette()->texture(matIndex);
        if (tex) {
            painter->setStandardEffect(QGL::LitDecalTexture2D);
            painter->setTexture(tex);
        } else {
            painter->setStandardEffect(QGL::LitMaterial);
            painter->setTexture((QGLTexture2D *)0);
        }
        setMaterial(matIndex);
        drawFirst = vertIndex;
        drawCount = nextVert - vertIndex;
        clampRange(&drawFirst, &drawCount, first, count);
        if (drawCount > 0)
            QGLGeometry::draw(painter, drawFirst, drawCount);
        vertIndex = nextVert;
        if (posn < materialIndex.size())
            matIndex = materialIndex[posn + 1];
        posn += 2;
    }
}
