#pragma once
#include "Graphics/GraphicsDevice.h"
#include "Graphics/DX11Object.h"

template<class T, size_t n>
class MappableDX11MultipleObjectsHolder : public DX11MultipleObjectsHolder<T, n>
{
public:
	void* Map(GraphicsDevice& device, index_t subresource, D3D11_MAP type, unsigned int flags, unsigned int& outRowPitch, unsigned int& outDepthPitch, index_t objectIndex = 0);

};