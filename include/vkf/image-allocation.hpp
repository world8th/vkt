#pragma once // #

//
#ifndef VKT_CORE_USE_VMA
#define VKT_CORE_USE_VMA
#endif

//
#include "./memory-allocation.hpp"

// 
namespace vkf {

    // 
    //class ImageRegion;
    //class VmaImageAllocation;
    class ImageAllocation : public std::enable_shared_from_this<ImageAllocation> {
    public:
        ImageAllocation() {};
        ImageAllocation(vkt::uni_arg<vkh::VkImageCreateInfo> createInfo, vkt::uni_arg<MemoryAllocationInfo> allocationInfo = MemoryAllocationInfo{}) : info(allocationInfo) { this->construct(allocationInfo, createInfo); }
        ImageAllocation(const vkt::uni_ptr<ImageAllocation>& allocation) : image(allocation->image), info(allocation->info) { *this = allocation; };
        ImageAllocation(const std::shared_ptr<ImageAllocation>& allocation) : image(allocation->image), info(allocation->info) { *this = vkt::uni_ptr<ImageAllocation>(allocation); };
        ~ImageAllocation() {
            if (!this->isManaged()) { // Avoid VMA Memory Corruption
                if ((this->image || this->info.memory) && this->info.device) {
                    this->info.deviceDispatch->DeviceWaitIdle();
                };
                if (this->image && this->info.device) {
                    this->info.deviceDispatch->DestroyImage(this->image, nullptr), this->image = nullptr;
                };
                if (this->info.memory && this->info.device) {
                    this->info.deviceDispatch->FreeMemory(this->info.memory, nullptr), this->info.memory = nullptr;
                };
            };
        };

        // 
        virtual ImageAllocation* construct(
            vkt::uni_arg<MemoryAllocationInfo> allocationInfo,
            vkt::uni_arg<vkh::VkImageCreateInfo> createInfo = vkh::VkImageCreateInfo{}
        ) {
            this->info = allocationInfo;

            // 
            VkExternalMemoryImageCreateInfo extMemInfo = {};
            extMemInfo.sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO;
            extMemInfo.pNext = nullptr;

            // 
            vkh::VkImageUsageFlags usage = createInfo->usage;
            if (this->info.memUsage == VMA_MEMORY_USAGE_CPU_TO_GPU) { usage.eTransferSrc = 1; };
            if (this->info.memUsage == VMA_MEMORY_USAGE_GPU_TO_CPU) { usage.eTransferDst = 1; };
            if (this->info.memUsage == VMA_MEMORY_USAGE_GPU_ONLY) { usage.eTransferDst = 1, usage.eTransferSrc = 1; };

            // 
            {
                auto* prevPtr = createInfo->pNext;
                createInfo->pNext = &extMemInfo;
                extMemInfo.pNext = prevPtr;
#ifdef VKT_WIN32_DETECTED
                extMemInfo.handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;
#endif
            };

            // 
            vkh::VkMemoryAllocateFlagsInfo allocFlags = {};
            vkt::unlock32(allocFlags.flags) = 0u;
            if (this->info.memUsage == VMA_MEMORY_USAGE_GPU_ONLY) {
                allocFlags.flags->eAddress = 1u;
            };

            //
#ifdef VKT_CORE_USE_XVK
            if (!this->info.deviceDispatch) { this->info.deviceDispatch = vkt::vkGlobal::device; };
            if (!this->info.instanceDispatch) { this->info.instanceDispatch = vkt::vkGlobal::instance; };

            // reload device and instance
            if (!this->info.device && this->info.deviceDispatch) { this->info.device = this->info.deviceDispatch->handle; };
            if (!this->info.instance && this->info.instanceDispatch) { this->info.instance = this->info.instanceDispatch->handle; };
#endif

            // 
            createInfo->usage = usage;
            vkt::handleVk(this->info.deviceDispatch->CreateImage(*createInfo, nullptr, &this->image));

            //
            VkMemoryDedicatedRequirementsKHR dedicatedReqs = { VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS_KHR, NULL };
            VkMemoryRequirements2 memReqs2 = { VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2, &dedicatedReqs };

            // 
            const VkImageMemoryRequirementsInfo2 imageReqInfo = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2, NULL, image };
            this->info.deviceDispatch->GetImageMemoryRequirements2(&imageReqInfo, &memReqs2);
            VkMemoryRequirements& memReqs = memReqs2.memoryRequirements;
            this->info.reqSize = this->info.range = memReqs.size;

