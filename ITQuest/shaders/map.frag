#version 330 core
uniform sampler2D texMap;
uniform sampler2D texture;
in vec2 mapTexCoord;

out vec4 fragColor;

void main() {
    vec4 map = texture(texMap, mapTexCoord);
    vec4 tex_grass = texture(texture, mapTexCoord);
    if(map.r >= 0.2 || tex_grass.g > 0.95)
        fragColor = mix(map, tex_grass, 0.8);
    else if(map.r <= 0.8)
        fragColor = map;

}
