#include "stdafx.h"
#include <thread>
#include <mutex>
#include <algorithm>
#include "Graphics/ShadersCollection.h"
#include "System/mtutils.h"
#include "System/pathutils.h"

const wchar_t* VERTEX_SHADER_FILE_TYPE = L".vs";
const wchar_t* PIXEL_SHADER_FILE_TYPE = L".ps";

ShaderID GetShaderID(const std::wstring& filePath, const std::vector<GraphicsShaderMacro>& shaderMacros)
{
	//std::string temp = wstrtostr(filePath);
	std::vector<std::string> aggregatedMacros(shaderMacros.size());
	for (size_t i = 0; i < shaderMacros.size(); i++)
	{
		const GraphicsShaderMacro& macro = shaderMacros[i];
		aggregatedMacros[i].reserve(macro.m_Name.size() + macro.m_Value.size());
		aggregatedMacros[i] += macro.m_Name;
		aggregatedMacros[i] += macro.m_Value;
	}
	std::sort(aggregatedMacros.begin(), aggregatedMacros.end());

	size_t hashSrcSize = filePath.size();
	for (const std::string& aggregatedMacro : aggregatedMacros)
	{
		hashSrcSize += aggregatedMacro.size();
	}
	std::string hashSrc;
	hashSrc.reserve(hashSrcSize);
	
	hashSrc += wstrtostr(filePath);
	for (const std::string& aggregatedMacro : aggregatedMacros)
	{
		hashSrc += aggregatedMacro;
	}

	return FNV(hashSrc.c_str(), hashSrc.size());
}

ShadersCollection::ShadersCollection(GraphicsDevice& device) : m_Device(device), m_FileChangesMonitor(GetShadersDirFullPath()) // to insert shaders dir here
{
	CalculatePaths();

	std::thread shadersChangesMonitorThread(&ShadersCollection::ShaderMonitorThreadFunc, this);
	shadersChangesMonitorThread.detach();
}

std::wstring GetFileTypeStr(const std::wstring& filePath) // returns .type
{
	long i = filePath.size() - 1;
	unsigned long dotIndex = 0;
	for (i; i >= 0; i--)
	{
		if (filePath[i] == L'.')
			dotIndex = i;
	}
	return filePath.substr(dotIndex, filePath.size() - i);
}

void ShadersCollection::ShaderMonitorThreadFunc()
{
	STLVector<std::wstring> changedFilesPaths;
	while (m_FileChangesMonitor.GatherChanges(changedFilesPaths, FileChangesMonitor::Flag_NoRepeatingFileNames))
	{
		//DynamicConditionalLockGuard lockGuard(m_ShadersMutex, changedFilesPaths.size() > 0);
		for (std::wstring& filePath : changedFilesPaths)
		{
			std::lock_guard<std::mutex> lockGuard(m_ShadersMutex);
			NormalizePath(filePath);
			std::wstring fileType = GetFileTypeStr(filePath);
			if (fileType == VERTEX_SHADER_FILE_TYPE)
			{
				m_CompilableVertexShaders.push(CompilableShader<GraphicsVertexShader>(filePath, { {} }));
				//decltype(m_VertexShaders)::iterator correspondingVertexShaderEntry = m_VertexShaders.find(filePath);
				//if (correspondingVertexShaderEntry != m_VertexShaders.end())
					//(*correspondingVertexShaderEntry).second = GraphicsVertexShader(m_Device, ShadersRootPath + filePath);
			} else
		    if (fileType == PIXEL_SHADER_FILE_TYPE)
			{
				m_CompilablePixelShaders.push(CompilableShader<GraphicsPixelShader>(filePath, { {} }));
				//decltype(m_PixelShaders)::iterator correspondingPixelShaderEntry = m_PixelShaders.find(filePath);
				//if (correspondingPixelShaderEntry != m_PixelShaders.end())
				//	(*correspondingPixelShaderEntry).second = GraphicsPixelShader(m_Device, ShadersRootPath + filePath);
			}
		}
		changedFilesPaths.resize(0);
	}
}

std::wstring ShadersCollection::ReducePath(const std::wstring& path)
{
	return ReducePathRank(path, m_ShadersRootDirRank - m_RootDirRank);
}

std::wstring ShadersCollection::GetShadersDirFullPath() const
{
	wchar_t buffer[1024];
	GetCurrentDirectory(1024, buffer);

	std::wstring rootPath = std::wstring(buffer);

	NormalizePath(rootPath);

	return rootPath + ShadersRootPath;
}

void ShadersCollection::ExecuteShadersCompilation(GraphicsDevice& device)
{
	while (m_CompilableVertexShaders.size())
	{
		CompilableShader<GraphicsVertexShader> compilableShader = m_CompilableVertexShaders.front();
		m_CompilableVertexShaders.pop();

		for (size_t i = 0; i < compilableShader.GetVariationsCount(); i++)
			m_VertexShaders[compilableShader.GetVariationID(i)] = compilableShader.Compile(device, i);
	}
	while (m_CompilablePixelShaders.size())
	{
		CompilableShader<GraphicsPixelShader> compilableShader = m_CompilablePixelShaders.front();
		m_CompilablePixelShaders.pop();

		for (size_t i = 0; i < compilableShader.GetVariationsCount(); i++)
			m_PixelShaders[compilableShader.GetVariationID(i)] = compilableShader.Compile(device, i);
	}
}

void ShadersCollection::CalculatePaths()
{
	wchar_t buffer[1024];
	GetCurrentDirectory(1024, buffer);
	NormalizePath(buffer);

	m_RootDirRank = GetPathRank(std::wstring(buffer));
	m_ShadersRootDirRank = GetPathRank(m_FileChangesMonitor.GetDirPath());

	m_ShadersRootPathRelativelyToMainRootPath = ReducePathRank(m_FileChangesMonitor.GetDirPath(), m_RootDirRank + 1) + L"/";
}