#version 460 core



out vec2 pos;

layout (location=0) in vec2 aPos;
uniform uvec2 uWorldDimmensions;
uniform mat4 uViewProjection;
uniform sampler2D uTexture;
void main(){
    vec2 position = vec2(gl_InstanceID % uWorldDimmensions.x, gl_InstanceID / uWorldDimmensions.x);
    gl_Position = uViewProjection * vec4(aPos + position,0.f,1.f);
    vec2 vertexPosInTile = aPos + vec2(0.5, 0.5); // Transform [-0.5, 0.5] -> [0, 1]
    pos = (position + vertexPosInTile) / vec2(uWorldDimmensions); // Map to [0, 1] over entire texture
    //pos = (aPos + vec2(0.5,0.5) + position) / vec2(uWorldDimmensions.x, uWorldDimmensions.y);
    //pos.y = 1 - pos.y;
}

