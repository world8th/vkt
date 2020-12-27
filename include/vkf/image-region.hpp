#pragma once // #

// 
#ifndef VKT_CORE_USE_VMA
#define VKT_CORE_USE_VMA
#endif

//
#ifdef VKT_CORE_USE_VMA
#include "./vma-image-allocation.hpp"
#else
#include "./image-allocation.hpp"
#endif

// 
namespace vkf {

    // 
    struct ImageRegionCreateInfoAllocated {
        vkt::uni_arg<MemoryAllocationInfo> allocationInfo = MemoryAllocationInfo{};
        vkt::uni_arg<vkh::VkImageCreateInfo> createInfo = vkh::VkImageCreateInfo{};
        vkt::uni_arg<vkh::VkImageViewCreateInfo> info = vkh::VkImageViewCreateInfo{};
        vkt::uni_arg<VkImageLayout> layout = VK_IMAGE_LAYOUT_GENERAL;
    };

    // 
    struct ImageRegionCreateInfoVMA {
        vkt::uni_arg<VmaAllocator> allocator = VmaAllocator{};
        vkt::uni_arg<vkh::VkImageCreateInfo> createInfo = vkh::VkImageCreateInfo{};
        vkt::uni_arg<vkh::VkImageViewCreateInfo> info = vkh::VkImageViewCreateInfo{};
        vkt::uni_arg<VkImageLayout> layout = VK_IMAGE_LAYOUT_GENERAL;
    };

    // 
    class ImageRegion : public std::enable_shared_from_this<ImageRegion> { public: 
        ~ImageRegion() {

        };

        ImageRegion() {};
        ImageRegion(const vkt::uni_ptr<ImageRegion>& region) { *this = region; };
        ImageRegion(const vkt::uni_ptr<ImageAllocation>& allocation, vkt::uni_arg<vkh::VkImageViewCreateInfo> info = vkh::VkImageViewCreateInfo{}, vkt::uni_arg<VkImageLayout> layout = VK_IMAGE_LAYOUT_GENERAL) : allocation(allocation), subresourceRange(info->subresourceRange) { this->construct(allocation, info, layout); };
        ImageRegion(const std::shared_ptr<ImageAllocation>& allocation, vkt::uni_arg<vkh::VkImageViewCreateInfo> info = vkh::VkImageViewCreateInfo{}, vkt::uni_arg<VkImageLayout> layout = VK_IMAGE_LAYOUT_GENERAL) : allocation(allocation), subresourceRange(info->subresourceRange) { this->construct(allocation, info, layout); };

#ifdef VKT_CORE_USE_VMA
        ImageRegion(const vkt::uni_ptr<VmaImageAllocation>& allocation, vkt::uni_arg<vkh::VkImageViewCreateInfo> info = vkh::VkImageViewCreateInfo{}, vkt::uni_arg<VkImageLayout> layout = VK_IMAGE_LAYOUT_GENERAL) : allocation(allocation.dyn_cast<ImageAllocation>()), subresourceRange(info->subresourceRange) { this->construct(allocation.dyn_cast<ImageAllocation>(), info, layout); };
        ImageRegion(const std::shared_ptr<VmaImageAllocation>& allocation, vkt::uni_arg<vkh::VkImageViewCreateInfo> info = vkh::VkImageViewCreateInfo{}, vkt::uni_arg<VkImageLayout> layout = VK_IMAGE_LAYOUT_GENERAL) : allocation(std::dynamic_pointer_cast<ImageAllocation>(allocation)), subresourceRange(info->subresourceRange) { this->construct(std::dynamic_pointer_cast<ImageAllocation>(allocation), info, layout); };
#endif

        // 
        virtual ImageRegion* construct(
            const vkt::uni_ptr<ImageAllocation>& allocation,
            vkt::uni_arg<vkh::VkImageViewCreateInfo> info = vkh::VkImageViewCreateInfo{},
            vkt::uni_arg<VkImageLayout> layout = VK_IMAGE_LAYOUT_GENERAL
        ) {
            info->image = allocation->getImage();
            this->allocation = allocation;
            this->subresourceRange = info->subresourceRange;
            this->imgInfo.imageLayout = VkImageLayout(*layout);

            // 
            vkt::handleVk(this->allocation->getInfo().deviceDispatch->CreateImageView(*info, nullptr, &this->imgInfo.imageView));

            // 
#ifdef VKT_OPENGL_INTEROP
            if (!this->allocation->info.glID) {
                VkImageViewHandleInfoNVX handleInfo = {};
                handleInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_HANDLE_INFO_NVX;
                handleInfo.pNext = nullptr;
                handleInfo.imageView = this->imgInfo.imageView;
                handleInfo.sampler = this->imgInfo.sampler;
                handleInfo.descriptorType = this->imgInfo.sampler ? VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER : VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                this->allocation->info.glID = this->allocation->info.deviceDispatch->GetImageViewHandleNVX(&handleInfo);
            };
#endif

            // 
            return this;
        };

