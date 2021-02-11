#version 460 core

layout(location = 0) in vec3 iColor;
layout(location = 0) out vec4 oColor;

void main()
{
    oColor = vec4(iColor, 1.0);
}
