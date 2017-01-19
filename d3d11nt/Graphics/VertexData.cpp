#include "stdafx.h"
#include "Graphics/VertexData.h"
#include "System/logicsafety.h"

VertexProperty::VertexProperty() : m_Size(0) {}
VertexProperty::VertexProperty(const std::string& name, size_t size, offset_t bytesOffset,
                               index_t semanticIndex/* = 0*/, index_t slotIndex/* = 0*/) : m_Name(name), m_Size(size), m_BytesOffset(bytesOffset),
                                                                                           m_SemanticIndex(semanticIndex), m_SlotIndex(slotIndex) {}

VertexFormat::VertexFormat() : m_NumSlots(0) {}
count_t VertexFormat::GetNumSlotsUsed() const
{
    return m_NumSlots;
}
size_t VertexFormat::GetVertexSizeInBytesForSlot(index_t slotIndex) const
{
    return m_PerSlotVertexSizes[slotIndex];
}
void VertexFormat::AddVertexProperty(const std::string& name, size_t size, index_t semanticIndex/* = 0*/, index_t slotIndex/* = 0*/)//const VertexProperty& vertexProperty)
{
    if (m_NumSlots < slotIndex + 1)
    {
        m_NumSlots = slotIndex + 1;
        m_PerSlotVertexSizes.resize(m_NumSlots);
        m_PerSlotVertexSizes[slotIndex] = 0;
    }

    m_VertexProperties.push_back(VertexProperty(name, size, m_PerSlotVertexSizes[slotIndex], semanticIndex, slotIndex));
    m_PerSlotVertexSizes[slotIndex] += size;
}

VertexData::VertexData() : m_NumVertexes(0)
{}
VertexData::VertexData(const VertexFormat& vertexFormat, count_t vertexesNum) : m_VertexFormat(vertexFormat), m_NumVertexes(vertexesNum)
{
    m_NumSlots = m_VertexFormat.GetNumSlotsUsed();
    m_Data.resize(m_NumSlots);
    for (int i = 0; i < m_NumSlots; i++)
    {
        m_Data[i].resize(m_NumVertexes * m_VertexFormat.GetVertexSizeInBytesForSlot(i));
    }
}
size_t VertexData::GetSizeInBytesForSlot(index_t slotIndex) const
{
    return m_Data[slotIndex].size();
}
count_t VertexData::GetNumVertexes() const
{
    return m_NumVertexes;
}
VertexProperty VertexData::GetVertexPropertyByName(const std::string& name, index_t semanticIndex/* = 0*/) const
{
    for (int i = 0; i < m_VertexFormat.m_VertexProperties.size(); i++)
    {
        const VertexProperty& vertexProperty = m_VertexFormat.m_VertexProperties.at(i);
        if (vertexProperty.m_Name == name && vertexProperty.m_SemanticIndex == semanticIndex)
            return vertexProperty;
    }
    popAssert(false);
}
VertexFormat VertexData::GetVertexFormat() const
{
    return m_VertexFormat;
}
void* VertexData::GetDataPtrForSlot(index_t slotIndex) const
{
    return (void*)m_Data[slotIndex].data();
}
void* VertexData::GetVertexPropertyDataPtrForVertexWithIndex(index_t index, const VertexProperty& vertexProperty)
{
    index_t byteIndex = index * m_VertexFormat.GetVertexSizeInBytesForSlot(vertexProperty.m_SlotIndex) + vertexProperty.m_BytesOffset;
    return &m_Data[vertexProperty.m_SlotIndex][byteIndex];
}