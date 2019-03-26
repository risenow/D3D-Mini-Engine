#pragma once
#include "Graphics/GraphicsDevice.h"
#include "Graphics/DX11Object.h"
#include "Graphics/VertexData.h"
#include "Graphics/GraphicsShader.h"

class GraphicsInputLayout : public D3D11ResourceHolder
{
public:
	GraphicsInputLayout();
    GraphicsInputLayout(GraphicsDevice& device, const VertexFormat& verteFormat, const GraphicsVertexShader& shader);

	void Bind(GraphicsDevice& device);

	ID3D11InputLayout* GetD3D11InputLayout();
private:
};