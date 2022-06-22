#version 330 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 DepthColor;

in vec4 FragPos;
in vec4 ScreenPos;

uniform sampler2D SceneColor;
uniform sampler2D SceneDepth;
uniform sampler2D InvertedScene;
uniform sampler2D NormalMap;

uniform vec3 lightDirection;
uniform vec3 cameraPosition;

uniform float time;

float lerp(float a, float b, float t)
{
	return a + (b - a) * t;
}

vec3 lerp(vec3 a, vec3 b, float t)
{
	return a + (b - a) * t;
}

void main()
{
	vec3 waterColor = vec3(0, 20 / 255.0, 32 / 255.0);
	vec2 screenUV = (ScreenPos.xy / ScreenPos.w) * .5 + .5;
	vec2 invertUV = screenUV;
	invertUV.x = 1 - invertUV.x;

	float edgeness = (1 - pow(abs(0.5 - screenUV.x) * 2, 8)) * (1 - pow(abs(0.5 - screenUV.y) * 2, 8));
	float depth = distance(cameraPosition, FragPos.xyz) / 100.0;
	float sceneDepth = texture(SceneDepth, screenUV).r;
	float deepness = sceneDepth - depth;

	vec2 waterUV1 = vec2(FragPos.x * .03 + time * .25, FragPos.z * .06 + time * .55);
	vec2 waterUV2 = vec2(FragPos.x * .04 + time * .35, FragPos.z * .05 + time * .45);
	vec2 waterUV3 = vec2(FragPos.x * .05 + time * .45, FragPos.z * .04 + time * .35);
	vec2 waterUV4 = vec2(FragPos.x * .06 + time * .55, FragPos.z * .03 + time * .25);

	vec2 normal;
	normal += texture(NormalMap, waterUV1).rg * 2 - 1;
	normal += texture(NormalMap, waterUV2).rg * 2 - 1;
	normal += texture(NormalMap, waterUV3).rg * 2 - 1;
	normal += texture(NormalMap, waterUV4).rg * 2 - 1;
	normal /= 4.0;

	normal *= .1 * edgeness * min(deepness, 1.0);

	vec3 sceneColor = texture(SceneColor, screenUV + normal).rgb;
	sceneColor = lerp(sceneColor, waterColor, min(deepness * 2, 1.0));
	vec3 reflectColor = texture(InvertedScene, invertUV + normal).rgb;

	vec3 viewDir = normalize(FragPos.xyz - cameraPosition);

	float fresnel = max(dot(-viewDir, vec3(0, 1, 0)), 0.0);

	vec3 surfaceBlend = lerp(reflectColor, sceneColor, fresnel);

	// specular highlight
	vec3 reflNormal = vec3(0, 1, 0);
	reflNormal.rg += normal * 2;
	vec3 refl = normalize(reflect(lightDirection, normalize(reflNormal)));
	float spec = pow(max(dot(refl, viewDir), 0.0), 56);

	FragColor = vec4(surfaceBlend + vec3(spec), 1.0);
	DepthColor = vec4(1, 0, 0, 1);
}
