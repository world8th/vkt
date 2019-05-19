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
                        hitAttributeNV vec2 HitAttribs;

void main() {
    PrimaryRay.UV = HitAttribs;
    PrimaryRay.depth = gl_HitTNV;
    PrimaryRay.index = gl_PrimitiveID;
    PrimaryRay.mdata[0] = gl_InstanceID;
}
