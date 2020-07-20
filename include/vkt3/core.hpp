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
#include <iostream>
#include <vector>

//
#include <misc/soa_vector.hpp>
#include <misc/half.hpp>

// 
#ifndef NSM
#define NSM api
#endif

// Currently Windows Only Supported
#if (defined(_WIN32) || defined(__MINGW32__) || defined(_MSC_VER_) || defined(__MINGW64__) || defined(WIN32) || defined(__WIN32) && !defined(__CYGWIN__))
#define VKT_WIN32_DETECTED
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
#if defined(VKT_ENABLE_GLFW_SUPPORT) || defined(ENABLE_OPENGL_INTEROP)
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#endif

// 
#ifdef ENABLE_OPENGL_INTEROP
#ifdef VKT_USE_GLAD
#include <glad/glad.h>
#else
#include <glbinding/gl/gl.h>
#include <glbinding/gl/extension.h>
#include <glbinding/glbinding.h>
#endif
#endif

// Enable Vulkan-HPP when defined
#ifdef VULKAN_HPP
#define ENABLE_VULKAN_HPP
#endif

//
#include <vulkan/vulkan.h>

// When enabled, use Vulkan-HPP support...
#ifdef ENABLE_VULKAN_HPP
#define VULKAN_HPP_ENABLE_DYNAMIC_LOADER_TOOL 0 // Avoid XVK conflict
#include <vulkan/vulkan.hpp>
#endif

// 
#ifdef VKT_CORE_ENABLE_XVK
#include <xvk/xvk.hpp>
#endif

// 
#ifdef VKT_CORE_ENABLE_VMA
#include <vma/vk_mem_alloc.h>
#endif

#define ASSIGN(obj,name) obj.name=name;

namespace vkt {

    //
    template<class T, class U = uint32_t> class bit_ops { protected: U n = 0u; //friend BitOps<T,U>;
        public: bit_ops<T,U>(const T& v = 0u) : n((U&)v){};
        public: bit_ops<T,U>(const U& n = 0u) : n(n){};
        public: bit_ops<T,U>(T&& v = 0u) : n((U&)v){};
        public: bit_ops<T,U>(U&& n = 0u) : n(n){};
        public: operator U&() { return n; };
        public: operator const U&() const { return n; };
        public: T operator &(const T& o){ return T{n&(U&)o}; };
        public: T operator |(const T& o){ return T{n|(U&)o}; };
        public: T operator ^(const T& o){ return T{n^(U&)o}; };
    };

    // 
    template<class S, class B, class U = uint32_t> class flags { protected: U n = 0u;
        public: flags<S,B,U>(const U& f = 0u) { *(U*)(this) = f; };
        public: flags<S,B,U>(const S& f = 0u) { *(U*)(this) = (U&)f; };
        public: flags<S,B,U>(const B& f = 0u) { *(U*)(this) = (U&)f; };
        public: operator U&() { return *(U*)this; };
        public: operator const U&() const { return *(U*)this; };
        public: operator B&() { return *(B*)this; };
        public: operator const B&() const { return *(B*)this; };
    };

    // boolean 32-bit capable for C++
    class bool32_t { // TODO: support operators
    protected: union {
        uint32_t b_ : 1; bool bb = false;
    };
    public: friend bool32_t;
        constexpr bool32_t(): b_(0u) {};
        bool32_t(const bool&a=false): b_(a?1u:0u) {};
        bool32_t(const uint32_t&a): b_(a&1u) {}; // make bitmasked
        bool32_t(const bool32_t&a): b_(a) {};

        // type conversion operators
        virtual operator bool() const {return bool(b_&1u);};
        virtual operator uint32_t() const {return (b_&1u);};

        // 
        virtual bool32_t& operator=(const bool&a){b_=(a?1u:0u);};
        virtual bool32_t& operator=(const uint32_t&a){b_=a&1u;};
        virtual bool32_t& operator=(const bool32_t&a){b_=a;};
    };

    inline void handle(const bool& valid = false) {
        if (!valid) {
            std::cerr << "std::optional is wrong or not initialized" << std::endl; assert(valid);
        };
    };

    template<class T = uint8_t>
    class uni_ptr {
    protected: //using T = uint8_t;
        std::optional<std::reference_wrapper<T>> regular = std::nullopt;
        std::shared_ptr<T> shared = {};
        //T storage = {};

    public: friend uni_ptr<T>; // 
        uni_ptr() {};
        uni_ptr(const uni_ptr<T>& ptr) : shared(ptr.shared), regular(std::ref(*ptr.regular)) {};
        uni_ptr(const std::shared_ptr<T>& shared) : shared(shared), regular(std::ref(*shared)) {};
        uni_ptr(T* ptr) : regular(std::ref(*ptr)) {};
        uni_ptr(T& ptr) : regular(std::ref(ptr)) {};  // for argument passing
        

