#pragma once // #

// Resolve conflict with another `core.hpp`
#ifndef VKT_CORE_HPP
#define VKT_CORE_HPP

//#include "utils.hpp"
//#include "structs.hpp"
#include <cstdint>
#include <cassert>
#include <memory>
#include <optional>
//#include <iostream>
#include <vector>
#include <functional>
#include <iosfwd>

//
#define VK_NO_PROTOTYPES
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0

// 
//#include <misc/soa_vector.hpp>
//#include <misc/half.hpp>

// 
#ifndef NSM
#define NSM api
#endif

// Currently Windows Only Supported
#if (defined(_WIN32) || defined(__MINGW32__) || defined(_MSC_VER_) || defined(__MINGW64__) || defined(WIN32) || defined(__WIN32) && !defined(__CYGWIN__))
#define VKT_WIN32_DETECTED true
#include <windows.h> // Fix HMODULE Type Error
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#define VK_USE_PLATFORM_WIN32_KHR
#else

#endif

// Default Backend
#if !defined(USE_D3D12) && !defined(USE_VULKAN)
#define USE_VULKAN
#endif

// 
#ifdef VKT_OPENGL_INTEROP
#ifdef VKT_USE_GLAD
#define GLFW_INCLUDE_NONE // Bad Include
#include <glad/glad.h>
#else
#define GLFW_INCLUDE_NONE // Bad Include
#include <glbinding/gl/gl.h>
#include <glbinding/gl/extension.h>
#include <glbinding/glbinding.h>
#endif
#else
#define GLFW_INCLUDE_NONE
#endif

// 
#if defined(VKT_USE_GLFW) 
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#endif

// Enable Vulkan-HPP when defined
#ifdef VULKAN_HPP
#define VKT_USE_VULKAN_HPP
#endif

// When enabled, use Vulkan-HPP support...
#ifdef VKT_USE_VULKAN_HPP
#define VULKAN_HPP_ENABLE_DYNAMIC_LOADER_TOOL 0 // Avoid XVK conflict
#include <vulkan/vulkan.hpp>
#else
#include <vulkan/vulkan.h>
#endif

// 
#ifdef VKT_CORE_USE_XVK
#include <xvk/xvk.hpp>
#endif

// 
#ifdef VKT_CORE_USE_VMA
#include <vma/vk_mem_alloc.h>
#endif

#define ASSIGN(obj,name) obj.name=name;

// 
#include <glm/glm.hpp>


// 
namespace vkt {

    //
    template<class T, class U = uint32_t> class bit_ops { protected: U n = 0u; //friend BitOps<T,U>;
        public: inline bit_ops<T,U>(const T& v = 0u) : n((U&)v){};
        public: inline bit_ops<T,U>(const U& n = 0u) : n(n){};
        public: inline bit_ops<T,U>(T&& v = 0u) : n((U&)v){};
        public: inline bit_ops<T,U>(U&& n = 0u) : n(n){};
        public: inline operator U&() { return n; };
        public: inline operator const U&() const { return n; };
        public: inline T operator &(const T& o){ return T{n&(U&)o}; };
        public: inline T operator |(const T& o){ return T{n|(U&)o}; };
        public: inline T operator ^(const T& o){ return T{n^(U&)o}; };
    };

    // 
    template<class S, class B, class U = uint32_t> class flags { protected: U n = 0u;
        public: inline flags<S,B,U>(const U& f = 0u) { *(U*)(this) = f; };
        public: inline flags<S,B,U>(const S& f = 0u) { *(U*)(this) = (U&)f; };
        public: inline flags<S,B,U>(const B& f = 0u) { *(U*)(this) = (U&)f; };
        public: inline operator U&() { return *(U*)this; };
        public: inline operator const U&() const { return *(U*)this; };
        public: inline operator B&() { return *(B*)this; };
        public: inline operator const B&() const { return *(B*)this; };
    };

    // boolean 32-bit capable for C++
    class bool32_t { // TODO: support operators
    protected: union {
        uint32_t b_ : 1; bool bb = false;
    };
    public: friend bool32_t;
        constexpr bool32_t(): b_(0u) {};
        inline bool32_t(const bool&a=false): b_(a?1u:0u) {};
        inline bool32_t(const uint32_t&a): b_(a&1u) {}; // make bitmasked
        inline bool32_t(const bool32_t&a): b_(a) {};

        // type conversion operators
        inline virtual operator bool() const {return bool(b_&1u);};
        inline virtual operator uint32_t() const {return (b_&1u);};

