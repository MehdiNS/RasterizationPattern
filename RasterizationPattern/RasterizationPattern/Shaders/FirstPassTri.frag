#version 430

layout (location = 0) in vec2 inTexCoord;

layout(binding=0, offset=0) uniform atomic_uint ac; 

layout (location = 0) out uint outAtomicCount;
layout (location = 1) out vec4 outFragColor;

uniform layout(binding = 0) sampler2D hdrBuffer;

void main () 
{
	outAtomicCount = atomicCounterIncrement(ac);
	outFragColor = vec4(texture2D(hdrBuffer, inTexCoord).rgb, 1.0);
}

