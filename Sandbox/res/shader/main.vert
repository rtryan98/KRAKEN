#version 460 core

layout(location = 0) in vec3 aPosition;
layout(location = 0) out vec2 oUvs;

vec2 uvs[6] = {
    vec2(0.0, 0.0),
    vec2(1.0, 0.0),
    vec2(0.0, 1.0),
    vec2(0.0, 1.0),
    vec2(1.0, 0.0),
    vec2(1.0, 1.0),
};

layout(binding = 0) uniform Camera
{
    mat4 proj;
    mat4 view;
} camera;

void main()
{
    oUvs = uvs[gl_VertexIndex];
    gl_Position = camera.proj * camera.view * vec4(aPosition, 1.0);
}