        // 
#ifdef VKT_CORE_USE_VMA
        virtual ImageRegion* construct(
            vkt::uni_ptr<VmaImageAllocation> allocation,
            vkt::uni_arg<vkh::VkImageViewCreateInfo> info = vkh::VkImageViewCreateInfo{},
            vkt::uni_arg<VkImageLayout> layout = VK_IMAGE_LAYOUT_GENERAL
        ) {
            this->construct(allocation.dyn_cast<ImageAllocation>(), info, layout);
            return this;
        };
#endif

        // 
        virtual ImageRegion& operator=(vkt::uni_ptr<ImageRegion> region) {
            this->allocation = region->uniPtr();
            this->subresourceRange = region->subresourceRange;
            this->imgInfo = region->imgInfo;
            return *this;
        };

        // alias Of getAllocation
        virtual vkt::uni_ptr<ImageAllocation>& uniPtr() { return this->allocation; };
        virtual const vkt::uni_ptr<ImageAllocation>& uniPtr() const { return this->allocation; };

        // alias Of getAllocation
        virtual vkt::uni_ptr<ImageAllocation>& uniPtrVma() { return this->allocation; };
        virtual const vkt::uni_ptr<ImageAllocation>& uniPtrVma() const { return this->allocation; };

        //virtual vkh::VkImageSubresourceRange& subresourceRange() { return this->subresourceRange; };
        virtual vkh::VkImageSubresourceLayers subresourceLayers(const uint32_t mipLevel = 0u) const { return {
            .aspectMask = this->subresourceRange.aspectMask,
            .mipLevel = this->subresourceRange.baseMipLevel + mipLevel,
            .baseArrayLayer = this->subresourceRange.baseArrayLayer,
            .layerCount = this->subresourceRange.layerCount
        };};

        // set methods for direct control
        virtual ImageRegion& setImageLayout(const VkImageLayout layout = {}) { this->imgInfo.imageLayout = layout; return *this; };
        virtual ImageRegion& setSampler(const VkSampler& sampler = {}) { this->imgInfo.sampler = sampler; return *this; };

        virtual VkSampler& refSampler() { return this->imgInfo.sampler; };
        virtual const VkSampler& refSampler() const { return this->imgInfo.sampler; };

        virtual ImageRegion& transfer(VkCommandBuffer& cmdBuf) const {
            vkt::imageBarrier(cmdBuf, vkt::ImageBarrierInfo{
                .instanceDispatch = this->allocation->getInfo().instanceDispatch,
                .deviceDispatch = this->allocation->getInfo().deviceDispatch,
                .image = this->allocation->getImage(),
                .targetLayout = reinterpret_cast<const VkImageLayout&>(this->imgInfo.imageLayout),
                .originLayout = this->allocation->getInfo().initialLayout,
                .subresourceRange = this->subresourceRange
            });
            const_cast<ImageRegion*>(this)->allocation->getInfo().initialLayout = reinterpret_cast<const VkImageLayout&>(this->imgInfo.imageLayout);
            return const_cast<ImageRegion&>(*this);
        };

        virtual ImageRegion& transfer(VkCommandBuffer& cmdBuf, VkImageLayout dstLayout) {
            this->imgInfo.imageLayout = dstLayout;
            return this->transfer(cmdBuf);
        };

        // 
        virtual operator ImageAllocation*() { return this->allocation; };
        virtual operator vkt::uni_ptr<ImageAllocation>&() { return this->allocation; };
        virtual operator std::shared_ptr<ImageAllocation>& () { return this->allocation; };
        virtual operator vkf::MemoryAllocationInfo&() { return this->allocation->getInfo(); };
        virtual operator vkh::VkImageSubresourceRange&() { return this->subresourceRange; };
        virtual operator    ::VkImageSubresourceRange&() { return this->subresourceRange; };
        virtual operator vkh::VkDescriptorImageInfo&() { return this->imgInfo; };
        virtual operator    ::VkDescriptorImageInfo&() { return this->imgInfo; };
        virtual operator VkImage&() { return *this->allocation; };
        virtual operator VkDevice&() { return *this->allocation; };
        virtual operator VkSampler&() { return this->imgInfo.sampler; };

