/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

//highp vec4 gl_Position; // should be written to
//mediump float gl_PointSize; // may be written to
attribute highp vec4 vertex;
attribute highp vec4 normal;
uniform mediump mat4 matrix;

uniform mediump vec4 acli;      // Ambient intensity of the light
uniform mediump vec4 dcli;      // Diffuse intensity of the light
uniform mediump vec3 pli;       // Position of the light
uniform mediump vec4 acm;       // Ambient color of the material
uniform mediump vec4 dcm;       // Diffuse color of the material
uniform mediump vec4 ecm;       // Emissive color of the material
uniform mediump vec4 acs;       // Light model's ambient color of the scene
uniform bool viewerAtInfinity;  // Light model indicates viewer at infinity

uniform float time;
uniform vec3 lightDir;
uniform highp mat3 qt_NormalMatrix;

varying mediump vec4 qAmbient;
varying mediump vec4 qDiffuse;
varying mediump vec3 qNormal;
varying mediump vec3 qLightDirection;
varying mediump vec3 qHalfVector;
varying mediump vec3 qVertexToLight;
varying highp vec4 qt_TexCoord0;
varying highp vec4 qt_TexCoord1;


void qLightVertex(vec4 vertex, vec3 normal)
{
    vec3 toEye;
    qLightDirection = normalize(pli);
    if (viewerAtInfinity)
        toEye = vec3(0, 0, 1);
    else
        toEye = normalize(-vertex.xyz);
    qHalfVector = normalize(qLightDirection + toEye);
    qVertexToLight = vertex.xyz - pli;
}

void main(void)
{
    vec4 v = vertex;
    vec3 toEye;
    qNormal = normalize(qt_NormalMatrix * vec3(normal));
    qLightVertex(vertex, qNormal);
    gl_Position = matrix * v;
};
