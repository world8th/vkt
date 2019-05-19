#ifndef _MATHLIB_H
#define _MATHLIB_H

// here also support of true half float and int16...



// NEXT standard consts in current
// Ray tracing NEXT capable shader standard development planned begin in 2019 year
const float32_t PHI = 1.6180339887498948482f;
const float32_t SFN = 0.00000011920928955078125f, SFO = 1.f+SFN;//1.00000011920928955078125f;
const float32_t INFINITY = 1e+5f, N_INFINITY = (INFINITY*(1.f-SFN));

const float32_t PI = 3.1415926535897932384626422832795028841971f;
const float32_t TWO_PI = 6.2831853071795864769252867665590057683943f;
const float32_t SQRT_OF_ONE_THIRD = 0.5773502691896257645091487805019574556476f;
const float32_t E = 2.7182818284590452353602874713526624977572f;
const float32_t INV_PI = 0.3183098861837907f;
const float32_t TWO_INV_PI = 0.6366197723675814f;
const float32_t INV_TWO_PI = 0.15915494309189535f;


//const float N1024 = 1024.f;
#ifdef USE_F16_BVH
const float16_t InZero = 0.0009765625hf, InOne = 1.0009765625hf;
#else
const float32_t InZero = 0.00000011920928955078125f, InOne = 1.00000011920928955078125f;
#endif

#ifdef USE_F16_BVH
const float16_t One1024 = 0.0009765625hf;
#else
const float32_t One1024 = 0.0009765625f;
#endif


// float 16 or 32 bit types
#ifdef USE_F16_BVH
#define ftype_ float16_t
#define fvec3_ f16vec3
#define fvec4_ f16vec4
#define fvec2_ f16vec2
#define fmat2x4_ f16mat2x4
#define fmat4x4_ f16mat4x4
#define fmat3x4_ f16mat3x4
#define fmat3x2_ f16mat3x2
#define fmat4x3_ f16mat4x3
#else 
#define ftype_ float
#define fvec2_ vec2
#define fvec3_ vec3
#define fvec4_ vec4
#define fmat2x4_ mat2x4
#define fmat4x4_ mat4x4
#define fmat3x4_ mat3x4
#define fmat3x2_ mat3x2
#define fmat4x3_ mat4x3
#endif


const float _FZERO = 0.f;
const int _IZERO = 0;


#define ISTORE(img, crd, data) imageStore(img,crd,data)
#define SGATHER(smp, crd, chnl) textureGather(smp,crd,chnl)


// AMD min3/max3 ...
#ifdef ENABLE_VEGA_INSTRUCTION_SET
#define min3_wrap(a,b,c) min3(a,b,c)
#define max3_wrap(a,b,c) max3(a,b,c)
#else
#define min3_wrap(a,b,c) min(a,min(b,c))
#define max3_wrap(a,b,c) max(a,max(b,c))
#endif
#define max3_vec(a) max3_wrap(a[0],a[1],a[2])
#define min3_vec(a) min3_wrap(a[0],a[1],a[2])
#define mid3_vec(a) mid3_wrap(a[0],a[1],a[2])


#ifdef ENABLE_VEGA_INSTRUCTION_SET
#define mid3_wrap(a,b,c) mid3(a,b,c)
#else
float mid3_wrap(in float a, in float b, in float c) {
    const float m = max3_wrap(a, b, c);
    [[flatten]] if (m == a) { return max(b, c); } else 
    [[flatten]] if (m == b) { return max(a, c); } else { return max(a, b); };
};

vec4 mid3_wrap(in vec4 a, in vec4 b, in vec4 c) {
    return vec4(mid3_wrap(a[0],b[0],c[0]), mid3_wrap(a[1],b[1],c[1]), mid3_wrap(a[2],b[2],c[2]), mid3_wrap(a[3],b[3],c[3]));
};
#endif

