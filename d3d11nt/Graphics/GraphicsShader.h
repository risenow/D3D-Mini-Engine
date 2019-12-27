#pragma once
#include <string>
#include <vector>
#include <limits>
#include <d3dcompiler.h>
#include "Graphics/DX11Object.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/dxlogicsafety.h"
#include "System/strutils.h"
#include "System/typesalias.h"
#include "System/arrayutil.h"

void CreateShader(GraphicsDevice& device, ID3DBlob* blob, ID3D11VertexShader** shader);
void CreateShader(GraphicsDevice& device, ID3DBlob* blob, ID3D11PixelShader** shader);
void CreateShader(GraphicsDevice& device, ID3DBlob* blob, ID3D11HullShader** shader);
void CreateShader(GraphicsDevice& device, ID3DBlob* blob, ID3D11DomainShader** shader);
void CreateShader(GraphicsDevice& device, ID3DBlob* blob, ID3D11GeometryShader** shader);
void CreateShader(GraphicsDevice& device, ID3DBlob* blob, ID3D11ComputeShader** shader);

enum GraphicsShaderType { GraphicsShaderType_Vertex = 0,
						  GraphicsShaderType_Pixel,
                          GraphicsShaderType_Compute,
                          GraphicsShaderType_Hull,
                          GraphicsShaderType_Domain,
                          GraphicsShaderType_Invalid };
enum GraphicsShaderMask { GraphicsShaderMask_Vertex = (1u << GraphicsShaderType_Vertex),
						  GraphicsShaderMask_Pixel  = (1u << GraphicsShaderType_Pixel),
                          GraphicsShaderMask_Compute  = (1u << GraphicsShaderType_Compute),
                          GraphicsShaderMask_Hull = (1u << GraphicsShaderType_Hull),
                          GraphicsShaderMask_Domain = (1u << GraphicsShaderType_Domain)
};
typedef unsigned long GraphicsShaderMaskType;

static const unsigned long GraphicsShaderType_Count = GraphicsShaderType_Invalid;

static const std::string ShaderEntryPointNameBase = "Entry";
static const std::string ShaderPerTypeEntryPointNamePrefixes[] = { "VS", "PS", "CS", "HS", "DS"};
static const std::string ShaderPerTypeTargetNamePrefixes[] = { "vs_", "ps_", "cs_", "hs_", "ds_" };
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
template<>
struct ShaderTypeIndex<ID3D11ComputeShader>
{
    static const index_t value = GraphicsShaderType_Compute;
};
template<>
struct ShaderTypeIndex<ID3D11HullShader>
{
    static const index_t value = GraphicsShaderType_Hull;
};
template<>
struct ShaderTypeIndex<ID3D11DomainShader>
{
    static const index_t value = GraphicsShaderType_Domain;
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
void BindShader(GraphicsDevice& device, ID3D11ComputeShader* computeShader);
void BindShader(GraphicsDevice& device, ID3D11HullShader* hullShader);
void BindShader(GraphicsDevice& device, ID3D11DomainShader* domainShader);

struct GraphicsShaderMacro
{
	GraphicsShaderMacro(const std::string& name, const std::string& value) : m_Name(name), m_Value(value)
	{}

