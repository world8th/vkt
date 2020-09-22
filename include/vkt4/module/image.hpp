#pragma once // #

// 
//#ifndef VKT_CORE_ENABLE_VMA
//#define VKT_CORE_ENABLE_VMA
//#endif

// 
//#include "./inline.hpp"

// 
namespace vkt {

#if defined(ENABLE_OPENGL_INTEROP) && !defined(VKT_USE_GLAD)
    using namespace gl;
#endif

    // 
    class ImageRegion;
    class VmaImageAllocation;
    class ImageAllocation : public std::enable_shared_from_this<ImageAllocation> {
    public:
        ImageAllocation() {};
        ImageAllocation(vkt::uni_arg<vkh::VkImageCreateInfo> createInfo, vkt::uni_arg<MemoryAllocationInfo> allocationInfo = MemoryAllocationInfo{}) : info(allocationInfo) { this->construct(allocationInfo, createInfo); }
        ImageAllocation(const vkt::uni_ptr<ImageAllocation>& allocation) : image(allocation->image), info(allocation->info) { *this = allocation; };
        ImageAllocation(const std::shared_ptr<ImageAllocation>& allocation) : image(allocation->image), info(allocation->info) { *this = vkt::uni_ptr<ImageAllocation>(allocation); };
        ~ImageAllocation() {
            if (this->image && this->info.device) {
                this->info.deviceDispatch->DeviceWaitIdle();
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
                allocFlags.flags.eAddress = 1u;
            };

            //
            if (!this->info.deviceDispatch) { this->info.deviceDispatch = vkGlobal::device; };
            if (!this->info.instanceDispatch) { this->info.instanceDispatch = vkGlobal::instance; };

            // reload device and instance
            if (!this->info.device && this->info.deviceDispatch) { this->info.device = this->info.deviceDispatch->handle; };
            if (!this->info.instance && this->info.instanceDispatch) { this->info.instance = this->info.instanceDispatch->handle; };

            // 
            createInfo->usage = usage;
            vkh::handleVk(this->info.deviceDispatch->CreateImage(*createInfo, nullptr, &this->image));

            // 
            VkMemoryRequirements memReqs = {};
            this->info.deviceDispatch->GetImageMemoryRequirements(this->image, &memReqs);
            this->info.reqSize = this->info.range = memReqs.size;

            //
#ifdef VKT_WIN32_DETECTED
            vkh::VkExportMemoryAllocateInfo exportAllocInfo{ .handleTypes = {.eOpaqueWin32 = 1} };
#else
            vkh::VkExportMemoryAllocateInfo exportAllocInfo{ .handleTypes = {.eOpaqueFd = 1} };
#endif

            // 
            vkh::VkMemoryAllocateInfo memAllocInfo = {};
            allocFlags.pNext = &exportAllocInfo;

            // 
            const vkh::VkMemoryPropertyFlags property = { .eDeviceLocal = 1 };
            memAllocInfo.pNext = &allocFlags;
            memAllocInfo.allocationSize = memReqs.size;
            memAllocInfo.memoryTypeIndex = uint32_t(this->info.getMemoryType(memReqs.memoryTypeBits, property));

            // 
            vkh::handleVk(this->info.deviceDispatch->AllocateMemory(memAllocInfo, nullptr, &this->info.memory));
            vkh::handleVk(this->info.deviceDispatch->BindImageMemory(this->image, this->info.memory, 0u));
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
#ifdef ENABLE_OPENGL_INTEROP
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
            return this;
        };

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
        VkImage image = VK_NULL_HANDLE;
        MemoryAllocationInfo info = {};
    };

    // 
    class VmaImageAllocation : public ImageAllocation {
    public: friend VmaImageAllocation; friend ImageAllocation;// 
        VmaImageAllocation() {};
        VmaImageAllocation(vkt::uni_arg<VmaAllocator> allocator, vkt::uni_arg<vkh::VkImageCreateInfo> createInfo = vkh::VkImageCreateInfo{}, vkt::uni_arg<VmaMemoryInfo> vmaInfo = VmaMemoryInfo{ .memUsage = VMA_MEMORY_USAGE_GPU_ONLY }) { this->construct(allocator, createInfo, vmaInfo); };

        // 
        VmaImageAllocation(const vkt::uni_ptr<VmaImageAllocation>& allocation) : allocation(allocation->allocation), allocationInfo(allocation->allocationInfo), allocator(allocation->allocator) { *this = allocation; };
        VmaImageAllocation(const vkt::uni_ptr<ImageAllocation>& allocation) { *this = allocation.dyn_cast<VmaImageAllocation>(); };

