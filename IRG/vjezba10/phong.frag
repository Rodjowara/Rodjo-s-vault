#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec4 FragLight;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform vec3 Ia, Id, Is;
uniform vec3 ka, kd, ks;
uniform float shininess;

uniform sampler2D sampler;
uniform sampler2D shadowMap;

out vec4 FragColor;

void main()
{
    vec3 N = normalize(Normal);
    vec3 L = normalize(lightPos - FragPos);
    vec3 V = normalize(viewPos - FragPos);
    vec3 R = reflect(-L, N);

    vec3 texColor = texture(sampler, TexCoord).rgb;

    vec3 projectionCoords = FragLight.xyz / FragLight.w;
    projectionCoords = projectionCoords * 0.5 + 0.5;

    float storedDepth = texture(shadowMap, projectionCoords.xy).r;
    float currentDepth = projectionCoords.z;
    float bias = 0.005;
    bool inShadow = currentDepth - bias > storedDepth;
    vec3 color;

    vec3 ambient = ka * Ia;
    vec3 diffuse =  texColor * Id * max(dot(N, L), 0.0);
    vec3 specular = ks * Is * pow(max(dot(R, V), 0.0), shininess);

    if(inShadow)
    {
        color = ambient;
    }
    else
    {
        color = ambient + diffuse + specular;
    }

    FragColor = vec4(color, 1.0);
}