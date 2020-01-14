#include "stdafx.h"
#include <thread>
#include <mutex>
#include <algorithm>
#include <set>
#include "Graphics/ShadersCollection.h"
#include "System/mtutils.h"
#include "System/pathutils.h"
#include "System/helperutil.h"

void GetAllMacrosCombinations(const std::vector<GraphicsShaderMacro>& macroSet, std::vector<ExtendedShaderVariation>& permutations, size_t hasAnyOfRule, size_t hasOnlyOneOfRule, size_t optionallyHasOnlyOneOfRule, size_t hasAllOfRule)
{
    size_t setBitmask = pow(2, macroSet.size()) - 1;
    for (size_t i = 0; i <= setBitmask; i++)
    {
        if (optionallyHasOnlyOneOfRule && !((i & optionallyHasOnlyOneOfRule) == 0 || isExpOf2(i & optionallyHasOnlyOneOfRule)))
            continue;
        if (hasOnlyOneOfRule && !isExpOf2(i & hasOnlyOneOfRule))
            continue;
        if (hasAnyOfRule && (i & hasAnyOfRule) == 0)
            continue;
        if (hasAllOfRule && (i & hasAllOfRule) != hasAllOfRule)
            continue;
        permutations.push_back(ExtendedShaderVariation());
        ShaderVariation& currentMutation = permutations.back().m_ShaderVariation;
        for (size_t j = 0; j < macroSet.size(); j++)
        {
            if (i & (1 << j))
                currentMutation.push_back(macroSet[j]);
        }
        permutations.back().m_Bits = i;
    }
}
//mb use bitfields for macro everywhere?
std::vector<ExtendedShaderVariation> GetAllPermutations(const std::vector<GraphicsShaderMacro>& macroSet, size_t hasAnyOfRule, size_t hasOnlyOneOfRule, size_t optionallyHasOnlyOneOfRule, size_t hasAllOfRule)
{
    std::vector<ExtendedShaderVariation> mutations;
    GetAllMacrosCombinations(macroSet, mutations, hasAnyOfRule, hasOnlyOneOfRule, optionallyHasOnlyOneOfRule, hasAllOfRule);

    return mutations;
}



const wchar_t* VERTEX_SHADER_FILE_TYPE = L".vs";
const wchar_t* PIXEL_SHADER_FILE_TYPE = L".ps";

const wchar_t* VERTEX_SHADER_FILE_SIGN = L"vs";
const wchar_t* PIXEL_SHADER_FILE_SIGN = L"ps";

