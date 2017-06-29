#version 420

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTexCoord;

layout(binding=0, offset=0) uniform atomic_uint ac; 

layout (location = 0) out uint outAtomicCount;
layout (location = 1) out vec4 outFragColor;

uniform layout(binding = 0) sampler2D colorBuffer;

/////////////////////////////////////////////////////////

void main () 
{
	vec3 color = texture2D(colorBuffer, inTexCoord).rgb;
    outFragColor = vec4(color, 1.0);
	outAtomicCount = atomicCounterIncrement(ac);
};	
