#pragma once

#include <vector>
#include <string>

namespace vkf {
    // default device layers
    const std::vector<const char*> wantedDeviceLayers_CStr = {
        "VK_LAYER_AMD_switchable_graphics"
        //"VK_LAYER_KHRONOS_validation",
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

    // default device extensions
    const std::vector<const char*> wantedDeviceExtensions_CStr = {
        "VK_EXT_swapchain_colorspace",
        "VK_EXT_external_memory_host",
        "VK_EXT_queue_family_foreign",
        "VK_EXT_sample_locations",
        "VK_EXT_conservative_rasterization",
        "VK_EXT_hdr_metadata",
        "VK_EXT_transform_feedback",
        "VK_EXT_extended_dynamic_state",

        // 
        "VK_EXT_descriptor_indexing",
        "VK_KHR_descriptor_update_template",
        "VK_KHR_push_descriptor",

        // 
        "VK_AMD_gcn_shader",
        "VK_AMD_shader_info",
        "VK_AMD_texture_gather_bias_lod",
        "VK_AMD_shader_image_load_store_lod",
        "VK_AMD_shader_trinary_minmax",
        "VK_AMD_draw_indirect_count",

        // Would To Be Have Multi-Vector Solution
        "VK_AMD_shader_explicit_vertex_parameter",
        "VK_NV_fragment_shader_barycentric",

        // 
        "VK_EXT_sampler_filter_minmax",
        "VK_KHR_sampler_ycbcr_conversion",
        "VK_KHR_sampler_mirror_clamp_to_edge",
        "VK_KHR_imageless_framebuffer",

        // 
        "VK_KHR_bind_memory2",
        "VK_KHR_maintenance1",
        "VK_KHR_maintenance2",
        "VK_KHR_maintenance3",
        "VK_KHR_driver_properties",
        "VK_KHR_image_format_list",
        "VK_KHR_dedicated_allocation",
        "VK_KHR_imageless_framebuffer",
        "VK_KHR_shader_draw_parameters",
        "VK_KHR_get_memory_requirements2",

        // 
        "VK_EXT_scalar_block_layout",
        "VK_KHR_storage_buffer_storage_class",
        "VK_KHR_relaxed_block_layout",
        "VK_KHR_variable_pointers",
        "VK_AMD_buffer_marker",

        // 
        "VK_EXT_subgroup_size_control",
        "VK_KHR_shader_subgroup_extended_types",

        // 
        "VK_KHR_16bit_storage",
        "VK_KHR_8bit_storage",
        "VK_AMD_gpu_shader_int16",
        "VK_AMD_gpu_shader_half_float",
        "VK_KHX_shader_explicit_arithmetic_types",
        "VK_KHR_shader_atomic_int64",
        "VK_KHR_shader_float16_int8",
        "VK_KHR_shader_float_controls",
        "VK_EXT_shader_atomic_float",

        // 
        "VK_KHR_timeline_semaphore",
        "VK_KHR_incremental_present",
        "VK_KHR_ray_tracing",
        "VK_KHR_ray_query",
        "VK_KHR_pipeline_library",
        "VK_KHR_deferred_host_operations",
        "VK_KHR_buffer_device_address",
        "VK_KHR_vulkan_memory_model",
        "VK_KHR_shader_clock",
        "VK_KHR_swapchain",
        "VK_KHR_surface",
        "VK_KHR_display",

        // 
        "VK_NV_device_diagnostic_checkpoints",
        "VK_NV_compute_shader_derivatives",
        "VK_NV_corner_sampled_image",
        "VK_NV_shader_image_footprint",
        "VK_NV_shader_subgroup_partitioned",
        "VK_NV_shader_sm_builtins",
        "VK_NV_ray_tracing",

        //
        "VK_KHR_external_fence",
        "VK_KHR_external_fence_capabilities",
        "VK_KHR_external_fence_win32",
        //"VK_KHR_external_fence_fd",

        // 
        "VK_KHR_external_memory",
        "VK_KHR_external_memory_capabilities",
        "VK_KHR_external_memory_win32",
        //"VK_KHR_external_memory_fd",

        // 
        "VK_KHR_external_semaphore",
        "VK_KHR_external_semaphore_capabilities",
        "VK_KHR_external_semaphore_win32",
        //"VK_KHR_external_semaphore_fd",

        // 
        "VK_NV_external_memory",
        "VK_NV_external_memory_capabilities",
        "VK_NV_external_memory_win32",

        //
        "VK_NVX_image_view_handle",
        "VK_EXT_validation_cache",
        "VK_EXT_validation_features",
        "VK_EXT_validation_flags",
        "VK_EXT_debug_marker",
        "VK_EXT_debug_report",
        "VK_EXT_debug_utils",

        //
        "VK_KHR_buffer_device_address",
        //"VK_EXT_buffer_device_address",
        "VK_NVX_image_view_handle",
        "VK_KHR_acceleration_structure",
        "VK_KHR_ray_tracing_pipeline",
        "VK_KHR_ray_query",
        "VK_EXT_inline_uniform_block",
        "VK_KHR_copy_commands2"
    };
};
