#version 130

varying vec2 uv;
uniform sampler2D tex;

void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	uv = gl_MultiTexCoord0.xy;
}