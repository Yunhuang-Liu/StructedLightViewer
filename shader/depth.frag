#version 450 core

uniform sampler2D texture;
in vec2 outCoord;

out vec4 outColor;

void main(){
    outColor = texture2D(texture,outCoord);
}
