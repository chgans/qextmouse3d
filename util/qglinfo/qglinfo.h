#ifndef QGLINFO_H
#define QGLINFO_H

#include <QtCore/qstring.h>
#include <QtCore/qobject.h>
#include <QtCore/qbytearray.h>

class QGLInfo : public QObject
{
    Q_OBJECT
public:
    QGLInfo(QObject *parent = 0);
    QString report() const;
signals:
    void reportHtml(const QString &);
private slots:
    void initialize();
private:
    QString reportQtGLVersionInfo() const;
    QString reportQtGLFeatures() const;
    QString reportGLVersionInfo() const;
    QString reportGLExtensionInfo() const;
    QString reportEGLVersionInfo() const;
    QString reportEGLExtensionInfo() const;
    QString formatExtensions(const QByteArray& extString) const;

    QString m_qtGLVersionInfo;
    QString m_qtGLFeatures;
    QString m_glVersionInfo;
    QString m_glExtensionInfo;
    QString m_eglVersionInfo;
    QString m_eglExtensionInfo;
};

#endif // QGLINFO_H