ShaderID GetShaderID(const std::wstring& filePath, uint32_t bits)
{
	std::string temp = wstrtostr(filePath);

	return FNV(temp.c_str(), temp.size()) ^ FNV((char*)&bits, sizeof(bits)); // WARNING!!!!!!!!!!!!!!(concatenation of hashes - VERY SUSPICIOUS)
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

std::wstring GetFileNameStr(const std::wstring& filePath) // returns .type
{
    long i = filePath.size() - 1;
    unsigned long dotIndex = 0;
    size_t lastSlashIndex = 0;
    for (i; i >= 0; i--)
    {
        if (filePath[i] == L'/')
            lastSlashIndex = i;
        if (filePath[i] == L'.')
            dotIndex = i;
    }
    return filePath.substr(lastSlashIndex + 1, dotIndex - lastSlashIndex - 1);
}

std::wstring GetShaderFileNameSign(const std::wstring& filename)
{
    return filename.substr(filename.size() - 2, 2);
}

void ShadersCollection::ShaderMonitorThreadFunc()
{
	STLVector<std::wstring> changedFilesPaths;

	while (m_FileChangesMonitor.GatherChanges(changedFilesPaths, FileChangesMonitor::Flag_NoRepeatingFileNames))
	{
		for (std::wstring& filePath : changedFilesPaths)
		{
			NormalizePath(filePath);
            m_ShadersChanged.push_back(filePath);
		}

        for (size_t i = 0; i < m_ShadersChanged.size(); i++)
        {
            std::wstring& filePath  = m_ShadersChanged[i];

            std::wstring shaderSign = GetShaderFileNameSign(GetFileNameStr(filePath));
            if (shaderSign == VERTEX_SHADER_FILE_SIGN)
            {
                std::lock_guard<std::mutex> lockGuard(m_ShadersLoadingMutex);
                bool loaded = FindVertexCompilableShader(filePath).Load(ShadersRootPath + filePath);
                if (loaded)
                    m_ShadersChanged.erase(m_ShadersChanged.begin() + i);
            } else
            if (shaderSign == PIXEL_SHADER_FILE_SIGN)
            {
                std::lock_guard<std::mutex> lockGuard(m_ShadersLoadingMutex);
                bool loaded = FindPixelCompilableShader(filePath).Load(ShadersRootPath + filePath);
                if (loaded)
                    m_ShadersChanged.erase(m_ShadersChanged.begin() + i);
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

	return rootPath + L'/' + ShadersRootPath;
}

void ShadersCollection::ExecuteShadersCompilation(GraphicsDevice& device)
{
    std::lock_guard<std::mutex> lockGuard(m_ShadersLoadingMutex);
	for (size_t i = 0; i < m_CompilableVertexShaders.size(); i++)
	{
		CompilableShader<GraphicsVertexShader>& compilableShader = m_CompilableVertexShaders[i];

        if (!compilableShader.IsChanged())
            continue;

		for (size_t i = 0; i < compilableShader.GetVariationsCount(); i++)
			m_VertexShaders[compilableShader.GetVariationID(i)] = compilableShader.Compile(device, i);
	}
    for (size_t i = 0; i < m_CompilablePixelShaders.size(); i++)
	{
		CompilableShader<GraphicsPixelShader>& compilableShader = m_CompilablePixelShaders[i];
		
        if (!compilableShader.IsChanged())
            continue;

		for (size_t i = 0; i < compilableShader.GetVariationsCount(); i++)
			m_PixelShaders[compilableShader.GetVariationID(i)] = compilableShader.Compile(device, i);
	}
    for (size_t i = 0; i < m_CompilableComputeShaders.size(); i++)
    {
        CompilableShader<GraphicsComputeShader>& compilableShader = m_CompilableComputeShaders[i];
        
        if (!compilableShader.IsChanged())
            continue;

        for (size_t i = 0; i < compilableShader.GetVariationsCount(); i++)
            m_ComputeShaders[compilableShader.GetVariationID(i)] = compilableShader.Compile(device, i);
    }

    for (size_t i = 0; i < m_CompilableHullShaders.size(); i++)
    {
        CompilableShader<GraphicsHullShader>& compilableShader = m_CompilableHullShaders[i];

        if (!compilableShader.IsChanged())
            continue;

        for (size_t i = 0; i < compilableShader.GetVariationsCount(); i++)
            m_HullShaders[compilableShader.GetVariationID(i)] = compilableShader.Compile(device, i);
    }
    for (size_t i = 0; i < m_CompilableHullShaders.size(); i++)
    {
        CompilableShader<GraphicsDomainShader>& compilableShader = m_CompilableDomainShaders[i];
        
        if (!compilableShader.IsChanged())
            continue;

        for (size_t i = 0; i < compilableShader.GetVariationsCount(); i++)
            m_DomainShaders[compilableShader.GetVariationID(i)] = compilableShader.Compile(device, i);
    }
}

void ShadersCollection::ReleaseGPUData()
{
    for (auto& sp : m_VertexShaders)
        sp.second.ReleaseGPUData();
    for (auto& sp : m_PixelShaders)
        sp.second.ReleaseGPUData();
    for (auto& sp : m_ComputeShaders)
        sp.second.ReleaseGPUData();
    for (auto& sp : m_DomainShaders)
        sp.second.ReleaseGPUData();
    for (auto& sp : m_HullShaders)
        sp.second.ReleaseGPUData();
}

void ShadersCollection::CalculatePaths()
{
	wchar_t buffer[1024];
	GetCurrentDirectory(1024, buffer);
	NormalizePathPlain(buffer);

	m_RootDirRank = GetPathRank(std::wstring(buffer));
	m_ShadersRootDirRank = GetPathRank(m_FileChangesMonitor.GetDirPath());

	m_ShadersRootPathRelativelyToMainRootPath = ReducePathRank(m_FileChangesMonitor.GetDirPath(), m_RootDirRank + 1) + L"/";
}