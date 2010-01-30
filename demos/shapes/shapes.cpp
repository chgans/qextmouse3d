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

#include "qglpainter.h"
#include "qgldisplaylist.h"
#include "qglcube.h"
#include "qglteapot.h"
#include "qglcamera.h"
#include "qgltexture2d.h"
#include <QApplication>
#include <QImage>
#include <QPainter>
#include <QDebug>

class ShapesWidget : public QGLWidget
{
    Q_OBJECT
public:
    ShapesWidget(QWidget *parent = 0);
    ~ShapesWidget();

protected:
    void initializeGL();
    void paintGL();

private:
    void paintPoints(QGLPainter *painter, const QRect& rect);
    void paintLines(QGLPainter *painter, const QRect& rect);
    void paintLineStrip(QGLPainter *painter, const QRect& rect);
    void paintLineLoop(QGLPainter *painter, const QRect& rect);
    void paintTriangles(QGLPainter *painter, const QRect& rect);
    void paintTriangleStrip(QGLPainter *painter, const QRect& rect);
    void paintTriangleFan(QGLPainter *painter, const QRect& rect);
    void paintCube(QGLPainter *painter, const QRect& rect);
    void paintTeapot(QGLPainter *painter, const QRect& rect);
    void drawText(QGLPainter *painter, const QRect& posn, const QString& str);

    static QVector2DArray basicPoints(const QRect& rect);

    QGLCamera camera;
    QGLDisplayList cube;
    QGLTeapot teapot;
    QGLLightModel oneSidedModel;
    QGLLightModel twoSidedModel;
};

ShapesWidget::ShapesWidget(QWidget *parent)
    : QGLWidget(parent)
{
    setWindowTitle(tr("GL Primitive Shapes"));
    oneSidedModel.setModel(QGLLightModel::OneSided);
    twoSidedModel.setModel(QGLLightModel::TwoSided);
}

ShapesWidget::~ShapesWidget()
{
}

void ShapesWidget::initializeGL()
{
    QGLPainter painter(this);

    painter.setCullFaces(QGL::CullDisabled);

    painter.setLightEnabled(0, true);
    painter.setLightModel(&twoSidedModel);
    painter.setFaceColor(QGL::FrontFaces, QColor(170, 202, 0));
    painter.setFaceColor(QGL::BackFaces, QColor(202, 170, 0));

    cube << QGLCube();
}

void ShapesWidget::paintGL()
{
    QGLPainter painter(this);

    painter.setViewport(size());
    painter.clear();

    painter.setDepthTestingEnabled(false);

    painter.projectionMatrix().setToIdentity();
    painter.projectionMatrix().ortho(rect());
    painter.modelViewMatrix().setToIdentity();

    QRect wrect = rect();
    int boxw = wrect.width() / 3;
    int boxh = wrect.height() / 3;

    paintLines(&painter, QRect(0, 0, boxw, boxh));
    paintLineStrip(&painter, QRect(boxw, 0, boxw, boxh));
    paintLineLoop(&painter, QRect(boxw * 2, 0, boxw, boxh));
    paintTriangles(&painter, QRect(0, boxh, boxw, boxh));
    paintTriangleStrip(&painter, QRect(boxw, boxh, boxw, boxh));
    paintTriangleFan(&painter, QRect(boxw * 2, boxh, boxw, boxh));
    paintPoints(&painter, QRect(0, boxh * 2, boxw, boxh));

    painter.setDepthTestingEnabled(true);

    paintCube(&painter, QRect(boxw, boxh * 2, boxw, boxh));
    paintTeapot(&painter, QRect(boxw * 2, boxh * 2, boxw, boxh));
}

QVector2DArray ShapesWidget::basicPoints(const QRect& rect)
{
    QVector2DArray vertices;
    int step = qMin(rect.width() / 6, rect.height() / 6);
    int midx = rect.x() + rect.width() / 2;
    int midy = rect.y() + rect.height() / 2;
    vertices.append(midx - step * 2, midy - step);
    vertices.append(midx + step, midy - step * 2);
    vertices.append(midx, midy - step);
    vertices.append(midx + step * 2 + step / 2, midy - step / 2);
    vertices.append(midx - step, midy);
    vertices.append(midx + step * 2, midy + step);
    vertices.append(midx - step, midy + step);
    vertices.append(midx + step * 2, midy);
    return vertices;
}

