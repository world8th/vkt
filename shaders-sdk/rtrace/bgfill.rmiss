#version 460 core
#extension GL_NV_ray_tracing : require
#extension GL_GOOGLE_include_directive : require


struct RayPayload {
    vec2 UV;
    float depth;
    uint index;
    uvec4 mdata;
};

layout ( location = 0 ) rayPayloadInNV RayPayload PrimaryRay;


void main() {
    PrimaryRay.UV = vec2(0,0);
    PrimaryRay.depth = 100.f;
    PrimaryRay.index = 0xFFFFFFFF;
    
}
