/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qmlgenerator.h"
#include "qmlstanza.h"

#include <QtCore/qfile.h>
#include <QtCore/qtextstream.h>
#include <QtCore/qvariant.h>

QmlGenerator::QmlGenerator(const QString &filePath)
    : m_filePath(filePath)
    , m_hasError(false)
{
}

void QmlGenerator::save() const
{
    QFile f(m_filePath);
    // flags: Write + throw away previous + do eol conversions on windows
    QString indent = QLatin1String("    ");
    if (f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        QTextStream os(&f);
        os << QLatin1String("import Qt 4.7") << endl;
        os << QLatin1String("import Qt3D 1.0") << endl << endl;

        QmlStanza item3d(QLatin1String("Item3D"));

        QmlStanza mesh(QLatin1String("Mesh"));
        item3d.addProperty(QLatin1String("mesh"), &mesh);

        mesh.addProperty(QLatin1String("source"), property("modelFileName"), true);
        mesh.addProperty(QLatin1String("options"), property("options", Optional), true);

        QString x_t = property("x_translation", Optional);
        QString y_t = property("y_translation", Optional);
        QString z_t = property("z_translation", Optional);
        QmlStanza tq(QLatin1String("Translation3D"));
        if (!x_t.isEmpty() || !y_t.isEmpty() || !z_t.isEmpty())
        {
            x_t = x_t.isEmpty() ? QLatin1String("0") : x_t;
            y_t = y_t.isEmpty() ? QLatin1String("0") : y_t;
            z_t = z_t.isEmpty() ? QLatin1String("0") : z_t;
            QString vec = QString::fromLatin1("Qt.vector3d(%1, %2, %3)"
                                  ).arg(x_t).arg(y_t).arg(z_t);
            tq.addProperty(QLatin1String("translate"), vec);
            item3d.addProperty(QLatin1String("transform"), &tq);
        }

        QmlStanza xrq(QLatin1String("Rotation3D"));
        QString x_r = property("x_rotation", Optional);
        if (!x_r.isEmpty())
        {
            xrq.addProperty(QLatin1String("angle"), x_r);
            xrq.addProperty(QLatin1String("axis"), QLatin1String("Qt.vector3d(1, 0, 0)"));
            item3d.addProperty(QLatin1String("transform"), &xrq);
        }

        QmlStanza yrq(QLatin1String("Rotation3D"));
        QString y_r = property("y_rotation", Optional);
        if (!y_r.isEmpty())
        {
            yrq.addProperty(QLatin1String("angle"), y_r);
            yrq.addProperty(QLatin1String("axis"), QLatin1String("Qt.vector3d(0, 1, 0)"));
            item3d.addProperty(QLatin1String("transform"), &yrq);
        }

        QmlStanza zrq(QLatin1String("Rotation3D"));
        QString z_r = property("z_rotation", Optional);
        if (!z_r.isEmpty())
        {
            zrq.addProperty(QLatin1String("angle"), z_r);
            zrq.addProperty(QLatin1String("axis"), QLatin1String("Qt.vector3d(0, 0, 1)"));
            item3d.addProperty(QLatin1String("transform"), &xrq);
        }

        QmlStanza sq(QLatin1String("Scale3D"));
        QString x_s = property("x_scale", Optional);
        QString y_s = property("y_scale", Optional);
        QString z_s = property("z_scale", Optional);
        if (x_s.isEmpty()) x_s = QLatin1String("1");
        if (y_s.isEmpty()) y_s = QLatin1String("1");
        if (z_s.isEmpty()) z_s = QLatin1String("1");
        if (x_s != QLatin1String("1") || y_s != QLatin1String("1") ||
                z_s != QLatin1String("1"))
        {
            QString scale;
            if (x_s != y_s || x_s != z_s || y_s != z_s) {
                scale = QLatin1String("Qt.vector3d(") + x_s +
                        QLatin1String(", ") + y_s +
                        QLatin1String(", ") + z_s + QLatin1String(")");
            } else {
                scale = x_s;
            }
            sq.addProperty(QLatin1String("scale"), x_s);
            item3d.addProperty(QLatin1String("transform"), &sq);
        }

        os << item3d;
    }
    else
    {
        m_errorString = QObject::tr("Could not write to file %1").arg(m_filePath);
        m_hasError = true;
    }
}

QString QmlGenerator::property(const char *propertyName,
                                       QmlGenerator::OptionStatus required) const
{
    QString result = m_content.value(QString::fromLatin1(propertyName), QString());
    if (result.isEmpty() && required)
    {
        m_hasError = true;
        if (!m_errorString.isEmpty())
            m_errorString += QLatin1Char('\n');
        m_errorString += QObject::tr("Expected property %1: none defined").arg(QString::fromLatin1(propertyName));
    }
    return result;
}
