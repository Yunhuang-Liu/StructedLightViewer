#version 450 core

uniform mat4x4 view;
uniform mat4x4 project;

in vec3 pos;
in vec3 color;

out vec3 outColor;

void main(){
    gl_Position = project * view * vec4(pos,1);
    outColor = color;
}
