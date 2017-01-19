#pragma once
#include "Graphics/DX11Object.h"
#include "Graphics/VertexData.h"
#include "Graphics/GraphicsShader.h"

class GraphicsInputLayout : public D3D11ResourceHolder
{
public:
    GraphicsInputLayout(const VertexFormat& verteFormat, const GraphicsShader& shader);
private:
};