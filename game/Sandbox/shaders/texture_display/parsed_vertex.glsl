#version 460 core

out vec2 TexCoord;


uniform sampler2D uTexture;
void main()
{    
    vec2 positions[6] = vec2[](
        vec2(-1.0,  1.0), // Top-left
        vec2(-1.0, -1.0), // Bottom-left
        vec2( 1.0, -1.0), // Bottom-right
        vec2(-1.0,  1.0), // Top-left
        vec2( 1.0, -1.0), // Bottom-right
        vec2( 1.0,  1.0)  // Top-right
    );

    vec2 texCoords[6] = vec2[](
        vec2(0.0, 1.0), // Top-left
        vec2(0.0, 0.0), // Bottom-left
        vec2(1.0, 0.0), // Bottom-right
        vec2(0.0, 1.0), // Top-left
        vec2(1.0, 0.0), // Bottom-right
        vec2(1.0, 1.0)  // Top-right
    );
    gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0); // Set vertex position
    TexCoord = texCoords[gl_VertexID]; 
}
