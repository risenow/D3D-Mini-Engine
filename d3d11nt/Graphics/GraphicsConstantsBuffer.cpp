#include "stdafx.h"
#include "Graphics/GraphicsConstantsBuffer.h"
#include "Graphics/GraphicsShader.h"

void BindMultipleGraphicsConstantBuffers(GraphicsDevice& device, index_t startSlotIndex, const std::vector<GraphicsBuffer*>& constantBuffers, GraphicsShaderMaskType stageMask)
{
	assert(stageMask != 0);

	void(__stdcall ID3D11DeviceContext::*setConstantBuffers[GraphicsShaderType_Count])(UINT, UINT, ID3D11Buffer*const*);
	setConstantBuffers[GraphicsShaderType_Vertex] = &ID3D11DeviceContext::VSSetConstantBuffers;
	setConstantBuffers[GraphicsShaderType_Pixel] = &ID3D11DeviceContext::PSSetConstantBuffers;

	std::vector<ID3D11Buffer*> buffers(constantBuffers.size());
	for (unsigned long i = 0; i < constantBuffers.size(); i++)
	{
		buffers[i] = (ID3D11Buffer*)constantBuffers[i]->GetDX11Object();
	}

	for (unsigned long i = 0; i < GraphicsShaderType_Count; i++)
	{
		if (stageMask & (1u<<i))
			(device.GetD3D11DeviceContext()->*setConstantBuffers[i])(startSlotIndex, constantBuffers.size(), buffers.data());
	}
}