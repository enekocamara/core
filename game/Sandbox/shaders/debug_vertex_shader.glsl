#version 460 core

layout (location = 0) in vec2 aPos;      // Position attribute
layout (location = 1) in vec2 aTexCoord; // Texture coordinate attribute
layout (location = 2) in vec4 aModelCol0; // Texture coordinate attribute
layout (location = 3) in vec4 aModelCol1; // Texture coordinate attribute
layout (location = 4) in vec4 aModelCol2; // Texture coordinate attribute
layout (location = 5) in vec4 aModelCol3; // Texture coordinate attribute

out vec2 TexCoord; // Output texture coordinate to the fragment shader
//out vec4 Color; // Output texture coordinate to the fragment shader

uniform mat4 ViewProjection;

uniform vec3 spriteColor;

void main()
{
    mat4 model = mat4(
        aModelCol0, // First column
        aModelCol1, // Second column
        aModelCol2, // Third column
        aModelCol3  // Fourth column
    );
    gl_Position = ViewProjection * model  * vec4(aPos, 0.0, 1.0); // Transform the vertex position
    //Color = vec4(aTexCoord, 0.f,1.f);
    if (gl_VertexID == 0){
        gl_Position = vec4(-1.f,1.f,0.f,1.f);    
        Color = vec4(aPos.y, aPos.y,0.f,1.0f);
    }
    if (gl_VertexID == 1){
        gl_Position = vec4(1.f,1.f,0.f,1.f);    
        //Color = vec4(0.f,1.f,0.f,1.0f);
        Color = vec4(aPos.y, aPos.y,0.f,1.0f);
    }
    if (gl_VertexID == 2){
        gl_Position = vec4(1.f,-1.f,0.f,1.f);    
        //Color = vec4(0.f,0.f,1.f,1.0f);
        Color = vec4(aPos.y, aPos.y,0.f,1.0f);
    }
    TexCoord = aTexCoord; // Pass texture coordinates to the fragment shader
}
