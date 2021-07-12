#version 460 core

layout(location = 0) in vec3 aPos;

layout(binding = 0) uniform Camera
{
    mat4 proj;
    mat4 view;
} uCamera;

void main()
{
    gl_Position = uCamera.proj * uCamera.view * vec4(aPos, 0.0);
}
