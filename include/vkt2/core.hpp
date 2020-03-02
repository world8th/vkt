#pragma once // #

#include "utils.hpp"
#include "structs.hpp"
#include <memory>

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
    protected: uint32_t b_:1;
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


    template<class T = uint8_t>
    class uni_arg {
    protected:
        T storage = {};
    public:
        uni_arg<T>() {};
        uni_arg<T>(const T& t) : storage(t) {};
        uni_arg<T>(const T* t) : storage(*t) {};

        // 
        virtual uni_arg* operator= (const T& ptr) { storage = ptr; return this; };
        virtual uni_arg* operator= (const T* ptr) { storage = *ptr; return this; };

        // experimental
        virtual operator T& () { return storage; };
        virtual operator const T& () const { return storage; };

        // 
        virtual operator T* () { return &storage; };
        virtual operator const T* () const { return &storage; };

        // 
        virtual T* operator->() { return &storage; };
        virtual const T* operator->() const { return &storage; };

        //
        virtual T& operator*() { return storage; };
        virtual const T& operator*() const { return storage; };
    };


    template<class T = uint8_t>
    class uni_ptr {
    protected: //using T = uint8_t;
        std::shared_ptr<T> shared = {};
        T* regular = nullptr;
        //T storage = {};

    public: // 
        uni_ptr<T>() {};
        uni_ptr<T>(const std::shared_ptr<T>& shared) { *this = shared; };
        uni_ptr<T>(T* regular) { *this = regular; };
        uni_ptr<T>(T& regular) { *this = regular; }; // for argument passing
        //uni_ptr<T>(const T* p) { *this = p; };
        //uni_ptr<T>(const T& p) { *this = p; };
        //uni_ptr<T>(T&& p) { *this = p; };
        //uni_ptr<T>(T p) : storage(p), regular(&storage) {};

        // 
        virtual std::shared_ptr<T>& get_shared() { return (this->shared = (this->shared ? this->shared : std::shared_ptr<T>(this->regular))); };
        virtual const std::shared_ptr<T>& get_shared() const { return (this->shared ? this->shared : std::shared_ptr<T>(this->regular)); };

        // 
        virtual T* get_ptr() { return (regular ? regular : &(*shared)); };
        virtual const T* get_ptr() const { return (regular ? regular : &(*shared)); };

        // experimental
        virtual operator T& () { return *get_ptr(); };
        virtual operator const T& () const { return *get_ptr(); };

        // 
        virtual operator T* () { return get_ptr(); };
        virtual operator const T* () const { return get_ptr(); };

        // 
        virtual operator std::shared_ptr<T>& () { return get_shared(); };
        virtual operator const std::shared_ptr<T>& () const { return get_shared(); };

        /* //
        template<class M = T>
        inline uni_ptr* operator=(const uni_ptr<M>& ptr) {
            this->storage = ptr;
            this->regular = ptr;
            this->shared = ptr;
        };

        //
        template<class M = T>
        inline uni_ptr* set(const uni_ptr<M>& ptr) {
            this->storage = ptr;
            this->regular = ptr;
            this->shared = ptr;
        };*/

        virtual uni_ptr* operator= (T* ptr) { regular = ptr; return this; };
        virtual uni_ptr* operator= (T& ptr) { regular = &ptr; return this; }; // for argument passing

        //virtual uni_ptr* operator= (const T* p) { storage = *p; regular = &storage; return this; };
        //virtual uni_ptr* operator= (const T& p) { storage = p; regular = &storage; return this; };
        //virtual uni_ptr* operator= (T p) { storage = p; regular = &storage; return this; };
        //virtual uni_ptr* operator= (T&& p) { storage = std::move(p); regular = &storage; return this; };

        virtual uni_ptr* operator= (const std::shared_ptr<T>& ptr) { shared = ptr; return this; };

        // 
        virtual T* operator->() { return get_ptr(); };
        virtual const T* operator->() const { return get_ptr(); };

        //
        virtual T& operator*() { return *get_ptr(); };
        virtual const T& operator*() const { return *get_ptr(); };
    };

};
