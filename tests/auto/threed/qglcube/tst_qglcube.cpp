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

#include "qglcube.h"
#include "qgldisplaylist.h"

#include "qtest_helpers_p.h"

class tst_QGLCube : public QObject
{
    Q_OBJECT
public:
    tst_QGLCube() {}
    ~tst_QGLCube() {}

private slots:
    void create();
    void size();
    void face();
    void texCoords();
};

#define QGL_CUBE_SIZE (6 * 6 * (3 + 3 + 2))
static float const cubeVertices[QGL_CUBE_SIZE] = {
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

    0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

    0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,

    0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
    0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f
};

static int findVertex(const QGLVertexArray &ary, const QVector3D &v, const QVector3D &n)
{
    QGLVertexDescription desc = ary.fields();
    int verts = desc.indexOf(QGL::Position);
    int norms = desc.indexOf(QGL::Normal);
    int result = -1;
    for (int i = 0; i < ary.vertexCount() && result == -1; ++i)
    {
        if (qFuzzyCompare(v, ary.vector3DAt(i, verts)) && qFuzzyCompare(n, ary.vector3DAt(i, norms)))
            result = i;
    }
    if (result == -1)
    {
        qDebug() << "Could not find expected vertex" << v << "with normal" << n;
    }
    return result;
}

static int findFace(const QVector3DArray &face, const QGLIndexArray &indx, const QGLVertexArray &vrts)
{
    Q_ASSERT(face.count() == 3);
    QGLVertexDescription desc = vrts.fields();
    int v = desc.indexOf(QGL::Position);
    Q_ASSERT(v != -1);
    int result = -1;
    int t[3] = { 0 };
    int cnt = 0;
    for (int i = 0; i < indx.size(); ++i)
    {
        if (cnt < 3)
        {
            // accumulate triangle indexes into t
            t[cnt++] = indx[i];
        }
        else
        {
            // for each set of 3 indexes try all three orderings to see if it matches face
            for (int j = 0; j < 3; ++j)
            {
                QVector3D a = vrts.vector3DAt(t[j], v);
                QVector3D b = vrts.vector3DAt(t[(j + 1) % 3], v);
                QVector3D c = vrts.vector3DAt(t[(j + 2) % 3], v);
                if (qFuzzyCompare(a, face[0]) &&
                    qFuzzyCompare(b, face[1]) &&
                    qFuzzyCompare(c, face[2]))
                {
                    result = i;
                    break;
                }
            }
            cnt = 0;
        }
    }
    if (result == -1)
    {
        qDebug() << "Could not find expected face" << face;
    }
    return result;
}

void tst_QGLCube::create()
{
    QGLCube cube;
    QGLDisplayList list;
    list.newSection(QGL::Faceted);
    list << cube;
    list.finalize();
    QGLVertexArray ary = list.geometry()->toVertexArray();
    QGLVertexDescription desc = ary.fields();
    int verts = desc.indexOf(QGL::Position);
    int texx = desc.indexOf(QGL::TextureCoord0);
    int norms = desc.indexOf(QGL::Normal);
    QVERIFY(verts != -1);
    QVERIFY(texx != -1);
    QVERIFY(norms != -1);
    for (int i = 0; i < QGL_CUBE_SIZE; i += 8)
    {
        const QVector3D *vdata = reinterpret_cast<const QVector3D*>(cubeVertices + i);
        const QVector2D *tdata = reinterpret_cast<const QVector2D*>(cubeVertices + i + 6);
        int ix = findVertex(ary, vdata[0], vdata[1]);
        QVERIFY(ix != -1);
        QCOMPARE(ary.vector2DAt(ix, texx), tdata[0]);
    }
}

void tst_QGLCube::size()
{
    qreal size0 = 3.2f;
    qreal size1 = 5.2f;
    QGLCube cube(size0);
    QCOMPARE(cube.size(), size0);
    {
        QGLDisplayList list;
        list.newSection(QGL::Faceted);
        list << cube;
        list.finalize();
        QGLVertexArray ary = list.geometry()->toVertexArray();
        int texx = ary.fields().indexOf(QGL::TextureCoord0);
        for (int i = 0, v = 0; i < QGL_CUBE_SIZE; ++v, i += 8)
        {
            const QVector3D *vdata = reinterpret_cast<const QVector3D*>(cubeVertices + i);
            const QVector2D *tdata = reinterpret_cast<const QVector2D*>(cubeVertices + i + 6);
            int ix = findVertex(ary, vdata[0] * size0, vdata[1]);
            QVERIFY(ix != -1);
            QCOMPARE(ary.vector2DAt(ix, texx), tdata[0]);
        }
    }
    cube.setSize(size1);
    {
        QGLDisplayList list;
        list.newSection(QGL::Faceted);
        list << cube;
        list.finalize();
        QGLVertexArray ary = list.geometry()->toVertexArray();
        int texx = ary.fields().indexOf(QGL::TextureCoord0);
        for (int i = 0, v = 0; i < QGL_CUBE_SIZE; ++v, i += 8)
        {
            const QVector3D *vdata = reinterpret_cast<const QVector3D*>(cubeVertices + i);
            const QVector2D *tdata = reinterpret_cast<const QVector2D*>(cubeVertices + i + 6);
            int ix = findVertex(ary, vdata[0] * size1, vdata[1]);
            QVERIFY(ix != -1);
            QCOMPARE(ary.vector2DAt(ix, texx), tdata[0]);
        }
    }
}

