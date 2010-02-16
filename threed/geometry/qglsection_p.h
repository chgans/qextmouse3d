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

#ifndef QGLSECTION_H
#define QGLSECTION_H

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

#include "qglpainter.h"
#include "qlogicalvertex.h"
#include "qbox3d.h"
#include "qglnamespace.h"

#include <QtOpenGL/qgl.h>
#include <QtGui/qmatrix4x4.h>

QT_BEGIN_NAMESPACE

class QGLPainter;
class QGLDisplayList;
class QGLSectionPrivate;
class QGeometryData;
class QGLSceneNode;

class Q_QT3D_EXPORT QGLSection : public QGeometryData
{
public:
    QGLSection(QGLDisplayList *d, QGL::Smoothing sm = QGL::Smooth, QGL::Strategy st = QGL::HashLookup);
    ~QGLSection();

    void reserve(int amount);

    QGLIndexArray indices() const;
    int indexCount() const;
    void append(const QLogicalVertex &lv);
    void append(const QLogicalVertex &a, const QLogicalVertex &b, const QLogicalVertex &c);
    void appendSmooth(const QLogicalVertex &lv);
    void appendSmooth(const QLogicalVertex &a, const QLogicalVertex &b, const QLogicalVertex &c)
    {
        appendSmooth(a);
        appendSmooth(b);
        appendSmooth(c);
    }
    void appendFaceted(const QLogicalVertex &lv);
    void appendFaceted(const QLogicalVertex &a, const QLogicalVertex &b, const QLogicalVertex &c)
    {
        appendFaceted(a);
        appendFaceted(b);
        appendFaceted(c);
    }
    void appendFlat(const QLogicalVertex &lv);
    void appendFlat(const QLogicalVertex &a, const QLogicalVertex &b, const QLogicalVertex &c)
    {
        appendFlat(a);
        appendFlat(b);
        appendFlat(c);
    }
    inline QGL::Smoothing smoothing() const;
    inline QGLDisplayList *displayList() const;
    QList<QGLSceneNode*> nodes() const;
    void addNode(QGLSceneNode *node);
    bool deleteNode(QGLSceneNode *node);
private:
    Q_DISABLE_COPY(QGLSection);
    friend class QGLDisplayList;

    int appendOne(const QLogicalVertex &vertex);

    QGL::Smoothing m_smoothing;
    QGLDisplayList *m_displayList;
    QGLSectionPrivate *d;
};

inline QGL::Smoothing QGLSection::smoothing() const
{
    return m_smoothing;
}

inline QGLDisplayList *QGLSection::displayList() const
{
    return m_displayList;
}


#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const QGLSection &section);
#endif

QT_END_NAMESPACE

#endif // QGLSECTION_H
