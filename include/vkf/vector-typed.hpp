#pragma once // #

//
#ifndef VKT_CORE_USE_VMA
#define VKT_CORE_USE_VMA
#endif

// 
#include <vkt/core.hpp>
#include <vkt/inline.hpp>

// 
#include "./vector-base.hpp"

// 
namespace vkf {
    
    // Wrapper Class
    template<class M>
    class Vector : public VectorBase //: public std::enable_shared_from_this<Vector<T>> 
    { public: using T = M; //
        Vector(): VectorBase() {};

        // 
        Vector(const vkt::uni_ptr<BufferAllocation>& allocation, vkt::uni_arg<VkDeviceSize> offset = VkDeviceSize(0ull), vkt::uni_arg<VkDeviceSize> size = VK_WHOLE_SIZE, vkt::uni_arg<VkDeviceSize> stride = sizeof(T)) : VectorBase(allocation, offset, size, stride) {};
        Vector(const std::shared_ptr<BufferAllocation>& allocation, vkt::uni_arg<VkDeviceSize> offset = VkDeviceSize(0ull), vkt::uni_arg<VkDeviceSize> size = VK_WHOLE_SIZE, vkt::uni_arg<VkDeviceSize> stride = sizeof(T)) : VectorBase(allocation, offset, size, stride) {};
        
        //
#ifdef VKT_CORE_USE_VMA
        Vector(const vkt::uni_ptr<VmaBufferAllocation>& allocation, vkt::uni_arg<VkDeviceSize> offset = VkDeviceSize(0ull), vkt::uni_arg<VkDeviceSize> size = VK_WHOLE_SIZE, vkt::uni_arg<VkDeviceSize> stride = sizeof(T)) : VectorBase(allocation, offset, size, stride) {};
        Vector(const std::shared_ptr<VmaBufferAllocation>& allocation, vkt::uni_arg<VkDeviceSize> offset = VkDeviceSize(0ull), vkt::uni_arg<VkDeviceSize> size = VK_WHOLE_SIZE, vkt::uni_arg<VkDeviceSize> stride = sizeof(T)) : VectorBase(allocation, offset, size, stride) {};
#endif

        // 
        ~Vector() {
            if (this->view) {
                this->allocation->info.deviceDispatch->DestroyBufferView(this->view, nullptr);
                this->view = VkBufferView{};
            };
        };

        // 
        template<class Tm = T> Vector(const vkt::uni_arg<Vector<Tm>>& V)  { *this = V; };
        template<class Tm = T> inline Vector<T>& operator=(const vkt::uni_arg<Vector<Tm>>& V) {
            this->allocation = V.uniPtr();
            this->bufInfo = vkh::VkDescriptorBufferInfo{ static_cast<VkBuffer>(V.buffer()), V.offset(), V.ranged() };
            this->bufRegion = vkh::VkStridedDeviceAddressRegionKHR{ V.deviceAddress(), V.stride(), (V.ranged() / V.stride())*V.stride() };
            this->bufInfo.range = this->bufRegion.size;
            return *this;
        };

        // typed casting 
        template<class Tm = T> inline Vector<Tm>& cast() { return reinterpret_cast<Vector<Tm>&>(*this); };
        template<class Tm = T> inline const Vector<Tm>& cast() const { return reinterpret_cast<const Vector<Tm>&>(*this); };

        // 
        //template<class Tm = T> Vector<Tm> cast() { return *this; };
        //template<class Tm = T> const vkt::uni_arg<Vector<Tm>>& cast() const { return Vector<Tm>(reinterpret_cast<Vector<T>&>(*this)); };

        // align by typed stride
        virtual Vector<T>& trim() { this->bufRegion.stride = sizeof(T); return *this; };

        // 
        virtual Vector<T>* address() { return this; };
        virtual const Vector<T>* address() const { return this; };

        // at function 
        virtual const T& at(const uintptr_t& i = 0u) const { return *mapped(i); };
        virtual T& at(const uintptr_t& i = 0u) { return *mapped(i); };

        // array operator 
        virtual const T& operator [] (const uintptr_t& i) const { return at(i); };
        virtual T& operator [] (const uintptr_t& i) { return at(i); };

        // begin ptr
        virtual const T* begin() const { return data(); };
        virtual T* const begin() { return data(); };

        // end ptr
        virtual const T* end() const { return reinterpret_cast<const T*>((const void*)&at(size() - 1ul)); };
        virtual T* end() { return reinterpret_cast<T*>((void*)&at(size() - 1ul)); };

        // 
        virtual const T* map(const uintptr_t& i = 0u) const { const_cast<Vector<T>*>(this)->pMapped = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(this->allocation->mapped()) + offset()); return reinterpret_cast<const T*>((const void*)&(reinterpret_cast<const T*>(this->pMapped))[i]); };
        virtual T* map(const uintptr_t& i = 0u) { this->pMapped = reinterpret_cast<uint8_t*>(this->allocation->mapped()) + offset(); return reinterpret_cast<T*>((void*)&(reinterpret_cast<T*>(this->pMapped))[i]); };

        // 
        virtual const T* mapped(const uintptr_t& i = 0u) const { const_cast<Vector<T>*>(this)->pMapped = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(this->allocation->mapped()) + offset()); return reinterpret_cast<const T*>((const void*)&(reinterpret_cast<const T*>((const void*)this->pMapped))[i]); };
        virtual T* mapped(const uintptr_t& i = 0u) { this->pMapped = reinterpret_cast<uint8_t*>(this->allocation->mapped()) + offset(); return reinterpret_cast<T*>((void*)&reinterpret_cast<T*>((void*)this->pMapped)[i]); };

        // 
        virtual const T* data(const uintptr_t& i = 0u) const { return mapped(i); };
        virtual T* data(const uintptr_t& i = 0u) { return mapped(i); };
    };

#ifdef VKT_CORE_USE_VMA
    template<class T = uint8_t>
    Vector<T>* MakeVmaVector(vkt::uni_ptr<VmaBufferAllocation> allocation, vkt::uni_arg<VkDeviceSize> offset = VkDeviceSize(0ull), vkt::uni_arg<VkDeviceSize> size = VK_WHOLE_SIZE, vkt::uni_arg<VkDeviceSize> stride = sizeof(T)) { return new Vector<T>(allocation, offset, size, stride); };
#endif

};
