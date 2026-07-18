#version 330 core

layout(triangles) in;
layout(line_strip, max_vertices=6) out;

void emitEdge(vec4 a, vec4 b){
	gl_Position = a;
	EmitVertex();

	gl_Position = b;
	EmitVertex();

	EndPrimitive();
}

void main(){
	vec4 v0 = gl_in[0].gl_Position;
	vec4 v1 = gl_in[1].gl_Position;
	vec4 v2 = gl_in[2].gl_Position;

	vec2 p0 = v0.xy/v0.w;
	vec2 p1 = v1.xy/v1.w;
	vec2 p2 = v2.xy/v2.w;

    float orientation =
          (p1.x - p0.x) * (p2.y - p0.y)
        - (p1.y - p0.y) * (p2.x - p0.x);

    if(orientation < 0.0)
        return;

    emitEdge(v0, v1);
    emitEdge(v1, v2);
    emitEdge(v2, v0);
}