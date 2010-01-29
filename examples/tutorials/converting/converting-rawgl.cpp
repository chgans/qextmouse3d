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

//! [class-defn]
#include <QApplication>
#include <QtOpenGL/qgl.h>

class TutorialWidget : public QGLWidget
{
    Q_OBJECT
public:
    TutorialWidget(QWidget *parent=0) : QGLWidget(parent) {}

protected:
    void resizeGL(int width, int height);
    void paintGL();
#if defined(QT_OPENGL_ES_2)
    void initializeGL();

private:
    GLuint program;
    GLuint vertexAttr;
    GLuint matrixUniform;
    GLuint colorUniform;
#endif
};
//! [class-defn]

//! [set-viewport]
void TutorialWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
}
//! [set-viewport]

#if !defined(QT_OPENGL_ES_2)

//! [clear-window]
void TutorialWidget::paintGL()
{
    QColor clearColor(palette().color(backgroundRole()));
    glClearColor(clearColor.redF(), clearColor.greenF(),
                 clearColor.blueF(), clearColor.alphaF());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//! [clear-window]

//! [set-matrices]
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#if defined(QT_OPENGL_ES_1)
    glOrthof(0, width(), height(), 0, -1.0f, 1.0f);
#else
    glOrtho(0, width(), height(), 0, -1.0f, 1.0f);
#endif

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
//! [set-matrices]

//! [draw]
    QColor color(170, 202, 0, 255);
    glColor4f(color.redF(), color.greenF(), color.blueF(), color.alphaF());

    static GLfloat vertices[] = {
        60.0f,  10.0f,  0.0f,
        110.0f, 110.0f, 0.0f,
        10.0f,  110.0f, 0.0f
    };

    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableClientState(GL_VERTEX_ARRAY);
//! [draw]
}

#else // QT_OPENGL_ES_2

void TutorialWidget::initializeGL()
{
    GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
    const char *vsrc[1] = {
        "attribute highp vec4 vertex;\n"
        "uniform mediump mat4 matrix;\n"
        "void main(void)\n"
        "{\n"
        "    gl_Position = matrix * vertex;\n"
        "}\n"
    };
    glShaderSource(vshader, 1, vsrc, 0);
    glCompileShader(vshader);

    GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
    const char *fsrc[1] = {
        "uniform mediump vec4 color;\n"
        "void main(void)\n"
        "{\n"
        "    gl_FragColor = color;\n"
        "}\n"
    };
    glShaderSource(fshader, 1, fsrc, 0);
    glCompileShader(fshader);

    program = glCreateProgram();
    glAttachShader(program, vshader);
    glAttachShader(program, fshader);
    glLinkProgram(program);

    vertexAttr = glGetAttribLocation(program, "vertex");
    matrixUniform = glGetUniformLocation(program, "matrix");
    colorUniform = glGetUniformLocation(program, "color");

    glUseProgram(program);
}

void TutorialWidget::paintGL()
{
    QColor clearColor(palette().color(backgroundRole()));
    glClearColor(clearColor.redF(), clearColor.greenF(),
                 clearColor.blueF(), clearColor.alphaF());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLfloat width = this->width();
    GLfloat height = this->height();
    GLfloat left = 0.0f;
    GLfloat right = width;
    GLfloat top = 0.0f;
    GLfloat bottom = height;
    GLfloat nearPlane = -1.0f;
    GLfloat farPlane = 1.0f;
    GLfloat clip = farPlane - nearPlane;
    GLfloat mat[4][4];
    mat[0][0] = 2.0f / width;
    mat[1][0] = 0.0f;
    mat[2][0] = 0.0f;
    mat[3][0] = -(left + right) / width;
    mat[0][1] = 0.0f;
    mat[1][1] = 2.0f / -height;
    mat[2][1] = 0.0f;
    mat[3][1] = -(top + bottom) / -height;
    mat[0][2] = 0.0f;
    mat[1][2] = 0.0f;
    mat[2][2] = -2.0f / clip;
    mat[3][2] = -(nearPlane + farPlane) / clip;
    mat[0][3] = 0.0f;
    mat[1][3] = 0.0f;
    mat[2][3] = 0.0f;
    mat[3][3] = 1.0f;
    glUniformMatrix4fv(matrixUniform, 1, GL_FALSE, mat[0]);

    QColor color(170, 202, 0, 255);
    glUniform4f(colorUniform, color.redF(), color.greenF(), color.blueF(), color.alphaF());

    static GLfloat vertices[] = {
        60.0f,  10.0f,  0.0f,
        110.0f, 110.0f, 0.0f,
        10.0f,  110.0f, 0.0f
    };

    glVertexAttribPointer(vertexAttr, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(vertexAttr);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(vertexAttr);
}

#endif // QT_OPENGL_ES_2

//! [main]
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TutorialWidget widget;
    widget.show();
    return app.exec();
}
//! [main]

#include "converting-rawgl.moc"
