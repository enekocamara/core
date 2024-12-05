#version 460 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D uTexture;

void main()
{
  vec4 texture_col = texture(uTexture, TexCoord);
  texture_col = (texture_col + 1) / 2;
  FragColor = vec4(vec3(texture_col.r),1.0f);
}