        // 
        inline virtual bool32_t& operator=(const bool&a){b_=(a?1u:0u);};
        inline virtual bool32_t& operator=(const uint32_t&a){b_=a&1u;};
        inline virtual bool32_t& operator=(const bool32_t&a){b_=a;};
    };

    inline void handle(const bool& valid = false) {
        if (!valid) {
            //std::cerr << "std::optional is wrong or not initialized" << std::endl; assert(valid);
        };
    };

    // Prefer Owner with Shared PTR!
    template<class T = uint8_t>
    class uni_ptr {
    protected: //using T = uint8_t;
        std::optional<std::reference_wrapper<T>> regular = std::nullopt;
        std::shared_ptr<T> shared = {};

    public:
        long size = 0ull;
        void* owner = nullptr;
        //T storage = {};

    public: friend uni_ptr<T>; // 
        inline uni_ptr() {};
        inline uni_ptr(const uni_ptr<T>& ptr) : shared(ptr.shared), regular(std::ref(*ptr.regular)), size(ptr.size), owner(ptr.owner) { 
            if (shared && !owner) { owner = &this->shared; }; // JavaCPP direct shared_ptr
        };
        inline uni_ptr(const std::shared_ptr<T>& shared) : shared(shared), regular(std::ref(*shared)) { 
            if (shared && !owner) { owner = &this->shared; }; // JavaCPP direct shared_ptr
        };
        inline uni_ptr(T* ptr) : regular(std::ref(*ptr)) {};
        inline uni_ptr(T& ptr) : regular(std::ref(ptr)) {};  // for argument passing
        inline uni_ptr(T* ptr, long size, void* owner) : regular(std::ref(*ptr)), size(size), owner(owner) {
            shared = owner != NULL && owner != ptr ? *(std::shared_ptr<T>*)owner : std::shared_ptr<T>(ptr);
        };
        inline uni_ptr* assign(T* ptr, int size, void* owner) {
            this->regular = std::ref(*ptr);
            this->size = size;
            this->owner = owner;
            this->shared = owner != NULL && owner != ptr ? *(std::shared_ptr<T>*)owner : std::shared_ptr<T>(ptr);
            return this;
        };
        static inline void deallocate(void* owner) {
            if (owner) { delete (std::shared_ptr<T>*)owner; }; // only when shared available
        };

        // 
        virtual inline uni_ptr* operator= (T* ptr) { regular = std::ref(*ptr); return this; };
        virtual inline uni_ptr* operator= (T& ptr) { regular = std::ref( ptr); return this; }; // for argument passing
        virtual inline uni_ptr* operator= (const std::shared_ptr<T>& ptr) {
            shared = ptr, regular = std::ref(*ptr); 
            if (shared && !owner) { owner = &this->shared; }; // JavaCPP direct shared_ptr
            return this;
        };
        virtual inline uni_ptr* operator= (const uni_ptr<T>& ptr) {
            T& ref = *ptr.regular;
            shared = ptr.shared, regular = std::ref(ref), owner = ptr.owner, size = ptr.size;
            if (shared && !owner) { owner = &this->shared; }; // JavaCPP direct shared_ptr
            return this;
        };

        // 
        template<class M = T>
        inline uni_ptr<M> dyn_cast() const { T& r = *regular; return shared ? uni_ptr<M>(std::dynamic_pointer_cast<M>(shared)) : uni_ptr<M>(dynamic_cast<M&>(r)); };

        // 
        //template<class... A>
        //uni_ptr<T>(A... args) : shared(std::make_shared<T>(args...)) {};

        // 
        virtual inline std::shared_ptr<T>& get_shared() { return (this->shared = (this->shared ? this->shared : std::shared_ptr<T>(get_ptr()))); };
        virtual inline std::shared_ptr<T>  get_shared() const { return (this->shared ? this->shared : std::shared_ptr<T>(const_cast<T*>(get_ptr()))); };

        // 
        virtual T* get_ptr() { 
            T& r = *regular;
            if (shared && (owner == NULL || owner == &r)) {
                owner = new std::shared_ptr<T>(shared);
            };
            return (T*)((void*)(shared ? &(*shared) : &r));
        };

        // 
        virtual inline const T* get_ptr() const {
            const T& r = *regular;
            return (T*)((void*)(shared ? &(*shared) : &r));
        };

        // 
        virtual inline bool has() { return regular && shared; };
        virtual inline bool has() const { return regular && shared; };

