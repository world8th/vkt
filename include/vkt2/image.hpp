#pragma once // #

#ifdef VKT_FORCE_VMA_IMPLEMENTATION
#ifndef VMA_IMPLEMENTATION
#define VMA_IMPLEMENTATION
#endif
#endif

#include "utils.hpp"
#include "structs.hpp"
#include "core.hpp"
#include <memory>

namespace vkt {

#ifdef ENABLE_OPENGL_INTEROP
    using namespace gl;
#endif

    // 
    class ImageRegion;
    class VmaImageAllocation;
    class ImageAllocation : public std::enable_shared_from_this<ImageAllocation> { public: 
        ImageAllocation() {};
        ImageAllocation(vkt::uni_arg<MemoryAllocationInfo> allocationInfo, vkt::uni_arg<vkh::VkImageCreateInfo> createInfo = vkh::VkImageCreateInfo{}) : info( allocationInfo) { this->construct( allocationInfo,  createInfo); }
        ImageAllocation(vkt::uni_ptr<ImageAllocation> allocation) : image(allocation->image), info(allocation->info) { *this = allocation; };
        ImageAllocation(std::shared_ptr<ImageAllocation> allocation) : image(allocation->image), info(allocation->info) { *this = vkt::uni_ptr<ImageAllocation>(allocation); };
        ~ImageAllocation() {
            if (this->image && this->info.device) {
                //this->info.device.waitIdle();
                //this->info.device.destroyImage(this->image);
                //this->info.device.freeMemory(this->info.memory);
                //this->image = vk::Image{};
            };
        };

        // 
        virtual ImageAllocation* construct(
            vkt::uni_arg<MemoryAllocationInfo> allocationInfo,
            vkt::uni_arg<vkh::VkImageCreateInfo> createInfo = vkh::VkImageCreateInfo{}
        ) {
            vkh::VkImageUsageFlags usage = createInfo->usage;
            if (allocationInfo->memUsage == VMA_MEMORY_USAGE_CPU_TO_GPU) { usage.eTransferSrc = 1; };
            if (allocationInfo->memUsage == VMA_MEMORY_USAGE_GPU_TO_CPU) { usage.eTransferDst = 1; };
            if (allocationInfo->memUsage == VMA_MEMORY_USAGE_GPU_ONLY) { usage.eTransferDst = 1, usage.eTransferSrc = 1; };

            // 
            this->image = this->info.device.createImage(createInfo->hpp().setUsage(usage));

            // 
            vk::MemoryRequirements memReqs = allocationInfo->device.getImageMemoryRequirements(image);
            vk::MemoryAllocateInfo memAllocInfo = {};
            vk::ExportMemoryAllocateInfo exportAllocInfo{ vk::ExternalMemoryHandleTypeFlagBits::eOpaqueWin32 };
            memAllocInfo.pNext = &exportAllocInfo;
            memAllocInfo.allocationSize = memReqs.size;
            memAllocInfo.memoryTypeIndex = uint32_t(allocationInfo->getMemoryType(memReqs.memoryTypeBits, { .eDeviceLocal = 1 }));

            // 
            this->info.device.bindImageMemory(image, info.memory = info.device.allocateMemory(memAllocInfo), 0);
            this->info.initialLayout = vk::ImageLayout(createInfo->initialLayout);

#if defined(ENABLE_OPENGL_INTEROP) && defined(VK_USE_PLATFORM_WIN32_KHR)
            this->info.handle = info.device.getMemoryWin32HandleKHR({ info.memory, vk::ExternalMemoryHandleTypeFlagBits::eOpaqueWin32 }, this->info.dispatch);
#endif
            this->info.range = memReqs.size;
            this->info.reqSize = memReqs.size;
            
            // 
            if (createInfo->queueFamilyIndexCount) {
                this->info.queueFamilyIndices = std::vector<uint32_t>(createInfo->queueFamilyIndexCount);
                memcpy(this->info.queueFamilyIndices.data(), createInfo->pQueueFamilyIndices, sizeof(uint32_t) * createInfo->queueFamilyIndexCount);
            };

            // 
#ifdef ENABLE_OPENGL_INTEROP
            GLenum format = GL_RGBA8;
            if (createInfo->format == VK_FORMAT_R16G16B16A16_UNORM) { format = GL_RGBA16; };
            if (createInfo->format == VK_FORMAT_R32G32B32A32_SFLOAT) { format = GL_RGBA32F; };
            if (createInfo->format == VK_FORMAT_R16G16B16A16_SFLOAT) { format = GL_RGBA16F; };
            if (createInfo->format == VK_FORMAT_R32G32B32_SFLOAT) { format = GL_RGB32F; };
            if (createInfo->format == VK_FORMAT_R16G16B16_SFLOAT) { format = GL_RGB16F; };
            if (createInfo->format == VK_FORMAT_R32G32_SFLOAT) { format = GL_RG32F; };
            if (createInfo->format == VK_FORMAT_R16G16_SFLOAT) { format = GL_RG16F; };

            // Import Memory
            glCreateTextures(GL_TEXTURE_2D, 1, &this->info.glID);
            glCreateMemoryObjectsEXT(1, &this->info.glMemory);
            glImportMemoryWin32HandleEXT(this->info.glMemory, this->info.reqSize, GL_HANDLE_TYPE_OPAQUE_WIN32_EXT, this->info.handle);

            // Create GL Image
            if (createInfo->imageType == VK_IMAGE_TYPE_1D) {
                glTextureStorageMem1DEXT(this->info.glID, createInfo->mipLevels, format, createInfo->extent.width, this->info.glMemory, 0);
            }
            else
            if (createInfo->imageType == VK_IMAGE_TYPE_2D) {
                glTextureStorageMem2DEXT(this->info.glID, createInfo->mipLevels, format, createInfo->extent.width, createInfo->extent.height, this->info.glMemory, 0);
            }
            else
            if (createInfo->imageType == VK_IMAGE_TYPE_3D) {
                glTextureStorageMem3DEXT(this->info.glID, createInfo->mipLevels, format, createInfo->extent.width, createInfo->extent.height, createInfo->extent.depth, this->info.glMemory, 0);
            }
#endif
            return this;
        };

