#pragma once
#include "System/FileChangesMonitor.h"
#include "System/typesalias.h"
#include "System/strutils.h"
#include "System/stlcontainersintegration.h"
#include "Graphics/GraphicsShader.h"
#include "Graphics/GraphicsDevice.h"
#include <queue>
//#include <hash_map> //use in future instead of map?
#include <mutex>
#include <fstream>

static const std::wstring ShadersRootPath = L"Data/Shaders/";

typedef FNVhash_t ShaderID;
typedef std::vector<GraphicsShaderMacro> ShaderVariation;

template<class T>
class CompilableShader
{
public:
	CompilableShader(const std::wstring& filePath, const std::vector<ShaderVariation>& shaderVariations = {}) : m_Changed(true), m_FilePath(filePath), m_ShaderVariations(shaderVariations) { Load(ShadersRootPath + filePath); }

	//bool m_Loaded;
	std::wstring m_FilePath;
	std::vector<ShaderVariation> m_ShaderVariations;
	std::string m_Content;

	ShaderID GetVariationID(size_t variationIndex) const
	{
		ShaderID id = GetShaderID(m_FilePath, m_ShaderVariations[variationIndex]);
		return GetShaderID(m_FilePath, m_ShaderVariations[variationIndex]);
	}
	size_t GetVariationsCount() const
	{
		return m_ShaderVariations.size();
	}

	bool Load(const std::wstring& filePath)
	{
		std::ifstream shaderFile(filePath);
        std::stringstream buffer;
        buffer << shaderFile.rdbuf();

        m_Content = buffer.str();//std::string(std::istreambuf_iterator<char>(shaderFile), std::istreambuf_iterator<char>());

        bool isf = shaderFile.fail();
		shaderFile.close();

        /*shaderFile.open(filePath);
        //std::stringstream buffer;
        std::string buffer2;
        shaderFile >> buffer2;//buffer2 << shaderFile.rdbuf();
        shaderFile.close();*/
        //m_Content = buffer.str();//std::string(std::istreambuf_iterator<char>(shaderFile), std::istreambuf_iterator<char>());
        

        //assert(m_Content.size() != 0);
        if (m_Content.size() != 0)
            m_Changed = true;

        return m_Changed;
	}
	T Compile(GraphicsDevice& device, size_t variationIndex)
	{
        m_Changed = false;
		return T::CreateFromTextContent(device, ShadersRootPath + m_FilePath, m_Content, m_ShaderVariations[variationIndex]);
	}

    bool IsChanged() const
    {
        return m_Changed;
    }

    void SetChanged()
    {
        m_Changed = true;
    }
private:
    bool m_Changed;
};

ShaderID GetShaderID(const std::wstring& filePath, const std::vector<GraphicsShaderMacro>& shaderMacros);

class ShadersCollection
{
public:
	ShadersCollection(GraphicsDevice& device);

	template<class T>
	void AddShader(const std::wstring& filePath, const std::vector<ShaderVariation>& shaderMacros = {})
	{
		LOG(std::string("Shader type is not supported"));
		popAssert(false);
	}

#define DeclAddShader(type, v) \
    template<> \
    void AddShader<##type>(const std::wstring& filePath, const std::vector<ShaderVariation>& shaderMacros) \
    { \
        std::lock_guard<std::mutex> lockGuard(m_ShadersLoadingMutex); \
        ##v .push_back(CompilableShader< ##type >(filePath, shaderMacros)); \
    }

