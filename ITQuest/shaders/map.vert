#version 330 core
layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;

out vec2 mapTexCoord;

void main() {
    gl_Position = vec4(vertexPosition, 0.0, 1.0);
    mapTexCoord = vertexTexCoord;
}
