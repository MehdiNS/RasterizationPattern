#version 420

uniform layout(binding = 0) usampler2D inAtomicCount;
uniform layout(binding = 1) sampler2D inFragColor;

layout (location = 0) in vec2 inTexCoord;
layout (location = 0) out vec4 outFragColor;

uniform uint currentTime;

const vec3 GREY = vec3(0.5f);
const vec3 GREEN = vec3(0.f,1.f,0.f);

void main()
{
	uint firstPassValue = texture(inAtomicCount, inTexCoord).x;
	vec3 color;
	if(firstPassValue.x > currentTime)
		color = GREY;
	else if(currentTime - firstPassValue.x < 500u)
		color = GREEN;
	else  
		color = texture2D(inFragColor, inTexCoord).xyz;
	
 	outFragColor = vec4(color, 1.);
}