    DeclAddShader(GraphicsVertexShader, m_CompilableVertexShaders)
    DeclAddShader(GraphicsPixelShader, m_CompilablePixelShaders)
    DeclAddShader(GraphicsComputeShader, m_CompilableComputeShaders)
    DeclAddShader(GraphicsHullShader, m_CompilableHullShaders)
    DeclAddShader(GraphicsDomainShader, m_CompilableDomainShaders)
  
    
	template<class T>
	T GetShader(const std::wstring& filePath, const std::vector<GraphicsShaderMacro>& shaderMacros)
	{
		LOG(std::string("Shader type is not supported"));
		popAssert(false);
		return T();
	}


#define DeclGetShader(type, v) \
    template<> \
    ##type GetShader< ##type >(const std::wstring& filePath, const std::vector<GraphicsShaderMacro>& shaderMacros) \
    { \
       std::lock_guard<std::mutex> lockGuard(m_ShadersLoadingMutex); \
       ShaderID id = GetShaderID(filePath, shaderMacros);  \
       return (* ##v .find(id)).second; \
    }

    DeclGetShader(GraphicsVertexShader, m_VertexShaders)
    DeclGetShader(GraphicsPixelShader, m_PixelShaders)
    DeclGetShader(GraphicsComputeShader, m_ComputeShaders)
    DeclGetShader(GraphicsDomainShader, m_DomainShaders)
    DeclGetShader(GraphicsHullShader, m_HullShaders)


	template<class T>
	T GetShader(ShaderID)
	{
		LOG(std::string("Shader type is not supported"));
		popAssert(false);
		return T();
	}

#define DeclGetShaderByID(type, v) \
    template<> \
    ##type GetShader< ##type >(ShaderID id) \
    { \
       std::lock_guard<std::mutex> lockGuard(m_ShadersLoadingMutex); \
       return (* ##v .find(id)).second; \
    }

    DeclGetShaderByID(GraphicsVertexShader, m_VertexShaders)
    DeclGetShaderByID(GraphicsPixelShader, m_PixelShaders)
    DeclGetShaderByID(GraphicsComputeShader, m_ComputeShaders)
    DeclGetShaderByID(GraphicsDomainShader, m_DomainShaders)
    DeclGetShaderByID(GraphicsHullShader, m_HullShaders)

#define DeclFindComilableShader(type, fname, v) \
    CompilableShader<##type>& ##fname(const std::wstring& file) \
    {\
        for (size_t i = 0; i < ##v.size(); i++) \
            if (##v[i].m_FilePath == (file)) \
            return (##v[i]); \
    }

    DeclFindComilableShader(GraphicsVertexShader, FindVertexCompilableShader, m_CompilableVertexShaders)
    DeclFindComilableShader(GraphicsPixelShader, FindPixelCompilableShader, m_CompilablePixelShaders)

    /*CompilableShader<GraphicsVertexShader>& FindVertexCompilableShader(const std::wstring& file)
    {
        for (size_t i = 0; i < m_CompilableVertexShaders.size(); i++)
            if (m_CompilableVertexShaders[i].m_FilePath == (file))
                return m_CompilableVertexShaders[i];
    }
    CompilableShader<GraphicsPixelShader>& FindPixelCompilableShader(const std::wstring& file)
    {
        for (size_t i = 0; i < m_CompilablePixelShaders.size(); i++)
            if (m_CompilablePixelShaders[i].m_FilePath == (file))
                return m_CompilablePixelShaders[i];
    }
    CompilableShader<GraphicsComputeShader>& FindComputeCompilableShader(const std::wstring& file)
    {
        for (size_t i = 0; i < m_CompilableComputeShaders.size(); i++)
            if (m_CompilableComputeShaders[i].m_FilePath == (file))
                return m_CompilableComputeShaders[i];
    }
    CompilableShader<GraphicsHullShader>& FindHullCompilableShader(const std::wstring& file)
    {
        for (size_t i = 0; i < m_CompilableHullShaders.size(); i++)
            if (m_CompilableHullShaders[i].m_FilePath == (file))
                return m_CompilableHullShaders[i];
    }
    CompilableShader<GraphicsDomainShader>& FindDomainCompilableShader(const std::wstring& file)
    {
        for (size_t i = 0; i < m_CompilableDomainShaders.size(); i++)
            if (m_CompilableDomainShaders[i].m_FilePath == (file))
                return m_CompilableDomainShaders[i];
    }*/

	std::wstring GetShadersDirFullPath() const;

	void ExecuteShadersCompilation(GraphicsDevice& device);

	std::mutex m_ShadersMutex;
    std::mutex m_ShadersLoadingMutex;
private:
	void CalculatePaths();
	std::wstring ReducePath(const std::wstring& path);
	void ShaderMonitorThreadFunc();

	//paths stuff
	unsigned long m_RootDirRank;
	unsigned long m_ShadersRootDirRank;
	std::wstring m_ShadersRootPathRelativelyToMainRootPath; // has format "x1/x2/x3/"(attention to last char)

	FileChangesMonitor m_FileChangesMonitor;
    std::vector<std::wstring> m_ShadersChanged;

	std::vector<CompilableShader<GraphicsVertexShader>> m_CompilableVertexShaders;
	std::vector<CompilableShader<GraphicsPixelShader>> m_CompilablePixelShaders;
    std::vector<CompilableShader<GraphicsComputeShader>> m_CompilableComputeShaders;
    std::vector<CompilableShader<GraphicsHullShader>> m_CompilableHullShaders;
    std::vector<CompilableShader<GraphicsDomainShader>> m_CompilableDomainShaders;
	STLMap<FNVhash_t, GraphicsVertexShader> m_VertexShaders;
	STLMap<FNVhash_t, GraphicsPixelShader>  m_PixelShaders;
    STLMap<FNVhash_t, GraphicsComputeShader>  m_ComputeShaders;
    STLMap<FNVhash_t, GraphicsHullShader>  m_HullShaders;
    STLMap<FNVhash_t, GraphicsDomainShader>  m_DomainShaders;

	GraphicsDevice& m_Device;
};