        //
        virtual inline T* get() { return get_ptr(); };
        virtual inline const T* get() const { return get_ptr(); };

        // 
        virtual inline T* ptr() { return get_ptr(); };
        virtual inline const T* ptr() const { return get_ptr(); };

        // 
        virtual inline T& ref() { return *regular; };
        virtual inline const T& ref() const { return *regular; };

        // experimental
        virtual inline operator T& () { return ref(); };
        virtual inline operator const T& () const { return ref(); };

        // 
        virtual inline operator T* () { return ptr(); };
        virtual inline operator const T* () const { return ptr(); };

        // 
        virtual inline operator std::shared_ptr<T>& () { return get_shared(); };
        virtual inline operator std::shared_ptr<T>  () const { return get_shared(); };

        // 
        virtual inline T* operator->() { return get_ptr(); };
        virtual inline const T* operator->() const { return get_ptr(); };

        // 
        virtual inline T& operator*() { return *get_ptr(); };
        virtual inline const T& operator*() const { return *get_ptr(); };
    };

    template<class T = uint8_t>
    class uni_arg {
    protected: 
        std::optional<T> storage = std::nullopt;
    public: // 
        uni_arg() {};
        uni_arg(const T& t) : storage(t) {};
        uni_arg(const T* t) : storage(*t) {};
        uni_arg(const uni_ptr<T>& p) : storage(*p) {}; // UnUsual and Vain
        uni_arg(const uni_arg<T>& a) : storage(*a) {};

        //
        virtual inline uni_arg<T>& operator= (const T& ptr) { storage = ptr; return *this; };
        virtual inline uni_arg<T>& operator= (const T* ptr) { storage = *ptr; return *this; };
        virtual inline uni_arg<T>& operator= (uni_arg<T> t) { storage = t.ref(); return *this; };
        virtual inline uni_arg<T>& operator= (uni_ptr<T> p) { storage = *p.ptr(); return *this; };

        // experimental
        virtual inline operator T& () { return ref(); };
        virtual inline operator const T& () const { return ref(); };

        // 
        virtual inline operator T* () { return ptr(); };
        virtual inline operator const T* () const { return ptr(); };

        // 
        virtual inline operator uni_ptr<T>() { handle(has()); return *storage; };
        virtual inline operator uni_ptr<const T>() const { handle(has()); return *storage; };

        // 
        virtual inline bool has_value() const { return storage.has_value(); };
        virtual inline bool has() const { return this->has_value(); };

        // 
        virtual inline T* ptr() { handle(has()); return (T*)((void*)(&(*storage))); };
        virtual inline const T* ptr() const { handle(has()); return (T*)((void*)(&(*storage))); };

        //
        template<class M = T> inline M* ptr() { handle(has()); return &reinterpret_cast<M&>(this->ref()); };
        template<class M = T> inline const M* ptr() const { handle(has()); return &reinterpret_cast<const M&>(this->ref()); };

        // 
        virtual inline T& ref() { handle(has()); return *storage; };
        virtual inline const T& ref() const { handle(has()); return *storage; };

        // 
        virtual inline T* operator->() { return ptr(); };
        virtual inline const T* operator->() const { return ptr(); };

        //
        virtual inline T& operator*() { return ref(); };
        virtual inline const T& operator*() const { return ref(); };
    };

    // Bi-Directional Conversion
    template<class T = uint8_t, class B = char8_t>
    class uni_dir {
    protected:
        std::optional<T> storage = std::nullopt;
    public: // 
        uni_dir() {};
        uni_dir(const T& t) : storage(t) {};
        uni_dir(const B& t) : storage(reinterpret_cast<const T&>(t)) {};
        uni_dir(const T* t) : storage(*t) {};
        uni_dir(const B* t) : storage(reinterpret_cast<const T&>(*t)) {};

        uni_dir(const uni_ptr<T>& p) : storage(*p) {}; // UnUsual and Vain
        uni_dir(const uni_ptr<B>& p) : storage(reinterpret_cast<T&>(*p)) {}; // UnUsual and Vain
        uni_dir(const uni_arg<T>& a) : storage(*a) {};
        uni_dir(const uni_arg<B>& a) : storage(reinterpret_cast<T&>(*a)) {};

