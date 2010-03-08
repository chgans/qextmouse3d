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
#include "qgloperation.h"

class tst_QGLOperation : public QObject
{
    Q_OBJECT
public:
    tst_QGLOperation() {}
    ~tst_QGLOperation() {}

private slots:
    void basicAndFlags();
    void itemAdds();
    void collectionAdds();
};

void tst_QGLOperation::basicAndFlags()
{
    QGLDisplayList list;
    list.newSection();
    QCOMPARE(list.currentPrimitive(), (QGLPrimitive*)0);
    QVERIFY(list.currentOperation() == QGL::NO_OP);
    {
        QGLOperation op(&list, QGL::TRIANGLE);
        QVERIFY(list.currentOperation() == QGL::TRIANGLE);
        QVERIFY(list.currentPrimitive() != (QGLPrimitive*)0);
        op.setFlags(QGL::USE_VERTEX_0_AS_CTR);
        op.setFlags(QGL::NO_CLOSE_PATH);
        // setFlags replaces current setting
        QVERIFY(list.currentPrimitive()->flags() == QGL::NO_CLOSE_PATH);
        op << QGL::USE_VERTEX_0_AS_CTR;
        // setting flags via << accumulates instead of replacing
        QVERIFY(list.currentPrimitive()->flags() ==
                (QGL::NO_CLOSE_PATH | QGL::USE_VERTEX_0_AS_CTR));
    }
    QCOMPARE(list.currentPrimitive(), (QGLPrimitive*)0);
    QVERIFY(list.currentOperation() == QGL::NO_OP);
}