        // 
        virtual const unsigned& getGLImage() const { return this->info.glID; };
        virtual const unsigned& getGLMemory() const { return this->info.glMemory; };

        // 
        virtual unsigned& getGLImage() { return this->info.glID; };
        virtual unsigned& getGLMemory() { return this->info.glMemory; };

        // BETA
        virtual vk::DispatchLoaderDynamic dispatchLoaderDynamic() {
            return this->info.dispatch;
        }

        // BETA
        virtual vk::DispatchLoaderDynamic dispatchLoaderDynamic() const {
            return this->info.dispatch;
        }

        virtual ImageAllocation& operator=(vkt::uni_ptr<ImageAllocation> allocation) {
            this->image = allocation->image;
            this->info = allocation->info;
            return *this;
        };

        // 
        virtual const vk::Image& getImage() const { return this->image; };
        virtual vk::Image& getImage() { return this->image; };

        // 
        virtual operator const vk::Image& () const { return this->image; };
        virtual operator const VkImage& () const { return this->image; };

        // 
        virtual operator vk::Image& () { return this->image; };
        virtual operator VkImage& () { return reinterpret_cast<VkImage&>(this->image); };

        // 
        virtual operator const vk::Device& () const { return info.device; };
        virtual operator const VkDevice& () const { return reinterpret_cast<const VkDevice&>(info.device); };

        //
        virtual vk::Device& getDevice() { return info.device; };
        virtual const vk::Device& getDevice() const { return info.device; };

        // 
        virtual const vk::Device& _getDevice() const { return info.device; };

        // 
        virtual operator vk::Device& () { return info.device; };
        virtual operator VkDevice& () { return reinterpret_cast<VkDevice&>(info.device); };

        // Get mapped memory
        virtual void* map() { return (info.pMapped = info.device.mapMemory(info.memory, 0u, info.range, {})); };
        virtual void* mapped() { return info.pMapped; };
        virtual void  unmap() { info.device.unmapMemory(info.memory); };

