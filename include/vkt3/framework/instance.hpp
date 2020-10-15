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

        VktInstance(){};

        operator VkInstance&(){
            return instance;
        };

        operator const VkInstance&() const {
            return instance;
        };

        virtual VkInstance& create(){
            assert((version = vkh::vsEnumerateInstanceVersion(vkGlobal::loader)) >= VK_MAKE_VERSION(1, 2, 131));

            // get required extensions
    #ifdef VKT_USE_GLFW
    #ifdef VKT_GLFW_LINKED
            uint32_t glfwExtensionCount = 0;
            const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

            // add glfw extensions to list
            for (uint32_t i = 0; i < glfwExtensionCount; i++) {
                wantedExtensions.push_back(glfwExtensions[i]);
            };
    #endif
    #endif

            // get our needed extensions
            auto installedExtensions = vkh::vsEnumerateInstanceExtensionProperties(vkGlobal::loader);
            auto extensions = std::vector<const char*>();
            for (auto w : wantedExtensions) {
                for (auto i : installedExtensions)
                {
                    if (std::string(i.extensionName).compare(w) == 0)
                    {
                        extensions.emplace_back(w);
                        break;
                    }
                }
            }

            // get validation layers
            auto installedLayers = vkh::vsEnumerateInstanceLayerProperties(vkGlobal::loader);
            auto layers = std::vector<const char*>();
            for (auto w : wantedLayers) {
                for (auto i : installedLayers)
                {
                    if (std::string(i.layerName).compare(w) == 0)
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
