#version 460 core

out vec4 FragColor; // Output color of the fragment

in vec3 Color; // Input texture coordinate from the vertex shader

void main()
{
    FragColor = vec4(1.f,0.f,0.f,1.f);
}