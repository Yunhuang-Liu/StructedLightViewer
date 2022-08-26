#version 450 core

uniform mat4x4 view;
uniform mat4x4 project;

in vec3 pos;

void main(){
    gl_Position = project * view * vec4(pos,1);
}
