// colorbar_vertex.glsl

#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 hsv_color;

out vec3 rgb_color;

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{
    gl_Position = vec4(pos, 1.0);
    rgb_color = hsv2rgb(hsv_color);
}
