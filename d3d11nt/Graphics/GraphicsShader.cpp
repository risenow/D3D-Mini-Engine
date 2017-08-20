#include "stdafx.h"
#include "Graphics/GraphicsShader.h"
#include "Graphics/dxlogicsafety.h"
#include "System/Log.h"
#include "System/logicsafety.h"
#include "System/strutils.h"

void CreateShader(GraphicsDevice& device, ID3DBlob* blob, ID3D11VertexShader** shader)
{
	device.GetD3D11Device()->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, shader);
}
void CreateShader(GraphicsDevice& device, ID3DBlob* blob, ID3D11PixelShader** shader)
{
	device.GetD3D11Device()->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, shader);
}
void CreateShader(GraphicsDevice& device, ID3DBlob* blob, ID3D11HullShader** shader)
{
	device.GetD3D11Device()->CreateHullShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, shader);
}
void CreateShader(GraphicsDevice& device, ID3DBlob* blob, ID3D11DomainShader** shader)
{
	device.GetD3D11Device()->CreateDomainShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, shader);
}
void CreateShader(GraphicsDevice& device, ID3DBlob* blob, ID3D11GeometryShader** shader)
{
	device.GetD3D11Device()->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, shader);
}

void BindShader(GraphicsDevice& device, ID3D11VertexShader* vertexShader)
{
	device.GetD3D11DeviceContext()->VSSetShader(vertexShader, nullptr, 0);
}
void BindShader(GraphicsDevice& device, ID3D11PixelShader* pixelShader)
{
	device.GetD3D11DeviceContext()->PSSetShader(pixelShader, nullptr, 0);
}