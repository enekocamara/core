
#version 460 core

layout (location = 0) in vec2 aPos;      // Position attribute
layout (location = 1) in vec3 aColor;      // Position attribute


//out vec3 Color; // Output texture coordinate to the fragment shader

void main()
{
    gl_Position = vec4(aPos, 0.0, 1.0); // Transform the vertex position
//    Color = aColor; // Output texture coordinate to the fragment shader
}