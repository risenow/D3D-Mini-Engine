#pragma once
#include <string>
#include <vector>
#include <d3d11.h>
#include <dxgi.h>
#include "Extern/glm/glm.hpp"
#include "System/typesalias.h"
#include "System/logicsafety.h"
#include "System/stlcontainersintegration.h"

template<class T> // glm type
DXGI_FORMAT GetVertexDXGIFormat()
{
	popAssert(false);
	return DXGI_FORMAT_UNKNOWN;
}
template<> // glm type
DXGI_FORMAT GetVertexDXGIFormat<glm::vec3>();
template<> // glm type
DXGI_FORMAT GetVertexDXGIFormat<glm::vec4>();


struct VertexPropertyPrototype
{
	VertexPropertyPrototype();

	std::string m_Name;
	size_t m_Size;

	index_t m_SlotIndex;
	index_t m_SemanticIndex;
	DXGI_FORMAT m_DXGIFormat;
};

template<class T> //glm type
VertexPropertyPrototype CreateVertexPropertyPrototype(const std::string& name, index_t semanticIndex = 0, index_t slotIndex = 0)
{
	VertexPropertyPrototype vertexPropertyPrototype;
	vertexPropertyPrototype.m_Name = name;
	vertexPropertyPrototype.m_SemanticIndex = semanticIndex;
	vertexPropertyPrototype.m_SlotIndex = slotIndex;
	vertexPropertyPrototype.m_DXGIFormat = GetVertexDXGIFormat<T>();
	vertexPropertyPrototype.m_Size = sizeof(T);
	return vertexPropertyPrototype;
}

struct VertexProperty
{
	VertexProperty();
	VertexProperty(const VertexPropertyPrototype& prototype, offset_t bytesOffset);

	VertexPropertyPrototype m_Prototype;
	offset_t m_BytesOffset;
};

struct VertexFormat
{
    VertexFormat();

    count_t GetNumSlotsUsed() const;
    size_t GetVertexSizeInBytesForSlot(index_t slotIndex) const;
	void AddVertexProperty(const VertexPropertyPrototype& vertexPropertyPrototype);

	std::vector<D3D11_INPUT_ELEMENT_DESC> GetD3D11InputElementDescs() const;

    STLVector<size_t> m_PerSlotVertexSizes;
    STLVector<VertexProperty> m_VertexProperties;
private:
    count_t m_NumSlots;
};

class VertexData
{
public:
    VertexData();
    VertexData(const VertexFormat& format, count_t vertexNum);

    size_t GetSizeInBytesForSlot(index_t slotIndex) const;
    count_t GetNumVertexes() const;
    VertexProperty GetVertexPropertyByName(const std::string& name, index_t semanticIndex = 0) const;
    VertexFormat GetVertexFormat() const;

    void* GetDataPtrForSlot(index_t slotIndex) const;
    void* GetVertexPropertyDataPtrForVertexWithIndex(index_t index, const VertexProperty& vertexProperty);
protected:
    VertexFormat m_VertexFormat;
    count_t m_NumVertexes;
    count_t m_NumSlots;

    typedef STLVector<byte_t> RawData;
    typedef STLVector<RawData> VertexSlotsData;
    VertexSlotsData m_Data;
};