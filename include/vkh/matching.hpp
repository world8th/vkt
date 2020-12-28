#pragma once // #

// TODO: REMOVE GLM REQUIREMENTS
#ifndef VKT_CORE_USE_GLM
#define VKT_CORE_USE_GLM
#endif

//
#include "./core.hpp"
#include "./enums.hpp"
#include "./bitfields.hpp"

//
namespace vkh {

    /// Description of blocks for compressed formats.
    struct BlockParams {
        uint8_t blockWidth;
        uint8_t blockHeight;
        uint8_t bytesPerBlock;
    };

    /// Get the details of vulkan texture formats.
    // TODO: Remove Vulkan-HPP Requirements
    inline BlockParams getBlockParams(VkFormat format) {
#ifdef VULKAN_HPP
        switch (vk::Format(format)) {
            case vk::Format::eR4G4UnormPack8: return BlockParams{1, 1, 1};
            case vk::Format::eR4G4B4A4UnormPack16: return BlockParams{1, 1, 2};
            case vk::Format::eB4G4R4A4UnormPack16: return BlockParams{1, 1, 2};
            case vk::Format::eR5G6B5UnormPack16: return BlockParams{1, 1, 2};
            case vk::Format::eB5G6R5UnormPack16: return BlockParams{1, 1, 2};
            case vk::Format::eR5G5B5A1UnormPack16: return BlockParams{1, 1, 2};
            case vk::Format::eB5G5R5A1UnormPack16: return BlockParams{1, 1, 2};
            case vk::Format::eA1R5G5B5UnormPack16: return BlockParams{1, 1, 2};
            case vk::Format::eR8Unorm: return BlockParams{1, 1, 1};
            case vk::Format::eR8Snorm: return BlockParams{1, 1, 1};
            case vk::Format::eR8Uscaled: return BlockParams{1, 1, 1};
            case vk::Format::eR8Sscaled: return BlockParams{1, 1, 1};
            case vk::Format::eR8Uint: return BlockParams{1, 1, 1};
            case vk::Format::eR8Sint: return BlockParams{1, 1, 1};
            case vk::Format::eR8Srgb: return BlockParams{1, 1, 1};
            case vk::Format::eR8G8Unorm: return BlockParams{1, 1, 2};
            case vk::Format::eR8G8Snorm: return BlockParams{1, 1, 2};
            case vk::Format::eR8G8Uscaled: return BlockParams{1, 1, 2};
            case vk::Format::eR8G8Sscaled: return BlockParams{1, 1, 2};
            case vk::Format::eR8G8Uint: return BlockParams{1, 1, 2};
            case vk::Format::eR8G8Sint: return BlockParams{1, 1, 2};
            case vk::Format::eR8G8Srgb: return BlockParams{1, 1, 2};
            case vk::Format::eR8G8B8Unorm: return BlockParams{1, 1, 3};
            case vk::Format::eR8G8B8Snorm: return BlockParams{1, 1, 3};
            case vk::Format::eR8G8B8Uscaled: return BlockParams{1, 1, 3};
            case vk::Format::eR8G8B8Sscaled: return BlockParams{1, 1, 3};
            case vk::Format::eR8G8B8Uint: return BlockParams{1, 1, 3};
            case vk::Format::eR8G8B8Sint: return BlockParams{1, 1, 3};
            case vk::Format::eR8G8B8Srgb: return BlockParams{1, 1, 3};
            case vk::Format::eB8G8R8Unorm: return BlockParams{1, 1, 3};
            case vk::Format::eB8G8R8Snorm: return BlockParams{1, 1, 3};
            case vk::Format::eB8G8R8Uscaled: return BlockParams{1, 1, 3};
            case vk::Format::eB8G8R8Sscaled: return BlockParams{1, 1, 3};
            case vk::Format::eB8G8R8Uint: return BlockParams{1, 1, 3};
            case vk::Format::eB8G8R8Sint: return BlockParams{1, 1, 3};
            case vk::Format::eB8G8R8Srgb: return BlockParams{1, 1, 3};
            case vk::Format::eR8G8B8A8Unorm: return BlockParams{1, 1, 4};
            case vk::Format::eR8G8B8A8Snorm: return BlockParams{1, 1, 4};
            case vk::Format::eR8G8B8A8Uscaled: return BlockParams{1, 1, 4};
            case vk::Format::eR8G8B8A8Sscaled: return BlockParams{1, 1, 4};
            case vk::Format::eR8G8B8A8Uint: return BlockParams{1, 1, 4};
            case vk::Format::eR8G8B8A8Sint: return BlockParams{1, 1, 4};
            case vk::Format::eR8G8B8A8Srgb: return BlockParams{1, 1, 4};
            case vk::Format::eB8G8R8A8Unorm: return BlockParams{1, 1, 4};
            case vk::Format::eB8G8R8A8Snorm: return BlockParams{1, 1, 4};
            case vk::Format::eB8G8R8A8Uscaled: return BlockParams{1, 1, 4};
            case vk::Format::eB8G8R8A8Sscaled: return BlockParams{1, 1, 4};
            case vk::Format::eB8G8R8A8Uint: return BlockParams{1, 1, 4};
            case vk::Format::eB8G8R8A8Sint: return BlockParams{1, 1, 4};
            case vk::Format::eB8G8R8A8Srgb: return BlockParams{1, 1, 4};
            case vk::Format::eA8B8G8R8UnormPack32: return BlockParams{1, 1, 4};
            case vk::Format::eA8B8G8R8SnormPack32: return BlockParams{1, 1, 4};
            case vk::Format::eA8B8G8R8UscaledPack32: return BlockParams{1, 1, 4};
            case vk::Format::eA8B8G8R8SscaledPack32: return BlockParams{1, 1, 4};
            case vk::Format::eA8B8G8R8UintPack32: return BlockParams{1, 1, 4};
            case vk::Format::eA8B8G8R8SintPack32: return BlockParams{1, 1, 4};
            case vk::Format::eA8B8G8R8SrgbPack32: return BlockParams{1, 1, 4};
            case vk::Format::eA2R10G10B10UnormPack32: return BlockParams{1, 1, 4};
            case vk::Format::eA2R10G10B10SnormPack32: return BlockParams{1, 1, 4};
            case vk::Format::eA2R10G10B10UscaledPack32: return BlockParams{1, 1, 4};
            case vk::Format::eA2R10G10B10SscaledPack32: return BlockParams{1, 1, 4};
            case vk::Format::eA2R10G10B10UintPack32: return BlockParams{1, 1, 4};
            case vk::Format::eA2R10G10B10SintPack32: return BlockParams{1, 1, 4};
            case vk::Format::eA2B10G10R10UnormPack32: return BlockParams{1, 1, 4};
            case vk::Format::eA2B10G10R10SnormPack32: return BlockParams{1, 1, 4};
            case vk::Format::eA2B10G10R10UscaledPack32: return BlockParams{1, 1, 4};
            case vk::Format::eA2B10G10R10SscaledPack32: return BlockParams{1, 1, 4};
            case vk::Format::eA2B10G10R10UintPack32: return BlockParams{1, 1, 4};
            case vk::Format::eA2B10G10R10SintPack32: return BlockParams{1, 1, 4};
            case vk::Format::eR16Unorm: return BlockParams{1, 1, 2};
            case vk::Format::eR16Snorm: return BlockParams{1, 1, 2};
            case vk::Format::eR16Uscaled: return BlockParams{1, 1, 2};
            case vk::Format::eR16Sscaled: return BlockParams{1, 1, 2};
            case vk::Format::eR16Uint: return BlockParams{1, 1, 2};
            case vk::Format::eR16Sint: return BlockParams{1, 1, 2};
            case vk::Format::eR16Sfloat: return BlockParams{1, 1, 2};
            case vk::Format::eR16G16Unorm: return BlockParams{1, 1, 4};
            case vk::Format::eR16G16Snorm: return BlockParams{1, 1, 4};
            case vk::Format::eR16G16Uscaled: return BlockParams{1, 1, 4};
            case vk::Format::eR16G16Sscaled: return BlockParams{1, 1, 4};
            case vk::Format::eR16G16Uint: return BlockParams{1, 1, 4};
            case vk::Format::eR16G16Sint: return BlockParams{1, 1, 4};
            case vk::Format::eR16G16Sfloat: return BlockParams{1, 1, 4};
            case vk::Format::eR16G16B16Unorm: return BlockParams{1, 1, 6};
            case vk::Format::eR16G16B16Snorm: return BlockParams{1, 1, 6};
            case vk::Format::eR16G16B16Uscaled: return BlockParams{1, 1, 6};
            case vk::Format::eR16G16B16Sscaled: return BlockParams{1, 1, 6};
            case vk::Format::eR16G16B16Uint: return BlockParams{1, 1, 6};
            case vk::Format::eR16G16B16Sint: return BlockParams{1, 1, 6};
            case vk::Format::eR16G16B16Sfloat: return BlockParams{1, 1, 6};
            case vk::Format::eR16G16B16A16Unorm: return BlockParams{1, 1, 8};
            case vk::Format::eR16G16B16A16Snorm: return BlockParams{1, 1, 8};
            case vk::Format::eR16G16B16A16Uscaled: return BlockParams{1, 1, 8};
            case vk::Format::eR16G16B16A16Sscaled: return BlockParams{1, 1, 8};
            case vk::Format::eR16G16B16A16Uint: return BlockParams{1, 1, 8};
            case vk::Format::eR16G16B16A16Sint: return BlockParams{1, 1, 8};
            case vk::Format::eR16G16B16A16Sfloat: return BlockParams{1, 1, 8};
            case vk::Format::eR32Uint: return BlockParams{1, 1, 4};
            case vk::Format::eR32Sint: return BlockParams{1, 1, 4};
            case vk::Format::eR32Sfloat: return BlockParams{1, 1, 4};
            case vk::Format::eR32G32Uint: return BlockParams{1, 1, 8};
            case vk::Format::eR32G32Sint: return BlockParams{1, 1, 8};
            case vk::Format::eR32G32Sfloat: return BlockParams{1, 1, 8};
            case vk::Format::eR32G32B32Uint: return BlockParams{1, 1, 12};
            case vk::Format::eR32G32B32Sint: return BlockParams{1, 1, 12};
            case vk::Format::eR32G32B32Sfloat: return BlockParams{1, 1, 12};
            case vk::Format::eR32G32B32A32Uint: return BlockParams{1, 1, 16};
            case vk::Format::eR32G32B32A32Sint: return BlockParams{1, 1, 16};
            case vk::Format::eR32G32B32A32Sfloat: return BlockParams{1, 1, 16};
            case vk::Format::eR64Uint: return BlockParams{1, 1, 8};
            case vk::Format::eR64Sint: return BlockParams{1, 1, 8};
            case vk::Format::eR64Sfloat: return BlockParams{1, 1, 8};
            case vk::Format::eR64G64Uint: return BlockParams{1, 1, 16};
            case vk::Format::eR64G64Sint: return BlockParams{1, 1, 16};
            case vk::Format::eR64G64Sfloat: return BlockParams{1, 1, 16};
            case vk::Format::eR64G64B64Uint: return BlockParams{1, 1, 24};
            case vk::Format::eR64G64B64Sint: return BlockParams{1, 1, 24};
            case vk::Format::eR64G64B64Sfloat: return BlockParams{1, 1, 24};
            case vk::Format::eR64G64B64A64Uint: return BlockParams{1, 1, 32};
            case vk::Format::eR64G64B64A64Sint: return BlockParams{1, 1, 32};
            case vk::Format::eR64G64B64A64Sfloat: return BlockParams{1, 1, 32};
            case vk::Format::eB10G11R11UfloatPack32: return BlockParams{1, 1, 4};
            case vk::Format::eE5B9G9R9UfloatPack32: return BlockParams{1, 1, 4};
            case vk::Format::eD16Unorm: return BlockParams{1, 1, 4};
            case vk::Format::eX8D24UnormPack32: return BlockParams{1, 1, 4};
            case vk::Format::eD32Sfloat: return BlockParams{1, 1, 4};
            case vk::Format::eS8Uint: return BlockParams{1, 1, 1};
            case vk::Format::eD16UnormS8Uint: return BlockParams{1, 1, 3};
            case vk::Format::eD24UnormS8Uint: return BlockParams{1, 1, 4};
            case vk::Format::eD32SfloatS8Uint: return BlockParams{0, 0, 0};
            case vk::Format::eBc1RgbUnormBlock: return BlockParams{4, 4, 8};
            case vk::Format::eBc1RgbSrgbBlock: return BlockParams{4, 4, 8};
            case vk::Format::eBc1RgbaUnormBlock: return BlockParams{4, 4, 8};
            case vk::Format::eBc1RgbaSrgbBlock: return BlockParams{4, 4, 8};
            case vk::Format::eBc2UnormBlock: return BlockParams{4, 4, 16};
            case vk::Format::eBc2SrgbBlock: return BlockParams{4, 4, 16};
            case vk::Format::eBc3UnormBlock: return BlockParams{4, 4, 16};
            case vk::Format::eBc3SrgbBlock: return BlockParams{4, 4, 16};
            case vk::Format::eBc4UnormBlock: return BlockParams{4, 4, 16};
            case vk::Format::eBc4SnormBlock: return BlockParams{4, 4, 16};
            case vk::Format::eBc5UnormBlock: return BlockParams{4, 4, 16};
            case vk::Format::eBc5SnormBlock: return BlockParams{4, 4, 16};
        }
#endif
        return BlockParams{0, 0, 0};
    }









}