/*
// experimental new paired logic system
#ifdef USE_INT16_BOOL_PAIR
#define pbvec2_ u16vec2
#define pbool_ uint16_t
#else
#define pbvec2_ uint32_t
#define pbool_ uint32_t
#endif


#ifdef USE_INT16_BOOL_PAIR
const pbool_ true_ = 1us, false_ = 0us; 
const pbvec2_ true2_ = 1us.xx, false2_ = 0us.xx;
#else
const pbool_ true_ = 1u, false_ = 0u; 
const pbvec2_ true2_ = 0x00010001u, false2_ = 0x00000000u;
#endif

pbvec2_ binarize(in bvec2 tbl){
#ifdef USE_INT16_BOOL_PAIR
    return pbvec2_(tbl);
#else
    return pbool_(tbl.x)|(pbool_(tbl.y)<<16u);
#endif
};

#ifdef USE_INT16_BOOL_PAIR
pbvec2_ binarize(in lowp uvec2 tbl){ return pbvec2_(tbl); }; // accepts u32vec2
pbvec2_ binarize(in pbvec2_ tbl){ return tbl; }; // accepts u16vec2
#else
pbvec2_ binarize(in lowp uvec2 tbl){ return pbvec2_(tbl.x|(tbl.y<<16u)); }; // accepts u32vec2
#endif

pbvec2_ binarize(in bool tbl){
#ifdef USE_INT16_BOOL_PAIR
    return pbvec2_(tbl.xx);
#else
    return pbool_(tbl)|(pbool_(tbl)<<16u);
#endif
};

pbvec2_ binarize(in pbool_ tbl){
#ifdef USE_INT16_BOOL_PAIR
    return pbvec2_(tbl.xx);
#else
    return tbl|(tbl<<16u);
#endif
};

pbool_ pl_x(in pbvec2_ tbl){
#ifdef USE_INT16_BOOL_PAIR
    return tbl.x;
#else
    return tbl&1u;
#endif
};

pbool_ pl_y(in pbvec2_ tbl){
#ifdef USE_INT16_BOOL_PAIR
    return tbl.y;
#else
    return tbl>>16u;
#endif
};

 bool  SSC(in pbool_ b) {return b==true_;};
pbool_ any(in pbvec2_ b) {return pl_x(b)|pl_y(b);};
pbool_ all(in pbvec2_ b) {return pl_x(b)&pl_y(b);};
#define IF(b)if(SSC(b))

*/

// null of indexing in float representation
const uint UINT_ZERO = 0x0u, UINT_NULL = 0xFFFFFFFFu;
#define FINT_NULL uintBitsToFloat(UINT_NULL)
#define FINT_ZERO uintBitsToFloat(UINT_ZERO)

// precision utils
#define precIssue(a) (sign(sign(a)+(SFN))*max(abs(a),SFN))


// vector math utils
float sqlen(in vec3 a) { return dot(a, a); }
float sqlen(in vec2 a) { return dot(a, a); }
float sqlen(in float v) { return v * v; }
int modi(in int a, in int b) { return (a % b + b) % b; };
//vec4 divW(in vec4 aw) { return aw / precIssue(aw.w); };
vec4 divW(in vec4 aw) { return aw / (aw.w); };
vec4 divW(in vec3 aw) { return vec4(aw,1.f); };
vec3 rotate_vector( in vec4 quat, in vec3 vect ) { return vect + 2.0 * cross( cross( vect, quat.xyz ) + quat.w * vect, quat.xyz ); };
vec4 rotation_quat( in vec3 axis, in float angl ) { return vec4(axis * sin(angl), cos(angl)); };

// memory managment
void swap(inout  int a, inout  int b) { const  int t = a; a = b; b = t; }
void swap(inout uint a, inout uint b) { const uint t = a; a = b; b = t; }

uint exchange(inout uint mem, in uint v) { const uint tmp = mem; mem = v; return tmp; }
 int exchange(inout  int mem, in  int v) { const  int tmp = mem; mem = v; return tmp; }

uint add(inout uint mem, in uint ops) { const uint tmp = mem; mem += ops; return tmp; }
 int add(inout  int mem, in  int ops) { const  int tmp = mem; mem += ops; return tmp; }


bool lessThan(in uint a, in uint b){ return a < b; };
bool all(in bool a){ return a; };
bool any(in bool a){ return a; };

bool not(in bool a) { return !a.x; };
bool and(in bool a, in bool b) { return a.x && b.x; };
bool or(in bool a, in bool b) { return a.x || b.x; };

// logical functions (bvec2)
bvec2 not(in bvec2 a) { return bvec2(!a.x, !a.y); };
bvec2 and(in bvec2 a, in bvec2 b) { return bvec2(a.x && b.x, a.y && b.y); };
bvec2 or(in bvec2 a, in bvec2 b) { return bvec2(a.x || b.x, a.y || b.y); };

// logical functions (bvec4)
bvec4 or(in bvec4 a, in bvec4 b) { return bvec4(a.x || b.x, a.y || b.y, a.z || b.z, a.w || b.w); };
bvec4 and(in bvec4 a, in bvec4 b) { return bvec4(a.x && b.x, a.y && b.y, a.z && b.z, a.w && b.w); };
bvec4 not(in bvec4 a) { return bvec4(!a.x, !a.y, !a.z, !a.w); };

