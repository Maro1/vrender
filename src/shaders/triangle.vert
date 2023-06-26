#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform GlobalUbo
{
    mat4 projectionView;
}
ubo;

layout(push_constant) uniform Push
{
    mat4 model;
}
push;

vec2 positions[3] = vec2[](vec2(0.0, -0.5), vec2(0.5, 0.5), vec2(-0.5, 0.5));

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main()
{
    gl_Position = ubo.projectionView * push.model * vec4(inPosition, 1.0);

    fragColor = vec3(0.2);
    fragTexCoord = inTexCoord;
}
