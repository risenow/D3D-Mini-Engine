#pragma once
#include "System/typesalias.h"
#include "System/stlcontainersintegration.h"
#include "Graphics/GraphicsShader.h"
#include "Graphics/GraphicsDevice.h"

//FULL SHADER SYSTEM REFACTORING IS NEEDED

typedef index_t ShaderHandle;

template<class T>
class Shader
{
	friend class ShaderCollection;
public:
	Shader();
private:
	void ComputeSRC();

	unsigned long m_CRC;
	T m_GraphicsShader;
	std::string m_FilePath;
};

class ShaderCollection
{
public:
	ShaderCollection(GraphicsDevice& device);

	template<class T>
	ShaderHandle AddShader(const std::string& filePath);
private:
	STLVector<Shader<GraphicsVertexShader>> m_VertexShaders;
	STLVector<Shader<GraphicsPixelShader>>  m_PixelShaders;
	GraphicsDevice& m_Device;
};