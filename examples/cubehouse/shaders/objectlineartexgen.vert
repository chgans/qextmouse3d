attribute highp vec4 vertex;
attribute highp vec4 normal;

uniform highp mat4 objectLinearTexgenMatrix;
uniform highp vec4 projectorDirection;
uniform highp mat3 qgl_NormalMatrix;
uniform highp mat4 matrix;

varying highp vec4 qTexCoord0;
varying highp vec4 qNormal;
varying float facingProjector;

void main(void)
{
    qTexCoord0 = objectLinearTexgenMatrix * vertex;
    qNormal = normalize(vec4(qgl_NormalMatrix * vec3(normal), normal.w));
    // We're facing the projector if the normal and projector direction
    // are pointing in opposite directions, i.e. if their dot product
    // is negative.
    facingProjector = -dot(normalize(qNormal), normalize(projectorDirection));
    facingProjector = clamp(facingProjector, 0.0, 1.0);
    gl_Position = matrix * vertex;
};
