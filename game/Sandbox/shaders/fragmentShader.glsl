#version 460 core

out vec4 FragColor; // Output color of the fragment

in vec2 TexCoord; // Input texture coordinate from the vertex shader

uniform sampler2D texture1; // Sampler for the texture
uniform vec3 spriteColor; // Color of the sprite

void main()
{
    // Sample the texture at the given texture coordinates
    vec4 texColor = texture(texture1, TexCoord);

    // Multiply the sampled color with the sprite color

    FragColor = vec4(texColor.rgb * spriteColor, 1.f); // Preserve the alpha channel
  //  FragColor  = vec4(1.f, 0.f,0.f,1.0f);
//    FragColor = Color; // Preserve the alpha channel
}