void tst_QGLCube::face()
{
    QGLCubeFace cfL(QGLCubeFace::Left);
    QGLCubeFace cfT(QGLCubeFace::Top);
    QGLCubeFace cfR(QGLCubeFace::Right);
    QGLCubeFace cfBt(QGLCubeFace::Bottom);
    QGLCubeFace cfF(QGLCubeFace::Front);
    QGLCubeFace cfBk(QGLCubeFace::Back);
    {
        QGLDisplayList list;
        list.newSection(QGL::Faceted);
        list << cfL << cfT << cfR << cfBt << cfF << cfBk;
        list.finalize();
        QGLVertexArray ary = list.geometry()->toVertexArray();
        QGLIndexArray indx = list.geometry()->indices();
        int texx = ary.fields().indexOf(QGL::TextureCoord0);
        QVector3DArray fm;
        for (int i = 0; i < QGL_CUBE_SIZE; i += 8)
        {
            const QVector3D *vdata = reinterpret_cast<const QVector3D*>(cubeVertices + i);
            const QVector2D *tdata = reinterpret_cast<const QVector2D*>(cubeVertices + i + 6);
            int ix = findVertex(ary, vdata[0], vdata[1]);
            QVERIFY(ix != -1);
            QCOMPARE(ary.vector2DAt(ix, texx), tdata[0]);
            if (fm.count() == 3)
            {
                int ifx = findFace(fm, indx, ary);
                QVERIFY(ifx != -1);
                fm.clear();
            }
            else
            {
                fm.append(vdata[0]);
            }
        }
    }
    qreal size0 = 3.2f;
    qreal size1 = 5.6f;
    QGLCubeFace::Face f = QGLCubeFace::Bottom;
    QGLCubeFace cubeFace(f, size0);
    int begin = f * 8 * 6;
    int end = (f + 1) * 8 * 6;
    QCOMPARE(cubeFace.size(), size0);
    {
        QGLDisplayList list;
        list.newSection(QGL::Faceted);
        list << cubeFace;
        list.finalize();
        QGLVertexArray ary = list.geometry()->toVertexArray();
        int texx = ary.fields().indexOf(QGL::TextureCoord0);
        for (int i = begin; i < end; i += 8)
        {
            const QVector3D *vdata = reinterpret_cast<const QVector3D*>(cubeVertices + i);
            const QVector2D *tdata = reinterpret_cast<const QVector2D*>(cubeVertices + i + 6);
            int ix = findVertex(ary, vdata[0] * size0, vdata[1]);
            QVERIFY(ix != -1);
            QCOMPARE(ary.vector2DAt(ix, texx), tdata[0]);
        }
    }
    cubeFace.setSize(size1);
    QCOMPARE(cubeFace.size(), size1);
    {
        QGLDisplayList list;
        list.newSection(QGL::Faceted);
        list << cubeFace;
        list.finalize();
        QGLVertexArray ary = list.geometry()->toVertexArray();
        int texx = ary.fields().indexOf(QGL::TextureCoord0);
        for (int i = begin; i < end; i += 8)
        {
            const QVector3D *vdata = reinterpret_cast<const QVector3D*>(cubeVertices + i);
            const QVector2D *tdata = reinterpret_cast<const QVector2D*>(cubeVertices + i + 6);
            int ix = findVertex(ary, vdata[0] * size1, vdata[1]);
            QVERIFY(ix != -1);
            QCOMPARE(ary.vector2DAt(ix, texx), tdata[0]);
        }
    }
}

