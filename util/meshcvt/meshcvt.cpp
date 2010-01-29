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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <QtGui/qvector3d.h>
#include "qglbeziergeometry.h"

static void meshError(const char *filename)
{
    fprintf(stderr, "%s: does not contain valid mesh data\n", filename);
    exit(1);
}

static int numPatches = 0;
static int numVertices = 0;
static int *patches = 0;
static QVector3D *vertices = 0;
static QVector3D *normals = 0;
static bool isTriangleMesh = false;

int main(int argc, char *argv[])
{
    int depth = 4;
    int bezierOnly = 0;
    int teapotAdjust = 0;
    int reversePatches = 0;
    char buffer[BUFSIZ];
    QGLBezierGeometry bezier;
    QGLGeometry mesh;
    char *filename;
    char *name;
    int numBezierNormals = 0;

    // Validate the command-line arguments.
    if (argc < 3) {
        fprintf(stderr, "Usage: %s [--bezier-only] [--teapot-adjust] [--reverse-patches] mesh-filename name [depth]\n", argv[0]);
        return 1;
    }
    if (!strcmp(argv[1], "--bezier-only")) {
        ++argv;
        --argc;
        bezierOnly = 1;
    }
    if (!strcmp(argv[1], "--teapot-adjust")) {
        // Adjust the vertices to correctly size and position the teapot.
        ++argv;
        --argc;
        teapotAdjust = 1;
    }
    if (!strcmp(argv[1], "--reverse-patches")) {
        // Reverse the coordinate order of patches (fixes teacup).
        ++argv;
        --argc;
        reversePatches = 1;
    }
    filename = argv[1];
    name = argv[2];
    if (argc > 3)
        depth = atoi(argv[3]);

    // Read the mesh data from the input file and determine
    // if it is a Bezier patch mesh or a triangle mesh.
    FILE *file;
    if ((file = fopen(filename, "r")) == NULL) {
        perror(filename);
        return 1;
    }
    if (fscanf(file, "%i\n", &numPatches) != 1)
        meshError(filename);
    patches = new int [numPatches * 17];
    for (int patch = 0; patch < numPatches; ++patch) {
        int *p = patches + patch * 17;
        if (!fgets(buffer, sizeof(buffer), file))
            meshError(filename);
        if (sscanf(buffer, "%i, %i, %i, %i, %i, %i, %i, %i, "
                           "%i, %i, %i, %i, %i, %i, %i, %i, %i\n",
                   p, p + 1, p + 2, p + 3,
                   p + 4, p + 5, p + 6, p + 7,
                   p + 8, p + 9, p + 10, p + 11,
                   p + 12, p + 13, p + 14, p + 15, p + 16) != 17) {
            if (sscanf(buffer, "%i, %i, %i, %i, %i, %i, %i, %i, "
                               "%i, %i, %i, %i, %i, %i, %i, %i\n",
                       p, p + 1, p + 2, p + 3,
                       p + 4, p + 5, p + 6, p + 7,
                       p + 8, p + 9, p + 10, p + 11,
                       p + 12, p + 13, p + 14, p + 15) != 16) {
                if (sscanf(buffer, "%i, %i, %i\n", p, p + 1, p + 2) != 3)
                    meshError(filename);
                isTriangleMesh = true;
            } else {
                p[16] = -1;     // Use the default minimum depth value.
            }
        }
        if (reversePatches && !isTriangleMesh) {
            int reversed[16];
            for (int i = 0; i < 16; ++i)
                reversed[i] = p[(i & 0x0C) + (3 - (i % 4))];
            for (int i = 0; i < 16; ++i)
                p[i] = reversed[i];
        }
        if (!isTriangleMesh) {
            // The standard patch indices are 1-based, not 0-based.
            // Correct for that problem here.
            for (int offset = 0; offset < 16; ++offset)
                --(p[offset]);
        }
    }
    if (fscanf(file, "%i\n", &numVertices) != 1)
        meshError(filename);
    vertices = new QVector3D [numVertices];
    for (int vertex = 0; vertex < numVertices; ++vertex) {
        float x, y, z, xnormal, ynormal, znormal;
        if (!fgets(buffer, sizeof(buffer), file))
            meshError(filename);
        if (sscanf(buffer, "%f, %f, %f, %f, %f, %f\n",
                   &x, &y, &z, &xnormal, &ynormal, &znormal) != 6) {
            if (sscanf(buffer, "%f, %f, %f\n", &x, &y, &z) != 3)
                meshError(filename);
        } else {
            if (!normals) {
                normals = new QVector3D [numVertices];
                qMemSet(normals, 0, sizeof(QVector3D) * numVertices);
            }
            normals[vertex].setX(xnormal);
            normals[vertex].setY(ynormal);
            normals[vertex].setZ(znormal);
            ++numBezierNormals;
        }
        if (teapotAdjust) {
            // Do the equivalent of the following transformation:
            // matrix.rotate(270.0f, 1.0f, 0.0f, 0.0f);
            // matrix.scale(0.5f, 0.5f, 0.5f);
            // matrix.translate(0.0f, 0.0f, -1.5f);
            z -= 1.5f;
            x *= 0.5f;
            y *= 0.5f;
            z *= 0.5f;
            float y2 = z;
            float z2 = -y;
            y = y2;
            z = z2;
        }
        vertices[vertex].setX(x);
        vertices[vertex].setY(y);
        vertices[vertex].setZ(z);
    }
    fclose(file);

    // Convert the input data into a triangle mesh.
    if (isTriangleMesh) {
        QGLVertexArray v;
        QGLIndexArray i;
        v.addField(QGL::Position, 3);
        if (normals)
            v.addField(QGL::Normal, 3);
        for (int vertex = 0; vertex < numVertices; ++vertex) {
            if (normals) {
                v.append(vertices[vertex]);
                v.append(normals[vertex]);
            } else {
                v.append(vertices[vertex]);
            }
        }
        for (int patch = 0; patch < numPatches; ++patch) {
            i.append(patches[patch * 17 + 0],
                     patches[patch * 17 + 1],
                     patches[patch * 17 + 2]);
        }
        mesh.setVertexArray(v);
        mesh.setIndexArray(i);
    } else {
        QGLVertexArray v;
        QGLIndexArray i;
        v.addField(QGL::Position, 3);
        for (int vertex = 0; vertex < numVertices; ++vertex) {
            v.append(vertices[vertex]);
            if (normals && !normals[vertex].isNull())
                bezier.setNormal(vertex, normals[vertex]);
        }
        for (int patch = 0; patch < numPatches; ++patch) {
            for (int offset = 0; offset < 16; ++offset)
                i.append(patches[patch * 17 + offset]);
            int minDepth = patches[patch * 17 + 16];
            if (minDepth > depth)
                depth = minDepth;
        }
        bezier.setVertexArray(v);
        bezier.setIndexArray(i);
        bezier.setSubdivisionDepth(depth);

        QGLGeometry *geom = bezier.subdivide();
        mesh.setVertexArray(geom->vertexArray());
        mesh.setIndexArray(geom->indexArray());
        delete geom;
    }

    // Generate the output data.
    printf("// Generated from %s by meshcvt, depth = %d\n\n", filename, depth);
    if (!bezierOnly) {
        QGLVertexArray v = mesh.vertexArray();
        QGLIndexArray i = mesh.indexArray();
        if (!isTriangleMesh)
            printf("#ifndef USE_BEZIER_DATA_ONLY\n\n");
        printf("#define %sVertexCount %d\n", name, v.vertexCount());
        printf("#define %sVertexStride %d\n", name, v.stride());
        printf("#define %sTriangleCount %d\n\n", name, i.size() / 3);
        printf("static float const %sVertexData[] = {\n", name);
        for (int vertex = 0; vertex < v.vertexCount(); ++vertex) {
            const float *vert = v.constData() + vertex * v.stride();
            printf("    ");
            for (int component = 0; component < v.stride(); ++component) {
                if (component != 0)
                    printf(", %f", vert[component]);
                else
                    printf("%f", vert[component]);
            }
            if (vertex < (v.vertexCount() - 1))
                printf(",\n");
            else
                printf("\n");
        }
        printf("};\n\n");
        printf("static ushort const %sTriangleData[] = {", name);
        for (int index = 0; index < i.size(); ++index) {
            if (index)
                printf(",");
            if ((index % 12) == 0)
                printf("\n    ");
            else if (index)
                printf(" ");
            printf("%d", i[index]);
        }
        printf("\n");
        printf("};\n");
        printf("\n");
        printf("static void %sLoadGeometry(QGLGeometry& geometry)\n{\n", name);
        printf("    geometry.setDrawingMode(QGL::Triangles);\n");
        printf("\n");
        printf("    QGLVertexArray varray(QGL::Position, 3");
        if (!v.attributeValue(QGL::Normal).isNull())
            printf(", QGL::Normal, 3");
        if (!v.attributeValue(QGL::TextureCoord0).isNull())
            printf(", QGL::TextureCoord0, 2");
        printf(");\n");
        printf("    varray.setRawData(%sVertexData, %sVertexCount * %sVertexStride);\n", name, name, name);
        printf("    geometry.setVertexArray(varray);\n");
        printf("\n");
        printf("    geometry.setIndexArray(QGLIndexArray::fromRawData(%sTriangleData, %sTriangleCount * 3));\n", name, name);
        printf("}\n");
        printf("\n");
        printf("static QGLGeometry *%sCreateGeometry()\n{\n", name);
        printf("    QGLGeometry *geometry = new QGLGeometry();\n");
        printf("    %sLoadGeometry(*geometry);\n", name);
        printf("    return geometry;\n");
        printf("}\n");
        if (!isTriangleMesh) {
            printf("\n#endif\n\n");
            printf("#if defined(USE_BEZIER_DATA_ONLY) || defined(USE_TRIANGLE_AND_BEZIER_DATA)\n\n");
        }
    }
    if (!isTriangleMesh) {
        // Output the Bezier data as well in case the programmer
        // wants to generate the triangle meshes at runtime.
        QGLVertexArray v = bezier.vertexArray();
        QGLIndexArray i = bezier.indexArray();
        printf("#define %sBezierVertexCount %d\n", name, numVertices);
        printf("#define %sBezierVertexStride %d\n", name, v.stride());
        printf("#define %sPatchCount %d\n", name, numPatches);
        printf("#define %sDepth %d\n", name, depth);
        printf("static float const %sBezierVertexData[] = {\n", name);
        for (int vertex = 0; vertex < v.vertexCount(); ++vertex) {
            const float *vert = v.constData() + vertex * v.stride();
            printf("    ");
            for (int component = 0; component < v.stride(); ++component) {
                if (component != 0)
                    printf(", %f", vert[component]);
                else
                    printf("%f", vert[component]);
            }
            if (vertex < (v.vertexCount() - 1))
                printf(",\n");
            else
                printf("\n");
        }
        printf("};\n\n");
        printf("static ushort const %sPatchData[] = {\n", name);
        for (int patch = 0; patch < numPatches; ++patch) {
            int *p = patches + patch * 17;
            printf("    %d, %d, %d, %d, %d, %d, %d, %d, "
                       "%d, %d, %d, %d, %d, %d, %d, %d",
                   p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7],
                   p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]);
            if (patch < (numPatches - 1))
                printf(",\n");
            else
                printf("\n");
        }
        printf("};\n");
        printf("\n");
        printf("static void %sLoadBezier(QGLBezierGeometry& geometry)\n{\n", name);
        printf("    QGLVertexArray varray(QGL::Position, 3");
        if (!v.attributeValue(QGL::Normal).isNull())
            printf(", QGL::Normal, 3");
        if (!v.attributeValue(QGL::TextureCoord0).isNull())
            printf(", QGL::TextureCoord0, 2");
        printf(");\n");
        printf("    varray.setRawData(%sBezierVertexData, %sBezierVertexCount * %sBezierVertexStride);\n", name, name, name);
        printf("    geometry.setVertexArray(varray);\n");
        printf("\n");
        printf("    geometry.setIndexArray(QGLIndexArray::fromRawData(%sPatchData, %sPatchCount * 16));\n", name, name);
        printf("\n");
        printf("    geometry.setSubdivisionDepth(%sDepth);\n", name);
        if (normals) {
            // Some of the Bezier patch vertices had specified normals.
            for (int vertex = 0; vertex < numVertices; ++vertex) {
                QVector3D n = normals[vertex];
                if (!n.isNull())
                    printf("    geometry.setNormal(%d, QVector3D(%ff, %ff, %ff));\n", vertex, n.x(), n.y(), n.z());
            }
        }
        printf("}\n");
        printf("\n");
        printf("static QGLBezierGeometry *%sCreateBezier()\n{\n", name);
        printf("    QGLBezierGeometry *geometry = new QGLBezierGeometry();\n");
        printf("    %sLoadBezier(*geometry);\n", name);
        printf("    return geometry;\n");
        printf("}\n");
        if (!bezierOnly)
            printf("\n#endif\n");
    }

    // Dump some statistics to stderr.
    fprintf(stderr, "Depth:               %d\n", depth);
    fprintf(stderr, "Number of vertices:  %d\n", mesh.vertexArray().vertexCount());
    fprintf(stderr, "Number of triangles: %d\n", mesh.indexArray().size() / 3);
    int storage = mesh.vertexArray().vertexCount() * sizeof(float) * mesh.vertexArray().stride();
    fprintf(stderr, "Triangle storage:    %d bytes\n", storage);
    if (!isTriangleMesh) {
        storage = bezier.vertexArray().vertexCount() * sizeof(float) * bezier.vertexArray().stride();
        storage += bezier.indexArray().size() * sizeof(short);
        fprintf(stderr, "Bezier storage:      %d bytes\n", storage);
    }

    return 0;
}
