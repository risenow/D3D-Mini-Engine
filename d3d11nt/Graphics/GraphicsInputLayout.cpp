#include "stdafx.h"
#include "Graphics/dxlogicsafety.h"
#include "Graphics/GraphicsInputLayout.h"

GraphicsInputLayout::GraphicsInputLayout() {}
GraphicsInputLayout::GraphicsInputLayout(GraphicsDevice& device, const VertexFormat& vertexFormat, const GraphicsVertexShader& shader)
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementsDesc = vertexFormat.GetD3D11InputElementDescs();
	ID3DBlob* shaderBlob = shader.GetBlob();
	D3D_HR_OP(device.GetD3D11Device()->CreateInputLayout(vertexFormat.GetD3D11InputElementDescs().data(), inputElementsDesc.size(), 
														 shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), (ID3D11InputLayout**)&GetDX11ObjectReference()));
}
ID3D11InputLayout* GraphicsInputLayout::GetD3D11InputLayout()
{
	return (ID3D11InputLayout*)GetDX11Object();
}
void GraphicsInputLayout::Bind(GraphicsDevice& device)
{
	device.GetD3D11DeviceContext()->IASetInputLayout(GetD3D11InputLayout());
}