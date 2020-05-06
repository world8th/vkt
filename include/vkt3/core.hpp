#pragma once // #

// Resolve conflict with another `core.hpp`
#ifndef VKT_CORE_HPP
#define VKT_CORE_HPP

//#include "utils.hpp"
//#include "structs.hpp"
#include <memory>
#include <optional>
#include <iostream>

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

    void handle(const bool& valid = false) {
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
        uni_ptr<T>() {};
        uni_ptr<T>(std::shared_ptr<T> shared) : shared(shared) {};
        uni_ptr<T>(T* ptr) : regular(std::ref(*ptr)){};
        uni_ptr<T>(T& ptr) : regular(std::ref(ptr)){};  // for argument passing
        uni_ptr<T>(const uni_ptr<T>& ptr) : shared(ptr.shared), regular(std::ref(*ptr.regular)) {};
        uni_ptr<T>(uni_ptr<T>& ptr) : shared(ptr.shared), regular(std::ref(*ptr.regular)){};

        // 
        virtual uni_ptr* operator= (T* ptr) { regular = std::ref(*ptr); return this; };
        virtual uni_ptr* operator= (T& ptr) { regular = std::ref( ptr); return this; }; // for argument passing
        virtual uni_ptr* operator= (std::shared_ptr<T> ptr) { shared = ptr; return this; };
        virtual uni_ptr* operator= (const uni_ptr<T>& ptr) { 
            T& ref = *ptr.regular;
            shared = ptr.shared, regular = std::ref(ref);
            return this;
        };
        virtual uni_ptr* operator= (uni_ptr<T>& ptr) { 
            T& ref = *ptr.regular;
            shared = ptr.shared, regular = std::ref(ref);
            return this;
        };

        // 
        template<class M = T>
        uni_ptr<M> dyn_cast() const { T& r = *regular; return shared ? uni_ptr<M>(std::dynamic_pointer_cast<M>(shared)) : uni_ptr<M>(dynamic_cast<M&>(r)); };

        // 
        //template<class... A>
        //uni_ptr<T>(A... args) : shared(std::make_shared<T>(args...)) {};

        // 
        virtual std::shared_ptr<T>& get_shared() { return (this->shared = (this->shared ? this->shared : std::shared_ptr<T>(get_ptr()))); };
        virtual std::shared_ptr<T>  get_shared() const { return (this->shared ? this->shared : std::shared_ptr<T>(const_cast<T*>(get_ptr()))); };

        // 
        virtual T* get_ptr() { 
            T& r = *regular;
            return (shared ? &(*shared) : &r);
        };

        // 
        virtual const T* get_ptr() const {
            const T& r = *regular;
            return (shared ? &(*shared) : &r);
        };

        // 
        virtual bool has() { return regular && shared; };
        virtual bool has() const { return regular && shared; };

        // 
        virtual T* ptr() { return get_ptr(); };
        virtual const T* ptr() const { return get_ptr(); };

        // 
        virtual T& ref() { T& r = *regular, s = *shared; return regular ? r : s; };
        virtual const T& ref() const { const T& r = *regular, s = *shared; return regular ? r : s; };

        // experimental
        virtual operator T& () { return ref(); };
        virtual operator const T& () const { return ref(); };

        // 
        virtual operator T* () { return ptr(); };
        virtual operator const T* () const { return ptr(); };

        // 
        virtual operator std::shared_ptr<T>& () { return get_shared(); };
        virtual operator std::shared_ptr<T>  () const { return get_shared(); };

        // 
        virtual T* operator->() { return get_ptr(); };
        virtual const T* operator->() const { return get_ptr(); };

        //
        virtual T& operator*() { return *get_ptr(); };
        virtual const T& operator*() const { return *get_ptr(); };
    };

    template<class T = uint8_t>
    class uni_arg {
    protected: std::optional<T> storage;//= { T{} };//std::nullopt;
    public: // 
        uni_arg<T>() {};
        uni_arg<T>(const T& t) : storage(t) {};
        uni_arg<T>(const T* t) : storage(*t) {};
        uni_arg<T>(uni_ptr<T> p) : storage(*p) {}; // UnUsual and Vain
        uni_arg<T>(const uni_arg<T>& a) : storage(*a) {};

        // 
        virtual uni_arg<T>& operator= (const T& ptr) { storage =  ptr; return *this; };
        virtual uni_arg<T>& operator= (const T* ptr) { storage = *ptr; return *this; };
        virtual uni_arg<T>& operator= (uni_arg<T> t) { storage = t.ref(); return *this; };
        virtual uni_arg<T>& operator= (uni_ptr<T> p) { storage = p.ref(); return *this; };

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
        virtual T& ref() { handle(has()); return *storage; };
        virtual const T& ref() const { handle(has()); return *storage; };

        // 
        virtual T* operator->() { return ptr(); };
        virtual const T* operator->() const { return ptr(); };

        //
        virtual T& operator*() { return ref(); };
        virtual const T& operator*() const { return ref(); };
    };

#ifdef ENABLE_OPENGL_INTEROP
    // FOR LWJGL-3 Request!
    void initializeGL(GLFWglproc(*glfwGetProcAddress)(const char*)) {
        glbinding::initialize(glfwGetProcAddress);
    };

    // FOR LWJGL-3 Request!
    void initializeGL() {
        glbinding::initialize(glfwGetProcAddress);
    };
#endif

};

#endif