void ShapesWidget::paintPoints(QGLPainter *painter, const QRect& rect)
{
    painter->setStandardEffect(QGL::FlatColor);
    painter->setColor(QColor(170, 202, 0));
    painter->setPointSize(1);

    painter->setVertexAttribute(QGL::Position, basicPoints(rect));
    painter->draw(QGL::Points, 8);

    drawText(painter, rect, tr("Points"));
}

void ShapesWidget::paintLines(QGLPainter *painter, const QRect& rect)
{
    painter->setLineWidth(1);

    painter->setVertexAttribute(QGL::Position, basicPoints(rect));
    painter->draw(QGL::Lines, 8);

    drawText(painter, rect, tr("Lines"));
}

void ShapesWidget::paintLineStrip(QGLPainter *painter, const QRect& rect)
{
    painter->setVertexAttribute(QGL::Position, basicPoints(rect));
    painter->draw(QGL::LineStrip, 8);

    drawText(painter, rect, tr("Line strip"));
}

void ShapesWidget::paintLineLoop(QGLPainter *painter, const QRect& rect)
{
    painter->setVertexAttribute(QGL::Position, basicPoints(rect));
    painter->draw(QGL::LineLoop, 8);

    drawText(painter, rect, tr("Line loop"));
}

void ShapesWidget::paintTriangles(QGLPainter *painter, const QRect& rect)
{
    painter->setStandardEffect(QGL::LitMaterial);

    QVector2DArray vertices;
    int step = qMin(rect.width() / 8, rect.height() / 8);
    int midx = rect.x() + rect.width() / 2;
    int midy = rect.y() + rect.height() / 2;

    vertices.append(midx - step * 3, midy);
    vertices.append(midx - step * 3, midy + step * 2);
    vertices.append(midx - step, midy + step * 2);

    vertices.append(midx - step * 2, midy - step);
    vertices.append(midx + step / 2, midy + step * 2);
    vertices.append(midx + step * 3, midy - step);

    painter->setVertexAttribute(QGL::Position, vertices);
    painter->setCommonNormal(QVector3D(0.0f, 0.0f, 1.0f));
    painter->draw(QGL::Triangles, 6);

    drawText(painter, rect, tr("Triangles"));
}

void ShapesWidget::paintTriangleStrip(QGLPainter *painter, const QRect& rect)
{
    painter->setStandardEffect(QGL::LitMaterial);

    QVector2DArray vertices;
    int step = qMin(rect.width() / 8, rect.height() / 8);
    int midx = rect.x() + rect.width() / 2;
    int midy = rect.y() + rect.height() / 2;

    vertices.append(midx - step * 3, midy - step * 2);
    vertices.append(midx - step * 3, midy + step);
    vertices.append(midx - step, midy - step * 2);
    vertices.append(midx, midy + step);
    vertices.append(midx + step, midy - step * 2);
    vertices.append(midx + step * 2, midy + step * 2);

    painter->setVertexAttribute(QGL::Position, vertices);
    painter->setCommonNormal(QVector3D(0.0f, 0.0f, 1.0f));
    painter->draw(QGL::TriangleStrip, 6);

    // Overpaint some lines to show the triangle boundaries.
    painter->setStandardEffect(QGL::FlatColor);
    painter->setColor(QColor(202, 170, 0));
    painter->setVertexAttribute(QGL::Position, vertices);
    painter->draw(QGL::LineStrip, 4, 1);

    drawText(painter, rect, tr("Triangle strip"));
}

