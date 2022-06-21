#version 330 core

out vec4 FragColor;
in vec2 fragCoord;

uniform sampler2D _MainTex;

void main()
{
    float strength = 1.2;

    vec4 color = texture2D(_MainTex, fragCoord);
    float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
    FragColor = vec4(strength * color.rgb, 1.0);
}