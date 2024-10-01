
#version 460 core

layout (location = 0) in vec2 aPos;      // Position attribute
layout (location = 1) in vec4 aTexCoord; // Texture coordinate attribute

out vec2 TexCoord; // Output texture coordinate to the fragment shader

uniform mat4 ViewProjection;
uniform mat4 model;


void main()
{
    gl_Position = ViewProjection * model  * vec4(aPos, 0.0, 1.0); // Transform the vertex position

     // Assign the correct texture coordinate based on vertex position
    if (aPos.x == -1.f && aPos.y == 1.f)
        TexCoord = aTexCoord.xy; // Top-left
    else if (aPos.x == 1.f && aPos.y == 1.f)
        TexCoord = aTexCoord.zy; // Top-right
    else if (aPos.x == 1.f && aPos.y == -1.f)
        TexCoord = aTexCoord.zw; // Bottom-right
    else 
        TexCoord = aTexCoord.xw; // Bottom-left
}