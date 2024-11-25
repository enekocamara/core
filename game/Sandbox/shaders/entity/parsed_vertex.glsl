#version 460 core


out vec2 TexCoord; // Output texture coordinate to the fragment shader


layout (location=0) in vec2 aPos;
layout (location=1) in vec4 aTexCoord;
layout (location=2) in vec4 aModel_0;
layout (location=3) in vec4 aModel_1;
layout (location=4) in vec4 aModel_2;
layout (location=5) in vec4 aModel_3;
uniform mat4 uViewProjection;
uniform sampler2D uTexture;
void main(){   
	mat4 aModel = mat4(aModel_0,aModel_1,aModel_2,aModel_3);
    gl_Position = uViewProjection * aModel  * vec4(aPos, 0.0, 1.0); // Transform the vertex position

    if (aPos.x == -1.f && aPos.y == 1.f)
        TexCoord = aTexCoord.xy; // Top-left
    else if (aPos.x == 1.f && aPos.y == 1.f)
        TexCoord = aTexCoord.zy; // Top-right
    else if (aPos.x == 1.f && aPos.y == -1.f)
        TexCoord = aTexCoord.zw; // Bottom-right
    else 
        TexCoord = aTexCoord.xw; // Bottom-left
}
