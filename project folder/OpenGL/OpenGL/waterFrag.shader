#version 330 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 DepthColor;

in vec4 FragPos;
in vec2 ScreenPos;

uniform sampler2D SceneColor;
uniform sampler2D SceneDepth;
uniform sampler2D InvertedScene;

uniform vec3 lightDirection;
uniform vec3 cameraPosition;

void main()
{
	FragColor = vec4(1);
	DepthColor = vec4(1, 0, 0, 1);
}
