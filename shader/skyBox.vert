#version 450 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 coord;

uniform mat4x4 view;
uniform mat4x4 project;

smooth out vec2 vCoord;
out float vid;

void main(){
    gl_Position = project * view * vec4(pos, 1);
    vCoord = coord;
    vid = gl_VertexID;
}
