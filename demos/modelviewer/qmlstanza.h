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

#ifndef QMLSTANZA_H
#define QMLSTANZA_H

#include <QtCore/qtextstream.h>
#include <QtCore/qmap.h>
#include <QtCore/qobject.h>

class QmlStanza : public QObject
{
Q_OBJECT
public:
    explicit QmlStanza(const QString &name, QObject *parent = 0);
    QString toString() const;
    void addProperty(const QString &name, QmlStanza *subItem);
    void addProperty(const QString &name, const QString &value);
    void setIndent(int indent) { m_indent = indent; }
    void setQuoted(bool enable) { m_quoted = enable; }
    bool isQuoted() const { return m_quoted; }
private:
    QString m_name;
    int m_indent;
    bool m_quoted;
    QMap<QString, QmlStanza *> m_content;
};

QTextStream &operator<<(QTextStream &, const QmlStanza &);

#endif // QMLSTANZA_H
