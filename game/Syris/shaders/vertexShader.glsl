#version 460 core

layout (location = 0) in vec4 pos;     // Position attribute
//layout (location = 1) in vec2 aTexCoord; // Texture coordinate attribute

out vec2 TexCoord; // Output texture coordinate to the fragment shader

uniform mat4 model; // Model matrix for transforming the vertex positions

uniform ve3 spriteColor;
void main()
{
    gl_Position = /*projection * model **/ vec4(pos.xy, 0.0, 1.0); // Transform the vertex position
    TexCoord = pos.zw; // Pass texture coordinates to the fragment shader
}
