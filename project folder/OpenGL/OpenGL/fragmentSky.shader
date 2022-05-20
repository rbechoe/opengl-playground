#version 330 core
out vec4 FragColor;

in vec4 worldPixel;

uniform vec3 cameraPosition;
uniform vec3 sunColor;
uniform int sunSize;

vec3 lerp(vec3 a, vec3 b, float t)
{
    return a + (b - a) * t;
}

void main() 
{
    vec3 lightDir = normalize(vec3(0, -0.5, -1));
    vec3 viewDirection = normalize(worldPixel.xyz - cameraPosition);

    vec3 top = vec3(68 / 255.0, 118 / 255.0, 189 / 255.0);
    vec3 bot = vec3(188 / 255.0, 214 / 255.0, 231 / 255.0);

    float sun = pow(max(dot(-viewDirection, lightDir), 0.0), sunSize);

    FragColor = vec4(lerp(bot, top, viewDirection.y) + sun * sunColor, 1.0);
}
