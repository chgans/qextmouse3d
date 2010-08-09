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

#include <QApplication>
#include <QTimer>
#include <QMouseEvent>
#include "qglpainter.h"
#include "qglabstracteffect.h"
#include "qgltexture2d.h"

#if !defined(QT_OPENGL_ES_1)
#include <QtOpenGL/qglshaderprogram.h>
#endif
#include "pageflipmath_p.h"

class PageFlipGradientEffect;

class PageFlipView : public QGLWidget
{
    Q_OBJECT
public:
    PageFlipView(QWidget *parent = 0);
    ~PageFlipView();

    void setBlend(bool value) { blend = value; }
    void setVertical(bool value) { vertical = value; }

protected:
    void resizeGL(int width, int height);
    void initializeGL();
    void paintGL();
    void mousePressEvent(QMouseEvent *e);

private slots:
    void animate();

private:
    bool blend;
    bool vertical;

    qreal posn;     // Position within the animation - 0...1
    QSize pageSize; // Size of a page within the window.

    QRect pageRect1;
    QRect pageRect2;

    QColor colors[4];
    int colorIndex;

    QGLTexture2D textures[4];

    QGLTexture2D gradientTexture;

    PageFlipMath pageFlipMath;

#if !defined(QT_OPENGL_ES_1)
    PageFlipGradientEffect *effect;
#endif

    void setAlphaValue(GLfloat value);
};

#if !defined(QT_OPENGL_ES_1)

class PageFlipGradientEffect : public QGLAbstractEffect
{
public:
    PageFlipGradientEffect();
    ~PageFlipGradientEffect();

    QList<QGL::VertexAttribute> requiredFields() const;
    void setActive(QGLPainter *painter, bool flag);
    void update(QGLPainter *painter, QGLPainter::Updates updates);
    void setVertexAttribute
        (QGL::VertexAttribute attribute, const QGLAttributeValue& value);

    void setAlphaValue(GLfloat value);

private:
    QGLShaderProgram *program;
    int matrixUniform;
};

#endif

PageFlipView::PageFlipView(QWidget *parent)
    : QGLWidget(parent)
{
    posn = 0.0f;
    blend = false;
    vertical = false;

    colors[0] = QColor(0, 192, 192, 255);
    colors[1] = QColor(192, 0, 0, 255);
    colors[2] = QColor(192, 192, 0, 255);
    colors[3] = QColor(128, 128, 0, 255);
    colorIndex = 0;

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(animate()));
    timer->start(40);

#if !defined(QT_OPENGL_ES_1)
    effect = new PageFlipGradientEffect();
#endif
}

PageFlipView::~PageFlipView()
{
#if !defined(QT_OPENGL_ES_1)
    delete effect;
#endif
}

void PageFlipView::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
}

void PageFlipView::initializeGL()
{
    QGLPainter painter(this);

    //QSize size = rect().size();
    //int width = size.width() / 3;
    //int height = (int)(width * 1.414f);
    int width = 227;
    int height = 320;
    pageSize = QSize(width, height);

    textures[0].setImage(QImage(":/qqpage1.png"));
    textures[1].setImage(QImage(":/qqpage2.png"));
    textures[2].setImage(QImage(":/qqpage3.png"));
    textures[3].setImage(QImage(":/qqpage4.png"));

    gradientTexture.setImage(QImage(":/gradient.png"));

    if (painter.hasOpenGLFeature(QGLFunctions::BlendColor))
        painter.glBlendColor(0, 0, 0, 0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (painter.hasOpenGLFeature(QGLFunctions::BlendEquation))
        painter.glBlendEquation(GL_FUNC_ADD);
    else if (painter.hasOpenGLFeature(QGLFunctions::BlendEquationSeparate))
        painter.glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);

    glEnable(GL_BLEND);

    if (vertical)
        pageFlipMath.setStartCorner(PageFlipMath::VerticalBottomRight);
    else
        pageFlipMath.setStartCorner(PageFlipMath::BottomRight);
}

