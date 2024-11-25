#version 460 core

out vec4 FragColor; // Output color of the fragment
in vec3 Color;
void main()
{
    FragColor = vec4(Color,1.f);
}