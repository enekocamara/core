#version 460 core


out vec3 fragMat;
layout (location=0) in vec2 aPos;
layout (location=1) in vec4 aModel_0;
layout (location=2) in vec4 aModel_1;
layout (location=3) in vec4 aModel_2;
layout (location=4) in vec4 aModel_3;
layout (location=5) in vec3 aMaterial;
uniform mat4 uViewProjection;
void main(){
	mat4 aModel = mat4(aModel_0,aModel_1,aModel_2,aModel_3);
    gl_Position = uViewProjection * aModel  * vec4(aPos, 0.0, 1.0);
    fragMat = aMaterial;
}

