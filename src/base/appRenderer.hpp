#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "vkt/fw.hpp"
//#include <tinygltf/tiny_gltf.h>

namespace rnd {

    // renderer biggest class 
    class Renderer : public std::enable_shared_from_this<Renderer> {
    public:
        GLFWwindow* window = nullptr; // bound GLFW window
        const uint32_t SGHZ = 16;

        operator vk::Instance& () { return appBase->instance; }
        operator const vk::Instance& () const { return appBase->instance; }
        operator vk::Queue& () { return appBase->queue; }
        operator const vk::Queue& () const { return appBase->queue; }
        operator vk::RenderPass& () { return appBase->renderpass; }
        operator const vk::RenderPass& () const { return appBase->renderpass; }

        // 
        std::shared_ptr<vkt::ComputeFramework> appBase = nullptr;

        vk::SwapchainKHR swapchain = {};
        std::vector<vkt::Framebuffer> framebuffers = {};

        // RadX-based object system
        std::shared_ptr<radx::Device> device;

        // just helpers 
        std::shared_ptr<radx::PhysicalDeviceHelper> physicalHelper;
        std::shared_ptr<radx::VmaAllocatedBuffer> vmaDeviceBuffer, vmaToHostBuffer, vmaHostBuffer;//, vmaToDeviceBuffer;

        // output image allocation
        std::shared_ptr<radx::VmaAllocatedImage> outputImage;

        // Pipeline Layout
        vk::Pipeline trianglePipeline;
        vk::PipelineLayout trianglePipelineLayout;
        vk::DescriptorSet inputDescriptorSet;
        vk::DescriptorSetLayout inputDescriptorLayout;

        double tPastFrameTime = 0.f;
        float guiScale = 1.0f;
        uint32_t canvasWidth = 1, canvasHeight = 1; // canvas size with SSAA accounting
        uint32_t windowWidth = 1, windowHeight = 1; // virtual window size (without DPI recalculation)
        uint32_t realWidth = 1, realHeight = 1; // real window size (with DPI accounting)
        uint32_t gpuID = 0;//1;

        // current rendering state
        int32_t currSemaphore = -1; uint32_t currentBuffer = 0;

        // names and directories
        std::string title = "vkt";
        std::string shaderPrefix = "./", shaderPack = "./shaders/";

        // methods for rendering
        void Arguments(int argc = 0, char** argv = nullptr);
        void Init(uint32_t canvasWidth, uint32_t canvasHeight, bool enableSuperSampling = false);
        void InitPipeline();
        void Draw();
        void HandleData();

    };

};