            //
#ifdef VKT_WIN32_DETECTED
            vkh::VkExportMemoryAllocateInfo exportAllocInfo{ .handleTypes = vkh::VkExternalMemoryHandleTypeFlags{.eOpaqueWin32 = 1} };
#else
            vkh::VkExportMemoryAllocateInfo exportAllocInfo{ .handleTypes = vkh::VkExternalMemoryHandleTypeFlags{.eOpaqueFd = 1} };
#endif

            // 
            vkh::VkMemoryAllocateInfo memAllocInfo = {};
            allocFlags.pNext = &exportAllocInfo;

            // prefer dedicated allocation, where and when possible
            VkMemoryDedicatedAllocateInfoKHR dedicatedInfo = { VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO_KHR, NULL, image, VK_NULL_HANDLE, };
            if (dedicatedReqs.prefersDedicatedAllocation || dedicatedReqs.requiresDedicatedAllocation) {
                exportAllocInfo.pNext = &dedicatedInfo;
            };

            // 
            const vkh::VkMemoryPropertyFlags property = { .eDeviceLocal = 1 };
            memAllocInfo.pNext = &allocFlags;
            memAllocInfo.allocationSize = memReqs.size;
            memAllocInfo.memoryTypeIndex = uint32_t(this->info.getMemoryType(memReqs.memoryTypeBits, property));

            // 
            vkt::handleVk(this->info.deviceDispatch->AllocateMemory(memAllocInfo, nullptr, &this->info.memory));
            vkt::handleVk(this->info.deviceDispatch->BindImageMemory(this->image, this->info.memory, 0u));
            this->info.initialLayout = createInfo->initialLayout;

#ifdef VKT_WIN32_DETECTED
            const auto handleInfo = VkMemoryGetWin32HandleInfoKHR{ VK_STRUCTURE_TYPE_MEMORY_GET_WIN32_HANDLE_INFO_KHR, nullptr, this->info.memory, VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT };
            this->info.deviceDispatch->GetMemoryWin32HandleKHR(&handleInfo, &this->info.handle);
#endif

            // 
            if (createInfo->queueFamilyIndexCount) {
                this->info.queueFamilyIndices = std::vector<uint32_t>(createInfo->queueFamilyIndexCount);
                memcpy(this->info.queueFamilyIndices.data(), createInfo->pQueueFamilyIndices, sizeof(uint32_t) * createInfo->queueFamilyIndexCount);
            };

            // 
#ifdef VKT_OPENGL_INTEROP
            GLenum format = GL_RGBA8;
            if (createInfo->format == VK_FORMAT_R8G8B8_UNORM) { format = gl::GLenum(GL_RGB8); };
            if (createInfo->format == VK_FORMAT_R16G16B16_UNORM) { format = gl::GLenum(GL_RGB16); };
            if (createInfo->format == VK_FORMAT_R16G16B16A16_UNORM) { format = gl::GLenum(GL_RGBA16); };
            if (createInfo->format == VK_FORMAT_R32G32B32A32_SFLOAT) { format = gl::GLenum(GL_RGBA32F); };
            if (createInfo->format == VK_FORMAT_R16G16B16A16_SFLOAT) { format = gl::GLenum(GL_RGBA16F); };
            if (createInfo->format == VK_FORMAT_R32G32B32_SFLOAT) { format = gl::GLenum(GL_RGB32F); };
            if (createInfo->format == VK_FORMAT_R16G16B16_SFLOAT) { format = gl::GLenum(GL_RGB16F); };
            if (createInfo->format == VK_FORMAT_R32G32_SFLOAT) { format = gl::GLenum(GL_RG32F); };
            if (createInfo->format == VK_FORMAT_R16G16_SFLOAT) { format = gl::GLenum(GL_RG16F); };

