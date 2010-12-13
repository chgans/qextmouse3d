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

// This shader uses two textures, one used "normally" applied on top of the material,
// and the other used as source for the diffuse material properties of the object.

// Built-in variables:
// mediump vec4 gl_FragCoord;
// bool gl_FrontFacing;
// mediump vec4 gl_FragColor;
// mediump vec4 gl_FragData[gl_MaxDrawBuffers]
// mediump vec2 gl_PointCoord; // Not defined unless drawing points.
uniform mediump vec4 scli;
uniform mediump vec4 scm;
uniform mediump vec4 color;
uniform mediump sampler2D texture0;
uniform mediump sampler2D texture1;
uniform float shininess;

varying float intensity;
varying mediump vec4 qAmbient;
varying mediump vec4 qDiffuse;
varying mediump vec3 qNormal;
varying mediump vec3 qLightDirection;
varying mediump vec3 qHalfVector;
varying mediump vec3 qVertexToLight;
varying highp vec4 qt_TexCoord0;
varying highp vec4 qt_TexCoord1;

mediump vec3 perPixelNormal;

void main(void)
{
    vec4 specularComponent = vec4( 0.0, 0.0, 0.0, 0.0 );
    perPixelNormal = normalize(qNormal);
    float intensity =  max(dot(perPixelNormal, qLightDirection), 0.0);

    if(intensity > 0.0)
    {
        float specularIntensity = max( dot(perPixelNormal, qHalfVector), 0.0 );
        if(specularIntensity > 0.0)
            specularComponent = scm * scli * pow(specularIntensity, shininess);
    }
    vec4 texture0Color = texture2D(texture0, qt_TexCoord0.st);
    vec4 diffuseColor = texture2D(texture1, qt_TexCoord0.st);
    vec4 lightingColor = qAmbient + diffuseColor * intensity ;

// The two fixed function lighting models are GL_MODULATE and GL_DECAL

// In GL_MODULATE the lighting and texture are combined, so the material
// and texture are combined.  Uncomment this line to see this effect:
//gl_FragColor = vec4(texture0Color.xyz * (lightingColor.xyz + specularComponent.xyz), texture0Color.a);

// In GL_DECAL the Texture is applied over the top of the lighting, like a
// sticker (or decal, hence the name).
gl_FragColor = vec4(lightingColor.xyz * (1.0 - texture0Color.a) + texture0Color.xyz * texture0Color.a, lightingColor.a) + specularComponent;

};
