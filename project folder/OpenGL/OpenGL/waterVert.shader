#version 330 core
layout(location = 0) in vec3 aPos;

out vec4 FragPos;
out vec2 ScreenPos;

uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;

uniform float WaterHeight;

void main()
{
	FragPos = world * vec4(aPos, 1.0);
	FragPos.y = WaterHeight;
	gl_Position = projection * view * FragPos;
	ScreenPos = (gl_Position.xy / gl_Position.w) * .5 + .5;
}
