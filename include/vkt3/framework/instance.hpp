#pragma once // #

#include "./incornation.hpp"
#include "./instance-extensions.hpp"

// 
namespace vkt {

    class VktInstance {
        public:
        vkt::Instance dispatch = {};
        vkh::VkInstanceCreateInfo createInfo = {};
        VkInstance instance = VK_NULL_HANDLE;
        uint32_t version = 0;
        
        vkh::VkApplicationInfo applicationInfo = {};
        VkDebugUtilsMessengerEXT messenger = VK_NULL_HANDLE;
        std::vector<VkPhysicalDevice> physicalDevices = {};

        //
        std::vector<std::string> extensions = {};
        std::vector<std::string> layers = {};

        // 
        const char** extensions_c_str = nullptr;
        const char** layers_c_str = nullptr;
        //std::vector<const char *> extensions_c_str = {};
        //std::vector<const char*> layers_c_str = {};

        // 
        VktInstance(){};

        operator VkInstance&(){
            return instance;
        };

        operator const VkInstance&() const {
            return instance;
        };

        virtual VkInstance& create(){
            vkt::vkGlobal();
            assert((version = vkh::vsEnumerateInstanceVersion(vkGlobal::loader)) >= VK_MAKE_VERSION(1, 2, 131));

    #ifdef VKT_USE_GLFW
            uint32_t glfwExtensionCount = 0;
            const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

            // add glfw extensions to list
            for (uint32_t i = 0; i < glfwExtensionCount; i++) {
                
            };
    #endif

            // get our needed extensions
            std::string layerName = "";
            std::vector<std::string> wantedExtensions = std::vector<std::string>(wantedInstanceExtensions_CStr.begin(), std::end(wantedInstanceExtensions_CStr));
            std::vector<VkExtensionProperties> installedExtensions = std::vector<VkExtensionProperties>();
            vkh::vsEnumerateInstanceExtensionProperties(vkGlobal::loader, installedExtensions, layerName);
            uint32_t extensionCount = 0u;
            for (auto& w : wantedExtensions) {
                for (auto& i : installedExtensions) {
                    if (w.compare(i.extensionName) == 0) { extensionCount++; break; };
                };
            };
            std::vector<std::string> extensions = std::vector<std::string>(extensionCount); extensionCount = 0u;
            for (auto& w : wantedExtensions) {
                for (auto& i : installedExtensions) {
                    if (w.compare(i.extensionName) == 0) { extensions[extensionCount++] = w; break; };
                };
            };
            this->extensions = extensions;


            // get validation layers
            std::vector<std::string> wantedLayers = std::vector<std::string>(wantedInstanceLayers_CStr.begin(), std::end(wantedInstanceLayers_CStr));
            std::vector<VkLayerProperties> installedLayers = std::vector<VkLayerProperties>();
            vkh::vsEnumerateInstanceLayerProperties(vkGlobal::loader, installedLayers);
            uint32_t layerCount = 0u;
            for (auto& w : wantedLayers) {
                for (auto& i : installedLayers) {
                    if (w.compare(i.layerName) == 0) { layerCount++; break; };
                };
            };
            std::vector<std::string> layers = std::vector<std::string>(layerCount); layerCount = 0u;
            for (auto& w : wantedLayers) {
                for (auto& i : installedLayers) {
                    if (w.compare(i.layerName) == 0) { layers[layerCount++] = w; break; };
                };
            };
            this->layers = layers;

            // app info
            auto appinfo = vkh::VkApplicationInfo{};
            appinfo.pNext = nullptr;
            appinfo.pApplicationName = "VKTest";
            appinfo.apiVersion = VK_MAKE_VERSION(1, 2, 135);

            // CANNOT DEALLOCATE!
            extensions_c_str = (const char**)malloc(extensionCount*sizeof(const char**));
            layers_c_str = (const char**)malloc(layerCount*sizeof(const char**));

            // 
            for (uint32_t i = 0; i < extensionCount; i++) { extensions_c_str[i] = extensions[i].c_str(); };
            for (uint32_t i = 0; i < layerCount; i++) { layers_c_str[i] = layers[i].c_str(); };

            // create instance info
            auto cinstanceinfo = vkh::VkInstanceCreateInfo{};
            cinstanceinfo.pApplicationInfo = &(this->applicationInfo = appinfo); // due JabaCPP unable to access
            cinstanceinfo.enabledExtensionCount = extensionCount;
            cinstanceinfo.ppEnabledExtensionNames = extensions_c_str;
            cinstanceinfo.enabledLayerCount = layerCount;
            cinstanceinfo.ppEnabledLayerNames = layers_c_str;

            // Dynamically Load the Vulkan library
            this->dispatch = std::make_shared<xvk::Instance>(vkt::vkGlobal::loader.get(), (this->createInfo = cinstanceinfo));
            this->instance = this->dispatch->handle;
            vkt::vkGlobal::instance = this->dispatch.get_shared();

            // get physical devices for application
            vkh::vsEnumeratePhysicalDevices(this->dispatch.get_shared(), physicalDevices);

            // 
            return instance;
        }
    };

};