        // 
        virtual MemoryAllocationInfo& getAllocationInfo() { return info; };
        virtual const MemoryAllocationInfo& getAllocationInfo() const { return info; };

        // 
        virtual ImageAllocation* address() { return this; };
        virtual const ImageAllocation* address() const { return this; };

        // Queue Family Indices
        virtual std::vector<uint32_t>& getQueueFamilyIndices() { return this->info.queueFamilyIndices; };
        virtual const std::vector<uint32_t>& getQueueFamilyIndices() const { return this->info.queueFamilyIndices; };

        // Bindless Textures Directly
#ifdef ENABLE_OPENGL_INTEROP
        virtual GLuint& getGL() { return this->info.glID; };
        virtual const GLuint& getGL() const { return this->info.glID; };

        virtual uint64_t deviceAddress() { return glGetTextureHandleARB(this->info.glID); };
        virtual const uint64_t deviceAddress() const { return glGetTextureHandleARB(this->info.glID); };
        virtual uint64_t deviceAddress(GLuint sampler) { return glGetTextureSamplerHandleARB(this->info.glID, sampler); };
        virtual const uint64_t deviceAddress(GLuint sampler) const { return glGetTextureSamplerHandleARB(this->info.glID, sampler); };
#endif

    // 
    protected: friend VmaImageAllocation; friend ImageAllocation; friend ImageRegion;
        vk::Image image = {};
        MemoryAllocationInfo info = {};
    };

    // 
    class VmaImageAllocation : public ImageAllocation {
    public: friend VmaImageAllocation; friend ImageAllocation;// 
        VmaImageAllocation() {};
        VmaImageAllocation(vkt::uni_arg<VmaAllocator> allocator, vkt::uni_arg<vkh::VkImageCreateInfo> createInfo = vkh::VkImageCreateInfo{}, vkt::uni_arg<VmaMemoryUsage> vmaUsage = VMA_MEMORY_USAGE_GPU_ONLY) { this->construct(allocator, createInfo, vmaUsage); };

        // 
        VmaImageAllocation(vkt::uni_ptr<VmaImageAllocation> allocation) : allocation(allocation->allocation), allocationInfo(allocation->allocationInfo), allocator(allocation->allocator) { *this = allocation; };
        VmaImageAllocation(vkt::uni_ptr<ImageAllocation> allocation) { *this = dynamic_cast<VmaImageAllocation&>(*allocation); };

        // 
        VmaImageAllocation(std::shared_ptr<VmaImageAllocation> allocation) : allocation(allocation->allocation), allocationInfo(allocation->allocationInfo), allocator(allocation->allocator) { *this = vkt::uni_ptr<VmaImageAllocation>(allocation); };
        VmaImageAllocation(std::shared_ptr<ImageAllocation> allocation) { *this = dynamic_cast<VmaImageAllocation&>(*vkt::uni_ptr<ImageAllocation>(allocation)); };

        //
        ~VmaImageAllocation() {
            this->info.device.waitIdle();
            vmaDestroyImage(allocator, image, allocation);
        };

        // 
        virtual VmaImageAllocation* construct(
            vkt::uni_arg<VmaAllocator> allocator,
            vkt::uni_arg<vkh::VkImageCreateInfo> createInfo = vkh::VkImageCreateInfo{},
            vkt::uni_arg<VmaMemoryUsage> vmaUsage = VMA_MEMORY_USAGE_GPU_ONLY
        ) {
            VmaAllocationCreateInfo vmaInfo = {}; vmaInfo.usage = vmaUsage;
            if (vmaUsage == VMA_MEMORY_USAGE_CPU_TO_GPU || vmaUsage == VMA_MEMORY_USAGE_GPU_TO_CPU) { vmaInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT; };
            auto result = vmaCreateImage(this->allocator = allocator.ref(), *createInfo, &vmaInfo, &reinterpret_cast<VkImage&>(image), &this->allocation, &this->allocationInfo);  assert(result == VK_SUCCESS);
            this->info.initialLayout = vk::ImageLayout(createInfo->initialLayout);
            this->info.range = allocationInfo.size;
            this->info.memory = allocationInfo.deviceMemory;
            this->info.memUsage = vmaUsage;

            // Get Dispatch Loader From VMA Allocator Itself!
            VmaAllocatorInfo info = {};
            vmaGetAllocatorInfo(this->allocator = allocator.ref(), &info);
            this->info.dispatch = vk::DispatchLoaderDynamic(info.instance, vkGetInstanceProcAddr, this->info.device = info.device, vkGetDeviceProcAddr); // 

            // 
            if (createInfo->queueFamilyIndexCount) {
                this->info.queueFamilyIndices = std::vector<uint32_t>(createInfo->queueFamilyIndexCount);
                memcpy(this->info.queueFamilyIndices.data(), createInfo->pQueueFamilyIndices, sizeof(uint32_t) * createInfo->queueFamilyIndexCount);
            };

            // 
            return this;
        };

