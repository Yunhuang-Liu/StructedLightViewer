#version 450 core
in vec4 outColor;
out vec4 fragColor;

void main(){
	fragColor = vec4(outColor.rgb / 255,outColor.a);
}