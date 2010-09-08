attribute highp vec4 vertex;
attribute highp vec4 texCoords;

uniform highp mat4 matrix;
uniform highp mat4 qgl_ProjectionMatrix;
uniform highp mat4 qgl_ModelViewMatrix;

uniform highp mat4 cameraCombinedMatrix;
uniform highp mat4 lightCombinedMatrix;

uniform highp mat4 cameraModelViewMatrix;
uniform highp mat4 inverseCameraModelViewMatrix;
uniform highp mat4 lightModelViewMatrix;

uniform highp mat4 cameraProjectionMatrix;
uniform highp mat4 lightProjectionMatrix;

varying highp vec4 qTexCoord0;
varying highp vec4 onScreenPosition;
varying highp vec4 lightScreenPosition;
varying highp vec4 worldSpaceVertex;
varying float lightDepth;
varying float actualDepth;

void main(void)
{
    qTexCoord0 = texCoords;
//    onScreenPosition = matrix * vertex;
//onScreenPosition = cameraCombinedMatrix * vertex; // works
    onScreenPosition = qgl_ProjectionMatrix *
    qgl_ModelViewMatrix
    * vertex;

    highp vec4 worldPosition = inverseCameraModelViewMatrix *
        qgl_ModelViewMatrix * vertex;

//    lightScreenPosition = lightCombinedMatrix * vertex;
//    lightScreenPosition = lightCombinedMatrix * worldPosition;
    lightScreenPosition = lightProjectionMatrix * lightModelViewMatrix * vertex;
    lightDepth = lightScreenPosition.z;

    actualDepth = onScreenPosition.z;

    worldSpaceVertex = vertex;

//    gl_Position = qgl_ProjectionMatrix * worldPosition;
//gl_Position = onScreenPosition;
gl_Position = onScreenPosition;
//    gl_Position = lightScreenPosition;
};
