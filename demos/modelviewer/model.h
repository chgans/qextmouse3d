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


#ifndef MODEL_H
#define MODEL_H

#include <QtCore/qabstractitemmodel.h>
#include <QtCore/qstringlist.h>

class QGLAbstractScene;
class QGLSceneNode;
class MVCNode;

class Model : public QAbstractItemModel
{
Q_OBJECT
public:
    explicit Model(QObject *parent = 0);
    QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex & index) const;
    QModelIndex selectNode(QGLSceneNode *node) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    int columnCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QString fullPath() const { return m_fullPath; }
    QGLSceneNode *scene() const { return m_sceneRoot; }
    QGLAbstractScene *manager() const { return m_sceneManager; }
    QString getOptions() const;
    QStringList components() const;


signals:
    void modelLoaded(const QString &path);
    void modelNotLoaded(const QString &path);
    void modelUnloaded(const QString &path);
    void modelLoadTime(int ms);
    void modelTriangles(int triangles);

public slots:
    void setFullPath(const QString &path);

private:
    void importModel();
    void buildModel();
    QModelIndex getParent(MVCNode *);

    friend class MVCNode;

    QString m_fullPath;
    QGLAbstractScene *m_sceneManager;
    QGLSceneNode *m_sceneRoot;
    MVCNode *m_modelRoot;
    mutable QStringList m_components;
    QMap<QGLSceneNode*,MVCNode*> m_selectionMap;
};

#endif // MODEL_H
