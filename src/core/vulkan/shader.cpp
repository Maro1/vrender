#include "shader.hpp"

#include "core/graphics_context.hpp"

#include "utils/log.hpp"

#include <SPIRV/GlslangToSpv.h>
#include <cstddef>
#include <fstream>
#include <iterator>
#include <type_traits>

namespace vrender
{


class ShaderIncluder : public glslang::TShader::Includer
{
public:
	IncludeResult *includeLocal(const char *headerName, const char *includerName, size_t inclusionDepth) override {
		auto directory = std::filesystem::path(includerName).parent_path();
		std::string fileStr = Shader::readFile(directory / headerName);

		auto content = new char[fileStr.size()];
		std::memcpy(content, fileStr.c_str(), fileStr.size());
		return new IncludeResult(headerName, content, fileStr.size(), content);
	}

	IncludeResult *includeSystem(const char *headerName, const char *includerName, size_t inclusionDepth) override {
		std::string fileStr = Shader::readFile(headerName);

		auto content = new char[fileStr.size()];
		std::memcpy(content, fileStr.c_str(), fileStr.size());
		return new IncludeResult(headerName, content, fileStr.size(), content);
	}

	void releaseInclude(IncludeResult *result) override {
		if (result) {
			delete[] static_cast<char *>(result->userData);
			delete result;
		}
	}

};

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
    m_VertModule = createShaderModule(vertexPath);
    m_FragModule = createShaderModule(fragmentPath);
}

Shader::~Shader()
{
    vkDestroyShaderModule(GraphicsContext::get().device()->device(), m_VertModule, nullptr);
    vkDestroyShaderModule(GraphicsContext::get().device()->device(), m_FragModule, nullptr);
}

TBuiltInResource Shader::shaderResources()
{
    TBuiltInResource resources;

    resources.maxLights                                 = 32;
    resources.maxClipPlanes                             = 6;
    resources.maxTextureUnits                           = 32;
    resources.maxTextureCoords                          = 32;
    resources.maxVertexAttribs                          = 64;
    resources.maxVertexUniformComponents                = 4096;
    resources.maxVaryingFloats                          = 64;
    resources.maxVertexTextureImageUnits                = 32;
    resources.maxCombinedTextureImageUnits              = 80;
    resources.maxTextureImageUnits                      = 32;
    resources.maxFragmentUniformComponents              = 4096;
    resources.maxDrawBuffers                            = 32;
    resources.maxVertexUniformVectors                   = 128;
    resources.maxVaryingVectors                         = 8;
    resources.maxFragmentUniformVectors                 = 16;
    resources.maxVertexOutputVectors                    = 16;
    resources.maxFragmentInputVectors                   = 15;
    resources.minProgramTexelOffset                     = -8;
    resources.maxProgramTexelOffset                     = 7;
    resources.maxClipDistances                          = 8;
    resources.maxComputeWorkGroupCountX                 = 65535;
    resources.maxComputeWorkGroupCountY                 = 65535;
    resources.maxComputeWorkGroupCountZ                 = 65535;
    resources.maxComputeWorkGroupSizeX                  = 1024;
    resources.maxComputeWorkGroupSizeY                  = 1024;
    resources.maxComputeWorkGroupSizeZ                  = 64;
    resources.maxComputeUniformComponents               = 1024;
    resources.maxComputeTextureImageUnits               = 16;
    resources.maxComputeImageUniforms                   = 8;
    resources.maxComputeAtomicCounters                  = 8;
    resources.maxComputeAtomicCounterBuffers            = 1;
    resources.maxVaryingComponents                      = 60;
    resources.maxVertexOutputComponents                 = 64;
    resources.maxGeometryInputComponents                = 64;
    resources.maxGeometryOutputComponents               = 128;
    resources.maxFragmentInputComponents                = 128;
    resources.maxImageUnits                             = 8;
    resources.maxCombinedImageUnitsAndFragmentOutputs   = 8;
    resources.maxCombinedShaderOutputResources          = 8;
    resources.maxImageSamples                           = 0;
    resources.maxVertexImageUniforms                    = 0;
    resources.maxTessControlImageUniforms               = 0;
    resources.maxTessEvaluationImageUniforms            = 0;
    resources.maxGeometryImageUniforms                  = 0;
    resources.maxFragmentImageUniforms                  = 8;
    resources.maxCombinedImageUniforms                  = 8;
    resources.maxGeometryTextureImageUnits              = 16;
    resources.maxGeometryOutputVertices                 = 256;
    resources.maxGeometryTotalOutputComponents          = 1024;
    resources.maxGeometryUniformComponents              = 1024;
    resources.maxGeometryVaryingComponents              = 64;
    resources.maxTessControlInputComponents             = 128;
    resources.maxTessControlOutputComponents            = 128;
    resources.maxTessControlTextureImageUnits           = 16;
    resources.maxTessControlUniformComponents           = 1024;
    resources.maxTessControlTotalOutputComponents       = 4096;
    resources.maxTessEvaluationInputComponents          = 128;
    resources.maxTessEvaluationOutputComponents         = 128;
    resources.maxTessEvaluationTextureImageUnits        = 16;
    resources.maxTessEvaluationUniformComponents        = 1024;
    resources.maxTessPatchComponents                    = 120;
    resources.maxPatchVertices                          = 32;
    resources.maxTessGenLevel                           = 64;
    resources.maxViewports                              = 16;
    resources.maxVertexAtomicCounters                   = 0;
    resources.maxTessControlAtomicCounters              = 0;
    resources.maxTessEvaluationAtomicCounters           = 0;
    resources.maxGeometryAtomicCounters                 = 0;
    resources.maxFragmentAtomicCounters                 = 8;
    resources.maxCombinedAtomicCounters                 = 8;
    resources.maxAtomicCounterBindings                  = 1;
    resources.maxVertexAtomicCounterBuffers             = 0;
    resources.maxTessControlAtomicCounterBuffers        = 0;
    resources.maxTessEvaluationAtomicCounterBuffers     = 0;
    resources.maxGeometryAtomicCounterBuffers           = 0;
    resources.maxFragmentAtomicCounterBuffers           = 1;
    resources.maxCombinedAtomicCounterBuffers           = 1;
    resources.maxAtomicCounterBufferSize                = 16384;
    resources.maxTransformFeedbackBuffers               = 4;
    resources.maxTransformFeedbackInterleavedComponents = 64;
    resources.maxCullDistances                          = 8;
    resources.maxCombinedClipAndCullDistances           = 8;
    resources.maxSamples                                = 4;
    resources.maxMeshOutputVerticesNV                   = 256;
    resources.maxMeshOutputPrimitivesNV                 = 512;
    resources.maxMeshWorkGroupSizeX_NV                  = 32;
    resources.maxMeshWorkGroupSizeY_NV                  = 1;
    resources.maxMeshWorkGroupSizeZ_NV                  = 1;
    resources.maxTaskWorkGroupSizeX_NV                  = 32;
    resources.maxTaskWorkGroupSizeY_NV                  = 1;
    resources.maxTaskWorkGroupSizeZ_NV                  = 1;
    resources.maxMeshViewCountNV                        = 4;

    resources.limits.nonInductiveForLoops                 = 1;
    resources.limits.whileLoops                           = 1;
    resources.limits.doWhileLoops                         = 1;
    resources.limits.generalUniformIndexing               = 1;
    resources.limits.generalAttributeMatrixVectorIndexing = 1;
    resources.limits.generalVaryingIndexing               = 1;
    resources.limits.generalSamplerIndexing               = 1;
    resources.limits.generalVariableIndexing              = 1;
    resources.limits.generalConstantMatrixVectorIndexing  = 1;

    return resources;
}

