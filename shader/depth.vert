#version 450 core

in vec2 pos;
in vec2 coord;

uniform float magCofficient;

out vec2 outCoord;

void main(){
    gl_Position = vec4( pos.x * magCofficient, pos.y * magCofficient, 0 , 1);
    outCoord = coord;
}
