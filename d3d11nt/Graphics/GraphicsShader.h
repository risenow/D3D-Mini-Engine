#pragma once
#include <string>
#include <d3dcompiler.h>
#include "Graphics/DX11Object.h"

class GraphicsShader : public D3D11ResourceHolder
{
public:
    enum ShaderType { ShaderType_Vertex, ShaderType_Pixel };
    GraphicsShader(const std::string& fileName, ShaderType type);
    GraphicsShader(ID3DBlob* blob, ShaderType type);

    ID3DBlob* GetBlob() const;
private:
    //ID3DBlob* m_Blob;
};