        // 
        VmaImageAllocation(const std::shared_ptr<VmaImageAllocation>& allocation) : allocation(allocation->allocation), allocationInfo(allocation->allocationInfo), allocator(allocation->allocator) { *this = vkt::uni_ptr<VmaImageAllocation>(allocation); };
        VmaImageAllocation(const std::shared_ptr<ImageAllocation>& allocation) { *this = dynamic_cast<VmaImageAllocation&>(*vkt::uni_ptr<ImageAllocation>(allocation)); };

        //
        ~VmaImageAllocation() {
            this->info.deviceDispatch->DeviceWaitIdle();
            vmaDestroyImage(allocator, image, allocation);
        };

        // 
        virtual VmaImageAllocation* construct(
            vkt::uni_arg<VmaAllocator> allocator,
            vkt::uni_arg<vkh::VkImageCreateInfo> createInfo = vkh::VkImageCreateInfo{},
            vkt::uni_arg<VmaMemoryInfo> memInfo = VmaMemoryInfo{ .memUsage = VMA_MEMORY_USAGE_GPU_ONLY }
        ) {
            VmaAllocationCreateInfo vmaInfo = {}; vmaInfo.usage = memInfo->memUsage;
            if (memInfo->memUsage == VMA_MEMORY_USAGE_CPU_TO_GPU || memInfo->memUsage == VMA_MEMORY_USAGE_GPU_TO_CPU) { vmaInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT; };
            vkh::handleVk(vmaCreateImage(this->allocator = allocator.ref(), *createInfo, &vmaInfo, &reinterpret_cast<VkImage&>(image), &this->allocation, &this->allocationInfo));
            this->info.initialLayout = VkImageLayout(createInfo->initialLayout);
            this->info.range = allocationInfo.size;
            this->info.memory = allocationInfo.deviceMemory;
            this->info.memUsage = memInfo->memUsage;

            // Get Dispatch Loader From VMA Allocator Itself!
            VmaAllocatorInfo info = {};
            vmaGetAllocatorInfo(this->allocator = allocator.ref(), &info);
            //this->info.dispatch = VkDispatchLoaderDynamic(info.instance, vkGetInstanceProcAddr, this->info.device = info.device, vkGetDeviceProcAddr); // 

            // 
            this->info.instanceDispatch = memInfo->instanceDispatch;
            this->info.deviceDispatch = memInfo->deviceDispatch;

            //
            if (!this->info.deviceDispatch) { this->info.deviceDispatch = vkGlobal::device; };
            if (!this->info.instanceDispatch) { this->info.instanceDispatch = vkGlobal::instance; };

            // when loader initialized
            if (vkt::vkGlobal::initialized) {
                if (!this->info.instanceDispatch) this->info.instanceDispatch = std::make_shared<xvk::Instance>(vkt::vkGlobal::loader.get(), info.instance);
                if (!this->info.deviceDispatch) this->info.deviceDispatch = std::make_shared<xvk::Device>(this->info.instanceDispatch, info.device);
            };

            // reload device and instance
            if (!this->info.device) { this->info.device = info.device; };
            if (!this->info.instance) { this->info.instance = info.instance; };

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

        // Allocation
        virtual operator const VmaAllocation& () const { return allocation; };
        virtual operator const VmaAllocationInfo& () const { return allocationInfo; };

        // 
        virtual operator VmaAllocation& () { return allocation; };
        virtual operator VmaAllocationInfo& () { return allocationInfo; };

        // 
        virtual VmaImageAllocation* address() { return this; };
        virtual const VmaImageAllocation* address() const { return this; };

    // 
    protected: friend VmaImageAllocation; friend ImageAllocation; // 
        VmaAllocationInfo allocationInfo = {};
        VmaAllocation allocation = VK_NULL_HANDLE;
        VmaAllocator allocator = VK_NULL_HANDLE;
    };

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
        ImageRegion() {};
        ImageRegion(const vkt::uni_ptr<ImageRegion>& region) { *this = region; };
        ImageRegion(const vkt::uni_ptr<ImageAllocation>& allocation, vkt::uni_arg<vkh::VkImageViewCreateInfo> info = vkh::VkImageViewCreateInfo{}, vkt::uni_arg<VkImageLayout> layout = VK_IMAGE_LAYOUT_GENERAL) : allocation(allocation), subresourceRange(info->subresourceRange) { this->construct(allocation, info, layout); };
        ImageRegion(const vkt::uni_ptr<VmaImageAllocation>& allocation, vkt::uni_arg<vkh::VkImageViewCreateInfo> info = vkh::VkImageViewCreateInfo{}, vkt::uni_arg<VkImageLayout> layout = VK_IMAGE_LAYOUT_GENERAL) : allocation(allocation.dyn_cast<ImageAllocation>()), subresourceRange(info->subresourceRange) { this->construct(allocation.dyn_cast<ImageAllocation>(), info, layout); };
        ImageRegion(const std::shared_ptr<ImageAllocation>& allocation, vkt::uni_arg<vkh::VkImageViewCreateInfo> info = vkh::VkImageViewCreateInfo{}, vkt::uni_arg<VkImageLayout> layout = VK_IMAGE_LAYOUT_GENERAL) : allocation(allocation), subresourceRange(info->subresourceRange) { this->construct(allocation, info, layout); };
        ImageRegion(const std::shared_ptr<VmaImageAllocation>& allocation, vkt::uni_arg<vkh::VkImageViewCreateInfo> info = vkh::VkImageViewCreateInfo{}, vkt::uni_arg<VkImageLayout> layout = VK_IMAGE_LAYOUT_GENERAL) : allocation(std::dynamic_pointer_cast<ImageAllocation>(allocation)), subresourceRange(info->subresourceRange) { this->construct(std::dynamic_pointer_cast<ImageAllocation>(allocation), info, layout); };
        ~ImageRegion() {
            
        };

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
            vkh::handleVk(this->allocation->info.deviceDispatch->CreateImageView(*info, nullptr, &this->imgInfo.imageView));

