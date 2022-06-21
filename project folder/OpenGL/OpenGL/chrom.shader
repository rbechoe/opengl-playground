#version 330 core
out vec4 FragColor;

in vec2 fragCoord;
uniform sampler2D _MainTex;

void main()
{
	float rFactor = 0.96;
	float gFactor = 0.98;
	float bFactor = 1.0;

	vec2 dir = fragCoord - vec2(0.5, 0.5);

	// atan for circular distortion
	float r = texture(_MainTex, atan(vec2(0.5, 0.5) + dir * rFactor)).r;
	float g = texture(_MainTex, atan(vec2(0.5, 0.5) + dir * gFactor)).g;
	float b = texture(_MainTex, atan(vec2(0.5, 0.5) + dir * bFactor)).b;

	FragColor = vec4(r, g, b, 1.0);
}