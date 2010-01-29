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

#include <QtTest/QtTest>
#include <QtOpenGL/qgl.h>
#include "qglvertexbuffer.h"
#include "qvectorarray.h"

class tst_QGLVertexBuffer : public QObject
{
    Q_OBJECT
public:
    tst_QGLVertexBuffer() {}
    ~tst_QGLVertexBuffer() {}

private slots:
    void create();
};

void tst_QGLVertexBuffer::create()
{
    // Interleave 4 positions and texture co-ordinates, which will
    // result in the numbers 1..20 ending up in the GL server's buffer.
    QVector3DArray positions;
    positions.append(1.0f, 2.0f, 3.0f);
    positions.append(6.0f, 7.0f, 8.0f);
    positions.append(11.0f, 12.0f, 13.0f);
    positions.append(16.0f, 17.0f, 18.0f);

    QVector2DArray texCoords;
    texCoords.append(4.0f, 5.0f);
    texCoords.append(9.0f, 10.0f);
    texCoords.append(14.0f, 15.0f);
    texCoords.append(19.0f, 20.0f);

    QGLVertexBuffer buffer;
    buffer.addAttribute(QGL::Position, positions);
    buffer.addAttribute(QGL::TextureCoord0, texCoords);

    QVERIFY(!buffer.isUploaded());
    QVERIFY(buffer.packingHint() == QGLVertexBuffer::Interleave);
    QVERIFY(buffer.usagePattern() == QGLBuffer::StaticDraw);

    // Create a context to upload into.  We populate the QGLVertexBuffer
    // with data before doing this to ensure that the client-side part of
    // the buffers can be created at application startup time before an
    // actual OpenGL context exists.
    QGLWidget glw;
    glw.makeCurrent();

    // Upload the buffer and bail out if we couldn't upload it
    // (i.e. vertex buffers are not supported in the GL server).
    if (!buffer.upload()) {
        QVERIFY(!buffer.isUploaded());
        return;
    }
    QVERIFY(buffer.isUploaded());

    // Verify the contents of the buffer in the GL server if we can map it.
    QVERIFY(buffer.bind());
    QCOMPARE(buffer.buffer()->size(), int(sizeof(float) * 20));
    float *mapped = reinterpret_cast<float *>
        (buffer.buffer()->map(QGLBuffer::ReadOnly));
    if (mapped) {
        for (int index = 0; index < 20; ++index)
            QCOMPARE(mapped[index], float(index + 1));
    }
    buffer.release();
}

QTEST_MAIN(tst_QGLVertexBuffer)

#include "tst_qglvertexbuffer.moc"