        // 
        virtual VmaImageAllocation& operator=(const VmaImageAllocation& allocation) {
            if (allocation.allocation) {
                vmaDestroyImage(allocator, *this, allocation); // don't assign into already allocated
            };
            this->image = allocation.image;
            this->allocation = allocation.allocation;
            this->allocationInfo = allocation.allocationInfo;
            this->allocator = allocation.allocator;
            this->info = allocation.info;

            //this->info.device = this->_getDevice();
            return *this;
        };

        // Get mapped memory
        virtual void* map() override { void* ptr = nullptr; vmaMapMemory(allocator, allocation, &ptr); return ptr; };
        virtual void* mapped() override { if (!allocationInfo.pMappedData) { vmaMapMemory(allocator, allocation, &allocationInfo.pMappedData); }; return allocationInfo.pMappedData; };
        virtual void  unmap() override { vmaUnmapMemory(allocator, allocation); allocationInfo.pMappedData = nullptr; };

        // Allocation
        virtual operator const VmaAllocation& () const { return allocation; };
        virtual operator const VmaAllocationInfo& () const { return allocationInfo; };

        // 
        virtual operator VmaAllocation& () { return allocation; };
        virtual operator VmaAllocationInfo& () { return allocationInfo; };

        // 
        virtual VmaImageAllocation* address() { return this; };
        virtual const VmaImageAllocation* address() const { return this; };

        //vk::DispatchLoaderDynamic(this->instance, vkGetInstanceProcAddr, this->device, vkGetDeviceProcAddr);
        virtual vk::DispatchLoaderDynamic dispatchLoaderDynamic() override {
            VmaAllocatorInfo info = {}; vmaGetAllocatorInfo(this->allocator, &info);
            return vk::DispatchLoaderDynamic(info.instance, vkGetInstanceProcAddr, info.device, vkGetDeviceProcAddr);
        };

        virtual vk::DispatchLoaderDynamic dispatchLoaderDynamic() const override {
            VmaAllocatorInfo info = {}; vmaGetAllocatorInfo(this->allocator, &info);
            return vk::DispatchLoaderDynamic(info.instance, vkGetInstanceProcAddr, info.device, vkGetDeviceProcAddr);
        };

    // 
    protected: friend VmaImageAllocation; friend ImageAllocation; // 
        VmaAllocationInfo allocationInfo = {};
        VmaAllocation allocation = {};
        VmaAllocator allocator = {};
    };

    // 
    struct ImageRegionCreateInfoAllocated {
        vkt::uni_arg<MemoryAllocationInfo> allocationInfo = MemoryAllocationInfo{};
        vkt::uni_arg<vkh::VkImageCreateInfo> createInfo = vkh::VkImageCreateInfo{};
        vkt::uni_arg<vkh::VkImageViewCreateInfo> info = vkh::VkImageViewCreateInfo{};
        vkt::uni_arg<vk::ImageLayout> layout = vk::ImageLayout::eGeneral;
    };

    // 
    struct ImageRegionCreateInfoVMA {
        vkt::uni_arg<VmaAllocator> allocator = VmaAllocator{};
        vkt::uni_arg<vkh::VkImageCreateInfo> createInfo = vkh::VkImageCreateInfo{};
        vkt::uni_arg<vkh::VkImageViewCreateInfo> info = vkh::VkImageViewCreateInfo{};
        vkt::uni_arg<vk::ImageLayout> layout = vk::ImageLayout::eGeneral;
    };

