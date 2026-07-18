#version 330 core

layout(triangles) in;
layout(line_strip, max_vertices=6) out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void emitEdge(vec4 a, vec4 b){
	gl_Position = a;
	EmitVertex();

	gl_Position = b;
	EmitVertex();

	EndPrimitive();
}

void main(){
	vec3 p0 = vec3(model * gl_in[0].gl_Position);
    vec3 p1 = vec3(model * gl_in[1].gl_Position);
    vec3 p2 = vec3(model * gl_in[2].gl_Position);

	vec3 e1 = p1 - p0;
	vec3 e2 = p2 - p0;

	vec3 normal = normalize(cross(e1, e2));

	vec3 viewDir = normalize(-p0);

	if(dot(normal, viewDir) <= 0.0)
        return;

	vec4 v0 = projection * view * vec4(p0, 1.0);
    vec4 v1 = projection * view * vec4(p1, 1.0);
    vec4 v2 = projection * view * vec4(p2, 1.0);

    emitEdge(v0, v1);
    emitEdge(v1, v2);
    emitEdge(v2, v0);
}