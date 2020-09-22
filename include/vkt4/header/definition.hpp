#pragma once // #

// 
#ifndef VKT_DEFINITION_H
#define VKT_DEFINITION_H

// 
#ifdef USE_CIMG
#include "tinyexr.h"
#define cimg_plugin "CImg/tinyexr_plugin.hpp"
//#define cimg_use_png
//#define cimg_use_jpeg
#include "CImg.h"
#endif

// 
//#include <misc/args.hxx>
//#include <misc/half.hpp>
//#include <misc/pcg_random.hpp>

// 
//#include <stdio.h>
//#include <stdlib.h>
//#include <time.h>

// 
#include <cmath>
#include <cfenv>
#include <ios>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <functional>
#include <memory>
#include <vector>
#include <thread>
#include <atomic>


// 
#include "core.hpp"

// 
#include <vkh/structures.hpp>

// 
#ifdef ENABLE_OPTIX_DENOISE
#include <cuda.h>
#include <cuda_runtime.h>
#include <optix/optix.h>
#include <optix/optix_stubs.h>
#include <optix/optix_types.h>
#endif


#endif
