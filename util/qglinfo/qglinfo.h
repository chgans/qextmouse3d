#ifndef QGLINFO_H
#define QGLINFO_H

#include <QtCore/qstring.h>
#include <QtCore/qobject.h>

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

    QString m_qtGLVersionInfo;
    QString m_qtGLFeatures;
    QString m_glVersionInfo;
    QString m_glExtensionInfo;
};

#endif // QGLINFO_H