	std::string m_Name;
	std::string m_Value;
};

void GetD3DShaderMacros(const std::vector<GraphicsShaderMacro>& inShaderMacros, std::vector<D3D_SHADER_MACRO>& outD3DShaderMacros);

template<class T>
class GraphicsShader
{
private:
	static const index_t BLOB_INDEX = 0;
	static const index_t SHADER_INDEX = 1;

#define popBlobDX11OBject m_DX11Objects[BLOB_INDEX]
#define popShaderDX11Object m_DX11Objects[SHADER_INDEX]

public:
	GraphicsShader()
	{}
	GraphicsShader(GraphicsDevice& device, const std::wstring& fileName, const std::vector<GraphicsShaderMacro>& graphicsShaderMacros = {})
	{
		std::string narrowFileName = wstrtostr(fileName);
		*this = GraphicsShader(device, narrowFileName, graphicsShaderMacros);
	}
	GraphicsShader(GraphicsDevice& device, const std::string& fileName, const std::vector<GraphicsShaderMacro>& graphicsShaderMacros = {})
	{
		ID3DBlob* errorMsgs = nullptr;

		std::vector<D3D_SHADER_MACRO> shaderMacros;
		GetD3DShaderMacros(graphicsShaderMacros, shaderMacros);

		//D3D_HR_OP(D3DCompileFromFile(strtowstr(fileName).c_str(), shaderMacros.data(), D3D_COMPILE_STANDARD_FILE_INCLUDE,
		//							 GetShaderEntryPointName<T>().c_str(), GetShaderD3DTarget<T>().c_str(), D3DCOMPILE_PREFER_FLOW_CONTROL | D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
		//							 (ID3DBlob**)&popBlobDX11OBject.GetRef(), &errorMsgs));
		if (errorMsgs)
		{
			LOG_FILE(std::string((char*)errorMsgs->GetBufferPointer()));
            std::string serror = std::string((char*)errorMsgs->GetBufferPointer());
			errorMsgs->Release();
		}

        std::string target = GetShaderD3DTarget<T>();
        std::string entryPoint = GetShaderEntryPointName<T>().c_str();

		CreateShader(device, (ID3DBlob*)m_Blob, (T**)&m_Shader);
#ifdef _DEBUG
		GetShader()->SetPrivateData(WKPDID_D3DDebugObjectName, fileName.size(), fileName.c_str());
#endif
	}

	static GraphicsShader<T> CreateFromTextContent(GraphicsDevice& device, const std::wstring& filePath, const std::string& content, const std::vector<GraphicsShaderMacro>& graphicsShaderMacros = {})
	{
		GraphicsShader<T> shader;

		ID3DBlob* errorMsgs = nullptr;

		std::vector<D3D_SHADER_MACRO> shaderMacros;
		GetD3DShaderMacros(graphicsShaderMacros, shaderMacros);

		D3D_SHADER_MACRO* macrosTest = shaderMacros.data();
		macrosTest++;
        std::string target = GetShaderD3DTarget<T>();
        std::string entry = GetShaderEntryPointName<T>();

		D3D_HR_OP(D3DCompile(content.c_str(), content.size(), wstrtostr_fast(filePath).c_str(), shaderMacros.data(), D3D_COMPILE_STANDARD_FILE_INCLUDE,
            GetShaderEntryPointName<T>().c_str(), GetShaderD3DTarget<T>().c_str(), D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_ENABLE_STRICTNESS |D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR, 0,
			(ID3DBlob**)&shader.GetBlobRef(), &errorMsgs));
		if (errorMsgs)
		{
			LOG(std::string((char*)errorMsgs->GetBufferPointer()));
			errorMsgs->Release();
		}

		CreateShader(device, (ID3DBlob*)shader.GetBlob(), (T**)&shader.GetShaderRef());
#ifdef _DEBUG
		//shader.GetShader()->SetPrivateData(WKPDID_D3DDebugObjectName, fileName.size(), fileName.c_str());
#endif

		return shader;
	}

	virtual ~GraphicsShader()
	{
		//ReleaseDX11Objects();
	}

	void Bind(GraphicsDevice& device)
	{
		BindShader(device, GetShader());
	}

    ID3DBlob* GetBlob() const
	{
		return (ID3DBlob*)m_Blob;
	}

	T* GetShader() const
	{
		return (T*)m_Shader;
	}

    ID3DBlob*& GetBlobRef()
    {
        return m_Blob;
    }

    typedef T* Tptr;

    Tptr& GetShaderRef()
    {
        return m_Shader;
    }

	bool IsValid() const
	{
		return (m_Shader != nullptr);
	}

    void ReleaseGPUData()
    {
        m_Blob->Release();
        m_Blob = nullptr;

        m_Shader->Release();
        m_Shader = nullptr;
    }
private:
    ID3DBlob* m_Blob;
    T* m_Shader;
    size_t m_VariationID; //????
};


typedef GraphicsShader<ID3D11VertexShader> GraphicsVertexShader;
typedef GraphicsShader<ID3D11PixelShader>  GraphicsPixelShader;
typedef GraphicsShader<ID3D11ComputeShader> GraphicsComputeShader;
typedef GraphicsShader<ID3D11HullShader>   GraphicsHullShader;
typedef GraphicsShader<ID3D11DomainShader> GraphicsDomainShader;