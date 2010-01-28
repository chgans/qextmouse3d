/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qglobjscenehandler.h"
#include "qglobjscene.h"
#include "qvectorarray.h"
#include "qgldisplaylist.h"
#include "qgloperation.h"
#include <QtCore/qiodevice.h>
#include <QtCore/qfile.h>
#include <QtGui/qimage.h>

QT_BEGIN_NAMESPACE

// Documentation for OBJ and MTL files from:
// http://www.fileformat.info/format/wavefrontobj/egff.htm
// http://www.fileformat.info/format/material/

static int objSkipWS(const QByteArray& line, int posn)
{
    while (posn < line.size() && (line[posn] == ' ' || line[posn] == '\t'))
        ++posn;
    return posn;
}

static int objSkipNonWS(const QByteArray& line, int posn, QChar stopch)
{
    while (posn < line.size() &&
            line[posn] != ' ' && line[posn] != '\t' && line[posn] != stopch)
        ++posn;
    return posn;
}

static qreal objReadFloat(const QByteArray& line, int *posn)
{
    *posn = objSkipWS(line, *posn);
    int end = objSkipNonWS(line, *posn, 0);
    qreal value;
    if (end > *posn)
        value = qreal(line.mid(*posn, end - *posn).toDouble());
    else
        value = 0.0f;
    *posn = end;
    return value;
}

static int objReadInteger(const QByteArray& line, int *posn)
{
    *posn = objSkipWS(line, *posn);
    int end = objSkipNonWS(line, *posn, '/');
    int value;
    if (end > *posn)
        value = line.mid(*posn, end - *posn).toInt();
    else
        value = 0;
    //*posn = objSkipNonWS(line, end, 0);
    *posn = end;
    return value;
}

static int objReadSlashInteger(const QByteArray& line, int *posn)
{
    if (*posn >= line.size() || line[*posn] != '/')
        return 0;
    ++(*posn);
    int end = objSkipNonWS(line, *posn, '/');
    int value;
    if (end > *posn)
        value = line.mid(*posn, end - *posn).toInt();
    else
        value = 0;
    //*posn = objSkipNonWS(line, end, 0);
    *posn = end;
    return value;
}

static QColor objReadColor(const QByteArray& line, int posn)
{
    qreal red = objReadFloat(line, &posn);
    qreal green = objReadFloat(line, &posn);
    qreal blue = objReadFloat(line, &posn);
    qreal alpha = 1.0f;
    posn = objSkipWS(line, posn);
    if (posn < line.size())
        alpha = objReadFloat(line, &posn);
    return QColor::fromRgbF(red, green, blue, alpha);
}