void PageFlipView::paintGL()
{
    QGLPainter painter(this);

    QRect rect = this->rect();
    int midx = rect.width() / 2;
    int topy = (rect.height() - pageSize.height()) / 2;

    painter.clear();
    QMatrix4x4 projm;
    projm.ortho(rect);
    painter.projectionMatrix() = projm;
    painter.modelViewMatrix().setToIdentity();

#if !defined(QT_OPENGL_ES_1)
    if (vertical) {
        pageRect2 = QRect(QPoint(midx - pageSize.width() / 2, topy), pageSize);
        pageRect1 = QRect(QPoint(pageRect2.x() - pageSize.width(), topy), pageSize);
    } else {
        pageRect1 = QRect(QPoint(midx - pageSize.width(), topy), pageSize);
        pageRect2 = QRect(QPoint(midx, topy), pageSize);
    }
#else
    pageRect1 = QRect(QPoint(-pageSize.width(), 0), pageSize);
    pageRect2 = QRect(QPoint(0, 0), pageSize);
#endif
    pageFlipMath.setPageRect(pageRect2);
    pageFlipMath.setShowPageReverse(false);
    pageFlipMath.compute(posn);

    QGLAttributeValue positions
        (2, GL_FLOAT, pageFlipMath.stride(), pageFlipMath.vertexArray());
    QGLAttributeValue texCoords
        (2, GL_FLOAT, pageFlipMath.stride(), pageFlipMath.vertexArray() + 2);
    QGLAttributeValue gradientCoords
        (1, GL_FLOAT, pageFlipMath.stride(), pageFlipMath.vertexArray() + 4);

#if defined(QT_OPENGL_ES_1)
    painter.setStandardEffect(QGL::FlatReplaceTexture2D);
#else
    painter.setUserEffect(effect);
#endif
    painter.setColor(colors[colorIndex]);
    painter.setTexture(0, &(textures[colorIndex]));
    painter.setTexture(1, &gradientTexture);
    painter.setVertexAttribute(QGL::Position, positions);
    painter.setVertexAttribute(QGL::TextureCoord0, texCoords);
    painter.setVertexAttribute(QGL::CustomVertex0, gradientCoords);
    setAlphaValue(1.0f);
    painter.update();
    pageFlipMath.drawPage(0);

    painter.setColor(colors[(colorIndex + 1) % 4]);
    painter.setTexture(0, &(textures[(colorIndex + 1) % 4]));
    setAlphaValue(1.0f);
    painter.update();
    pageFlipMath.drawPage(1);

    painter.setColor(colors[(colorIndex + 2) % 4]);
    if (!pageFlipMath.showPageReverse())
        painter.setTexture(0, &(textures[(colorIndex + 2) % 4]));
    if (blend)
        setAlphaValue(0.75f);
    else
        setAlphaValue(1.0f);
    painter.update();
    pageFlipMath.drawPage(2);

    painter.setColor(colors[(colorIndex + 3) % 4]);
    painter.setTexture(0, &(textures[(colorIndex + 3) % 4]));
    setAlphaValue(1.0f);
    painter.update();
    pageFlipMath.drawPage(3);

    painter.setTexture(0, (QGLTexture2D *)0);
    painter.setTexture(1, (QGLTexture2D *)0);

    painter.setStandardEffect(QGL::FlatColor);
    painter.setVertexAttribute(QGL::Position, positions);
    painter.setVertexAttribute(QGL::TextureCoord0, texCoords);
    painter.setVertexAttribute(QGL::CustomVertex0, gradientCoords);
    painter.setColor(QColor(0, 0, 0, 255));
    painter.update();
    pageFlipMath.drawOutline(2);
}

