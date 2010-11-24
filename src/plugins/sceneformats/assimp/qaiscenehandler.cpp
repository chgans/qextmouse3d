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

#include "qaiscenehandler.h"
#include "qaiscene.h"
#include "ailoaderiosystem.h"

#include "aiScene.h"
#include "aiPostProcess.h"
#include "DefaultLogger.h"

#include <QtCore/qdir.h>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

QAiSceneHandler::QAiSceneHandler()
    : m_options(aiProcessPreset_TargetRealtime_Quality)  // see decodeOptions
    , m_showWarnings(false)
    , m_mayHaveLinesPoints(false)
{
    // by default remove per vertex colors from the data - no-one uses that in
    // models - if they need it it can be turned on with UseVertexColors
    m_removeComponentFlags = m_importer.GetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS);
    m_removeComponentFlags |= aiComponent_COLORS;

    // by default remove points and lines from the model, since these are usually
    // degenerate structures from bad modelling or bad import/export.  if they
    // are needed it can be turned on with IncludeLinesPoints
    m_removeSortFlags = m_importer.GetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE);
    m_removeSortFlags |= aiPrimitiveType_POINT | aiPrimitiveType_LINE;
}

QAiSceneHandler::~QAiSceneHandler()
{
    // nothing to do
}

void QAiSceneHandler::decodeOptions(const QString &options)
{
    if (options.isEmpty())
        return;

    // see aiPostProcess.h for this default set of values - its exactly
    // what we want, but allow the user to override some settings
    m_options = static_cast<aiPostProcessSteps>(aiProcessPreset_TargetRealtime_Quality);

    static const char *validOptions[] = {
        "NoOptions"
        "ShowWarnings",
        "CalculateNormals",
        "ForceFaceted",
        "IncludeAllMaterials",
        "IncludeLinesPoints",
        "FixNormals",
        "DeDupMeshes",
        "Optimize",
        "FlipUVs",
        "FlipWinding",
        "UseVertexColors",
        "VertexSplitLimitx2",
        "TriangleSplitLimitx2"
    };

    static int optionKeys[] = {
        NoOptions,
        ShowWarnings,
        CalculateNormals,
        ForceFaceted,
        IncludeAllMaterials,
        IncludeLinesPoints,
        FixNormals,
        DeDupMeshes,
        Optimize,
        FlipUVs,
        FlipWinding,
        UseVertexColors,
        VertexSplitLimitx2,
        TriangleSplitLimitx2
        -1
    };

    QStringList opList = options.simplified().split(QLatin1Char(' '), QString::SkipEmptyParts);
    for (int i = 0; i < opList.count(); ++i)
    {
        QString op = opList.at(i);
        op = op.trimmed();
        int k = 0;
        for ( ; optionKeys[k] != -1; ++k)
            if (op == QLatin1String(validOptions[k]))
                break;
        if (optionKeys[k] != -1) // found
        {
            Options o = static_cast<Options>(optionKeys[k]);
            switch (o)
            {
            case NoOptions:
                break;
            case ShowWarnings:
                if (m_showWarnings)
                {
                    m_importer.SetExtraVerbose(true);
                }
                else
                {
                    m_showWarnings = true;
                    m_options |= aiProcess_ValidateDataStructure;
                }
                break;
            case CalculateNormals:
                m_removeComponentFlags |= aiComponent_NORMALS;
                m_options |= aiProcess_GenSmoothNormals;
                m_options &= ~aiProcess_GenNormals;
                break;
            case ForceFaceted:
                m_removeComponentFlags |= aiComponent_NORMALS;
                m_options |= aiProcess_GenNormals;
                m_options &= ~aiProcess_GenSmoothNormals;
                break;
            case IncludeAllMaterials:
                m_options &= ~aiProcess_RemoveRedundantMaterials;
                break;
            case IncludeLinesPoints:
                m_removeSortFlags &= ~(aiPrimitiveType_LINE | aiPrimitiveType_POINT);
                m_mayHaveLinesPoints = true;
                // leave it with the FindDegenerates option turned on - we want zero
                // area triangles to display as proper GL lines or points
                break;
            case FixNormals:
                m_options |= aiProcess_FixInfacingNormals;
                break;
            case DeDupMeshes:
                m_options |= aiProcess_FindInstances;
                break;
            case Optimize:
                m_options |= aiProcess_OptimizeGraph | aiProcess_OptimizeMeshes;
                break;
            case FlipUVs:
                m_options |= aiProcess_FlipUVs;
                break;
            case FlipWinding:
                m_options |= aiProcess_FlipWindingOrder;
                break;
            case UseVertexColors:
                m_removeComponentFlags &= ~aiComponent_COLORS;
                break;
            case VertexSplitLimitx2:
                m_meshSplitVertexLimit << 1;
                // repeating this in the option string more than once works...
                break;
            case TriangleSplitLimitx2:
                // ....and we're OK with that, just don't overdo it
                m_meshSplitTriangleLimit << 1;
                break;
            }
        }
        else
        {
            qWarning("Bad option: \"%s\" in %s", qPrintable(op),
                     qPrintable(options));
        }
    }
}

