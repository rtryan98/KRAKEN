#ifndef YGG_COMPLEX_GLSL
#define YGG_COMPLEX_GLSL

vec2 YggComplexMult(vec2 a, vec2 b)
{
    return vec2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}

vec2 YggComplexExp(vec2 a)
{
    return vec2(cos(a.y), sin(a.y)) * exp(a.x);
}

#endif
