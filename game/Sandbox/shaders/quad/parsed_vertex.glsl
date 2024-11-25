
#version 460 core


out vec3 Color;

layout (location=0) in vec2 aPos;
layout (location=1) in vec4 aModel_0;
layout (location=2) in vec4 aModel_1;
layout (location=3) in vec4 aModel_2;
layout (location=4) in vec4 aModel_3;
layout (location=5) in vec3 color;
void main(){
	mat4 aModel = mat4(aModel_0,aModel_1,aModel_2,aModel_3);
    gl_Position = aModel * vec4(aPos, 0.f, 1.f); // Transform the vertex position
    Color = vec3(color);
}
