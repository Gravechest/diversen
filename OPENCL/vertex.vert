#version 460 core
layout (location = 0) in vec2 verticles;
uniform float playerpos;
void main(){
	gl_Position = vec4(vec2(verticles[0] + playerpos,verticles[1]),1.0,1.0);
}
