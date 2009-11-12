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
#include "qglobjgeometry.h"
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
    QGLObjGeometry *geometry = new QGLObjGeometry();
    QByteArray line;
    QByteArray keyword;
    int posn, index, count;
    int tindex, nindex;
    QGLVertexArray positions(QGL::Position, 3);
    QGLVertexArray texCoords(QGL::TextureCoord0, 2);
    QGLVertexArray normals(QGL::Normal, 3);
    QGLVertexArray finalPositions(QGL::Position, 3);
    QGLVertexArray finalTexCoords(QGL::TextureCoord0, 2);
    QGLVertexArray finalNormals(QGL::Normal, 3);
    QGLIndexArray indices;
    qreal x, y, z;
    bool smooth = false;
    QGLIndexArray materialIndices;
    QGLMaterialParameters *material = 0;
    QGLObjGroup group;
    bool needNormals = false;

    // Create the default group properties.
    group.name = QString();
    group.firstVertex = 0;
    group.vertexCount = 0;

    // Create the material palette for the geometry object.
    palette = new QGLMaterialCollection();
    geometry->setPalette(palette);

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
            posn = objSkipWS(line, posn);
            count = 0;
            QList<int> extras;
            while (posn < line.size()) {
                // Note: we currently only read the initial vertex
                // index and also use it for texture co-ordinates
                // and normals.  e.g. "2/2", "3/3", etc.  This will
                // need to be fixed to handle "2/1", "3/7", etc.
                index = objReadInteger(line, &posn);
                tindex = objReadSlashInteger(line, &posn);
                nindex = objReadSlashInteger(line, &posn);
                if (index < 0)
                    index = positions.vertexCount() + index;
                else if (index > 0)
                    --index;        // Indices in obj are 1-based.
                if (index >= 0 && index < positions.vertexCount()) {
                    finalPositions.append(positions.vector3DAt(index, 0));
                    index = finalPositions.vertexCount() - 1;
                }
                if (tindex < 0)
                    tindex = texCoords.vertexCount() + tindex;
                else if (tindex > 0)
                    --tindex;        // Indices in obj are 1-based.
                else
                    tindex = -1;
                if (tindex >= 0 && tindex < texCoords.vertexCount())
                    finalTexCoords.append(texCoords.vector2DAt(tindex, 0));
                else if (!texCoords.isEmpty())
                    finalTexCoords.append(0.0f, 0.0f);
                if (nindex < 0)
                    nindex = normals.vertexCount() + nindex;
                else if (nindex > 0)
                    --nindex;        // Indices in obj are 1-based.
                else
                    nindex = -1;
                if (nindex >= 0 && nindex < normals.vertexCount())
                    finalNormals.append(normals.vector3DAt(nindex, 0));
                else if (!normals.isEmpty()) {
                    finalNormals.append(0.0f, 0.0f, 0.0f);
                    needNormals = true;
                }
                if (count < 3)
                    indices.append(index);
                else
                    extras.append(index);
                ++count;
                posn = objSkipNonWS(line, posn, 0);
                posn = objSkipWS(line, posn);
            }
            if (count >= 4) {
                // Subdivide quads and polygons into triangles for simplicity.
                int first = indices[indices.size() - 3];
                int last = indices[indices.size() - 1];
                for (index = 0; index < extras.size(); ++index) {
                    int next = extras[index];
                    indices.append(first);
                    indices.append(last);
                    indices.append(next);
                    last = next;
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
                    material = palette->materialByIndex(index);
                    materialIndices.append(indices.size());
                    materialIndices.append(index);
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
            smooth = (arg == "on" || arg == "1");
        } else if (keyword == "g" || keyword == "o") {
            // Label the faces that follow as part of a named group or object.
            posn = objSkipWS(line, posn);
            QString objectName = QString::fromLocal8Bit(line.mid(posn));
            if (group.firstVertex < indices.size() &&
                    !group.name.isEmpty()) {
                group.vertexCount = indices.size() - group.firstVertex;
                geometry->groups.append(group);
            }
            group.name = objectName;
            group.firstVertex = indices.size();
            group.vertexCount = 0;
        } else {
            qWarning() << "unsupported obj command: " << keyword.constData();
        }
    }

    // Add the last named group.
    if (group.firstVertex < indices.size() && !group.name.isEmpty()) {
        group.vertexCount = indices.size() - group.firstVertex;
        geometry->groups.append(group);
    }

    // Replace the vertex temporary arrays with the final arrangement.
    positions = finalPositions;
    texCoords = finalTexCoords;
    normals = finalNormals;

    // Calculate normals if they aren't present.
    if (normals.isEmpty() || needNormals) {
        // Reserve space and clear all of the normals.
        if (normals.isEmpty()) {
            normals.reserve(positions.vertexCount());
            for (index = 0; index < positions.vertexCount(); ++index)
                normals.append(0.0f, 0.0f, 0.0f);
        }

        // Calculate face-specific normals and apply them to the vertices.
        // If a vertex is shared between multiple faces, it will accumulate
        // the sum to be normalized below.
        for (index = 0; index <= (indices.size() - 3); index += 3) {
            int index1 = indices[index];
            int index2 = indices[index + 1];
            int index3 = indices[index + 2];
            QVector3D pt1 = positions.vector3DAt(index1, 0);
            QVector3D pt2 = positions.vector3DAt(index2, 0);
            QVector3D pt3 = positions.vector3DAt(index3, 0);
            QVector3D normal = QVector3D::crossProduct(pt2 - pt1, pt3 - pt1);
            QVector3D norm1 = normals.vector3DAt(index1, 0);
            QVector3D norm2 = normals.vector3DAt(index2, 0);
            QVector3D norm3 = normals.vector3DAt(index3, 0);
            if (smooth) {
                normals.setAt(index1, 0, norm1 + normal);
                normals.setAt(index2, 0, norm2 + normal);
                normals.setAt(index3, 0, norm3 + normal);
            } else {
                // When smoothing is off, then shared vertices need
                // to be split into several different vertices so
                // that they can have separate normals for each face.
                if (norm1.isNull()) {
                    normals.setAt(index1, 0, normal);
                } else {
                    indices[index] = positions.vertexCount();
                    positions.append(pt1);
                    if (!texCoords.isEmpty())
                        texCoords.append(texCoords.vector2DAt(index1, 0));
                    normals.append(normal);
                }
                if (norm2.isNull()) {
                    normals.setAt(index2, 0, normal);
                } else {
                    indices[index + 1] = positions.vertexCount();
                    positions.append(pt2);
                    if (!texCoords.isEmpty())
                        texCoords.append(texCoords.vector2DAt(index2, 0));
                    normals.append(normal);
                }
                if (norm3.isNull()) {
                    normals.setAt(index3, 0, normal);
                } else {
                    indices[index + 2] = positions.vertexCount();
                    positions.append(pt3);
                    if (!texCoords.isEmpty())
                        texCoords.append(texCoords.vector2DAt(index3, 0));
                    normals.append(normal);
                }
            }
        }

        // If any normals are still zero, set them to (0, 0, 1).
        // Otherwise normalize the cross-product sums.
        for (index = 0; index < normals.vertexCount(); ++index) {
            QVector3D current = normals.vector3DAt(index, 0);
            if (current.isNull())
                normals.setAt(index, 0, QVector3D(0.0f, 0.0f, 1.0f));
            else
                normals.setAt(index, 0, current.normalized());
        }
    }

    // Create the combined vertex array.
    QGLVertexArray combined = positions;
    if (!texCoords.isEmpty())
        combined = combined.interleaved(texCoords);
    if (!normals.isEmpty())
        combined = combined.interleaved(normals);

    geometry->setVertexArray(combined);
    geometry->setIndexArray(indices);
    geometry->setMaterialIndexArray(materialIndices);
    geometry->setDrawingMode(QGL::Triangles);

    // Create a scene from the geometry, which will split it out
    // into several mesh objects for each of the group names.
    return new QGLObjScene(geometry);
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
