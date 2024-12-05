#version 460 core
in vec2 pos;

uniform sampler2D uTexture;


struct Material{
    float height;
    float humidity;
    float heat;
};
out vec4 FragColor;

Material createMaterial(vec3 values) {
    Material mat;
    mat.height = values.x;
    mat.humidity = values.y;
    mat.heat = values.z;
    return mat;
}

struct Palette{
    vec3 a;
    vec3 b;
    vec3 c;
    vec3 d;
};

Palette createPalette(vec3 a, vec3 b, vec3 c, vec3 d){
    Palette palette;
    palette.a = a;
    palette.b = b;
    palette.c = c;
    palette.d = d;
    return palette;
}

vec3 palette(float t, Palette pal)
{
    return pal.a + pal.b*cos( 6.283185*(pal.c*t+pal.d) );
}

//Palette water = createPalette(vec3(-1.172, -0.582, 0.448), vec3(-1.322, -1.322,-0.202), vec3(0.898, -0.632, -0.762), vec3(-2.312, 2.178, -0.922));
//Palette water = createPalette(vec3(-1.522, -0.812, 0.448),vec3(-1.322, -1.322, -0.202),vec3(0.898, -0.632, -0.762),vec3(-2.312,2.178,-0.922));
void main()
{

    Material mat = createMaterial((texture(uTexture, pos).xyz + 1.f) / 2.f); 
    //    Material mat = createMaterial((fragMat + 1.f) / 2.f);
    Palette water = createPalette(vec3(-1.522, -0.812, 0.448),vec3(-1.322, -1.322, -0.202),vec3(0.898, -0.632, -0.762),vec3(-2.312,2.178,-0.922));
    if (mat.height < 0.5)
        FragColor = vec4(palette(mat.height * 2, water),1.f);
    else
        FragColor = vec4(0.1f, 1 - mat.height * 0.8, 0.1f, 1.0f);
        /*
    if (pos.x < 0.f || pos.x > 9){
        FragColor = vec4(1.f, 0.f, 0.f, 1.f);
    }else if (pos.y < 0.f || pos.y > 9){
        FragColor = vec4(1.f, 0.f, 1.f, 1.5f);
    } else
        FragColor = vec4(pos.x, pos.y, 0.f, 1.f);*/

}
