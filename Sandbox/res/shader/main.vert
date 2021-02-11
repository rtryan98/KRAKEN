#version 460 core

vec2 positions[3] = {
    vec2(-0.5, -0.5),
    vec2(0.5, -0.5),
    vec2(0.0, 0.5)
};

vec3 colors[3] = {
    vec3(0.0, 0.0, 1.0),
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0)
};

layout(location = 0) out vec3 oColor;

void main()
{
    oColor = colors[gl_VertexIndex];
    gl_Position = vec4(positions[gl_VertexIndex].x, positions[gl_VertexIndex].y, 0.0, 1.0);
}
