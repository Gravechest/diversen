#version 460 core

out vec4 FragColor;

layout (std4,binding = 0) uniform map{
	int map;
};

void main(){
	FragColor = vec4(0.5,0.0,0.0,1.0);
}
