#pragma once
#include "Graphics/GraphicsDevice.h"
#include "Graphics/DX11Object.h"
#include "Graphics/VertexData.h"
#include "Graphics/GraphicsShader.h"

{
public:
	GraphicsInputLayout();
    GraphicsInputLayout(GraphicsDevice& device, const VertexFormat& verteFormat, const GraphicsVertexShader& shader);

	void Bind(GraphicsDevice& device);

    void ReleaseGPUData()
    {
        m_InputLayout->Release();
        m_InputLayout = nullptr;
    }

	ID3D11InputLayout* GetD3D11InputLayout();
private:
    ID3D11InputLayout* m_InputLayout;
};