            // Import Memory
            if (this->info.handle) {
                if (!this->info.glID) {
                    glCreateTextures(GLenum(GL_TEXTURE_2D), 1, &this->info.glID);
                };
                if (!this->info.glMemory) {
                    glCreateMemoryObjectsEXT(1, &this->info.glMemory);
                    glImportMemoryWin32HandleEXT(this->info.glMemory, this->info.reqSize, GL_HANDLE_TYPE_OPAQUE_WIN32_EXT, this->info.handle);

                    // Create GL Image
                    if (createInfo->imageType == VK_IMAGE_TYPE_1D) { glTextureStorageMem1DEXT(this->info.glID, createInfo->mipLevels, format, createInfo->extent.width, this->info.glMemory, 0); } else
                    if (createInfo->imageType == VK_IMAGE_TYPE_2D) { glTextureStorageMem2DEXT(this->info.glID, createInfo->mipLevels, format, createInfo->extent.width, createInfo->extent.height, this->info.glMemory, 0); } else
                    if (createInfo->imageType == VK_IMAGE_TYPE_3D) { glTextureStorageMem3DEXT(this->info.glID, createInfo->mipLevels, format, createInfo->extent.width, createInfo->extent.height, createInfo->extent.depth, this->info.glMemory, 0); }
                };
            };
#endif
            // 
            this->createInfo = createInfo;

            // 
            return this;
        };

        // 
        virtual bool isManaged() const { return false; };

        // 
        virtual const unsigned& getGLImage() const { return this->info.glID; };
        virtual const unsigned& getGLMemory() const { return this->info.glMemory; };

        // 
        virtual unsigned& getGLImage() { return this->info.glID; };
        virtual unsigned& getGLMemory() { return this->info.glMemory; };

        // 
        virtual ImageAllocation& operator=(vkt::uni_ptr<ImageAllocation> allocation) {
            this->image = allocation->image;
            this->info = allocation->info;
            return *this;
        };

        // 
        virtual const VkImage& getImage() const { return this->image; };
        virtual VkImage& getImage() { return this->image; };

        // 
        virtual operator const VkImage& () const { return this->image; };
        virtual operator VkImage& () { return this->image; };

        // 
        virtual operator const VkDevice& () const { return info.device; };
        virtual operator VkDevice& () { return info.device; };

        //
        virtual VkDevice& getDevice() { return info.device; };
        virtual const VkDevice& getDevice() const { return info.device; };

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
#ifdef VKT_OPENGL_INTEROP
        virtual GLuint& getGL() { return this->info.glID; };
        virtual const GLuint& getGL() const { return this->info.glID; };

        virtual uint64_t deviceAddress() { return glGetTextureHandleARB(this->info.glID); };
        virtual const uint64_t deviceAddress() const { return glGetTextureHandleARB(this->info.glID); };
        virtual uint64_t deviceAddress(GLuint sampler) { return glGetTextureSamplerHandleARB(this->info.glID, sampler); };
        virtual const uint64_t deviceAddress(GLuint sampler) const { return glGetTextureSamplerHandleARB(this->info.glID, sampler); };
#endif

        // 
        virtual vkh::VkImageCreateInfo& getCreateInfo() { return createInfo; };
        virtual const vkh::VkImageCreateInfo& getCreateInfo() const { return createInfo; };

        //
        virtual vkf::MemoryAllocationInfo& getInfo() { return info; };
        virtual const vkf::MemoryAllocationInfo& getInfo() const { return info; };

    // 
    protected: friend ImageAllocation;
        VkImage image = VK_NULL_HANDLE;
        vkf::MemoryAllocationInfo info = {};
        vkh::VkImageCreateInfo createInfo = {};
    };

};
