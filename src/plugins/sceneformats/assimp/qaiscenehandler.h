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

#ifndef QGL3DSSCENEHANDLER_H
#define QGL3DSSCENEHANDLER_H

#include "qglsceneformatplugin.h"

#include "aiPostProcess.h"
#include "assimp.hpp"

#include <QtCore/qurl.h>

QT_BEGIN_NAMESPACE

class QIODevice;

class QAiSceneHandler : public QGLSceneFormatHandler
{
public:
    enum Options {
        NoOptions,
        ShowWarnings,        // show any warnings while loading the file
        CalculateNormals,    // replace normals from the file with smooth generated ones
        ForceFaceted,        // generate non-smooth normals (implies CalculateNormals)
        IncludeAllMaterials, // include even redundant (unused) materials
        IncludeLinesPoints,  // include even collapsed triangles (lines or points)
        FixNormals,          // try to fix incorrect (in facing) normals
        DeDupMeshes,         // replace copied meshes with refs to a single instance
        Optimize,            // collapse meshes, nodes & scene heierarchies
        FlipUVs,             // flips UV's on the y-axis (for upside-down textures)
        FlipWinding,         // makes faces CW instead of CCW
        UseVertexColors,     // use vertex colors that are in a model
        VertexSplitLimitx2,  // double the vertex count which will split a large mesh
        TriangleSplitLimitx2 // double the triangle count which will split a large mesh
    };

    QAiSceneHandler();
    ~QAiSceneHandler();

    QGLAbstractScene *read();

    void decodeOptions(const QString &options);

    bool showWarnings() const { return m_showWarnings; }
    bool mayHaveLinesPoints() const { return m_mayHaveLinesPoints; }

    aiPostProcessSteps options() const { return m_options; }
    quint32 removeComponentFlags() const { return m_removeComponentFlags; }
    quint32 removeSortFlags() const { return m_removeSortFlags; }

private:
    aiPostProcessSteps m_options;
    bool m_showWarnings;
    bool m_mayHaveLinesPoints;
    int m_meshSplitVertexLimit;
    int m_meshSplitTriangleLimit;
    Assimp::Importer m_importer;
    quint32 m_removeComponentFlags;
    quint32 m_removeSortFlags;
};

QT_END_NAMESPACE

#endif
