#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 FragPos[];

out vec3 Color;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform vec3 Ia;
uniform vec3 Id;
uniform vec3 Is;

uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;

uniform float shininess;

void main()
{
    vec3 p0 = FragPos[0];
    vec3 p1 = FragPos[1];
    vec3 p2 = FragPos[2];

    vec3 N = normalize(cross(p1 - p0, p2 - p0));

    vec3 center = (p0 + p1 + p2) / 3.0;

    vec3 L = normalize(lightPos - center);
    vec3 V = normalize(viewPos - center);

    vec3 R = reflect(-L, N);

    vec3 ambient = Ia * ka;

    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = Id * kd * diff;

    float spec = pow(max(dot(R, V), 0.0), shininess);
    vec3 specular = Is * ks * spec;

    vec3 result = ambient + diffuse + specular;

    for(int i = 0; i < 3; i++)
    {
        Color = result;

        gl_Position = gl_in[i].gl_Position;

        EmitVertex();
    }

    EndPrimitive();
}