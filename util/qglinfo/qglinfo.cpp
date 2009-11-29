#include "qglinfo.h"

#include <QtOpenGL/qgl.h>
#include <QtOpenGL/qglpixelbuffer.h>
#include <QtOpenGL/qglframebufferobject.h>
#include <QtOpenGL/qglshaderprogram.h>

#include <QtCore/qtimer.h>
#include <QtCore/qdatetime.h>

QGLInfo::QGLInfo(QObject *parent)
    : QObject(parent)
{
    QTimer::singleShot(0, this, SLOT(initialize()));
}

static QString nice(const QString &s)
{
    QString r(s);
    r.replace("\n", "<br>\n");
    r.replace("true", "<span style=\"color: green\">true</span>");
    r.replace("false", "<span style=\"color: red\">false</span>");
    return r;
}

void QGLInfo::initialize()
{
    QWidget *win = qobject_cast<QGLWidget *>(parent());
    // We need some kind of GL context to do the querying.
    QGLWidget *glWidget = new QGLWidget(win);
    glWidget->makeCurrent();
    m_qtGLVersionInfo = reportQtGLVersionInfo();
    m_qtGLFeatures = reportQtGLFeatures();
    m_glVersionInfo = reportGLVersionInfo();
    m_glExtensionInfo = reportGLExtensionInfo();
    glWidget->doneCurrent();
    delete glWidget;

    QString html = tr("<h1>Qt GL Info Report</h1>"
                      "<p>Generated at: %1</p>"
                      "<h2>Qt GL Version Info</h2>"
                      "<p>%2</p>"
                      "<h2>Qt GL Features</h2>"
                      "<p>%3</p>"
                      "<h2>GL Version Info</h2>"
                      "<p>%4</p>"
                      "<h2>GL Extension Info</h2>"
                      "<p>%5</p>")
            .arg(QDateTime::currentDateTime().toString())
            .arg(nice(m_qtGLVersionInfo))
            .arg(nice(m_qtGLFeatures))
            .arg(nice(m_glVersionInfo))
            .arg(nice(m_glExtensionInfo));
    emit reportHtml(html);
}

QString QGLInfo::report() const
{
    QString report;
    report += m_qtGLVersionInfo;
    report += QChar('\n');
    report += m_qtGLFeatures;
    report += m_glVersionInfo;
    report += tr("OpenGL extensions:\n");
    report += m_glExtensionInfo;
    return report;
}

QString QGLInfo::reportQtGLVersionInfo() const
{
    // Dump what Qt thinks the version is.
    QGLFormat::OpenGLVersionFlags flags = QGLFormat::openGLVersionFlags();
    QByteArray version;
    if ((flags & QGLFormat::OpenGL_Version_1_1) != 0)
        version += "1.1 ";
    if ((flags & QGLFormat::OpenGL_Version_1_2) != 0)
        version += "1.2 ";
    if ((flags & QGLFormat::OpenGL_Version_1_3) != 0)
        version += "1.3 ";
    if ((flags & QGLFormat::OpenGL_Version_1_4) != 0)
        version += "1.4 ";
    if ((flags & QGLFormat::OpenGL_Version_1_5) != 0)
        version += "1.5 ";
    if ((flags & QGLFormat::OpenGL_Version_2_0) != 0)
        version += "2.0 ";
    if ((flags & QGLFormat::OpenGL_Version_2_1) != 0)
        version += "2.1 ";
    if ((flags & QGLFormat::OpenGL_Version_3_0) != 0)
        version += "3.0 ";
    if ((flags & QGLFormat::OpenGL_ES_Common_Version_1_0) != 0)
        version += "ES/1.0 ";
    if ((flags & QGLFormat::OpenGL_ES_CommonLite_Version_1_0) != 0)
        version += "ES/1.0CL ";
    if ((flags & QGLFormat::OpenGL_ES_Common_Version_1_1) != 0)
        version += "ES/1.1 ";
    if ((flags & QGLFormat::OpenGL_ES_CommonLite_Version_1_1) != 0)
        version += "ES/1.1CL ";
    if ((flags & QGLFormat::OpenGL_ES_Version_2_0) != 0)
        version += "ES/2.0 ";
    flags &= ~(QGLFormat::OpenGL_Version_1_1 |
               QGLFormat::OpenGL_Version_1_2 |
               QGLFormat::OpenGL_Version_1_3 |
               QGLFormat::OpenGL_Version_1_4 |
               QGLFormat::OpenGL_Version_1_5 |
               QGLFormat::OpenGL_Version_2_0 |
               QGLFormat::OpenGL_Version_2_1 |
               QGLFormat::OpenGL_Version_3_0 |
               QGLFormat::OpenGL_ES_Common_Version_1_0 |
               QGLFormat::OpenGL_ES_CommonLite_Version_1_0 |
               QGLFormat::OpenGL_ES_Common_Version_1_1 |
               QGLFormat::OpenGL_ES_CommonLite_Version_1_1 |
               QGLFormat::OpenGL_ES_Version_2_0);
    if (flags != 0)
        version += "Other=0x" + QByteArray::number(int(flags), 16);
    return QString("QGLFormat::openGLVersionFlags: ") + version;
}

static QString printBool(const char *text, bool value)
{
    return QString(text) + (value ? "true\n" : "false\n");
}

QString QGLInfo::reportQtGLFeatures() const
{
    QString d;
    d += printBool("QGLFormat::hasOpenGL: ", QGLFormat::hasOpenGL());
    d += printBool("QGLFormat::hasOpenGLOverlays: ", QGLFormat::hasOpenGLOverlays());
    d += printBool("QGLPixelBuffer::hasOpenGLPbuffers: ", QGLPixelBuffer::hasOpenGLPbuffers());
    d += printBool("QGLFramebufferObject::hasOpenGLFramebufferObjects: ",
              QGLFramebufferObject::hasOpenGLFramebufferObjects());
    d += printBool("QGLFramebufferObject::hasOpenGLFramebufferBlit: ",
              QGLFramebufferObject::hasOpenGLFramebufferBlit());
#if !defined(QT_OPENGL_ES_1_CL) && !defined(QT_OPENGL_ES_1)
    d += printBool("QGLShaderProgram::hasOpenGLShaderPrograms: ",
              QGLShaderProgram::hasOpenGLShaderPrograms());
#endif
    return d;
}

QString QGLInfo::reportGLVersionInfo() const
{
    QString d;
    d += "OpenGL vendor string: ";
    d += reinterpret_cast<const char *>(glGetString(GL_VENDOR));
    d += "\n";
    d += "OpenGL renderer string: ";
    d += reinterpret_cast<const char *>(glGetString(GL_RENDERER));
    d += "\n";
    d += "OpenGL version string: ";
    d += reinterpret_cast<const char *>(glGetString(GL_VERSION));
    d += "\n";
    return d;
}

QString QGLInfo::reportGLExtensionInfo() const
{
    QString d;
    QByteArray extString
        (reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS)));
    QList<QByteArray> extns = extString.split(' ');
    qSort(extns);
    QByteArray line;
    foreach (QByteArray extn, extns) {
        if (extn.isEmpty())
            continue;
        if (!line.isEmpty() && (line.size() + extn.size() + 1) > 70) {
            d += "    " + line + "\n";
            line = QByteArray();
        }
        line += extn;
        line += char(' ');
    }
    if (!line.isEmpty())
        d += "    " + line + "\n";
    return d;
}
