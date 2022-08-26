#version 450 core

uniform sampler2DArray sTexture;

in float vid;
in vec2 vCoord;
out vec4 fragColor;

void main(){
    vec3 texCood = vec3(vCoord.st, floor(vid / 4));
    fragColor = texture(sTexture, texCood);
}