void ShapesWidget::paintTriangleFan(QGLPainter *painter, const QRect& rect)
{
    painter->setStandardEffect(QGL::LitMaterial);

    QVector2DArray vertices;
    int step = qMin(rect.width() / 8, rect.height() / 8);
    int midx = rect.x() + rect.width() / 2;
    int midy = rect.y() + rect.height() / 2;

    vertices.append(midx - step, midy + step);
    vertices.append(midx + step * 2, midy + step * 2);
    vertices.append(midx + step * 2, midy);
    vertices.append(midx + step, midy - step);
    vertices.append(midx - step, midy - step * 2);

    painter->setVertexAttribute(QGL::Position, vertices);
    painter->setCommonNormal(QVector3D(0.0f, 0.0f, 1.0f));
    painter->draw(QGL::TriangleFan, 5);

    // Overpaint some lines to show the triangle boundaries.
    painter->setStandardEffect(QGL::FlatColor);
    painter->setColor(QColor(202, 170, 0));
    QGLIndexArray indices;
    indices.append(0);
    indices.append(2);
    indices.append(0);
    indices.append(3);
    painter->setVertexAttribute(QGL::Position, vertices);
    painter->draw(QGL::Lines, indices);

    drawText(painter, rect, tr("Triangle fan"));
}

void ShapesWidget::paintCube(QGLPainter *painter, const QRect& rect)
{
    painter->setStandardEffect(QGL::LitMaterial);

    painter->projectionMatrix().push();
    painter->modelViewMatrix().push();

    painter->setViewport(rect);
    camera.apply(painter, rect.size());
    painter->modelViewMatrix().rotate(45.0f, 1.0f, 1.0f, 1.0f);

    cube.draw(painter);

    painter->projectionMatrix().pop();
    painter->modelViewMatrix().pop();

    painter->setViewport(size());

    drawText(painter, rect, tr("Cube"));
}

void ShapesWidget::paintTeapot(QGLPainter *painter, const QRect& rect)
{
    painter->setStandardEffect(QGL::LitMaterial);

    painter->projectionMatrix().push();
    painter->modelViewMatrix().push();

    painter->setViewport(rect);
    camera.apply(painter, rect.size());

    // Need a one-sided lighting model for the teapot.
    painter->setLightModel(&oneSidedModel);

    teapot.draw(painter);

    painter->setLightModel(&twoSidedModel);

    painter->projectionMatrix().pop();
    painter->modelViewMatrix().pop();

    painter->setViewport(size());

    drawText(painter, rect, tr("Teapot"));
}

// Draw text centered on the bottom of the "posn" rectangle.
void ShapesWidget::drawText
        (QGLPainter *painter, const QRect& posn, const QString& str)
{
    QFontMetrics metrics = fontMetrics();
    QRect rect = metrics.boundingRect(str);
    rect.adjust(0, 0, 1, 1);

    QFont f = font();

    QImage image(rect.size(), QImage::Format_ARGB32);
    image.fill(0);
    QPainter p2(&image);
    p2.setFont(f);
    p2.setPen(Qt::white);
    p2.drawText(-rect.x(), metrics.ascent(), str);
    p2.end();

    QGLTexture2D texture;
    texture.setImage(image);

    int x = posn.x() + (posn.width() - rect.width()) / 2;
    int y = posn.y() + posn.height() - metrics.ascent() - metrics.descent();
    y -= 10;

    QVector2DArray vertices;
    vertices.append(x + rect.x(), y + metrics.ascent());
    vertices.append(x + rect.x(), y - metrics.descent());
    vertices.append(x + rect.x() + rect.width(), y - metrics.descent());
    vertices.append(x + rect.x() + rect.width(), y + metrics.ascent());

    QVector2DArray texCoord;
    texCoord.append(0.0f, 0.0f);
    texCoord.append(0.0f, 1.0f);
    texCoord.append(1.0f, 1.0f);
    texCoord.append(1.0f, 0.0f);

    painter->setStandardEffect(QGL::FlatReplaceTexture2D);
    painter->setTexture(&texture);
    painter->setVertexAttribute(QGL::Position, vertices);
    painter->setVertexAttribute(QGL::TextureCoord0, texCoord);
    painter->draw(QGL::TriangleFan, 4);
    painter->setStandardEffect(QGL::FlatColor);
    painter->setTexture((QGLTexture2D *)0);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ShapesWidget w;
    if (QApplication::arguments().contains("-maximize"))
        w.showMaximized();
    else if (QApplication::arguments().contains("-fullscreen"))
        w.showFullScreen();
    else
        w.show();
    return app.exec();
}

#include "shapes.moc"
