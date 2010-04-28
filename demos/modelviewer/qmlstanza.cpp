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


#include "qmlstanza.h"

QmlStanza::QmlStanza(const QString &name, QObject *parent)
    : QObject(parent)
    , m_name(name)
    , m_indent(0)
{
}

QString QmlStanza::toString() const
{
    static QString indent("    ");

    QString result = m_name;
    if (m_content.size() > 0)
    {
        result += " {\n";
        QMap<QString, QmlStanza *>::const_iterator it = m_content.constBegin();
        for ( ; it != m_content.constEnd(); ++it)
        {
            result += indent.repeated(m_indent + 1);
            result += it.key() + ": ";
            result += it.value()->toString() + "\n";
        }
        result += indent.repeated(m_indent) + "}";
    }
    return result;
}

void QmlStanza::addProperty(const QString &name, const QString &value)
{
    addProperty(name, new QmlStanza(value, this));
}

void QmlStanza::addProperty(const QString &name, QmlStanza *subItem)
{
    subItem->setIndent(m_indent + 1);
    m_content.insertMulti(name, subItem);
}

QTextStream &operator<<(QTextStream &s, const QmlStanza &q)
{
    s << q.toString() << endl;
    return s;
}
