#version 450 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec4 color;

uniform mat4x4 R1Inv;
uniform mat4x4 model;
uniform mat4x4 view;
uniform mat4x4 project;

out vec4 outColor;

void main(){
	gl_Position = project * view * model * R1Inv * vec4(pos,1);
	outColor = color;
}