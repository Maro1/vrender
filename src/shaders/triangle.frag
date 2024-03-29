#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_debug_printf : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 2) uniform sampler2D texSampler;

void main()
{
    outColor = texture(texSampler, fragTexCoord);
    // outColor = vec4(0.0, 0.0, gl_FragCoord.z / 5, 1.0);
}
