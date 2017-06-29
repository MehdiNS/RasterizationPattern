#version 420

layout (location = 0) in vec2 inTexCoord;
layout (location = 0) out vec4 outFragColor;

uniform layout(binding = 0) sampler2D inFragColor;

void main()
{
	vec3 color = texture(inFragColor, inTexCoord).xyz;
 	outFragColor = vec4(color, 1.);
}