    // 
    class ImageRegion : public std::enable_shared_from_this<ImageRegion> { public: 
        ImageRegion() {};
        ImageRegion(const vkt::uni_ptr<ImageRegion>& region) { *this = region; };
        ImageRegion(const vkt::uni_ptr<ImageAllocation>& allocation, vkt::uni_arg<vkh::VkImageViewCreateInfo> info = vkh::VkImageViewCreateInfo{}, vkt::uni_arg<vk::ImageLayout> layout = vk::ImageLayout::eGeneral) : allocation(allocation), subresourceRange(info->subresourceRange) { this->construct(allocation, info, layout); };
        ImageRegion(const vkt::uni_ptr<VmaImageAllocation>& allocation, vkt::uni_arg<vkh::VkImageViewCreateInfo> info = vkh::VkImageViewCreateInfo{}, vkt::uni_arg<vk::ImageLayout> layout = vk::ImageLayout::eGeneral) : allocation(allocation.dyn_cast<ImageAllocation>()), subresourceRange(info->subresourceRange) { this->construct(allocation.dyn_cast<ImageAllocation>(), info, layout); };
        ImageRegion(const std::shared_ptr<ImageAllocation>& allocation, vkt::uni_arg<vkh::VkImageViewCreateInfo> info = vkh::VkImageViewCreateInfo{}, vkt::uni_arg<vk::ImageLayout> layout = vk::ImageLayout::eGeneral) : allocation(vkt::uni_ptr<ImageAllocation>(allocation)), subresourceRange(info->subresourceRange) { this->construct(vkt::uni_ptr<ImageAllocation>(allocation), info, layout); };
        ImageRegion(const std::shared_ptr<VmaImageAllocation>& allocation, vkt::uni_arg<vkh::VkImageViewCreateInfo> info = vkh::VkImageViewCreateInfo{}, vkt::uni_arg<vk::ImageLayout> layout = vk::ImageLayout::eGeneral) : allocation(std::dynamic_pointer_cast<ImageAllocation>(allocation)), subresourceRange(info->subresourceRange) { this->construct(vkt::uni_ptr<ImageAllocation>(std::dynamic_pointer_cast<ImageAllocation>(allocation)), info, layout); };
        ~ImageRegion() {
            
        };

        // alias Of getAllocation
        virtual vkt::uni_ptr<ImageAllocation>& uniPtr() { return allocation; };
        virtual vkt::uni_ptr<ImageAllocation> uniPtr() const { return allocation; };

        // 
        virtual ImageRegion* construct(
            vkt::uni_ptr<ImageAllocation> allocation,
            vkt::uni_arg<vkh::VkImageViewCreateInfo> info = vkh::VkImageViewCreateInfo{},
            vkt::uni_arg<vk::ImageLayout> layout = vk::ImageLayout::eGeneral
        ) {
            info->image = allocation->getImage();
            this->allocation = allocation;
            this->subresourceRange = info->subresourceRange;
            this->imgInfo.imageView = this->allocation->getDevice().createImageView(info->hpp(), nullptr, this->allocation->info.dispatch);
            this->imgInfo.imageLayout = VkImageLayout(*layout);

            // 
#ifdef ENABLE_OPENGL_INTEROP
            if (!this->allocation->info.glID) {
                vk::ImageViewHandleInfoNVX handleInfo = {};
                handleInfo.imageView = this->imgInfo.imageView;
                handleInfo.sampler = this->imgInfo.sampler;
                handleInfo.descriptorType = this->imgInfo.sampler ? vk::DescriptorType::eCombinedImageSampler : vk::DescriptorType::eSampledImage;
                this->allocation->info.glID = this->allocation->getDevice().getImageViewHandleNVX(&handleInfo, this->allocation->dispatchLoaderDynamic());
            };
#endif

            // 
            return this;
        };