// mixing functions
void mixed(inout float src, inout float dst, in float coef) { dst *= coef; src *= 1.0f - coef; }
void mixed(inout vec3 src, inout vec3 dst, in float coef) { dst *= coef; src *= 1.0f - coef; }
void mixed(inout vec3 src, inout vec3 dst, in vec3 coef) { dst *= coef; src *= 1.0f - coef; }

float clamp01(in float c) { return clamp(c, 0.f.x,    (1.f-SFN).x);    };
vec2  clamp01(in vec2 c)  { return clamp(c, 0.f.xx,   (1.f-SFN).xx);   };
vec3  clamp01(in vec3 c)  { return clamp(c, 0.f.xxx,  (1.f-SFN).xxx);  };
vec4  clamp01(in vec4 c)  { return clamp(c, 0.f.xxxx, (1.f-SFN).xxxx); };

// matrix math (simular DX12)
vec4 mult4(in vec4 vec, in mat4 tmat) { return tmat * vec; };
vec4 mult4(in mat4 tmat, in vec4 vec) { return vec * tmat; };

vec4 mult4(in vec3 vec, in mat3x4 tmat) { return tmat * vec; };
vec3 mult4(in mat3x4 tmat, in vec4 vec) { return vec * tmat; };

vec3 mult4(in vec4 vec, in mat4x3 tmat) { return tmat * vec; };
vec4 mult4(in mat4x3 tmat, in vec3 vec) { return vec * tmat; };

// it can be preferred in RTX shaders (column major)
vec3 mult4(in mat4x3 tmat, in vec4 vec) { return tmat * vec; };
vec4 mult4(in mat3x4 tmat, in vec3 vec) { return tmat * vec; };

mat4 mult4(in mat4 tmat, in mat4 otmat) { return otmat*tmat; };


const vec2 swiz01 = vec2(0.f,1.f);
vec4 point4(in vec4 p) {return p*swiz01.yyyx+swiz01.xxxy;};
vec4 point4(in vec4 p, in float w) {return p*swiz01.yyyx+w*swiz01.xxxy;};
vec4 point4(in vec3 p) {return vec4(p,1.f);};
vec4 point4(in vec3 p, in float w) {return vec4(p,w);};
vec4 vector4(in vec4 p) {return p*swiz01.yyyx;};
//vec4 crossp4(in vec4 a, in vec4 b) { return vec4(cross(a.xyz,b.xyz),a.w*b.w); };
vec4 crossp4(in vec3 a, in vec3 b) { return vec4(cross(a,b),0.f); };
vec4 crossp4(in vec4 a, in vec4 b) { return crossp4(a.xyz,b.xyz); };


// 64-bit packing
//#define U2P unpack32
//#define P2U pack64

// 128-bit packing (2x64bit)
//u32vec4 U4P(in u64vec2 pckg) { return u32vec4(U2P(pckg.x ), U2P(pckg.y )); };
//u64vec2 P4U(in u32vec4 pckg) { return u64vec2(P2U(pckg.xy), P2U(pckg.zw)); };

// float packing
   u32vec2 packHalf4x16(in highp vec4 floats) { return u32vec2(packHalf2x16(floats.xy), packHalf2x16(floats.zw)); };
highp vec4 unpackHalf4x16(in u32vec2 hilo) { return vec4(unpackHalf2x16(hilo.x), unpackHalf2x16(hilo.y)); };

u32vec2 packSnorm4x16(in  vec4 floats) { return u32vec2(packSnorm2x16(floats.xy), packSnorm2x16(floats.zw)); };
   vec4 unpackSnorm4x16(in u32vec2 hilo) { return vec4(unpackSnorm2x16(hilo.x), unpackSnorm2x16(hilo.y)); };

u32vec2 packUnorm4x16(in  vec4 floats) { return u32vec2(packUnorm2x16(floats.xy), packUnorm2x16(floats.zw)); };
   vec4 unpackUnorm4x16(in u32vec2 hilo) { return vec4(unpackUnorm2x16(hilo.x), unpackUnorm2x16(hilo.y)); };


