#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inUV;

layout (binding = 0) uniform UBO 
{
	mat4 projection;
	mat4 model;
	float lodBias;
} ubo;

layout (location = 0) out vec2 outUV;
layout (location = 1) out float outLodBias;

void main() 
{
	outUV = inUV;
	outLodBias = ubo.lodBias;
	gl_Position = ubo.projection * ubo.model * vec4(inPos.xy, 0.0, 1.0);
}
