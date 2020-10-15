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
        std::vector<const char*> extensions = {};
        std::vector<const char*> layers = {};

        // get required extensions
        std::vector<const char*> wantedExtensions = wantedInstanceExtensions;

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
                //wantedExtensions.push_back(glfwExtensions[i]);
            };
    #endif

            // get our needed extensions
            std::string layerName = "";
            std::vector<VkExtensionProperties> installedExtensions = {};
            vkh::vsEnumerateInstanceExtensionProperties(vkGlobal::loader, installedExtensions, layerName);
            auto extensions = std::vector<const char*>();
            for (auto w : wantedExtensions) {
                for (auto i : installedExtensions)
                {
                    if (strcmp(w, i.extensionName) == 0)
                    {
                        extensions.emplace_back(w);
                        break;
                    }
                }
            }

            std::vector<const char*> wantedLayers = wantedInstanceLayers;

            // get validation layers
            std::vector<VkLayerProperties> installedLayers = {};
            vkh::vsEnumerateInstanceLayerProperties(vkGlobal::loader, installedLayers);
            auto layers = std::vector<const char*>();
            for (auto w : wantedLayers) {
                for (auto i : installedLayers)
                {
                    if (strcmp(w, i.layerName) == 0)
                    {
                        layers.emplace_back(w);
                        break;
                    }
                }
            }

            //
            this->extensions = extensions;
            this->layers = layers;

            // app info
            auto appinfo = vkh::VkApplicationInfo{};
            appinfo.pNext = nullptr;
            appinfo.pApplicationName = "VKTest";
            appinfo.apiVersion = VK_MAKE_VERSION(1, 2, 135);

            // create instance info
            auto cinstanceinfo = vkh::VkInstanceCreateInfo{};
            cinstanceinfo.pApplicationInfo = &(this->applicationInfo = appinfo); // due JabaCPP unable to access
            cinstanceinfo.enabledExtensionCount = static_cast<uint32_t>(this->extensions.size());
            cinstanceinfo.ppEnabledExtensionNames = this->extensions.data();
            cinstanceinfo.enabledLayerCount = static_cast<uint32_t>(this->layers.size());
            cinstanceinfo.ppEnabledLayerNames = this->layers.data();

            // Dynamically Load the Vulkan library
            this->dispatch = std::make_shared<xvk::Instance>(vkt::vkGlobal::loader.get(), (this->createInfo = cinstanceinfo));
            this->instance = this->dispatch->handle;
            vkt::vkGlobal::instance = this->dispatch.get_shared();

            // get physical device for application
            physicalDevices = vkh::vsEnumeratePhysicalDevices(this->dispatch.get_shared());

            //
            //uint32_t count = 0u; vkh::handleVk(this->instanceDispatch->EnumeratePhysicalDevices(&count, nullptr));
            //this->physicalDevices.resize(count);
            //vkh::handleVk(this->instanceDispatch->EnumeratePhysicalDevices(&count, this->physicalDevices.data()));

            // 
    #ifdef VKT_VULKAN_DEBUG
            if (CreateDebugUtilsMessengerEXT(instance, &debugCreateInfo, nullptr, &reinterpret_cast<VkDebugUtilsMessengerEXT&>(messenger)) != VK_SUCCESS) {
                throw std::runtime_error("failed to set up debug callback");
            }
    #endif

            // 
            return instance;
        }
    };

};