// bit utils
lowp int lsb(in uint vlc) { return findLSB(vlc); }
lowp int msb(in uint vlc) { return findMSB(vlc); }
lowp uint bitcnt(in uint vlc) { return uint(bitCount(vlc)); }
lowp uint bitcnt(in uvec2 lh) { uvec2 bic = bitCount(lh); return uint(bic.x+bic.y); }
//uint bitcnt(in uvec2 lh) { return bitCount(P2U(lh)); }
lowp uint bitcnt(in uint64_t lh) { ivec2 bic = bitCount(unpack32(lh)); return uint(bic.x+bic.y); }

// bit measure utils
lowp int lsb(in uvec2 pair) {
#ifdef AMD_PLATFORM
    return findLSB(pack64(pair));
#else
    const ivec2 hl = findLSB(pair);
    return hl.x >= 0 ? hl.x : 32 + hl.y;
#endif
};

lowp int msb(in uvec2 pair) {
#ifdef AMD_PLATFORM
    return findMSB(pack64(pair));
#else
    const ivec2 hl = findMSB(pair);
    return hl.y >= 0 ? 32 + hl.y : hl.x;
#endif
};


lowp int lsb(in uvec4 quadr){ return lsb(quadr.xy); };
lowp int msb(in uvec4 quadr){ return msb(quadr.xy); };
lowp uint bitcnt(in uvec4 quadr){ return bitcnt(quadr.xy); };

lowp int lsb(in uint64_t pair) { return lsb(unpack32(pair)); };
lowp int msb(in uint64_t pair) { return msb(unpack32(pair)); };

// bit insert and extract
 int BFE_HW(in  int base, in  int offset , in int bits) { return bitfieldExtract(base, offset, bits); }
uint BFE_HW(in uint base, in  int offset , in int bits) { return bitfieldExtract(base, offset, bits); }
 int BFI_HW(in  int base, in  int inserts, in int offset, in int bits) { return bitfieldInsert(base, inserts, offset, bits); }
uint BFI_HW(in uint base, in uint inserts, in int offset, in int bits) { return bitfieldInsert(base, inserts, offset, bits); }

// int operations
 int tiled(in  int x, in  int y) {return (x / y) +  int((x % y) != 0 ); }
uint tiled(in uint x, in uint y) {return (x / y) + uint((x % y) != 0u); }

// color space utils
const float HDR_GAMMA = 2.2f;
vec3 fromLinear(in vec3 linearRGB) { return mix(vec3(1.055)*pow(linearRGB, vec3(1.0/2.4)) - vec3(0.055), linearRGB * vec3(12.92), lessThan(linearRGB, vec3(0.0031308))); }
vec3 toLinear(in vec3 sRGB) { return mix(pow((sRGB + vec3(0.055))/vec3(1.055), vec3(2.4)), sRGB/vec3(12.92), lessThan(sRGB, vec3(0.04045))); }
vec4 fromLinear(in vec4 linearRGB) { return vec4(fromLinear(linearRGB.xyz), linearRGB.w); }
vec4 toLinear(in vec4 sRGB) { return vec4(toLinear(sRGB.xyz), sRGB.w); }




// BVH utility
u32vec2 bitfieldReverse64(in u32vec2 p){return bitfieldReverse(p).yx;}


int nlz(in uvec2 x) { return 63 - msb(x); }
int nlz(in uint x) { return 31 - msb(x); }
int nlz(in int x) { return nlz(uint(x)); }


#ifdef EXPERIMENTAL_UNORM16_DIRECTION
#define dirtype_t float
#define dirtype_t_decode(f) unpackUnorm2x16(floatBitsToUint(f)).yx
#define dirtype_t_encode(f) uintBitsToFloat(packUnorm2x16(f.yx))
#else
#define dirtype_t uvec2
#define dirtype_t_decode(f) uintBitsToFloat(f)
#define dirtype_t_encode(f) floatBitsToUint(f)
#endif


dirtype_t lcts(in vec3 direct) { return dirtype_t_encode(vec2(fma(atan(direct.z,direct.x),INV_TWO_PI,0.5f),acos(-direct.y)*INV_PI)); };
     vec3 dcts(in vec2 hr) { hr = fma(hr,vec2(TWO_PI,PI),vec2(-PI,0.f)); const float up=-cos(hr.y),over=sqrt(fma(up,-up,1.f)); return vec3(cos(hr.x)*over,up,sin(hr.x)*over); };
     vec3 dcts(in dirtype_t hr) { return dcts(dirtype_t_decode(hr)); };




#define f32_f16   packHalf4x16
#define f16_f32 unpackHalf4x16

