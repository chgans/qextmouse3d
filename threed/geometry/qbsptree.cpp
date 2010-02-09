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

#include "qbsptree.h"

const QBSPTree::BSPIndex QBSPTree::blank[Stride] = {
    MaxIndex,
    MaxIndex,
    MaxIndex,
    MaxIndex,
    MaxIndex,
    MaxIndex,
    MaxIndex
};

const QBSPTree::BSPIndex QBSPTree::MaxIndex = UINT16_MAX;


#ifndef QT_NO_DEBUG

#include <stdlib.h>

void QBSPTree::dump()
{
    const QArray<BSPIndex> *ptrs = pointerData();
    const QArray<QVector3D> *vec = vectorData();
    fprintf(stderr, "======================== QBSPTree - %p ========================\n", this);
    for (int i = 0; i < ptrs->size(); i += QBSPTree::Stride)
    {
        int ix = i / QBSPTree::Stride;
        fprintf(stderr, "Node: %d - QVector3D(%0.3f, %0.3f, %0.3f)\n",
                ix, vec->at(ix).x(), vec->at(ix).y(), vec->at(ix).z());

        if (ptrs->at(i + QBSPTree::EqualTo) == QBSPTree::MaxIndex)
            fprintf(stderr, "    EqualTo:      <null>\n");
        else
            fprintf(stderr, "    EqualTo:      %d\n", ptrs->at(i + QBSPTree::EqualTo));

        if (ptrs->at(i + QBSPTree::LessThanX) == QBSPTree::MaxIndex)
            fprintf(stderr, "    LessThanX:    <null>\n");
        else
            fprintf(stderr, "    LessThanX:    %d\n", ptrs->at(i + QBSPTree::LessThanX));

        if (ptrs->at(i + QBSPTree::GreaterThanX) == QBSPTree::MaxIndex)
            fprintf(stderr, "    GreaterThanX: <null>\n");
        else
            fprintf(stderr, "    GreaterThanX: %d\n", ptrs->at(i + QBSPTree::GreaterThanX));

        if (ptrs->at(i + QBSPTree::LessThanY) == QBSPTree::MaxIndex)
            fprintf(stderr, "    LessThanY:    <null>\n");
        else
            fprintf(stderr, "    LessThanY:    %d\n", ptrs->at(i + QBSPTree::LessThanY));

        if (ptrs->at(i + QBSPTree::GreaterThanY) == QBSPTree::MaxIndex)
            fprintf(stderr, "    GreaterThanY: <null>\n");
        else
            fprintf(stderr, "    GreaterThanY: %d\n", ptrs->at(i + QBSPTree::GreaterThanY));

        if (ptrs->at(i + QBSPTree::LessThanZ) == QBSPTree::MaxIndex)
            fprintf(stderr, "    LessThanZ:    <null>\n");
        else
            fprintf(stderr, "    LessThanZ:    %d\n", ptrs->at(i + QBSPTree::LessThanZ));

        if (ptrs->at(i + QBSPTree::GreaterThanZ) == QBSPTree::MaxIndex)
            fprintf(stderr, "    GreaterThanZ: <null>\n");
        else
            fprintf(stderr, "    GreaterThanZ: %d\n", ptrs->at(i + QBSPTree::GreaterThanZ));

    }
}

#endif
