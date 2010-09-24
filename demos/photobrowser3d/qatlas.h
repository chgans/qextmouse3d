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

#ifndef QATLAS_H
#define QATLAS_H

#include <QSize>

#include "qarray.h"
#include "qgeometrydata.h"

class QImage;
class QAreaAllocator;
class QGLTexture2D;
class QGLMaterial;
class QGeometryData;

class QAtlas
{
public:
    QAtlas();
    ~QAtlas();

    QImage *data() const { return m_data; }
    void setData(QImage *data) { m_data = data; }

    QGLTexture2D *texture() const { return m_tex; }
    void setTexture(QGLTexture2D *texture) { m_tex = texture; }

    QAreaAllocator *allocator() const { return m_allocator; }
    void setAllocator(QAreaAllocator *allocator) { m_allocator = allocator; }

    QRect allocate(const QSize &size, const QImage &image, const QGL::IndexArray &indices);

    void release(QRect frame);

    void setGeometry(QGeometryData *geometry) { m_geometry = geometry; }
    QGeometryData *geometry() { return m_geometry; }

    QGLMaterial *material() { return m_material; }

private:
    QSize m_size;
    QImage *m_data;
    QAreaAllocator *m_allocator;
    QGLTexture2D *m_tex;
    QGLMaterial *m_material;
    QGeometryData *m_geometry;
};

#endif // QATLAS_H