// issue compatible gather (may haven't optimization itself)
vec4 sifonGather(in sampler2D SMP, in vec2 TXL, in const int CMP) { 
    return ( CMP == 3 ? textureGather(SMP, TXL, 3) : ( CMP == 2 ? textureGather(SMP, TXL, 2) : ( CMP == 1 ? textureGather(SMP, TXL, 1) : textureGather(SMP, TXL, 0) ) ) ); 
};

// bilinear interpolation remedy 
const vec2 offsetf[4] = { vec2(0,1), vec2(1,1), vec2(1,0), vec2(0,0) };
vec4 textureHQ(in sampler2D SMP, in vec2 TXL, in int LOD) {
    const vec2 sz = textureSize(SMP,LOD), is = 1.f/sz, tc = fma(TXL,sz,-0.5f.xx), tm = (floor(tc+SFN)+0.5f)*is;
    const vec4 il = vec4(fract(tc),1.f-fract(tc)), cf = vec4(il.z*il.y,il.x*il.y,il.x*il.w,il.z*il.w);
    return mult4(mat4(textureGather(SMP,tm,0),textureGather(SMP,tm,1),textureGather(SMP,tm,2),textureGather(SMP,tm,3)),cf);
};








// new packing library 

// uint32_t 
#define u32x1_t uint32_t
#define u32x2_t u32vec2
#define u32x4_t u32vec4
#define p32x1_t(a) u16x2pack(u16x2_t(a))

// uint16_t support to read (extended)
#ifdef ENABLE_INT16_SUPPORT
#define u16x1_t uint16_t
#define u16x4_t u16vec4
#define u16x2_t u16vec2
#define u16x2pack pack32
#define u16x2unpack unpack16
#define m16pq  
#else
#define u16x1_t uint
#define u16x4_t uvec4
#define u16x2_t uvec2
#define m16pq highp
      u32x1_t u16x2pack  (in highp u16x2_t a) { return (a.y<<16u)|a.x; };
highp u16x2_t u16x2unpack(in u32x1_t a) { return u16x2_t(a&0xFFFFu,a>>16u); };
#endif

// uint8_t support to read
#ifdef ENABLE_NATIVE_U8
#define u8x4_t u8vec4
#define u8x2_t u8vec2
#define u8x1_t uint8_t
#define u8x4pack pack32
#define u8x2pack pack16
#define u8x4unpack unpack8
#define m8pq  
#else
#define u8x4_t u16x4_t
#define u8x2_t u16x2_t
#define u8x1_t u16x1_t

// 8-bit precision qualifier
#ifdef ENABLE_INT16_SUPPORT
#define m8pq  
#else
#define m8pq lowp
#endif

              const m8pq u8x2_t bshift16 = {0u,8u}; const m8pq u8x4_t bshift32 = bshift16.xyxy;
u16x1_t u8x2pack(in m8pq u8x2_t v2) { v2 <<= bshift16; return u16x1_t(v2[0]|v2[1]); };
u32x1_t u8x4pack(in m8pq u8x4_t v4) { v4 <<= bshift32; return p32x1_t(v4.xz|v4.yw); };
 u8x4_t u8x4unpack(in u32x1_t u4) { return u8x4_t(u4.x>>0u,u4>>8u,u4>>16u,u4>>24u)&u8x1_t(0xFFu); };
#endif

// coding library (16-bit)
#ifdef ENABLE_INT16_SUPPORT
u32x1_t p2x_16(in u16x2_t a) { return u16x2pack(a); };
u32x1_t p2x_16(in highp uvec2 a) { return bitfieldInsert(a.x,a.y,16,16); };
#else
#define p2x_16 u16x2pack
#endif
//#define up2x_16 u16x2unpack

// coding library (8-bit)
const lowp uvec4 u8x4shf = {0u,8u,16u,24u};
const lowp uvec2 u8x2shf = {0u,8u};

// for 16-bit types 
const m8pq u8x2_t u8x2shf16x = {0us,8us};


// 32-bit packing 
u32x1_t p4x_8(in lowp uvec4 a) { a<<=u8x4shf; return (a[0]|a[1]|a[2]|a[3]); };
m8pq u8x4_t up4x_8(in u32x1_t a)  { return u8x4_t((a.xxxx>>u8x4shf)&0xFFu); };


// 16-bit packing (32-bit proxy)
u16x1_t p2x_8(in lowp uvec2 a) { a<<=u8x2shf; return u16x1_t(a[0]|a[1]); };
m8pq u8x2_t up2x_8(in mediump uint a)  { return u8x2_t((a.xx>>u8x2shf)&0xFFu); };