void tst_QGLCube::texCoords()
{
    QGLCubeFace cfR(QGLCubeFace::Right);
    QCOMPARE(cfR.textureCoord(QGLCubeFace::TopRight), QVector2D(1.0f, 1.0f));
    QCOMPARE(cfR.textureCoord(QGLCubeFace::BottomLeft), QVector2D());

    QGLCubeFace::Face f = QGLCubeFace::Left;
    QGLCubeFace cfL(QGLCubeFace::Left);
    QVector2D leftFaceBL(0.2f, 0.2f);
    QVector2D leftFaceTR(0.8f, 0.8f);
    QVector2D leftFaceBR(0.8f, 0.2f);
    QVector2D leftFaceTL(0.2f, 0.8f);
    cfL.setTextureCoord(QGLCubeFace::BottomLeft, leftFaceBL.x(), leftFaceBL.y());
    cfL.setTextureCoord(QGLCubeFace::BottomRight, leftFaceBR.x(), leftFaceBR.y());
    cfL.setTextureCoord(QGLCubeFace::TopRight, leftFaceTR.x(), leftFaceTR.y());
    cfL.setTextureCoord(QGLCubeFace::TopLeft, leftFaceTL.x(), leftFaceTL.y());
    QCOMPARE(cfL.textureCoord(QGLCubeFace::BottomLeft), leftFaceBL);
    QCOMPARE(cfL.textureCoord(QGLCubeFace::BottomRight), leftFaceBR);
    QCOMPARE(cfL.textureCoord(QGLCubeFace::TopRight), leftFaceTR);
    QCOMPARE(cfL.textureCoord(QGLCubeFace::TopLeft), leftFaceTL);
    {
        int begin = f * 8 * 6;
        QGLDisplayList list;
        list.newSection(QGL::Faceted);
        list << cfL;
        list.finalize();
        QGLVertexArray ary = list.geometry()->toVertexArray();
        int texx = ary.fields().indexOf(QGL::TextureCoord0);

        int i = begin;  // starts at bottom right
        const QVector3D *vdata = reinterpret_cast<const QVector3D*>(cubeVertices + i);
        int ix = findVertex(ary, vdata[0], vdata[1]);
        QVERIFY(ix != -1);
        QCOMPARE(ary.vector2DAt(ix, texx), leftFaceBR);

        i = begin + 8;  // next is top right
        vdata = reinterpret_cast<const QVector3D*>(cubeVertices + i);
        ix = findVertex(ary, vdata[0], vdata[1]);
        QVERIFY(ix != -1);
        QCOMPARE(ary.vector2DAt(ix, texx), leftFaceTR);

        i = begin + 16;  // next is top left
        vdata = reinterpret_cast<const QVector3D*>(cubeVertices + i);
        ix = findVertex(ary, vdata[0], vdata[1]);
        QVERIFY(ix != -1);
        QCOMPARE(ary.vector2DAt(ix, texx), leftFaceTL);

        i = begin + 40;  // skip two dups, next is bottom left
        vdata = reinterpret_cast<const QVector3D*>(cubeVertices + i);
        ix = findVertex(ary, vdata[0], vdata[1]);
        QVERIFY(ix != -1);
        QCOMPARE(ary.vector2DAt(ix, texx), leftFaceBL);
    }

    f = QGLCubeFace::Top;
    QGLCubeFace cfT(QGLCubeFace::Top);
    QVector2D topFaceBL(1.0f, 0.0f);
    QVector2D topFaceTR(0.0f, 1.0f);
    QVector2D topFaceBR(0.0, 0.0f);
    QVector2D topFaceTL(1.0f, 1.0f);
    cfT.setTextureCoord(QGLCubeFace::BottomLeft, topFaceBL);
    cfT.setTextureCoord(QGLCubeFace::BottomRight, topFaceBR);
    cfT.setTextureCoord(QGLCubeFace::TopRight, topFaceTR);
    cfT.setTextureCoord(QGLCubeFace::TopLeft, topFaceTL);
    QCOMPARE(cfT.textureCoord(QGLCubeFace::BottomLeft), topFaceBL);
    QCOMPARE(cfT.textureCoord(QGLCubeFace::BottomRight), topFaceBR);
    QCOMPARE(cfT.textureCoord(QGLCubeFace::TopRight), topFaceTR);
    QCOMPARE(cfT.textureCoord(QGLCubeFace::TopLeft), topFaceTL);
    {
        int begin = f * 8 * 6;
        QGLDisplayList list;
        list.newSection(QGL::Faceted);
        list << cfT;
        list.finalize();
        QGLVertexArray ary = list.geometry()->toVertexArray();
        int texx = ary.fields().indexOf(QGL::TextureCoord0);

        int i = begin;  // starts at bottom right
        const QVector3D *vdata = reinterpret_cast<const QVector3D*>(cubeVertices + i);
        int ix = findVertex(ary, vdata[0], vdata[1]);
        QVERIFY(ix != -1);
        QCOMPARE(ary.vector2DAt(ix, texx), topFaceBR);

        i = begin + 8;  // next is top right
        vdata = reinterpret_cast<const QVector3D*>(cubeVertices + i);
        ix = findVertex(ary, vdata[0], vdata[1]);
        QVERIFY(ix != -1);
        QCOMPARE(ary.vector2DAt(ix, texx), topFaceTR);

        i = begin + 16;  // next is top left
        vdata = reinterpret_cast<const QVector3D*>(cubeVertices + i);
        ix = findVertex(ary, vdata[0], vdata[1]);
        QVERIFY(ix != -1);
        QCOMPARE(ary.vector2DAt(ix, texx), topFaceTL);

        i = begin + 40;  // skip two dups, next is bottom left
        vdata = reinterpret_cast<const QVector3D*>(cubeVertices + i);
        ix = findVertex(ary, vdata[0], vdata[1]);
        QVERIFY(ix != -1);
        QCOMPARE(ary.vector2DAt(ix, texx), topFaceBL);
    }
}

QTEST_APPLESS_MAIN(tst_QGLCube)

#include "tst_qglcube.moc"
