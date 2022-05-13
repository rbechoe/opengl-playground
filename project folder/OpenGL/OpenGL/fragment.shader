#version 330 core
out vec4 FragColor;

in vec3 color;
in vec2 uv;
in vec3 normal;
in vec4 worldPixel;

uniform sampler2D diffuse;

void main() 
{
    vec4 diffuseColor = texture(diffuse, uv);

    vec3 lightDir = normalize(vec3(0, -1, -1));
    vec3 camPos = vec3(0, 3, -3);
    vec3 viewDirection = normalize(worldPixel.xyz - camPos);

    vec3 lightReflect = normalize(reflect(-lightDir, normal));
    float specular = pow(max(dot(lightReflect, viewDirection), 0.0), 64);

    float light = max(dot(-lightDir, normal), 0.25);
    FragColor = diffuseColor * light + specular;
}