varying float depth;

void main(void)
{
    float stupidMathsDepth = depth;
    stupidMathsDepth = (stupidMathsDepth + 10.0) / 20.0;
//    gl_FragColor = vec4(vec3(stupidMathsDepth), 1.0);
    gl_FragColor = vec4(1.0, 0.0, 1.0, 1.0);
};
