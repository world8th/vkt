#version 460 core

layout (location = 0) in vec2 vcoord;

layout (location = 0) out vec4 fragColor;

void main(){
    fragColor = vec4(1.f, 0.5f, 0.f, 1.f);
};
