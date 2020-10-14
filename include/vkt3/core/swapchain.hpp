#pragma once // #

#include "./incornation.hpp"
#include "./instance.hpp"
#include "./device.hpp"

// 
namespace vkt {

    // TODO: SwapChain 
    class VktSwapChain {
        std::shared_ptr<VktInstance> instance = {};
        std::shared_ptr<VktDevice> device = {};

        VktSwapChain(){
            
        }
        VktSwapChain(std::shared_ptr<VktInstance> instance, std::shared_ptr<VktDevice> device) : instance(instance), device(device) {
            
        }
        VktSwapChain(std::shared_ptr<VktDevice> device) : instance(instance), device(device.instance) {
            
        }

        
    };
    
};
