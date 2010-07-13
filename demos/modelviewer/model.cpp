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


#include "model.h"
#include "qglabstractscene.h"
#include "qglscenenode.h"

#include <QtCore/qsettings.h>
#include <QtCore/qdatetime.h>
#include <QtGui/qitemselectionmodel.h>

#include <QIcon>

struct MVCNode
{
    MVCNode(QGLSceneNode *_n, Model *_m)
        : node(_n), model(_m), parent(0), row(0), col(0) {}
    ~MVCNode() { qDeleteAll(rows); }
    void addMVCChildren(QGLSceneNode *node)
    {
        QList<QGLSceneNode*> ch = node->childNodeList();
        for (int row = 0; row < ch.size(); ++row)
        {
            QGLSceneNode *c = ch.at(row);
            MVCNode *n = new MVCNode(c, model);
            model->m_selectionMap.insert(c, n);
            n->row = row;
            n->parent = this;
            rows.append(n);
            n->addMVCChildren(c);
        }
    }
    QGLSceneNode *node;
    Model *model;
    QList<MVCNode*> rows;
    MVCNode *parent;
    int row;
    int col;
};

Model::Model(QObject *parent)
    : QAbstractItemModel(parent)
    , m_sceneManager(0)
    , m_sceneRoot(0)
    , m_modelRoot(0)
{
}

QString Model::getOptions() const
{
    QSettings settings;
    settings.beginGroup("ModelSettings");
    QByteArray coded = QUrl::toPercentEncoding(m_fullPath);
    QString modelEncoded(coded);
    settings.beginGroup(modelEncoded);

    QString options;
    QStringList keys = settings.childKeys();
    QStringList groups = settings.childGroups();
    QString key;
    QString group;
    while (keys.size() > 0 || groups.size() > 0)
    {
        if (keys.size() > 0)
        {
            key = keys.takeFirst();
            if (!key.startsWith("UI_"))
            {
                bool value = settings.value(key, false).toBool();
                if (value)
                {
                    QString op = group.isEmpty() ? key : QString("%1=%2").arg(group).arg(key);
                    options = options.isEmpty() ? op : options + " " + op;
                }
            }
        }
        if (keys.size() == 0 && groups.size() > 0)
        {
            if (!group.isEmpty())
                settings.endGroup();
            group = groups.takeFirst();
            settings.beginGroup(group);
            keys = settings.childKeys();
        }
    }
    return options;
}

QStringList Model::components() const
{
    if (m_components.isEmpty() && m_sceneRoot)
    {
        QList<QGLSceneNode *> all = m_sceneRoot->allChildren();
        QList<QGLSceneNode *>::const_iterator it = all.constBegin();
        for ( ; it != all.constEnd(); ++it)
        {
            QGLSceneNode *s = *it;
            if (!m_components.contains(s->objectName()))
                m_components.append(s->objectName());
        }
    }
    return m_components;
}

void Model::setFullPath(const QString &path)
{
    if (!m_fullPath.isEmpty())
        emit modelUnloaded(m_fullPath);
    m_fullPath = path;
    importModel();
    buildModel();
    if (m_sceneRoot)
        emit modelLoaded(m_fullPath);
    reset();
}

QModelIndex Model::selectNode(QGLSceneNode *node) const
{
    MVCNode *n = m_selectionMap.value(node, 0);
    Q_ASSERT(n);
    QModelIndex parent = createIndex(n->parent->row, n->parent->col, n->parent);
    QModelIndex ix = index(n->row, n->col, parent);
    return ix;
}


void Model::importModel()
{
    delete m_sceneManager;
    m_sceneManager = 0;
    m_sceneRoot = 0;
    QString options = getOptions();
    QTime time;
    time.start();
    m_sceneManager = QGLAbstractScene::loadScene(m_fullPath, QString(), options);
    int ms = time.elapsed();
    emit modelLoadTime(ms);
    if (!m_sceneManager)
        return;
    m_sceneManager->setPickable(true);
    QGLSceneNode *obj = m_sceneManager->defaultObject(QGLSceneNode::Main);
    m_sceneRoot = qobject_cast<QGLSceneNode *>(obj);
#ifndef QT_NO_DEBUG_STREAM
    int totalIndexes = 0;
    QList<QGLSceneNode *> children = m_sceneRoot->allChildren();
    QList<QGLSceneNode*>::const_iterator it(children.begin());
    for ( ; it != children.end(); ++it)
    {
        QGLSceneNode  *n = *it;
        totalIndexes += n->count();
    }
    emit modelTriangles(totalIndexes / 3);
#endif
}

void Model::buildModel()
{
    delete m_modelRoot;
    m_selectionMap.clear();
    m_modelRoot = new MVCNode(m_sceneRoot, this);
    m_selectionMap.insert(m_sceneRoot, m_modelRoot);
    m_modelRoot->addMVCChildren(m_sceneRoot);
}

QModelIndex Model::index(int row, int column, const QModelIndex & parent) const
{
    QModelIndex result;
    MVCNode *p = 0;
    if (m_modelRoot)
    {
        if (parent.isValid())
        {
            p = static_cast<MVCNode*>(parent.internalPointer());
            Q_ASSERT(p);
            if (column == 0 && row < p->rows.count())
            {
                MVCNode *node = p->rows.at(row);
                result = createIndex(row, column, node);
            }
        }
        else
        {
            Q_ASSERT(row == 0 && column == 0);
            result = createIndex(row, column, m_modelRoot);
        }
    }
    return result;
}

QModelIndex Model::parent(const QModelIndex &index) const
{
    QModelIndex result;
    MVCNode *node = 0;
    if (index.isValid())
    {
        node = static_cast<MVCNode*>(index.internalPointer());
        MVCNode *parent = node->parent;
        if (parent)
            result = createIndex(parent->row, parent->col, parent);
    }
    return result;
}

int Model::rowCount(const QModelIndex & parent) const
{
    int count = 0;
    if (m_sceneRoot)
    {
        if (parent.isValid())
        {
            MVCNode *node = static_cast<MVCNode*>(parent.internalPointer());
            count = node->rows.size();
        }
        else
        {
            count = 1;
        }
    }
    return count;
}

int Model::columnCount(const QModelIndex &parent) const
{
    int count = 0;
    if (m_modelRoot)
    {
        if (parent.isValid())
        {
            MVCNode *node = static_cast<MVCNode*>(parent.internalPointer());
            if (node->rows.size() > 0)
                count = 1;
        }
        else
        {
            count = 1;
        }
    }
    return count;
}

QVariant Model::data(const QModelIndex & index, int role) const
{
    QVariant result;
    MVCNode *d = static_cast<MVCNode*>(index.internalPointer());
    if (role == Qt::DisplayRole)
    {
        return d->node->objectName();
    }
    else if (role == Qt::DecorationRole)
    {
        if (d->node == m_sceneRoot)
            result = QIcon(":/images/file16x16.png");
        else if (d->node->childNodeList().count() > 0)
            result = QIcon(":/images/mesh16x16.png");
        else
            result = QIcon(":/images/red-dot.png");
    }
    return result;
}