        // 
        virtual operator const ImageAllocation*() const { return this->allocation; };
        virtual operator const vkt::uni_ptr<ImageAllocation>&() const { return this->allocation; };
        virtual operator const std::shared_ptr<ImageAllocation>& () const { return this->allocation.get_shared(); };
        virtual operator const vkf::MemoryAllocationInfo&() const { return this->allocation->getInfo(); };
        virtual operator const vkh::VkImageSubresourceRange&() const { return this->subresourceRange; };
        virtual operator const    ::VkImageSubresourceRange&() const { return this->subresourceRange; };
        virtual operator const vkh::VkDescriptorImageInfo&() const { return this->imgInfo; };
        virtual operator const    ::VkDescriptorImageInfo&() const { return this->imgInfo; };
        virtual operator const VkImage&() const { return *this->allocation; };
        virtual operator const VkDevice&() const { return *this->allocation; };
        virtual operator const VkImageView&() const { return this->imgInfo.imageView; };
        virtual operator const VkImageLayout&() const { return this->imgInfo.imageLayout; };
        virtual operator const VkSampler&() const { return this->imgInfo.sampler; };

        // 
        //virtual operator const vkh::VkImageSubresourceLayers& () const { return this->subresourceLayers(); };
        //virtual operator const    ::VkImageSubresourceLayers& () const { return this->subresourceLayers(); };

        // 
        //virtual operator const VkImageSubresourceLayers() const { return VkImageSubresourceLayers{ reinterpret_cast<const VkImageAspectFlags&>(subresourceRange.aspectMask), subresourceRange.baseMipLevel, subresourceRange.baseArrayLayer, subresourceRange.layerCount }; };

#ifdef VKT_OPENGL_INTEROP // Bindless Textures Directly
        virtual uint64_t deviceAddress () { 
            if (this->getGL()) {
                return glGetTextureHandleARB(this->getGL());
            };
            return 0;
        };
        virtual const uint64_t deviceAddress() const { 
            if (this->getGL()) {
                return glGetTextureHandleARB(this->getGL());
            };
            return 0;
        };
        virtual uint64_t deviceAddress(GLuint sampler) { 
            return glGetTextureSamplerHandleARB(this->getGL(), sampler); 
        };
        virtual const uint64_t deviceAddress(GLuint sampler) const { 
            return glGetTextureSamplerHandleARB(this->getGL(), sampler); 
        };
#else   // Get By Vulkan API Directly
#ifdef VKT_NVX_IMAGE_ADDRESS
        virtual uint64_t deviceAddress() { return this->allocation->getDevice().getImageViewAddressNVX(this->getImageView(), this->getAllocation()->dispatchLoaderDynamic()).deviceAddress; };
        virtual const uint64_t deviceAddress() const { return this->allocation->getDevice().getImageViewAddressNVX(this->getImageView(), this->getAllocation()->dispatchLoaderDynamic()).deviceAddress; };
#endif
#endif

        // 
        virtual VkImageView& getImageView() { return reinterpret_cast<VkImageView&>(this->imgInfo.imageView); };
        virtual VkImageLayout& getImageLayout() { return reinterpret_cast<VkImageLayout&>(this->imgInfo.imageLayout); };
        virtual VkSampler& getSampler() { return reinterpret_cast<VkSampler&>(this->imgInfo.sampler); };
        virtual vkh::VkImageSubresourceRange& getImageSubresourceRange() { return this->subresourceRange; };

        // 
        virtual const VkImageView& getImageView() const { return reinterpret_cast<const VkImageView&>(this->imgInfo.imageView); };
        virtual const VkImageLayout& getImageLayout() const { return reinterpret_cast<const VkImageLayout&>(this->imgInfo.imageLayout); };
        virtual const VkSampler& getSampler() const { return reinterpret_cast<const VkSampler&>(this->imgInfo.sampler); };
        virtual const vkh::VkImageSubresourceRange& getImageSubresourceRange() const { return this->subresourceRange; };

        // ALIASES
        //virtual vkh::VkImageSubresourceLayers getImageSubresourceLayers() const { return this->subresourceLayers(); };

        // 
        virtual unsigned& getGL() {
            VkImageViewHandleInfoNVX handleInfo = {};
            handleInfo.imageView = this->imgInfo.imageView;
            handleInfo.sampler = this->imgInfo.sampler;
            handleInfo.descriptorType = this->imgInfo.sampler ? VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER : VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            if (!this->getGLImage()) {
                //this->getGLImage() = this->allocation->getDevice().getImageViewHandleNVX(&handleInfo, this->allocation->dispatchLoaderDynamic());
            };
            return this->getGLImage();
        };

