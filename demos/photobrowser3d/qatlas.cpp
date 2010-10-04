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

#include "qatlas.h"
#include "qareaallocator_p.h"
#include "qgltexture2d.h"
#include "qglpainter.h"
#include "qgeometrydata.h"
#include "qglmaterial.h"

#include <QImage>
#include <QThread>

QAtlas::QAtlas()
    : m_size(1024, 1024)
    , m_data(new QImage(m_size, QImage::Format_ARGB32))
    , m_allocator(new QSimpleAreaAllocator(m_size))
    , m_tex(new QGLTexture2D)
    , m_material(new QGLMaterial)
    , m_geometry(0)
{
    qDebug() << "created atlas texture" << m_tex << QThread::currentThread();
    // show errors in red
    m_data->fill(qRgb(255, 0, 0));
    m_tex->setImage(*m_data);
    m_material->setTexture(m_tex, 1);
}

QAtlas::~QAtlas()
{
    delete m_data;
    delete m_allocator;
    delete m_tex;
}

QRect QAtlas::allocate(const QSize &size, const QImage &image, const QGL::IndexArray &indices)
{
    qDebug() << "QAtlas::allocate" << size << "out of" << m_size;
    QRect a = m_allocator->allocate(size);
    if (a.isEmpty())
    {
        qWarning("QAtlas::allocate: overflowed");
        return a;
    }
    QPainter painter;
    painter.begin(m_data);
    painter.drawImage(a, image);
    painter.end();
    m_tex->setImage(*m_data);
    m_geometry.texCoord(indices.at(0), QGL::TextureCoord1) = QVector2D(a.left(), a.bottom());
    m_geometry.texCoord(indices.at(1), QGL::TextureCoord1) = QVector2D(a.right(), a.bottom());
    m_geometry.texCoord(indices.at(2), QGL::TextureCoord1) = QVector2D(a.right(), a.top());
    m_geometry.texCoord(indices.at(3), QGL::TextureCoord1) = QVector2D(a.left(), a.top());
    qDebug() << "QAtlas::allocate";
    m_data->save("atlas_allocate.png");
    return a;
}

void QAtlas::release(QRect frame)
{
    m_allocator->release(frame);
}

Q_GLOBAL_STATIC(QAtlas, atlasInstance);

QAtlas *QAtlas::instance()
{
    return atlasInstance();
}
