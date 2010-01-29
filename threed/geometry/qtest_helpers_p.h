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

#ifndef QTEST_HELPERS_P_H
#define QTEST_HELPERS_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtTest/QtTest>
#include <QtGui/qvector3d.h>
#include <QtGui/qvector2d.h>
#include "qcolor4b.h"

namespace QTest {
    char *toString(const QVector3D &v)
    {
        char *msg = new char[128];
        qt_snprintf(msg, 128, "(%0.4f, %0.4f, %0.4f)", v.x(), v.y(), v.z());
        return msg;
    }

    char *toString(const QVector2D &v)
    {
        char *msg = new char[128];
        qt_snprintf(msg, 128, "(%0.4f, %0.4f)", v.x(), v.y());
        return msg;
    }

    char *toString(const QColor4B &c)
    {
        char *msg = new char[128];
        qt_snprintf(msg, 128, "R: %0.2f, G: %0.2f, B: %0.2f, A: %0.2)",
                    c.redF(), c.greenF(), c.blueF(), c.alphaF());
        return msg;
    }

    template<> bool qCompare<QVector3D>(const QVector3D &t1, const QVector3D &t2,
                                        const char *actual, const char *expected,
                                        const char *file, int line)
    {
        if (t1 == t2)
        {
            return compare_helper(true, "COMPARE()", file, line);
        }
        else
        {
            return compare_helper(false, "Compared QVector3D values are not the same:",
                                  toString(t1), toString(t2), actual, expected, file, line);
        }
    }

    template<> bool qCompare<QVector2D>(const QVector2D &t1, const QVector2D &t2,
                                        const char *actual, const char *expected,
                                        const char *file, int line)
    {
        if (t1 == t2)
        {
            return compare_helper(true, "COMPARE()", file, line);
        }
        else
        {
            return compare_helper(false, "Compared QVector2D values are not the same:",
                                  toString(t1), toString(t2), actual, expected, file, line);
        }
    }

    template<> bool qCompare<QColor4B>(const QColor4B &t1, const QColor4B &t2,
                                        const char *actual, const char *expected,
                                        const char *file, int line)
    {
        if (t1 == t2)
        {
            return compare_helper(true, "COMPARE()", file, line);
        }
        else
        {
            return compare_helper(false, "Compared QColor4B values are not the same:",
                                  toString(t1), toString(t2), actual, expected, file, line);
        }
    }

};

#endif // QTEST_HELPERS_P_H