        //
        virtual inline uni_dir<T, B>& operator= (const T& ptr) { storage = ptr; return *this; };
        virtual inline uni_dir<T, B>& operator= (const T* ptr) { storage = *ptr; return *this; };
        virtual inline uni_dir<T, B>& operator= (uni_arg<T> t) { storage = t.ref(); return *this; };
        virtual inline uni_dir<T, B>& operator= (uni_ptr<T> p) { storage = *p.ptr(); return *this; };

        //
        virtual inline uni_dir<T, B>& operator= (const B& ptr) { storage = reinterpret_cast<const T&>(ptr); return *this; };
        virtual inline uni_dir<T, B>& operator= (const B* ptr) { storage = reinterpret_cast<const T&>(*ptr); return *this; };
        virtual inline uni_dir<T, B>& operator= (uni_arg<B> t) { storage = reinterpret_cast<T&>(t.ref()); return *this; };
        virtual inline uni_dir<T, B>& operator= (uni_ptr<B> p) { storage = reinterpret_cast<T&>(*p.ptr()); return *this; };

        // 
        virtual inline operator T& () { return ref(); };
        virtual inline operator const T& () const { return ref(); };

        // 
        virtual inline operator T* () { return ptr(); };
        virtual inline operator const T* () const { return ptr(); };

        // 
        virtual inline operator B& () { return reinterpret_cast<B&>(ref()); };
        virtual inline operator const B& () const { return reinterpret_cast<const B&>(ref()); };

        // 
        virtual inline operator B* () { return reinterpret_cast<B*>(ptr()); };
        virtual inline operator const B* () const { return reinterpret_cast<const B*>(ptr()); };

        // 
        virtual inline operator uni_ptr<T>() { handle(has()); return *storage; };
        virtual inline operator uni_ptr<const T>() const { handle(has()); return *storage; };

        // 
        virtual inline bool has_value() const { return storage.has_value(); };
        virtual inline bool has() const { return this->has_value(); };

        // 
        virtual inline T* ptr() { handle(has()); return &(*storage); };
        virtual inline const T* ptr() const { handle(has()); return &(*storage); };

        //
        template<class M = T> inline M* ptr() { handle(has()); return &reinterpret_cast<M&>(this->ref()); };
        template<class M = T> inline const M* ptr() const { handle(has()); return &reinterpret_cast<const M&>(this->ref()); };

        // 
        virtual inline T& ref() { handle(has()); return *storage; };
        virtual inline const T& ref() const { handle(has()); return *storage; };

        // 
        virtual inline T* operator->() { return ptr(); };
        virtual inline const T* operator->() const { return ptr(); };

        //
        virtual inline T& operator*() { return ref(); };
        virtual inline const T& operator*() const { return ref(); };
    };

    // aggregate cache
    inline auto* cache = new unsigned char[256u*256u];

    template<class T = uint64_t>
    inline T& aggregate(T str){
        memcpy(cache, &str, sizeof(T));
        return reinterpret_cast<T&>(*cache);
    }

    template<class T = uint64_t>
    inline T& aggregate(T str, T& cache){
        memcpy(cache, &str, sizeof(T));
        return reinterpret_cast<T&>(*cache);
    }

    template<class T = uint32_t>
    inline uint32_t& unlock32(T& cache){
        return reinterpret_cast<uint32_t& >(cache);
    }

    template<class T = uint32_t>
    inline uint32_t& zero32(T& cache){
        return (reinterpret_cast<uint32_t& >(cache) = 0u);
    }

    template<class T = uint64_t>
    inline uint64_t& unlock64(T& cache){
        return reinterpret_cast<uint64_t&>(cache);
    }

    template<class T = uint64_t>
    inline uint64_t& zero64(T& cache){
        return (reinterpret_cast<uint64_t& >(cache) = 0u);
    }


    //
    enum default_t : uint32_t {

    };

    // first are bitfield struct, second are original flags type, third are bits enum
    template<class M, class O, class B = default_t>
    class union_t {
    public:
        // must be union (same value)
        union {
            M bitfield;
            O value = O(0u);
        };

        // construct ops
        union_t() : value(O(0u)) {};
        union_t(const M& bitfield) : bitfield(bitfield) {};
        union_t(const O& value) : value(value) {};
        union_t(const B& bit) : value(bit) {};
        //constexpr union_t() : value(O(0u)) {};
        //constexpr union_t(const M& bitfield) : bitfield(bitfield) {};
        //constexpr union_t(const O& value) : value(value) {};
        //constexpr union_t(const B& bit) : value(bit) {};