            // 
#ifdef ENABLE_OPENGL_INTEROP
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
        virtual ImageRegion* construct(
            vkt::uni_ptr<VmaImageAllocation> allocation,
            vkt::uni_arg<vkh::VkImageViewCreateInfo> info = vkh::VkImageViewCreateInfo{},
            vkt::uni_arg<VkImageLayout> layout = VK_IMAGE_LAYOUT_GENERAL
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
        virtual vkt::uni_ptr<ImageAllocation>& uniPtr() { return this->allocation; };
        virtual const vkt::uni_ptr<ImageAllocation>& uniPtr() const { return this->allocation; };

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

        virtual VkSampler& refSampler() { return this->imgInfo.sampler; };
        virtual const VkSampler& refSampler() const { return this->imgInfo.sampler; };

        virtual ImageRegion& transfer(VkCommandBuffer& cmdBuf) const {
            vkt::imageBarrier(cmdBuf, vkt::ImageBarrierInfo{
                .instanceDispatch = this->allocation->info.instanceDispatch,
                .deviceDispatch = this->allocation->info.deviceDispatch,
                .image = this->allocation->getImage(),
                .targetLayout = reinterpret_cast<const VkImageLayout&>(this->imgInfo.imageLayout),
                .originLayout = this->allocation->info.initialLayout,
                .subresourceRange = this->subresourceRange
            });
            const_cast<ImageRegion*>(this)->allocation->info.initialLayout = reinterpret_cast<const VkImageLayout&>(this->imgInfo.imageLayout);
            return const_cast<ImageRegion&>(*this);
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
        virtual operator VkImage&() { return *this->allocation; };
        virtual operator VkDevice&() { return *this->allocation; };
        virtual operator VkSampler&() { return this->imgInfo.sampler; };

        // 
        virtual operator const ImageAllocation*() const { return this->allocation; };
        virtual operator const vkt::uni_ptr<ImageAllocation>&() const { return this->allocation; };
        virtual operator const std::shared_ptr<ImageAllocation>& () const { return this->allocation; };
        virtual operator const vkt::MemoryAllocationInfo&() const { return this->allocation->info; };
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
        virtual operator const vkh::VkImageSubresourceLayers& () const { return this->subresourceLayers(); };
        virtual operator const    ::VkImageSubresourceLayers& () const { return this->subresourceLayers(); };

        // 
        virtual operator const VkImageSubresourceLayers() const { return VkImageSubresourceLayers{ reinterpret_cast<const VkImageAspectFlags&>(subresourceRange.aspectMask), subresourceRange.baseMipLevel, subresourceRange.baseArrayLayer, subresourceRange.layerCount }; };

#ifdef ENABLE_OPENGL_INTEROP // Bindless Textures Directly
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
#ifdef ENABLE_NVX_IMAGE_ADDRESS
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
        virtual vkh::VkImageSubresourceLayers getImageSubresourceLayers() const { return this->subresourceLayers(); };

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
        virtual vkh::VkDescriptorImageInfo* getDescriptorPtr() { return &imgInfo; };
        virtual const vkh::VkDescriptorImageInfo* getDescriptorPtr() const { return &imgInfo; };

    protected: friend VmaImageAllocation; friend ImageAllocation; // 
        vkh::VkDescriptorImageInfo imgInfo = { .sampler = VK_NULL_HANDLE, .imageView = VK_NULL_HANDLE, .imageLayout = VK_IMAGE_LAYOUT_UNDEFINED };
        vkt::uni_ptr<ImageAllocation> allocation = {};

    public: // irrevalent sources
        vkh::VkImageSubresourceRange subresourceRange = {};
    };

};