void PageFlipView::mousePressEvent(QMouseEvent *e)
{
    int x = e->x();
    int y = e->y();
    bool changed = true;
    if (vertical) {
        if (x >= pageRect2.x() && x < (pageRect2.x() + 20) &&
                y >= pageRect2.y() && y < (pageRect2.y() + 20))
            pageFlipMath.setStartCorner(PageFlipMath::VerticalTopLeft);
        else if (x >= pageRect2.x() && x < (pageRect2.x() + 20) &&
                    y >= (pageRect2.bottom() - 20) && y <= pageRect2.bottom())
            pageFlipMath.setStartCorner(PageFlipMath::VerticalBottomLeft);
        else if (x >= (pageRect2.right() - 20) && x <= pageRect2.right() &&
                    y >= pageRect2.y() && y < (pageRect2.y() + 20))
            pageFlipMath.setStartCorner(PageFlipMath::VerticalTopRight);
        else if (x >= (pageRect2.right() - 20) && x <= pageRect2.right() &&
                    y >= (pageRect2.bottom() - 20) && y <= pageRect2.bottom())
            pageFlipMath.setStartCorner(PageFlipMath::VerticalBottomRight);
        else
            changed = false;
    } else {
        if (x >= pageRect1.x() && x < (pageRect1.x() + 20) &&
                y >= pageRect1.y() && y < (pageRect1.y() + 20))
            pageFlipMath.setStartCorner(PageFlipMath::TopLeft);
        else if (x >= pageRect1.x() && x < (pageRect1.x() + 20) &&
                    y >= (pageRect1.bottom() - 20) && y <= pageRect1.bottom())
            pageFlipMath.setStartCorner(PageFlipMath::BottomLeft);
        else if (x >= pageRect2.x() && x < (pageRect2.x() + 20) &&
                    y >= pageRect2.y() && y < (pageRect2.y() + 20))
            pageFlipMath.setStartCorner(PageFlipMath::TopLeftOnePage);
        else if (x >= pageRect2.x() && x < (pageRect2.x() + 20) &&
                    y >= (pageRect2.bottom() - 20) && y <= pageRect2.bottom())
            pageFlipMath.setStartCorner(PageFlipMath::BottomLeftOnePage);
        else if (x >= (pageRect2.right() - 20) && x <= pageRect2.right() &&
                    y >= pageRect2.y() && y < (pageRect2.y() + 20))
            pageFlipMath.setStartCorner(PageFlipMath::TopRight);
        else if (x >= (pageRect2.right() - 20) && x <= pageRect2.right() &&
                    y >= (pageRect2.bottom() - 20) && y <= pageRect2.bottom())
            pageFlipMath.setStartCorner(PageFlipMath::BottomRight);
        else
            changed = false;
    }
    if (changed)
        posn = 0.0f;
    QGLWidget::mousePressEvent(e);
}

void PageFlipView::animate()
{
    posn += 0.04f;
    if (posn >= 1.0f) {
        posn = 0.0f;
        colorIndex = (colorIndex + 2) % 4;
    }
    updateGL();
}

void PageFlipView::setAlphaValue(GLfloat value)
{
#if !defined(QT_OPENGL_ES_1)
    effect->setAlphaValue(value);
#else
    Q_UNUSED(value);
#endif
}

#if !defined(QT_OPENGL_ES_1)

PageFlipGradientEffect::PageFlipGradientEffect()
{
    program = 0;
    matrixUniform = -1;
}

PageFlipGradientEffect::~PageFlipGradientEffect()
{
    delete program;
}

QList<QGL::VertexAttribute> PageFlipGradientEffect::requiredFields() const
{
    QList<QGL::VertexAttribute> fields;
    fields += QGL::Position;
    fields += QGL::TextureCoord0;
    fields += QGL::CustomVertex0;
    return fields;
}

static char const gradientVertexShader[] =
    "attribute highp vec4 vertex;\n"
    "attribute highp vec4 texcoord;\n"
    "attribute highp float gradctrl;\n"
    "uniform mediump mat4 matrix;\n"
    "varying highp vec4 qTexCoord;\n"
    "varying highp float qGradCtrl;\n"
    "void main(void)\n"
    "{\n"
    "    gl_Position = matrix * vertex;\n"
    "    qTexCoord = texcoord;\n"
    "    qGradCtrl = gradctrl;\n"
    "}\n";

