#pragma once
#include <string>
#include <vector>
#include "System/typesalias.h"

struct VertexProperty
{
    VertexProperty();
    VertexProperty(const std::string& name, size_t size, offset_t bytesOffset, index_t semanticIndex = 0, index_t slotIndex = 0);

    std::string m_Name;
    size_t m_Size;

    offset_t m_BytesOffset;
    index_t m_SlotIndex;
    index_t m_SemanticIndex;
};

struct VertexFormat
{
    VertexFormat();

    count_t GetNumSlotsUsed() const;
    size_t GetVertexSizeInBytesForSlot(index_t slotIndex) const;
    void AddVertexProperty(const std::string& name, size_t size, index_t semanticIndex = 0, index_t slotIndex = 0);//const VertexProperty& vertexProperty);

    std::vector<size_t> m_PerSlotVertexSizes;
    std::vector<VertexProperty> m_VertexProperties;
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

    typedef std::vector<byte_t> RawData;
    typedef std::vector<RawData> VertexSlotsData;
    VertexSlotsData m_Data;
};