        // 
        virtual ImageRegion* construct(
            vkt::uni_ptr<VmaImageAllocation> allocation,
            vkt::uni_arg<vkh::VkImageViewCreateInfo> info = vkh::VkImageViewCreateInfo{},
            vkt::uni_arg<vk::ImageLayout> layout = vk::ImageLayout::eGeneral
        ) {
            this->construct(allocation.dyn_cast<ImageAllocation>(), info, layout);
            return this;
        };

        // 
        virtual ImageRegion& operator=(vkt::uni_ptr<ImageRegion> region) {
            this->allocation = region->uniPtr();
            this->subresourceRange = region->subresourceRange;
            this->imgInfo = region->imgInfo;
            return *this;
        };

        // alias Of getAllocation
        virtual vkt::uni_ptr<ImageAllocation>& uniPtrVma() { return this->allocation; };
        virtual const vkt::uni_ptr<ImageAllocation>& uniPtrVma() const { return this->allocation; };

        //virtual vkh::VkImageSubresourceRange& subresourceRange() { return this->subresourceRange; };
        virtual vkh::VkImageSubresourceLayers subresourceLayers(const uint32_t mipLevel =  0u) const { return {
            .aspectMask = this->subresourceRange.aspectMask,
            .mipLevel = this->subresourceRange.baseMipLevel + mipLevel,
            .baseArrayLayer = this->subresourceRange.baseArrayLayer,
            .layerCount = this->subresourceRange.layerCount
        };};

        // set methods for direct control
        virtual ImageRegion& setImageLayout(const VkImageLayout layout = {}) { this->imgInfo.imageLayout = layout; return *this; };
        virtual ImageRegion& setSampler(const VkSampler& sampler = {}) { this->imgInfo.sampler = sampler; return *this; };
        virtual ImageRegion& setImageLayout(const vk::ImageLayout layout = {}) { this->imgInfo.imageLayout = reinterpret_cast<const VkImageLayout&>(layout); return *this; };
        virtual ImageRegion& setSampler(const vk::Sampler& sampler = {}) { this->imgInfo.sampler = reinterpret_cast<const VkSampler&>(sampler); return *this; };
        virtual ImageRegion& transfer(vk::CommandBuffer& cmdBuf) {
            vkt::imageBarrier(cmdBuf, vkt::ImageBarrierInfo{ 
                .image = this->allocation->getImage(),
                .targetLayout = reinterpret_cast<const vk::ImageLayout&>(this->imgInfo.imageLayout),
                .originLayout = this->allocation->info.initialLayout,
                .subresourceRange = this->subresourceRange
            });
            return *this;
        };
        virtual const ImageRegion& transfer(vk::CommandBuffer& cmdBuf) const {
            vkt::imageBarrier(cmdBuf, vkt::ImageBarrierInfo{
                .image = this->allocation->getImage(),
                .targetLayout = reinterpret_cast<const vk::ImageLayout&>(this->imgInfo.imageLayout),
                .originLayout = this->allocation->info.initialLayout,
                .subresourceRange = this->subresourceRange
            });
            return *this;
        };

        // 
        virtual operator ImageAllocation*() { return this->allocation; };
        virtual operator vkt::uni_ptr<ImageAllocation>&() { return this->allocation; };
        virtual operator std::shared_ptr<ImageAllocation>& () { return this->allocation; };
        virtual operator vkt::MemoryAllocationInfo&() { return this->allocation->info; };
        virtual operator vkh::VkImageSubresourceRange&() { return this->subresourceRange; };
        virtual operator    ::VkImageSubresourceRange&() { return this->subresourceRange; };
        virtual operator vkh::VkDescriptorImageInfo&() { return this->imgInfo; };
        virtual operator    ::VkDescriptorImageInfo&() { return this->imgInfo; };
        virtual operator vk::DescriptorImageInfo&() { return this->imgInfo; };
        virtual operator vk::ImageSubresourceRange&() { return this->subresourceRange; };
        virtual operator vk::ImageView&() { return reinterpret_cast<vk::ImageView&>(this->imgInfo.imageView); };
        virtual operator vk::ImageLayout&() { return reinterpret_cast<vk::ImageLayout&>(this->imgInfo.imageLayout); };
        virtual operator vk::Image&() { return *this->allocation; };
        virtual operator vk::Sampler&() { return reinterpret_cast<vk::Sampler&>(this->imgInfo.sampler); };
        virtual operator vk::Device&() { return *this->allocation; };
        virtual operator VkImageView&() { return reinterpret_cast<VkImageView&>(this->imgInfo.imageView); };
        virtual operator VkImageLayout&() { return reinterpret_cast<VkImageLayout&>(this->imgInfo.imageLayout); };
        virtual operator VkImage&() { return *this->allocation; };
        virtual operator VkSampler&() { return this->imgInfo.sampler; };
        virtual operator VkDevice&() { return reinterpret_cast<VkDevice&>(this->allocation->getDevice()); };