// 16-bit packing (natural)
//#ifdef ENABLE_INT16_SUPPORT
//u16x1_t p2x_8(in m8pq u8x2_t a) { a<<=u8x2shf16x; return u16x1_t(a[0]|a[1]); };
//m8pq u8x2_t up2x_8(in u16x1_t a)  { return u8x2_t((a.xx>>u8x2shf16x)&0xFFus); };
//#endif


#ifdef USE_INT16_FOR_MORTON
    u32x1_t splitBy4(in m8pq u8x1_t a) {
        u16x2_t r = (u16x2_t(a).xx>>u16x2_t(0us,4us))&0xFus;   // ---- ----  ---- 7654  ---- ---- ---- 3210
                r = (r | (r << 6us.xx)) & 0x0303us.xx;          // ---- --76  ---- --54  ---- --32 ---- --10
                r = (r | (r << 3us.xx)) & 0x1111us.xx;          // ---7 ---6  ---5 ---4  ---3 ---2 ---1 ---0
        return u16x2pack(r);
    };
#else
    u32x1_t splitBy4(in lowp uint a) {
        u32x1_t r = (a | (a << 12u)) & 0x000F000Fu; // ---- ----  ---- 7654  ---- ---- ---- 3210
                r = (r | (r <<  6u)) & 0x03030303u; // ---- --76  ---- --54  ---- --32 ---- --10
                r = (r | (r <<  3u)) & 0x11111111u; // ---7 ---6  ---5 ---4  ---3 ---2 ---1 ---0
        return r;
    };
#endif

    u32x1_t splitBy2(in highp uint a) {
        u32x1_t r = (a | (a << 8u)) & 0x00FF00FFu; // 
                r = (r | (r << 4u)) & 0x0F0F0F0Fu; // 
                r = (r | (r << 2u)) & 0x33333333u; // 
                r = (r | (r << 1u)) & 0x55555555u;
        return r;
    };

    // scatter 32-bit number (two 16-bit) into 64-bit number
    u32x2_t splitBy2(in highp uvec2 a) { return u32x2_t(splitBy2(a.x),splitBy2(a.y)); };

    // 
    u32x1_t interleave8x4(in m8pq u8x4_t a) { return u32x1_t((splitBy4(a.x) << 0u) | (splitBy4(a.y) << 1u) | (splitBy4(a.z) << 2u) | (splitBy4(a.w) << 3u)); };
    u32x1_t interleave8x4(in u32x1_t a) { return interleave8x4(u8x4unpack(a)); };
    u32x1_t interleave16x2(in m16pq u16x2_t a) { return u32x1_t((splitBy2(a.x) << 0u) | (splitBy2(a.y) << 1u)); };
    u32x1_t interleave16x2(in u32x1_t a) { return interleave16x2(u16x2unpack(a)); };

    // encode new morton code by two 64-bit elements
    u32x4_t interleave64x2(in u32x4_t a){
        const m16pq u16x4_t _a = u16x4_t(u16x2unpack(a.x),u16x2unpack(a.y)), _b = u16x4_t(u16x2unpack(a.z),u16x2unpack(a.w));
        const u32x4_t 
            interleave_a = u32x4_t(splitBy2(_a.xy),splitBy2(_a.zw)),
            interleave_b = u32x4_t(splitBy2(_b.xy),splitBy2(_b.zw));
        return u32x4_t((interleave_a<<0u)|(interleave_b<<1u));
    };

    // encode new morton code by four 32-bit elements
    u32x4_t interleave32x4(in u32x4_t a) {
        return u32x4_t(
            interleave8x4(u8x4pack(u8x4_t((a>> 0u)&0xFFu)&u8x1_t(0xFFu))),
            interleave8x4(u8x4pack(u8x4_t((a>> 8u)&0xFFu)&u8x1_t(0xFFu))),
            interleave8x4(u8x4pack(u8x4_t((a>>16u)&0xFFu)&u8x1_t(0xFFu))),
            interleave8x4(u8x4pack(u8x4_t((a>>24u)&0xFFu)&u8x1_t(0xFFu)))
        );
    };

    // encode new morton code by two 32-bit elements
    highp u32x4_t interleave32x2(in highp u32x4_t a) {
        return u32x4_t(
            interleave16x2(u16x2pack(u16x2_t((a.xy>> 0u)&0xFFFFu)&u16x1_t(0xFFFFu))),
            interleave16x2(u16x2pack(u16x2_t((a.xy>>16u)&0xFFFFu)&u16x1_t(0xFFFFu))),
            0u.xx
        );
    };


#endif