void tst_QGLOperation::itemAdds()
{
    QVector3D a(-1.0f, -1.0f, 0.0f);
    QVector3D b(1.0f, -1.0f, 0.0f);
    QVector3D c(1.0f, 1.0f, 0.0f);
    QVector3D d(-1.0f, 1.0f, 0.0f);
    QGLDisplayList list;
    list.newSection();
    QGLPrimitive *p = 0;
    {
        QGLOperation op(&list, QGL::TRIANGLE);
        p = list.currentPrimitive();
        op.addVertex(a);
        op.addVertex(b);
        op << c << d;
        QCOMPARE(p->vertex(0), a);
        QCOMPARE(p->vertex(1), b);
        QCOMPARE(p->vertex(2), c);
        QCOMPARE(p->vertex(3), d);
        QCOMPARE(p->fields(), QGL::fieldMask(QGL::Position));
        QCOMPARE(p->count(), 4);
        // we know from the QGLDisplayList tests that the primitives
        // get correctly posted to the section - no need to test that
        // again here
    }
    QCOMPARE(list.currentPrimitive(), (QGLPrimitive*)0);
    QVERIFY(list.currentOperation() == QGL::NO_OP);

    // do some normals
    list.newSection();
    QVector3D n1(1.0f, 0.0f, 0.0f);
    QVector3D n2(-1.0f, 0.0f, 0.0f);
    QVector3D n3(0.0f, 1.0f, 0.0f);
    QVector3D n4(0.0f, -1.0f, 0.0f);
    {
        QGLOperation op(&list, QGL::QUAD);
        QVERIFY(list.currentOperation() == QGL::QUAD);
        p = list.currentPrimitive();
        op << a << b << c << d;
        op.addNormal(n1);    // cannot do wth shove operator for now
        op.addNormal(n2);
        op.addNormal(n3);
        op.addNormal(n4);
        QCOMPARE(p->normal(0), n1);
        QCOMPARE(p->normal(1), n2);
        QCOMPARE(p->normal(2), n3);
        QCOMPARE(p->normal(3), n4);
        QCOMPARE(p->fields(),
                 QGL::fieldMask(QGL::Position) | QGL::fieldMask(QGL::Normal));
        QCOMPARE(p->count(QGL::Position), 4);
        QCOMPARE(p->count(QGL::Normal), 4);
    }

    list.newSection();
    {
        QGLOperation op(&list, QGL::QUAD);
        p = list.currentPrimitive();
        op << a << b << c << d;
        op.addColor(Qt::red);
        op.addColor(Qt::green);
        op.addColor(Qt::blue);
        op.addColor(Qt::yellow);
        QCOMPARE(p->color(0), QColor4B(Qt::red));
        QCOMPARE(p->color(1), QColor4B(Qt::green));
        QCOMPARE(p->color(2), QColor4B(Qt::blue));
        QCOMPARE(p->color(3), QColor4B(Qt::yellow));
        QCOMPARE(p->fields(),
                 QGL::fieldMask(QGL::Position) | QGL::fieldMask(QGL::Color));
        QCOMPARE(p->count(QGL::Position), 4);
        QCOMPARE(p->count(QGL::Color), 4);
    }

    list.newSection();
    QVector2D t0(0.1f, 0.2f);
    QVector2D t1(0.3f, 0.4f);
    QVector2D t2(0.5f, 0.6f);
    QVector2D t3(0.7f, 0.8f);
    {
        QGLOperation op(&list, QGL::QUAD);
        p = list.currentPrimitive();
        op << a << b << c << d;
        op.addTexCoord(t0);
        op.addTexCoord(t1);
        op << t2 << t3;
        QCOMPARE(p->texCoord(0), t0);
        QCOMPARE(p->texCoord(1), t1);
        QCOMPARE(p->texCoord(2), t2);
        QCOMPARE(p->texCoord(3), t3);
        QCOMPARE(p->fields(),
                 QGL::fieldMask(QGL::Position) | QGL::fieldMask(QGL::TextureCoord0));
        QCOMPARE(p->count(QGL::Position), 4);
        QCOMPARE(p->count(QGL::TextureCoord0), 4);
    }

    list.newSection();
    {
        QGLOperation op(&list, QGL::QUAD);
        p = list.currentPrimitive();
        op << a << b << c << d;
        op.addTexCoord(t0, QGL::TextureCoord4);
        op.addTexCoord(t1, QGL::TextureCoord4);
        op.addTexCoord(t2, QGL::TextureCoord4);
        op.addTexCoord(t3, QGL::TextureCoord4);
        QCOMPARE(p->texCoord(0, QGL::TextureCoord4), t0);
        QCOMPARE(p->texCoord(1, QGL::TextureCoord4), t1);
        QCOMPARE(p->texCoord(2, QGL::TextureCoord4), t2);
        QCOMPARE(p->texCoord(3, QGL::TextureCoord4), t3);
        QCOMPARE(p->fields(),
                 QGL::fieldMask(QGL::Position) | QGL::fieldMask(QGL::TextureCoord4));
        QCOMPARE(p->count(QGL::Position), 4);
        QCOMPARE(p->count(QGL::TextureCoord4), 4);
    }

    list.newSection();
    {
        QGLOperation op(&list, QGL::QUAD);
        p = list.currentPrimitive();
        op << a << b << c << d;
        op.addAttribute(t0);
        op.addAttribute(t1);
        op.addAttribute(t2);
        op.addAttribute(t3);
        op.addAttribute(n1, QGL::CustomVertex4);
        op.addAttribute(n2, QGL::CustomVertex4);
        op.addAttribute(n3, QGL::CustomVertex4);
        op.addAttribute(n4, QGL::CustomVertex4);
        QCOMPARE(p->vector3DAttribute(0), QVector3D(t0));
        QCOMPARE(p->vector3DAttribute(1), QVector3D(t1));
        QCOMPARE(p->vector3DAttribute(2), QVector3D(t2));
        QCOMPARE(p->vector3DAttribute(3), QVector3D(t3));
        QCOMPARE(p->vector3DAttribute(0, QGL::CustomVertex4), n1);
        QCOMPARE(p->vector3DAttribute(1, QGL::CustomVertex4), n2);
        QCOMPARE(p->vector3DAttribute(2, QGL::CustomVertex4), n3);
        QCOMPARE(p->vector3DAttribute(3, QGL::CustomVertex4), n4);
        QCOMPARE(p->fields(), QGL::fieldMask(QGL::Position) |
                 QGL::fieldMask(QGL::CustomVertex0) | QGL::fieldMask(QGL::CustomVertex4));
        QCOMPARE(p->count(QGL::Position), 4);
        QCOMPARE(p->count(QGL::CustomVertex0), 4);
        QCOMPARE(p->count(QGL::CustomVertex4), 4);
    }
}

void tst_QGLOperation::collectionAdds()
{

}

QTEST_APPLESS_MAIN(tst_QGLOperation)

#include "tst_qgloperation.moc"