        // 
        virtual operator const ImageAllocation*() const { return this->allocation; };
        virtual operator const vkt::uni_ptr<ImageAllocation>&() const { return this->allocation; };
        virtual operator const std::shared_ptr<ImageAllocation>& () const { return this->allocation; };
        virtual operator const vkt::MemoryAllocationInfo&() const { return this->allocation->info; };
        virtual operator const vkh::VkImageSubresourceRange&() const { return this->subresourceRange; };
        virtual operator const    ::VkImageSubresourceRange&() const { return this->subresourceRange; };
        virtual operator const vkh::VkDescriptorImageInfo&() const { return this->imgInfo; };
        virtual operator const    ::VkDescriptorImageInfo&() const { return this->imgInfo; };
        virtual operator const vk::DescriptorImageInfo&() const { return this->imgInfo; };
        virtual operator const vk::ImageSubresourceRange&() const { return this->subresourceRange; };
        virtual operator const vk::ImageView&() const { return reinterpret_cast<const vk::ImageView&>(this->imgInfo.imageView); };
        virtual operator const vk::ImageLayout&() const { return reinterpret_cast<const vk::ImageLayout&>(this->imgInfo.imageLayout); };
        virtual operator const vk::Image&() const { return *this->allocation; };
        virtual operator const vk::Sampler&() const { return reinterpret_cast<const vk::Sampler&>(this->imgInfo.sampler); };
        virtual operator const vk::Device&() const { return *this->allocation; };
        
        // 
        virtual operator const VkImageView&() const { return this->imgInfo.imageView; };
        virtual operator const VkImageLayout&() const { return this->imgInfo.imageLayout; };
        virtual operator const VkImage&() const { return *this->allocation; };
        virtual operator const VkSampler&() const { return this->imgInfo.sampler; };
        virtual operator const VkDevice& () const { return reinterpret_cast<const VkDevice&>(this->allocation->getDevice()); };
        virtual operator const vk::ImageSubresourceLayers() const { return vk::ImageSubresourceLayers{ reinterpret_cast<const vk::ImageAspectFlags&>(subresourceRange.aspectMask), subresourceRange.baseMipLevel, subresourceRange.baseArrayLayer, subresourceRange.layerCount }; };

#ifdef ENABLE_OPENGL_INTEROP
        // Bindless Textures Directly
        virtual uint64_t deviceAddress () { 
            if (this->getGL()) {
                return glGetTextureHandleARB(this->getGL());
            } else {
                return this->allocation->getDevice().getImageViewAddressNVX(this->getImageView(), this->allocation->dispatchLoaderDynamic()).deviceAddress;
            };
        };
        virtual const uint64_t deviceAddress() const { 
            if (this->getGL()) {
                return glGetTextureHandleARB(this->getGL());
            } else {
                return this->allocation->getDevice().getImageViewAddressNVX(this->getImageView(), this->allocation->dispatchLoaderDynamic()).deviceAddress;
            };
        };
        virtual uint64_t deviceAddress(GLuint sampler) { 
            return glGetTextureSamplerHandleARB(this->getGL(), sampler); 
        };
        virtual const uint64_t deviceAddress(GLuint sampler) const { 
            return glGetTextureSamplerHandleARB(this->getGL(), sampler); 
        };
#else   // Get By Vulkan API Directly
#ifdef ENABLE_NVX_IMAGE_ADDRESS
        virtual uint64_t deviceAddress() { return this->allocation->getDevice().getImageViewAddressNVX(this->getImageView(), this->getAllocation()->dispatchLoaderDynamic()).deviceAddress; };
        virtual const uint64_t deviceAddress() const { return this->allocation->getDevice().getImageViewAddressNVX(this->getImageView(), this->getAllocation()->dispatchLoaderDynamic()).deviceAddress; };
#endif
#endif

