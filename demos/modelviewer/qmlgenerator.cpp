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
    QString indent = "    ";
    if (f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        QTextStream os(&f);
        os << "import Qt 4.7" << endl;
        os << "import Qt.labs.threed 1.0" << endl;

        QmlStanza item3d("Item3d");

        QmlStanza mesh("Mesh");
        item3d.addProperty("mesh", &mesh);

        mesh.addProperty("source", property("modelFileName"));
        mesh.addProperty("options", property("options", QmlGenerator::Optional));

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
    QString result = m_content.value(propertyName, QString());
    if (result.isEmpty() && required)
    {
        m_hasError = true;
        if (!m_errorString.isEmpty())
            m_errorString += "\n";
        m_errorString += QObject::tr("Expected property %1: none defined").arg(propertyName);
    }
    return result;
}
