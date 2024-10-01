#version 460 core

out vec4 FragColor; // Output color of the fragment

in vec2 TexCoord; // Input texture coordinate from the vertex shader

uniform sampler2D texture1; // Sampler for the texture

void main()
{
    FragColor = texture(texture1, TexCoord);
}
