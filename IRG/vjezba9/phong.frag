#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform vec3 Ia, Id, Is;
uniform vec3 ka, kd, ks;
uniform float shininess;

uniform sampler2D sampler;

out vec4 FragColor;

void main()
{
    vec3 N = normalize(Normal);
    vec3 L = normalize(lightPos - FragPos);
    vec3 V = normalize(viewPos - FragPos);
    vec3 R = reflect(-L, N);

    vec3 texColor = texture(sampler, TexCoord).rgb;

    vec3 ambient = ka * Ia;
    vec3 diffuse =  texColor * Id * max(dot(N, L), 0.0);
    vec3 specular = ks * Is * pow(max(dot(R, V), 0.0), shininess);

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}