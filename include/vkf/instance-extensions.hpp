#include <vector>
#include <string>

namespace vkf {
    // instance extensions
    const std::vector<const char*> wantedInstanceExtensions_CStr = {
        "VK_KHR_get_physical_device_properties2",
        "VK_KHR_get_surface_capabilities2",

        "VK_EXT_direct_mode_display",
        "VK_EXT_swapchain_colorspace",

        "VK_KHR_swapchain",
        "VK_KHR_surface",
        "VK_KHR_display",

        "VK_EXT_debug_marker",
        "VK_EXT_debug_report",
        "VK_EXT_debug_utils",

        "VK_KHR_win32_surface",

        "VK_NV_device_diagnostic_checkpoints"
    };

    // instance layers
    const std::vector<const char*> wantedInstanceLayers_CStr = {
        "VK_LAYER_KHRONOS_validation",
        "VK_LAYER_LUNARG_assistant_layer",
        "VK_LAYER_LUNARG_standard_validation",
        "VK_LAYER_LUNARG_parameter_validation",
        "VK_LAYER_LUNARG_core_validation",

        //"VK_LAYER_LUNARG_api_dump",
        //"VK_LAYER_LUNARG_object_tracker",
        //"VK_LAYER_LUNARG_device_simulation",
        "VK_LAYER_GOOGLE_threading",
        "VK_LAYER_GOOGLE_unique_objects"
        "VK_LAYER_RENDERDOC_Capture"
    };
};