static char const gradientFragmentShader[] =
    "uniform sampler2D tex;\n"
    "uniform sampler2D gradient;\n"
    "uniform mediump float alphaValue;\n"
    "varying highp vec4 qTexCoord;\n"
    "varying highp float qGradCtrl;\n"
    "void main(void)\n"
    "{\n"
    "    mediump vec4 col = texture2D(tex, qTexCoord.st);\n"
    "    mediump vec4 gradcol = texture2D(gradient, vec2(qGradCtrl, qTexCoord.t));\n"
    "    gl_FragColor = vec4((col * gradcol).xyz, alphaValue);\n"
    "}\n";

void PageFlipGradientEffect::setActive(QGLPainter *painter, bool flag)
{
    Q_UNUSED(painter);
    if (!program) {
        if (!flag)
            return;
        program = new QGLShaderProgram();
        program->addShaderFromSourceCode(QGLShader::Vertex, gradientVertexShader);
        program->addShaderFromSourceCode(QGLShader::Fragment, gradientFragmentShader);
        program->bindAttributeLocation("vertex", QGL::Position);
        program->bindAttributeLocation("texcoord", QGL::TextureCoord0);
        program->bindAttributeLocation("gradctrl", QGL::CustomVertex0);
        if (!program->link()) {
            qWarning("PageFlipGradientEffect::setActive(): could not link shader program");
            delete program;
            program = 0;
            return;
        }
        matrixUniform = program->uniformLocation("matrix");
        program->bind();
        program->setUniformValue("tex", 0);
        program->setUniformValue("gradient", 1);
        program->setUniformValue("alphaValue", 1.0f);
        program->enableAttributeArray(QGL::Position);
        program->enableAttributeArray(QGL::TextureCoord0);
        program->enableAttributeArray(QGL::CustomVertex0);
    } else if (flag) {
        program->bind();
        program->setUniformValue("tex", 0);
        program->setUniformValue("gradient", 1);
        program->setUniformValue("alphaValue", 1.0f);
        program->enableAttributeArray(QGL::Position);
        program->enableAttributeArray(QGL::TextureCoord0);
        program->enableAttributeArray(QGL::CustomVertex0);
    } else {
        program->disableAttributeArray(QGL::Position);
        program->disableAttributeArray(QGL::TextureCoord0);
        program->disableAttributeArray(QGL::CustomVertex0);
        program->release();
    }
}

void PageFlipGradientEffect::update
        (QGLPainter *painter, QGLPainter::Updates updates)
{
    if ((updates & QGLPainter::UpdateMatrices) != 0)
        program->setUniformValue(matrixUniform, painter->combinedMatrix());
}

void PageFlipGradientEffect::setVertexAttribute
    (QGL::VertexAttribute attribute, const QGLAttributeValue& value)
{
    if (attribute == QGL::Position)
        setAttributeArray(program, QGL::Position, value);
    else if (attribute == QGL::TextureCoord0)
        setAttributeArray(program, QGL::TextureCoord0, value);
    else if (attribute == QGL::CustomVertex0)
        setAttributeArray(program, QGL::CustomVertex0, value);
}

void PageFlipGradientEffect::setAlphaValue(GLfloat value)
{
    program->setUniformValue("alphaValue", value);
}

#endif // !QT_OPENGL_ES_1

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    PageFlipView view;
    if (QApplication::arguments().contains("-blend"))
        view.setBlend(true);
    if (QApplication::arguments().contains("-vertical"))
        view.setVertical(true);
    if (QApplication::arguments().contains("-maximize"))
        view.showMaximized();
    else if (QApplication::arguments().contains("-fullscreen"))
        view.showFullScreen();
    else
        view.show();
    return app.exec();
}

#include "pageflip.moc"
