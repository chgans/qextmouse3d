// Built-in variables:
// mediump vec4 gl_FragCoord;
// bool gl_FrontFacing;
// mediump vec4 gl_FragColor;
// mediump vec4 gl_FragData[gl_MaxDrawBuffers]
// mediump vec2 gl_PointCoord; // Not defined unless drawing points.
uniform mediump vec4 color;

void main(void)
{
    gl_FragColor = color;
};