        // 
        virtual inline uni_ptr* operator= (T* ptr) { regular = std::ref(*ptr); return this; };
        virtual inline uni_ptr* operator= (T& ptr) { regular = std::ref( ptr); return this; }; // for argument passing
        virtual inline uni_ptr* operator= (std::shared_ptr<T> ptr) { shared = ptr, regular = std::ref(*ptr); return this; };
        virtual inline uni_ptr* operator= (const uni_ptr<T>& ptr) { 
            T& ref = *ptr.regular;
            shared = ptr.shared, regular = std::ref(ref);
            return this;
        };
        virtual inline uni_ptr* operator= (uni_ptr<T>& ptr) { 
            T& ref = *ptr.regular;
            shared = ptr.shared, regular = std::ref(ref);
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
        virtual inline T* get_ptr() { 
            T& r = *regular;
            return (shared ? &(*shared) : &r);
        };

        // 
        virtual inline const T* get_ptr() const {
            const T& r = *regular;
            return (shared ? &(*shared) : &r);
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
        virtual uni_arg<T>& operator= (const T& ptr) { storage = ptr; return *this; };
        virtual uni_arg<T>& operator= (const T* ptr) { storage = *ptr; return *this; };
        virtual uni_arg<T>& operator= (uni_arg<T> t) { storage = t.ref(); return *this; };
        virtual uni_arg<T>& operator= (uni_ptr<T> p) { storage = *p.ptr(); return *this; };

        // experimental
        virtual operator T& () { return ref(); };
        virtual operator const T& () const { return ref(); };

        // 
        virtual operator T* () { return ptr(); };
        virtual operator const T* () const { return ptr(); };

        // 
        virtual operator uni_ptr<T>() { handle(has()); return *storage; };
        virtual operator uni_ptr<const T>() const { handle(has()); return *storage; };

        // 
        virtual bool has_value() const { return storage.has_value(); };
        virtual bool has() const { return this->has_value(); };

        // 
        virtual T* ptr() { handle(has()); return &(*storage); };
        virtual const T* ptr() const { handle(has()); return &(*storage); };

        //
        template<class M = T> inline M* ptr() { handle(has()); return &reinterpret_cast<M&>(this->ref()); };
        template<class M = T> inline const M* ptr() const { handle(has()); return &reinterpret_cast<const M&>(this->ref()); };

        // 
        virtual T& ref() { handle(has()); return *storage; };
        virtual const T& ref() const { handle(has()); return *storage; };

        // 
        virtual T* operator->() { return ptr(); };
        virtual const T* operator->() const { return ptr(); };

        //
        virtual T& operator*() { return ref(); };
        virtual const T& operator*() const { return ref(); };
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
        virtual uni_dir<T, B>& operator= (const T& ptr) { storage = ptr; return *this; };
        virtual uni_dir<T, B>& operator= (const T* ptr) { storage = *ptr; return *this; };
        virtual uni_dir<T, B>& operator= (uni_arg<T> t) { storage = t.ref(); return *this; };
        virtual uni_dir<T, B>& operator= (uni_ptr<T> p) { storage = *p.ptr(); return *this; };

        //
        virtual uni_dir<T, B>& operator= (const B& ptr) { storage = reinterpret_cast<const T&>(ptr); return *this; };
        virtual uni_dir<T, B>& operator= (const B* ptr) { storage = reinterpret_cast<const T&>(*ptr); return *this; };
        virtual uni_dir<T, B>& operator= (uni_arg<B> t) { storage = reinterpret_cast<T&>(t.ref()); return *this; };
        virtual uni_dir<T, B>& operator= (uni_ptr<B> p) { storage = reinterpret_cast<T&>(*p.ptr()); return *this; };

        // 
        virtual operator T& () { return ref(); };
        virtual operator const T& () const { return ref(); };

        // 
        virtual operator T* () { return ptr(); };
        virtual operator const T* () const { return ptr(); };

        // 
        virtual operator B& () { return reinterpret_cast<B&>(ref()); };
        virtual operator const B& () const { return reinterpret_cast<const B&>(ref()); };

        // 
        virtual operator B* () { return reinterpret_cast<B*>(ptr()); };
        virtual operator const B* () const { return reinterpret_cast<const B*>(ptr()); };

        // 
        virtual operator uni_ptr<T>() { handle(has()); return *storage; };
        virtual operator uni_ptr<const T>() const { handle(has()); return *storage; };

        // 
        virtual bool has_value() const { return storage.has_value(); };
        virtual bool has() const { return this->has_value(); };

        // 
        virtual T* ptr() { handle(has()); return &(*storage); };
        virtual const T* ptr() const { handle(has()); return &(*storage); };

        //
        template<class M = T> inline M* ptr() { handle(has()); return &reinterpret_cast<M&>(this->ref()); };
        template<class M = T> inline const M* ptr() const { handle(has()); return &reinterpret_cast<const M&>(this->ref()); };

        // 
        virtual T& ref() { handle(has()); return *storage; };
        virtual const T& ref() const { handle(has()); return *storage; };

        // 
        virtual T* operator->() { return ptr(); };
        virtual const T* operator->() const { return ptr(); };

        //
        virtual T& operator*() { return ref(); };
        virtual const T& operator*() const { return ref(); };
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


#ifdef ENABLE_OPENGL_INTEROP
    // FOR LWJGL-3 Request!
    inline void initializeGL(GLFWglproc(*glfwGetProcAddress)(const char*)) {
#ifdef VKT_USE_GLAD
        if (!gladLoadGLLoader(glfwGetProcAddress)) {
            printf("Something went wrong!\n"); exit(-1);
        }
        printf("OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);
#else
        glbinding::initialize(glfwGetProcAddress, false);
#endif
    };


    // FOR LWJGL-3 Request!
#ifdef VKT_ENABLE_GLFW_LINKED
    inline void initializeGL() {
#ifdef VKT_USE_GLAD
        if (!gladLoadGLLoader(glfwGetProcAddress)) {
            printf("Something went wrong!\n"); exit(-1);
        }
        printf("OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);
#else
        glbinding::initialize(glfwGetProcAddress, false);
#endif
    };
#endif
#endif


};

#endif
