#version 330 core
layout(location=0) in vec3 vPos;
layout(location=1) in vec3 vColor;
layout(location=2) in vec2 vUV;

out vec3 color;
out vec2 uv;

void main() 
{
	gl_Position = vec4(vPos, 1.0);
	color = vColor;
	uv = vUV;
}
