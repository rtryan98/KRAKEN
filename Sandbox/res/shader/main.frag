#version 460 core

layout(location = 0) in vec2 iUvs;
layout(location = 0) out vec4 oColor;

layout(binding = 1) uniform sampler2D albedo;

void main()
{
    oColor = vec4(texture(albedo, iUvs).rgb, 1.0);
}
