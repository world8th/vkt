#version 460 core

layout ( location = 0 ) in vec2 vcoord;
layout ( location = 0 ) out vec4 uFragColor;
layout ( binding = 2, rgba32f ) uniform image2D outputImage;

void main() {
    //imageStore(outputImage, ivec2(vcoord*imageSize(outputImage)), vec4(1.f.xxx,1.f));
    const vec2 size = imageSize(outputImage);
    vec2 coord = gl_FragCoord.xy; //coord.y = size.y - coord.y;
    uFragColor = vec4(imageLoad(outputImage,ivec2(coord)).xyz,1.f);
}