        // assign ops
        union_t& operator=(const B& bit) {
            this->value = bit;
            return *this;
        };
        union_t& operator=(const M& bitfield) {
            this->bitfield = bitfield;
            return *this;
        };
        union_t& operator=(const O& value) {
            this->value = value;
            return *this;
        };

        // direct Vulkan API access
        O* operator &() { return &value; };
        const O* operator &() const { return &value; };
        O& operator *() { return value; };
        const O& operator *() const { return value; };

        // pointers
        operator B* () { return &reinterpret_cast<B&>(value); };
        operator const B* () const { return &reinterpret_cast<const B&>(value); };
        operator O* () { return &value; };
        operator const O* () const { return &value; };
        operator M* () { return &bitfield; };
        operator const M* () const { return &bitfield; };

        // reference
        operator B& () { return reinterpret_cast<B&>(value); };
        operator const B& () const { return reinterpret_cast<const B&>(value); };
        operator O& () { return value; };
        operator const O& () const { return value; };
        operator M& () { return bitfield; };
        operator const M& () const { return bitfield; };

        // bitfield access
        M* operator ->() {
            return &bitfield;
        };

        // bitfield copy ops
        union_t operator ~() const {
            return union_t(~value);
        };
        union_t operator |(const B& bit) const {
            return union_t(value | bit);
        };
        union_t operator &(const B& bit) const {
            return union_t(value & bit);
        };
        union_t operator ^(const B& bit) const {
            return union_t(value ^ bit);
        };

        // bitfield assign ops
        union_t& operator |=(const B& bit) {
            value |= bit;
            return *this;
        };
        union_t& operator &=(const B& bit) {
            value &= bit;
            return *this;
        };
        union_t& operator ^=(const B& bit) {
            value ^= bit;
            return *this;
        };
    };


#ifdef VKT_OPENGL_INTEROP
#ifdef VKT_USE_GLFW
    // FOR LWJGL-3 Request!
    inline void initializeGL(GLFWglproc(*glfwGetProcAddress)(const char*)) {
#ifdef VKT_USE_GLAD
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            printf("Something went wrong!\n"); exit(-1);
        }
        printf("OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);
#else
        glbinding::initialize(glfwGetProcAddress, false);
#endif
    };

    // FOR LWJGL-3 Request!
#ifdef VKT_GLFW_LINKED
    inline void initializeGL() {
#ifdef VKT_USE_GLAD
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            printf("Something went wrong!\n"); exit(-1);
        }
        printf("OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);
#else
        glbinding::initialize(glfwGetProcAddress, false);
#endif
    };
#endif
#endif
#endif

};

// 
namespace vkh {
    static inline std::string errorString(VkResult errorCode) {
        switch (errorCode) {
#define STR(r) case VK_ ##r: return #r
            STR(NOT_READY);
            STR(TIMEOUT);
            STR(EVENT_SET);
            STR(EVENT_RESET);
            STR(INCOMPLETE);
            STR(ERROR_OUT_OF_HOST_MEMORY);
            STR(ERROR_OUT_OF_DEVICE_MEMORY);
            STR(ERROR_INITIALIZATION_FAILED);
            STR(ERROR_DEVICE_LOST);
            STR(ERROR_MEMORY_MAP_FAILED);
            STR(ERROR_LAYER_NOT_PRESENT);
            STR(ERROR_EXTENSION_NOT_PRESENT);
            STR(ERROR_FEATURE_NOT_PRESENT);
            STR(ERROR_INCOMPATIBLE_DRIVER);
            STR(ERROR_TOO_MANY_OBJECTS);
            STR(ERROR_FORMAT_NOT_SUPPORTED);
            STR(ERROR_SURFACE_LOST_KHR);
            STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
            STR(SUBOPTIMAL_KHR);
            STR(ERROR_OUT_OF_DATE_KHR);
            STR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
            STR(ERROR_VALIDATION_FAILED_EXT);
            STR(ERROR_INVALID_SHADER_NV);
#undef STR
        default:
            return "UNKNOWN_ERROR";
        }
    };

    static inline decltype(auto) handleVk(vkt::uni_arg<VkResult> result) {
        if (result != VK_SUCCESS) { // TODO: Fix Ubuntu Issue
            //std::cerr << "ERROR: VkResult Error Code: " << std::to_string(result) << " (" << errorString(result) << ")..." << std::endl; throw (*result);

            assert(result == VK_SUCCESS);
#ifdef VKT_GLFW_LINKED
            glfwTerminate();
#endif
            exit(result);
        };
        return result;
    };

    
};


#endif