#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

out vec3 vertexColor;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 Ia;
uniform vec3 Id;
uniform vec3 Is;

uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;

uniform float shininess;

void main()
{
    vec3 FragPos = vec3(model * vec4(aPos, 1.0));

    vec3 N = normalize(mat3(transpose(inverse(model)))*aNormal);
    vec3 L = normalize(lightPos - FragPos);
    vec3 V = normalize(viewPos - FragPos);
    vec3 R = reflect(-L, N);

    vec3 ambient = ka * Ia;
    vec3 specular = ks * Is * pow(max(dot(R,V),0.0), shininess);
    vec3 diffuse = kd * Id * max(dot(N,L), 0.0);

    vertexColor = ambient + diffuse + specular;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}