QGLAbstractScene *QGLObjSceneHandler::read()
{
    QByteArray line;
    QByteArray keyword;
    int posn, index, count;
    int tindex, nindex;
    QVector3DArray positions;
    QVector2DArray texCoords;
    QVector3DArray normals;
    QVector3DArray facePositions;
    QVector2DArray faceTexCoords;
    QVector3DArray faceNormals;
    QGLIndexArray indices;
    qreal x, y, z;
    QGLIndexArray materialIndices;
    QGLMaterialParameters *material = 0;
    QGL::Smoothing smoothing = QGL::Faceted;
    QGLSceneNode *defaultNode;
    QList<QGLSceneObject *> groups;

    // Create the display list and start an initial Faceted section.
    QGLDisplayList *dlist = new QGLDisplayList();
    dlist->newSection(smoothing);
    palette = dlist->geometry()->palette();
    defaultNode = dlist;
    defaultNode->setObjectName(QLatin1String("__main"));
    dlist->pushNode();

    while (!device()->atEnd()) {
        // Read the next line, including any backslash continuations.
        line = device()->readLine().trimmed();
        while (line.endsWith('\\')) {
            line.truncate(line.size() - 1);
            if (device()->atEnd())
                break;
            line += device()->readLine().trimmed();
        }
        if (line.startsWith('#') || line.isEmpty())
            continue;   // Skip comments and blank lines.

        // Extract the keyword at the start of the line.
        posn = 0;
        while (posn < line.size() &&
               line[posn] != ' ' && line[posn] != '\t')
            ++posn;
        keyword = line.left(posn);

        // Determine how to process this line from the keyword.
        if (keyword == "v") {
            x = objReadFloat(line, &posn);
            y = objReadFloat(line, &posn);
            z = objReadFloat(line, &posn);
            positions.append(x, y, z);
        } else if (keyword == "vt") {
            x = objReadFloat(line, &posn);
            y = objReadFloat(line, &posn);
            texCoords.append(x, y);
        } else if (keyword == "vn") {
            x = objReadFloat(line, &posn);
            y = objReadFloat(line, &posn);
            z = objReadFloat(line, &posn);
            normals.append(x, y, z);
        } else if (keyword == "f") {
            facePositions.resize(0);
            faceTexCoords.resize(0);
            faceNormals.resize(0);
            posn = objSkipWS(line, posn);
            count = 0;
            while (posn < line.size()) {
                // Note: we currently only read the initial vertex
                // index and also use it for texture co-ordinates
                // and normals.  e.g. "2/2", "3/3", etc.  This will
                // need to be fixed to handle "2/1", "3/7", etc.
                index = objReadInteger(line, &posn);
                tindex = objReadSlashInteger(line, &posn);
                nindex = objReadSlashInteger(line, &posn);
                if (index < 0)
                    index = positions.count() + index;
                else if (index > 0)
                    --index;        // Indices in obj are 1-based.
                if (index >= 0 && index < positions.count())
                    facePositions.append(positions[index]);
                if (tindex < 0)
                    tindex = texCoords.count() + tindex;
                else if (tindex > 0)
                    --tindex;        // Indices in obj are 1-based.
                else
                    tindex = -1;
                if (tindex >= 0 && tindex < texCoords.count())
                    faceTexCoords.append(texCoords[tindex]);
                if (nindex < 0)
                    nindex = normals.count() + nindex;
                else if (nindex > 0)
                    --nindex;        // Indices in obj are 1-based.
                else
                    nindex = -1;
                if (nindex >= 0 && nindex < normals.count())
                    faceNormals.append(normals[nindex]);
                ++count;
                posn = objSkipNonWS(line, posn, 0);
                posn = objSkipWS(line, posn);
            }
            if (0) { //count <= 3) {
                QGLOperation op(dlist, QGL::TRIANGLE);
                op.addVertexArray(facePositions);
                if (!faceTexCoords.isEmpty())
                    op.addTexCoordArray(faceTexCoords);
                if (!faceNormals.isEmpty())
                    op.addNormalArray(faceNormals);
            } else {
                // Subdivide quads and polygons into triangles for simplicity.
                QGLOperation op(dlist, QGL::TRIANGLE);
                for (index = 2; index < facePositions.size(); ++index) {
                    op.addVertex(facePositions[0]);
                    op.addVertex(facePositions[index - 1]);
                    op.addVertex(facePositions[index]);
                    if (!faceTexCoords.isEmpty()) {
                        op.addTexCoord(faceTexCoords.value(0));
                        op.addTexCoord(faceTexCoords.value(index - 1));
                        op.addTexCoord(faceTexCoords.value(index));
                    }
                    if (!faceNormals.isEmpty()) {
                        op.addNormal(faceNormals.value(0));
                        op.addNormal(faceNormals.value(index - 1));
                        op.addNormal(faceNormals.value(index));
                    }
                }
            }
        } else if (keyword == "usemtl") {
            // Specify a material for the faces that follow.
            posn = objSkipWS(line, posn);
            QString materialName = QString::fromLocal8Bit(line.mid(posn));
            if (!materialName.isEmpty() &&
                materialName != QLatin1String("(null)")) {
                index = palette->materialIndexByName(materialName);
                if (index != -1) {
                    QGLSceneNode *node = dlist->newNode();
                    node->setMaterial(index);
                    if (palette->texture(index))
                        node->setEffect(QGL::LitDecalTexture2D);
                    else
                        node->setEffect(QGL::LitMaterial);
                } else {
                    qWarning() << "obj material" << materialName << "not found";
                    material = 0;
                }
            }
        } else if (keyword == "mtllib") {
            // Load a material library.
            posn = objSkipWS(line, posn);
            QByteArray filename = line.mid(posn);
            loadMaterialLibrary(QString::fromLocal8Bit(filename));
        } else if (keyword == "s") {
            // Set smoothing on or off.
            posn = objSkipWS(line, posn);
            index = objSkipNonWS(line, posn, 0);
            QByteArray arg = line.mid(posn, index - posn);
            QGL::Smoothing smooth;
            if (arg == "on" || arg == "1")
                smooth = QGL::Smooth;
            else
                smooth = QGL::Faceted;
            if (smoothing != smooth) {
                smoothing = smooth;
                dlist->newSection(smooth);
            }
        } else if (keyword == "g" || keyword == "o") {
            // Label the faces that follow as part of a named group or object.
            posn = objSkipWS(line, posn);
            QString objectName = QString::fromLocal8Bit(line.mid(posn));
            dlist->popNode();
            QGLSceneNode *node = dlist->pushNode();
            node->setObjectName(objectName);
            groups.append(node);
        } else {
            qWarning() << "unsupported obj command: " << keyword.constData();
        }
    }

    // Create a scene from the geometry, which will split it out
    // into several mesh objects for each of the group names.
    dlist->finalize();
    return new QGLObjScene(dlist, defaultNode, groups);
}

