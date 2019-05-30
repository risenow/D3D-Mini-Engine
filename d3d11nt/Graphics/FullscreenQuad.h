#pragma once
#include "Graphics/GraphicsDevice.h"
#include "Graphics/GraphicsShader.h"
#include "Graphics/ShadersCollection.h"

class FullscreenQuad
{
public:
   // FullscreenQuad() {}
    FullscreenQuad(ShadersCollection& shadersCollection) : m_VertexShader(shadersCollection.GetShader<GraphicsVertexShader>(L"Test/fsqvs.hlsl", {}))
    {}
    void Render(GraphicsDevice& device)
    {
        m_VertexShader.Bind(device);

        device.GetD3D11DeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        device.GetD3D11DeviceContext()->Draw(3, 0);
    }
private:
    GraphicsVertexShader m_VertexShader;

};