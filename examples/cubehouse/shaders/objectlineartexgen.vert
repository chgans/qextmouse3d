attribute highp vec4 qgl_Vertex;
attribute highp vec4 qgl_Normal;
uniform highp mat3 qgl_NormalMatrix;
uniform highp mat4 qgl_ModelViewProjectionMatrix;

uniform highp mat4 objectLinearTexgenMatrix;
uniform highp vec4 projectorDirection;

varying highp vec4 texCoord;
varying float facingProjector;

void main(void)
{
    texCoord = objectLinearTexgenMatrix * qgl_Vertex;
    vec4 normal = normalize(vec4(qgl_NormalMatrix * vec3(qgl_Vertex), qgl_Vertex.w));
    // We're facing the projector if the normal and projector direction
    // are pointing in opposite directions, i.e. if their dot product
    // is negative.
    facingProjector = -dot(normalize(normal), normalize(projectorDirection));
    facingProjector = clamp(facingProjector, 0.0, 1.0);
    gl_Position = qgl_ModelViewProjectionMatrix * qgl_Vertex;
};