void QGLObjSceneHandler::loadMaterialLibrary(const QString& name)
{
    QUrl materialUrl = url().resolved(name);
    if (materialUrl.scheme() == QLatin1String("file")) {
        QFile file(materialUrl.toLocalFile());
        if (!file.open(QIODevice::ReadOnly))
            qWarning() << "QGLObjSceneHandler::loadMaterialLibrary: could not open:" << materialUrl.toLocalFile();
        else
            loadMaterials(&file);
    } else {
        // TODO
        qWarning("QGLObjSceneHandler::loadMaterialLibrary: non-file urls not supported");
    }
}

void QGLObjSceneHandler::loadMaterials(QIODevice *device)
{
    QByteArray line;
    QByteArray keyword;
    int posn, index;
    QGLMaterialParameters *material = 0;
    QString materialName;
    QString textureName;

    while (!device->atEnd()) {
        // Read the next line, including any backslash continuations.
        line = device->readLine().trimmed();
        while (line.endsWith('\\')) {
            line.truncate(line.size() - 1);
            if (device->atEnd())
                break;
            line += device->readLine().trimmed();
        }
        if (line.startsWith('#') || line.isEmpty())
            continue;   // Skip comments and blank lines.

        // Extract the keyword at the start of the line.
        posn = 0;
        while (posn < line.size() &&
               line[posn] != ' ' && line[posn] != '\t')
            ++posn;
        keyword = line.left(posn);

        // Determine how to process this line from the keyword.
        if (keyword == "newmtl") {
            // Start a new material definition.
            posn = objSkipWS(line, posn);
            materialName = QString::fromLocal8Bit(line.mid(posn));
            index = palette->materialIndexByName(materialName);
            if (index != -1) {
                qWarning() << "redefining obj material:" << materialName;
                material = palette->materialByIndex(index);
            } else {
                material = new QGLMaterialParameters();
                material->setObjectName(materialName);
                palette->addMaterial(material);
            }
        } else if (keyword == "Ka") {
            // Ambient color of the material.
            if (material)
                material->setAmbientColor(objReadColor(line, posn));
        } else if (keyword == "Kd") {
            // Diffuse color of the material.
            if (material)
                material->setDiffuseColor(objReadColor(line, posn));
        } else if (keyword == "Ks") {
            // Specular color of the material.
            if (material)
                material->setSpecularColor(objReadColor(line, posn));
        } else if (keyword == "map_Kd") {
            // Texture associated with the material.
            posn = objSkipWS(line, posn);
            textureName = QString::fromLocal8Bit(line.mid(posn));
            QGLTexture2D *texture = loadTexture(textureName);
            if (texture) {
                index = palette->materialIndexByName(materialName);
                if (index >= 0)
                    palette->setTexture(index, texture);
                else
                    delete texture;
            }
        } else if (keyword == "d") {
            // "Dissolve factor" of the material, which is its opacity.
            if (material) {
                qreal alpha = objReadFloat(line, &posn);
                QColor ambient = material->ambientColor();
                QColor diffuse = material->diffuseColor();
                ambient.setAlphaF(alpha);
                diffuse.setAlphaF(alpha);
                material->setAmbientColor(ambient);
                material->setDiffuseColor(diffuse);
            }
        } else if (keyword == "Ns") {
            // Specular exponent of the material.
            if (material)
                material->setShininess(qRound(objReadFloat(line, &posn)));
        } else if (keyword == "illum") {
            // Illumination model - ignored at present.
        } else if (keyword == "Ni") {
            // Optical density - ignored at present.
        } else {
            qWarning() << "unsupported obj material command: " << keyword.constData();
        }
    }
}

QGLTexture2D *QGLObjSceneHandler::loadTexture(const QString& name)
{
    QUrl textureUrl = url().resolved(name);
    if (textureUrl.scheme() == QLatin1String("file")) {
        QFile file(textureUrl.toLocalFile());
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "QGLObjSceneHandler::loadTexture: could not open:" << textureUrl.toLocalFile();
            return 0;
        } else {
            file.close();
            QImage image(textureUrl.toLocalFile());
            QGLTexture2D *tex = new QGLTexture2D();
            tex->setImage(image);
            return tex;
        }
    } else {
        // TODO
        qWarning("QGLObjSceneHandler::loadTexture: non-file urls not supported");
        return 0;
    }
}

QT_END_NAMESPACE
