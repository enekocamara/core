#version 460 core

out vec4 FragColor; // Output color of the fragment

in vec2 TexCoord; // Input texture coordinate from the vertex shader

uniform sampler2D uTexture;

void main()
{
  FragColor = texture(uTexture, TexCoord);
  //FragColor = vec4(TexCoord.x, 0.f, TexCoord.y,1.0f);
}
