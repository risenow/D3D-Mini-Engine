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
	CompilableShader(const std::wstring& filePath, const std::vector<ShaderVariation>& shaderVariations = {}) : m_FilePath(filePath), m_ShaderVariations(shaderVariations) { Load(ShadersRootPath + filePath); }

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

	void Load(const std::wstring& filePath)
	{
		std::ifstream shaderFile(filePath);
		m_Content = std::string(std::istreambuf_iterator<char>(shaderFile), std::istreambuf_iterator<char>());
		shaderFile.close();
	}
	T Compile(GraphicsDevice& device, size_t variationIndex) const
	{
		//return T(device, ShadersRootPath + m_FilePath);
		return T::CreateFromTextContent(device, ShadersRootPath + m_FilePath, m_Content, m_ShaderVariations[variationIndex]);
	}
};

ShaderID GetShaderID(const std::wstring& filePath, const std::vector<GraphicsShaderMacro>& shaderMacros);

class ShadersCollection
{
public:
	ShadersCollection(GraphicsDevice& device);

	template<class T>
	void AddShader(const std::wstring& filePath, const std::vector<ShaderVariation>& shaderMacros = {})
	{
		LOG("Shader type is not supported");
		popAssert(false);
	}
	template<>
	void AddShader<GraphicsVertexShader>(const std::wstring& filePath, const std::vector<ShaderVariation>& shaderMacros) //filePath relatively to shaders root
	{
		std::lock_guard<std::mutex> lockGuard(m_ShadersMutex);
		m_CompilableVertexShaders.push(CompilableShader<GraphicsVertexShader>(filePath, shaderMacros));
	}
	template<>
	void AddShader<GraphicsPixelShader>(const std::wstring& filePath, const std::vector<ShaderVariation>& shaderMacros)
	{
		std::lock_guard<std::mutex> lockGuard(m_ShadersMutex);
		m_CompilablePixelShaders.push(CompilableShader<GraphicsPixelShader>(filePath, shaderMacros));
	}

	template<class T>
	T GetShader(const std::wstring& filePath, const std::vector<GraphicsShaderMacro>& shaderMacros)
	{
		LOG("Shader type is not supported");
		popAssert(false);
		return T();
	}
	template<>
	GraphicsVertexShader GetShader<GraphicsVertexShader>(const std::wstring& filePath, const std::vector<GraphicsShaderMacro>& shaderMacros)
	{
		ShaderID id = GetShaderID(filePath, shaderMacros);
		return (*m_VertexShaders.find(id)).second;
	}
	template<>
	GraphicsPixelShader GetShader<GraphicsPixelShader>(const std::wstring& filePath, const std::vector<GraphicsShaderMacro>& shaderMacros)
	{
		ShaderID id = GetShaderID(filePath, shaderMacros);
		return (*m_PixelShaders.find(id)).second;
	}

	template<class T>
	T GetShader(ShaderID)
	{
		LOG("Shader type is not supported");
		popAssert(false);
		return T();
	}
	template<>
	GraphicsVertexShader GetShader<GraphicsVertexShader>(ShaderID id)
	{
		return (*m_VertexShaders.find(id)).second;
	}
	template<>
	GraphicsPixelShader GetShader<GraphicsPixelShader>(ShaderID id)
	{
		return (*m_PixelShaders.find(id)).second;
	}

	std::wstring GetShadersDirFullPath() const;

	void ExecuteShadersCompilation(GraphicsDevice& device);

	std::mutex m_ShadersMutex;
private:
	void CalculatePaths();
	std::wstring ReducePath(const std::wstring& path);
	void ShaderMonitorThreadFunc();

	//paths stuff
	unsigned long m_RootDirRank;
	unsigned long m_ShadersRootDirRank;
	std::wstring m_ShadersRootPathRelativelyToMainRootPath; // has format "x1/x2/x3/"(attention to last char)

	FileChangesMonitor m_FileChangesMonitor;
	
	std::queue<CompilableShader<GraphicsVertexShader>> m_CompilableVertexShaders;
	std::queue<CompilableShader<GraphicsPixelShader>> m_CompilablePixelShaders;
	STLMap<FNVhash_t, GraphicsVertexShader> m_VertexShaders;
	STLMap<FNVhash_t, GraphicsPixelShader>  m_PixelShaders;
	//STLMap<std::string
	GraphicsDevice& m_Device;
};