        // 
        virtual vk::ImageView& getImageView() { return reinterpret_cast<vk::ImageView&>(this->imgInfo.imageView); };
        virtual vk::ImageLayout& getImageLayout() { return reinterpret_cast<vk::ImageLayout&>(this->imgInfo.imageLayout); };
        virtual vk::Sampler& getSampler() { return reinterpret_cast<vk::Sampler&>(this->imgInfo.sampler); };
        virtual vk::ImageSubresourceRange& getImageSubresourceRange() { return this->subresourceRange; };

        // 
        virtual const vk::ImageView& getImageView() const { return reinterpret_cast<const vk::ImageView&>(this->imgInfo.imageView); };
        virtual const vk::ImageLayout& getImageLayout() const { return reinterpret_cast<const vk::ImageLayout&>(this->imgInfo.imageLayout); };
        virtual const vk::Sampler& getSampler() const { return reinterpret_cast<const vk::Sampler&>(this->imgInfo.sampler); };
        virtual const vk::ImageSubresourceRange& getImageSubresourceRange() const { return this->subresourceRange; };


        // 
        virtual unsigned& getGL() {
            if (this->getGLImage()) {
                return this->getGLImage();
            } else {
                vk::ImageViewHandleInfoNVX handleInfo = {};
                handleInfo.imageView = this->imgInfo.imageView;
                handleInfo.sampler = this->imgInfo.sampler;
                handleInfo.descriptorType = this->imgInfo.sampler ? vk::DescriptorType::eCombinedImageSampler : vk::DescriptorType::eSampledImage;
                if (!this->allocation->info.glID) {
                    this->allocation->info.glID = this->allocation->getDevice().getImageViewHandleNVX(&handleInfo, this->allocation->dispatchLoaderDynamic());
                };
                return this->allocation->info.glID;
            }
        };

        // 
        virtual const unsigned& getGL() const {
            if (this->getGLImage()) {
                return this->getGLImage();
            } else {
                vk::ImageViewHandleInfoNVX handleInfo = {};
                handleInfo.imageView = this->imgInfo.imageView;
                handleInfo.sampler = this->imgInfo.sampler;
                handleInfo.descriptorType = this->imgInfo.sampler ? vk::DescriptorType::eCombinedImageSampler : vk::DescriptorType::eSampledImage;
                if (!this->allocation->info.glID) {
                    return this->allocation->getDevice().getImageViewHandleNVX(&handleInfo, this->allocation->dispatchLoaderDynamic());
                };
                return this->allocation->info.glID;
            };
        };

        // 
        virtual const unsigned& getGLImage() const { return this->allocation->getGLImage(); };
        virtual const unsigned& getGLMemory() const { return this->allocation->getGLMemory(); };

        // 
        virtual unsigned& getGLImage() { return this->allocation->getGLImage(); };
        virtual unsigned& getGLMemory() { return this->allocation->getGLMemory(); };

        // 
        virtual vk::Device& getDevice() { return reinterpret_cast<vk::Device&>(allocation->getDevice()); };
        virtual vk::Image& getImage() { return reinterpret_cast<vk::Image&>(allocation->getImage()); };

        // 
        virtual const vk::Device& getDevice() const { return reinterpret_cast<const vk::Device&>(allocation->getDevice()); };
        virtual const vk::Image& getImage() const { return reinterpret_cast<const vk::Image&>(allocation->getImage()); };

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

    protected: friend VmaImageAllocation; friend ImageAllocation; // 
        vkh::VkDescriptorImageInfo imgInfo = {};
        vkt::uni_ptr<ImageAllocation> allocation = {};

    public: // irrevalent sources
        vkh::VkImageSubresourceRange subresourceRange = {};
    };

};