std::string Shader::readFile(const std::string& path)
{
    std::ifstream file(path);

        if (!file.is_open())
    {
        V_LOG_ERROR("Could not open shader file with path: {}", path);
        return std::string();
    }

    return std::string(std::istreambuf_iterator<char>{file}, {});
}

VkShaderModule Shader::createShaderModule(const std::string& path)
{
    auto stage = path.substr(path.find_last_of(".") + 1) == "vert" ? EShLangVertex : EShLangFragment;
    glslang::TProgram program;
    glslang::TShader shader(stage);

    auto contentStr = readFile(path);
    auto content = contentStr.c_str();
    shader.setStrings(&content, 1);
    shader.setEnvInput(glslang::EShSourceGlsl, stage, glslang::EShClientVulkan, 110);
    shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_1);
    shader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_5);

    TBuiltInResource resources = shaderResources();
    EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);

    ShaderIncluder includer;

    std::string preprocessed;
    if (!shader.preprocess(&resources, 450, ENoProfile, false, false, messages, &preprocessed,
                           includer))
    {
        V_LOG_WARNING(shader.getInfoLog());
        V_LOG_DEBUG(shader.getInfoDebugLog());
        V_LOG_ERROR("Could not pre-process shader at path: {}", path);
    }

    if (!shader.parse(&resources, 450, false, messages))
    {
        V_LOG_WARNING(shader.getInfoLog());
        V_LOG_DEBUG(shader.getInfoDebugLog());
        V_LOG_ERROR("Could not parse shader at path: {}", path);
    }

    program.addShader(&shader);
    program.link(messages);
    program.buildReflection();

    glslang::SpvOptions spvOptions;
#ifdef NDEBUG
    spvOptions.disableOptimizer = false;
    spvOptions.generateDebugInfo = false;
    spvOptions.optimizeSize = true;
#else
    spvOptions.disableOptimizer = true;
    spvOptions.generateDebugInfo = true;
    spvOptions.optimizeSize = false;
#endif

    std::vector<uint32_t> spirv;
    auto i = program.getIntermediate(stage);
    glslang::GlslangToSpv(*program.getIntermediate((EShLanguage) stage), spirv, &spvOptions);

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = spirv.size() * sizeof(uint32_t);
    createInfo.pCode = spirv.data();

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(GraphicsContext::get().device()->device(), &createInfo, nullptr, &shaderModule) !=
        VK_SUCCESS)
    {
        return VK_NULL_HANDLE;
    }
    return shaderModule;
}
}; // namespace vrender
