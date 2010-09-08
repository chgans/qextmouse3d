attribute highp vec4 vertex;
varying float depth;
uniform highp mat4 qgl_ModelViewMatrix;
uniform highp mat4 qgl_ProjectionMatrix;

attribute highp vec4 texCoords;

uniform highp mat4 cameraCombinedMatrix;
uniform highp mat4 cameraModelViewMatrix;
uniform highp mat4 cameraProjectionMatrix;

uniform highp mat4 lightCombinedMatrix;

varying highp vec4 qTexCoord0;
varying highp vec4 onScreenPosition;
varying highp vec4 lightScreenPosition;
varying highp vec4 worldSpaceVertex;

void main(void)
{
    qTexCoord0 = texCoords;

    // qgl_ProjectionMatrix == cameraProjectionMatrix
//    onScreenPosition = qgl_ProjectionMatrix * qgl_ModelViewMatrix * vertex;
//onScreenPosition = cameraProjectionMatrix * qgl_ModelViewMatrix * vertex;

    onScreenPosition = cameraProjectionMatrix
    * cameraModelViewMatrix
//    * qgl_ModelViewMatrix
    * vertex;
    depth = onScreenPosition.z;

    gl_Position = onScreenPosition;
};
