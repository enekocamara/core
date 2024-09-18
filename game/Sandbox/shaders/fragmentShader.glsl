#version 460 core

out vec4 FragColor; // Output color of the fragment

in vec2 TexCoord; // Input texture coordinate from the vertex shader

uniform sampler2D texture1; // Sampler for the texture
uniform vec3 spriteColor; // Color of the sprite

void main()
{
    vec4 texColor = texture(texture1, TexCoord);
    FragColor = texColor;
}
