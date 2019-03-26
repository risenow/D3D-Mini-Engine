#pragma once
#include "Graphics/GraphicsDevice.h"
#include "Graphics/GraphicsBuffer.h"
#include "Graphics/GraphicsShader.h"

void BindMultipleGraphicsConstantBuffers(GraphicsDevice& device, index_t startSlotIndex, const std::vector<GraphicsBuffer*>& constantBuffers, GraphicsShaderMaskType stageMask);

template<class T>
class GraphicsConstantsBuffer : public GraphicsBuffer //mb rename to ConstantsBuffer(as VertexBuffer)
{
public:
	GraphicsConstantsBuffer() {}
	GraphicsConstantsBuffer(GraphicsDevice& device) : GraphicsBuffer(device, sizeof(T), GraphicsBuffer::BindFlag_Constant, GraphicsBuffer::UsageFlag_Default, GraphicsBuffer::MiscFlag_Default)
	{}
	GraphicsConstantsBuffer(GraphicsDevice& device, T& consts) : GraphicsBuffer(device, sizeof(T), GraphicsBuffer::BindFlag_Constant, GraphicsBuffer::UsageFlag_Default, GraphicsBuffer::MiscFlag_Default, &consts)
	{}

	void Bind(GraphicsDevice& device, GraphicsShaderMaskType stageMask, index_t startSlotIndex = 0)
	{
		BindMultipleGraphicsConstantBuffers(device, startSlotIndex, { this }, stageMask);
	}
	void Update(GraphicsDevice& device, T constants)
	{
		//need a perfomance test vs map unmap seq
		device.GetD3D11DeviceContext()->UpdateSubresource((ID3D11Resource*)GetDX11Object(), 0, nullptr, &constants, sizeof(constants), 0);
	}
private:
};


