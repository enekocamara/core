#version 460 core



out vec3 Color;

layout (location=0) in vec2 aPos;
layout (location=1) in vec4 aModel_0;
layout (location=2) in vec4 aModel_1;
layout (location=3) in vec4 aModel_2;
layout (location=4) in vec4 aModel_3;
uniform mat4 ViewProjection;
void main(){
	mat4 aModel = mat4(aModel_0,aModel_1,aModel_2,aModel_3);
    Color = vec3(aModel[0][0], aModel[1][1], aModel[2][2]);
    aModel = mat4(1.f);
    gl_Position = ViewProjection * aModel   * vec4(aPos, 0.0, 1.0); // Transform the vertex position
}

