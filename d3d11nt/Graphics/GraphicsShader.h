#pragma once
#include <string>
#include <limits>
#include <d3dcompiler.h>
#include "Graphics/DX11Object.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/dxlogicsafety.h"
#include "System/typesalias.h"
#include "System/arrayutil.h"

void CreateShader(GraphicsDevice& device, ID3DBlob* blob, ID3D11VertexShader** shader);
void CreateShader(GraphicsDevice& device, ID3DBlob* blob, ID3D11PixelShader** shader);
void CreateShader(GraphicsDevice& device, ID3DBlob* blob, ID3D11HullShader** shader);
void CreateShader(GraphicsDevice& device, ID3DBlob* blob, ID3D11DomainShader** shader);
void CreateShader(GraphicsDevice& device, ID3DBlob* blob, ID3D11GeometryShader** shader);

enum GraphicsShaderType { GraphicsShaderType_Vertex = 0,
						  GraphicsShaderType_Pixel,
                          GraphicsShaderType_Invalid };
enum GraphicsShaderMask { GraphicsShaderMask_Vertex = (1u << GraphicsShaderType_Vertex),
						  GraphicsShaderMask_Pixel  = (1u << GraphicsShaderType_Pixel) };
typedef unsigned long GraphicsShaderMaskType;

static const unsigned long GraphicsShaderType_Count = GraphicsShaderType_Invalid;

static const std::string ShaderEntryPointNameBase = "Entry";
static const std::string ShaderPerTypeEntryPointNamePrefixes[] = { "VS", "PS" };
static const std::string ShaderPerTypeTargetNamePrefixes[] = { "vs_", "ps_" };
static_assert(popGetArraySize(ShaderPerTypeEntryPointNamePrefixes) == popGetArraySize(ShaderPerTypeTargetNamePrefixes), "");


template<class T>
struct ShaderTypeIndex
{
	static const index_t value = GraphicsShaderType_Invalid;
};
template<>
struct ShaderTypeIndex<ID3D11VertexShader>
{
	static const index_t value = GraphicsShaderType_Vertex;
};
template<>
struct ShaderTypeIndex<ID3D11PixelShader>
{
	static const index_t value = GraphicsShaderType_Pixel;
};

template<class T>
index_t GetShaderTypeIndex()
{
	index_t shaderTypeIndex = ShaderTypeIndex<T>::value;
	popAssert(shaderTypeIndex != GraphicsShaderType_Invalid);
	return shaderTypeIndex;
}

template<class T>
std::string GetShaderD3DTarget(const std::string& targetVersionStr = "5_0")
{
	return ShaderPerTypeTargetNamePrefixes[GetShaderTypeIndex<T>()] + targetVersionStr;
}
template<class T>
std::string GetShaderEntryPointName()
{
	return ShaderPerTypeEntryPointNamePrefixes[GetShaderTypeIndex<T>()] + ShaderEntryPointNameBase;
}

void BindShader(GraphicsDevice& device, ID3D11VertexShader* vertexShader);
void BindShader(GraphicsDevice& device, ID3D11PixelShader* pixelShader);

template<class T>
class GraphicsShader : public DX11MultipleObjectsHolder<IUnknown, 2>
{
private:
	static const index_t BLOB_INDEX = 0;
	static const index_t SHADER_INDEX = 1;

#define popBlobDX11OBject m_DX11Objects[BLOB_INDEX]
#define popShaderDX11Object m_DX11Objects[SHADER_INDEX]

public:
	GraphicsShader(GraphicsDevice& device, const std::string& fileName)
	{
		ID3DBlob* errorMsgs = nullptr;
		const D3D_SHADER_MACRO* shaderMacro = nullptr;
		D3D_HR_OP(D3DCompileFromFile(strtowstr(fileName).c_str(), shaderMacro, D3D_COMPILE_STANDARD_FILE_INCLUDE,
									 GetShaderEntryPointName<T>().c_str(), GetShaderD3DTarget<T>().c_str(), 0, 0,
									 (ID3DBlob**)&popBlobDX11OBject.GetRef(), &errorMsgs));
		if (errorMsgs)
		{
			LOG_FILE(std::string((char*)errorMsgs->GetBufferPointer()));
			errorMsgs->Release();
		}

		CreateShader(device, (ID3DBlob*)popBlobDX11OBject.Get(), (T**)&popShaderDX11Object.GetRef());
#ifdef _DEBUG
		GetShader()->SetPrivateData(WKPDID_D3DDebugObjectName, fileName.size(), fileName.c_str());
#endif
	}

	virtual ~GraphicsShader()
	{
		ReleaseDX11Objects();
	}

	void Bind(GraphicsDevice& device)
	{
		BindShader(device, GetShader());
	}

    ID3DBlob* GetBlob() const
	{
		return (ID3DBlob*)popBlobDX11OBject.Get();
	}

	T* GetShader() const
	{
		return (T*)popShaderDX11Object.Get();
	}

private:

};


typedef GraphicsShader<ID3D11VertexShader> GraphicsVertexShader;
typedef GraphicsShader<ID3D11PixelShader>  GraphicsPixelShader;