        // 
        virtual const unsigned& getGL() const {
            VkImageViewHandleInfoNVX handleInfo = {};
            handleInfo.imageView = this->imgInfo.imageView;
            handleInfo.sampler = this->imgInfo.sampler;
            handleInfo.descriptorType = this->imgInfo.sampler ? VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER : VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            if (!this->getGLImage()) {
                //return this->allocation->getDevice().getImageViewHandleNVX(&handleInfo, this->allocation->dispatchLoaderDynamic());
            };
            return this->getGLImage();
        };

        // 
        virtual const unsigned& getGLImage() const { return this->allocation->getGLImage(); };
        virtual const unsigned& getGLMemory() const { return this->allocation->getGLMemory(); };

        // 
        virtual unsigned& getGLImage() { return this->allocation->getGLImage(); };
        virtual unsigned& getGLMemory() { return this->allocation->getGLMemory(); };

        // 
        virtual VkDevice& getDevice() { return reinterpret_cast<VkDevice&>(allocation->getDevice()); };
        virtual VkImage& getImage() { return reinterpret_cast<VkImage&>(allocation->getImage()); };

        // 
        virtual const VkDevice& getDevice() const { return reinterpret_cast<const VkDevice&>(allocation->getDevice()); };
        virtual const VkImage& getImage() const { return reinterpret_cast<const VkImage&>(allocation->getImage()); };

        // 
        virtual VkDevice& deviceHandle() { return reinterpret_cast<VkDevice&>(allocation->getDevice()); };
        virtual VkImage& imageHandle() { return reinterpret_cast<VkImage&>(allocation->getImage()); };

        // 
        virtual const VkDevice& deviceHandle() const { return reinterpret_cast<const VkDevice&>(allocation->getDevice()); };
        virtual const VkImage& imageHandle() const { return reinterpret_cast<const VkImage&>(allocation->getImage()); };

        // 
        virtual std::vector<uint32_t>& getQueueFamilyIndices() { return this->allocation->getQueueFamilyIndices(); };
        virtual const std::vector<uint32_t>& getQueueFamilyIndices() const { return this->allocation->getQueueFamilyIndices(); };

        // 
        virtual ImageAllocation* operator->() { return &(*allocation); };
        virtual ImageAllocation& operator*() { return (*allocation); };
        virtual const ImageAllocation* operator->() const { return &(*allocation); };
        virtual const ImageAllocation& operator*() const { return (*allocation); };

        // 
        virtual bool has() const { return allocation ? true : false; };
        virtual bool has_value() const { return this->has(); };

        // 
        virtual ImageRegion* address() { return this; };
        virtual const ImageRegion* address() const { return this; };

        // alias of uniPtr
        virtual vkt::uni_ptr<ImageAllocation>& getAllocation() { return allocation; };
        virtual const vkt::uni_ptr<ImageAllocation>& getAllocation() const { return allocation; };

        // for JavaCPP
        virtual ImageAllocation* getAllocationPtr() { return allocation.ptr(); };
        virtual const ImageAllocation* getAllocationPtr() const { return allocation.ptr(); };

        // 
        virtual vkh::VkDescriptorImageInfo& getDescriptor() { return imgInfo; };
        virtual const vkh::VkDescriptorImageInfo& getDescriptor() const { return imgInfo; };

        // 
        virtual ::VkDescriptorImageInfo* getDescriptorPtr() { return &imgInfo; };
        virtual const ::VkDescriptorImageInfo* getDescriptorPtr() const { return &imgInfo; };

        // 
        virtual vkh::VkImageCreateInfo& getCreateInfo() { return allocation->getCreateInfo(); };
        virtual const vkh::VkImageCreateInfo& getCreateInfo() const { return allocation->getCreateInfo(); };

        //
        virtual vkf::MemoryAllocationInfo& getAllocationInfo() { return allocation->getInfo(); };
        virtual const vkf::MemoryAllocationInfo& getAllocationInfo() const { return allocation->getInfo(); };

        //
    protected: friend VmaImageAllocation; friend ImageAllocation; // 
        vkh::VkDescriptorImageInfo imgInfo = { .sampler = VK_NULL_HANDLE, .imageView = VK_NULL_HANDLE, .imageLayout = VK_IMAGE_LAYOUT_UNDEFINED };
        vkt::uni_ptr<ImageAllocation> allocation = {};

    public: // irrevalent sources
        vkh::VkImageSubresourceRange subresourceRange = {};
    };

};