QGLAbstractScene *QAiSceneHandler::read()
{
    importer.SetIOHandler(new AiLoaderIOSystem(device()));

    Assimp::Logger *log = 0;
    Assimp::Logger::LogSeverity severity = Assimp::Logger::NORMAL;
    if (m_showWarnings)
    {
        severity = Assimp::Logger::VERBOSE;
        log = Assimp::DefaultLogger::create("AssimpLog.txt", severity, aiDefaultLogStream_FILE);
    }
#ifdef Q_CC_MSVC
    log->attachStream(aiDefaultLogStream_DEBUGGER, severity);
#else
    log->attachStream(aiDefaultLogStream_STDERR, severity);
#endif

    QString path;
    QUrl u = url();
    if (u.scheme() != QLatin1String("file"))
    {
        qWarning("Non-file URL's not yet supported");
        return 0;
    }
    path = u.toLocalFile();

    if (m_removeComponentFlags)
        m_options |= aiProcess_RemoveComponent;
    else
        m_options &= ~aiProcess_RemoveComponent;

    m_importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, m_removeComponentFlags);
    m_importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, m_removeSortFlags);
    m_importer.SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, m_meshSplitVertexLimit);
    m_importer.SetPropertyInteger(AI_CONFIG_PP_SLM_TRIANGLE_LIMIT, m_meshSplitTriangleLimit);

    // force this on, and provide no way to turn it off.  Its set by the
    // aiProcessPreset_TargetRealtime_Quality option in the constructor.
    // Guarantees that all meshes only have one primitive type
    Q_ASSERT(m_options & aiProcess_SortByPType);

    // the importer owns the scene, so when the it goes out of scope on exiting
    // this function the scene will get destroyed
    const aiScene* scene = importer.ReadFile(path.toStdString(), m_options);

    if (scene && scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
    {
        if (log)
            log->warn("Mesh incomplete - aborting load");
        scene = 0;
    }

    if ((m_options & aiProcess_ValidateDataStructure)
            && scene && !(scene->mFlags & AI_SCENE_FLAGS_VALIDATED))
    {
        if (log)
            log->warn("Mesh invalid - aborting load");
        scene = 0;
    }

    if (!scene)
    {
        QString c = QDir::current().absolutePath();
        qWarning("Asset importer error: %s\n", importer.GetErrorString());
        if (log)
            qWarning("For details check log: %s/AssimpLog.txt\n", qPrintable(c));
        return 0;
    }


    QAiScene *qscene = new QAiScene(scene, this);

    Assimp::DefaultLogger::kill();

    return qscene;
}

QT_END_NAMESPACE
