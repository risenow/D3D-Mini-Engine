#include "stdafx.h"
#include "Graphics/GraphicsShader.h"
#include "Graphics/dxlogicsafety.h"
#include "System/Log.h"
#include "System/logicsafety.h"
#include "System/strutils.h"

std::string ShaderTypeToD3DTarget(GraphicsShader::ShaderType type, const std::string& targetVersionStr = "5_0")
{
    std::string vertexShaderPrefix = "vs_";
    std::string pixelShaderPrefix = "ps_";
    switch (type)
    {
    case GraphicsShader::ShaderType_Vertex:
        return vertexShaderPrefix + targetVersionStr;
        break;
    case GraphicsShader::ShaderType_Pixel:
        return "ps_5_0";
        break;
    default:
        popAssert(false);
        return "";
    }
}

std::string GetEntryPointNameForShaderWithType(GraphicsShader::ShaderType type)
{
    const std::string base = "Entry";
    std::string prefix = "";
    switch (type)
    {
    case GraphicsShader::ShaderType_Vertex:
        prefix = "VS";
        break;
    case GraphicsShader::ShaderType_Pixel:
        prefix = "PS";
        break;
    default:
        popAssert(false);
        break;
    }

    return prefix + base;
}

GraphicsShader::GraphicsShader(const std::string& fileName, ShaderType type)
{
    ID3DBlob* errorMsgs = nullptr;
    const D3D_SHADER_MACRO* shaderMacro = nullptr;
    D3D_HR_OP(D3DCompileFromFile(strtowstr(fileName).c_str(), shaderMacro, D3D_COMPILE_STANDARD_FILE_INCLUDE, 
                                 GetEntryPointNameForShaderWithType(type).c_str(), ShaderTypeToD3DTarget(type).c_str(), 0, 0, 
                                 (ID3DBlob**)&m_DX11Object.GetRef(), &errorMsgs));
    LOG_FILE(std::string((char*)errorMsgs->GetBufferPointer()));
    errorMsgs->Release();
}
ID3DBlob* GraphicsShader::GetBlob() const
{
    return (ID3DBlob*)m_DX11Object.Get();
}