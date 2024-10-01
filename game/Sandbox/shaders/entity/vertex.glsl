#version 460 core

layout (location = 0) in vec2 aPos;      // Position attribute
layout (location = 1) in vec4 aTexCoord; // Texture coordinate attribute
layout (location = 2) in vec4 aModelCol0; // Texture coordinate attribute
layout (location = 3) in vec4 aModelCol1; // Texture coordinate attribute
layout (location = 4) in vec4 aModelCol2; // Texture coordinate attribute
layout (location = 5) in vec4 aModelCol3; // Texture coordinate attribute

out vec2 TexCoord; // Output texture coordinate to the fragment shader

uniform mat4 ViewProjection;

void main()
{
    mat4 model = mat4(
        aModelCol0, // First column
        aModelCol1, // Second column
        aModelCol2, // Third column
        aModelCol3  // Fourth column
    );
    
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