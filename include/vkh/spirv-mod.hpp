//
// Created by helixd on 01.07.2020.
//
#pragma once

//
#ifndef LANCER_SPIRV_MOD_H
#define LANCER_SPIRV_MOD_H

//
#include <vector>
#include <string>
#include <unordered_map>

//
#include <spirv/unified1/GLSL.std.450.h>
#include <spirv/unified1/spirv.hpp>
#include <spirv_cross_c.h>

//
namespace vkh {

    static inline spvc_bool g_fail_on_error = SPVC_TRUE;

    static inline void error_callback(void *userdata, const char *error) {
        (void) userdata;
        if (g_fail_on_error) {
            fprintf(stderr, "Error: %s\n", error);
            //exit(1);
        } else
            printf("Expected error hit: %s.\n", error);
    };

    struct SOSemantic {
        uint32_t buffer = 0u;
        uint32_t offset = 0u;
        uint32_t stride = 0u;
    };

    static inline std::vector<uint32_t> StreamOutputChange(const std::vector<uint32_t>& unModSource, const std::unordered_map<int, SOSemantic>& semantics) {
        auto modSource = unModSource;

        //
        std::unordered_map<int, std::string> namings = {};
        std::unordered_map<std::string, int> mapping = {};
        std::unordered_map<int, int> strides = {};
        std::unordered_map<int, int> offsets = {};
        std::unordered_map<int, int> buffers = {};
        std::unordered_map<int, int> outputs = {};

        // Where Located That Decorations? (for future usage)
        std::unordered_map<int, size_t> mappingExist = {};
        std::unordered_map<int, size_t> stridesExist = {};
        std::unordered_map<int, size_t> offsetsExist = {};
        std::unordered_map<int, size_t> buffersExist = {};
        std::unordered_map<int, size_t> outputsExist = {};

        // Getting for names and per names...
        // Checking if decorations exists!
        // 16-bit OpCode, 16-bit length, 32-bit NameID, 32-bit DecorationType, 32-bit value
        for (int i = 5; i != unModSource.size(); i += (unModSource[i] >> 16)) {
            spv::Op op = spv::Op(unModSource[i] & 0xffff);
            if (op == spv::Op::OpDecorate) {
                int name = unModSource[i + 1];
                {
                    if (spv::Decoration(unModSource[i + 2]) == spv::Decoration::DecorationXfbStride) {
                        stridesExist[name] = i;
                    };
                    if (spv::Decoration(unModSource[i + 2]) == spv::Decoration::DecorationOffset) {
                        offsetsExist[name] = i;
                    };
                    if (spv::Decoration(unModSource[i + 2]) == spv::Decoration::DecorationXfbBuffer) {
                        buffersExist[name] = i;
                    };
                };
            } else if (op == spv::Op::OpName) {
                mapping[(const char *) &unModSource[i + 2]] = unModSource[i + 1];//i, (i + (unModSource[i] >> 16));
                namings[unModSource[i + 1]] = (const char *) &unModSource[i + 2];
            };
        };

        //
        {
            spvc_context context = NULL;
            spvc_parsed_ir ir = NULL;
            spvc_compiler compiler = NULL;
            spvc_compiler_options options = NULL;
            spvc_resources resources = NULL;
            const spvc_reflected_resource *list = NULL;
            const char *result = NULL;
            size_t count = size_t(0ll), i = size_t(0ll);

            //
            spvc_context_create(&context);
            spvc_context_set_error_callback(context, error_callback, NULL);
            spvc_context_parse_spirv(context, unModSource.data(), unModSource.size(), &ir);
            spvc_context_create_compiler(context, SPVC_BACKEND_NONE, ir, SPVC_CAPTURE_MODE_TAKE_OWNERSHIP, &compiler);
            spvc_compiler_create_shader_resources(compiler, &resources);

            // output names
            spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_STAGE_OUTPUT, &list, &count);
            for (i = 0; i < count; i++) {
                //locations[list[i].id] = spvc_compiler_get_decoration(compiler_glsl, list[i].id, SpvDecorationLocation);
                outputs[list[i].id] = spvc_compiler_get_decoration(compiler, list[i].id, SpvDecorationLocation);
                offsets[list[i].id] = spvc_compiler_get_decoration(compiler, list[i].id, SpvDecorationOffset);
                strides[list[i].id] = spvc_compiler_get_decoration(compiler, list[i].id, SpvDecorationXfbStride);
                buffers[list[i].id] = spvc_compiler_get_decoration(compiler, list[i].id, SpvDecorationXfbBuffer);
                //names[list[i].id] = list[i].name;
            };
        };

        // Да-да, в ручную, "ручками"...
        uint32_t I = 0;
        while (I < 5 && outputs.size() > 0) {
            const auto loc = outputs.begin();
            bool done = false;
            for (int i = 5; i < modSource.size(); i += std::max(uint64_t(modSource[i] >> 16), uint64_t(1ull))) {
                spv::Op op = spv::Op(modSource[i] & 0xffff);
                if (op == spv::Op::OpDecorate) {
                    const auto nameID = modSource[i + 1];
                    //std::string name = namings[modSource[i + 1]];
                    if (nameID == loc->first && semantics.find(loc->second) != semantics.end() && 
                        spv::Decoration(modSource[i + 2]) == spv::Decoration::DecorationLocation) {
                        const auto shift = i + (modSource[i] >> 16);

                        // Place Stride info
                        if (strides.find(nameID) == strides.end() || stridesExist.find(nameID) == stridesExist.end()) {
                            modSource.insert(modSource.begin() + shift, {
                                    (spv::Op::OpDecorate | (4u << 16u)),
                                    uint32_t(nameID),
                                    spv::Decoration::DecorationXfbStride,
                                    semantics[loc->second].stride
                            });
                        };

                        // Place Buffers info
                        if (buffers.find(nameID) == buffers.end() || buffersExist.find(nameID) == buffersExist.end()) {
                            modSource.insert(modSource.begin() + shift, {
                                    (spv::Op::OpDecorate | (4u << 16u)),
                                    uint32_t(nameID),
                                    spv::Decoration::DecorationXfbBuffer,
                                    semantics[loc->second].buffer
                            });
                        };

                        // Place Offset info
                        if (offsets.find(nameID) == offsets.end() || offsetsExist.find(nameID) == offsetsExist.end()) {
                            modSource.insert(modSource.begin() + shift, {
                                    (spv::Op::OpDecorate | (4u << 16u)),
                                    uint32_t(nameID),
                                    spv::Decoration::DecorationOffset,
                                    semantics[loc->second].offset
                            });
                        };

                        // Placed! Let's Restart! (For Correct)
                        {
                            done = true;
                            break;
                        }; //
                    };
                };
            };
            outputs.erase(outputs.begin());
            I++;
        };

        return modSource;
    };

};

#endif //LANCER_SPIRV_MOD_H
