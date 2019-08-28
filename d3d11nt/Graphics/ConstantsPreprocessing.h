#ifndef CONSTANTS_PREPROCESSING
#define CONSTANTS_PREPROCESSING

#ifdef CPU_SIDE_CODE
#include "System/Mat4x4Storage.h"
#define DECLARE_CONSTANT_STRUCTURE struct
#define DECLARE_MATERIAL_CONSTANT_STRUCTURE struct
#define DECLARE_FLOAT float
#define DECLARE_FLOAT3 SimpleMath::Vector3
#define DECLARE_FLOAT4 SimpleMath::Vector4
#define DECLARE_FLOAT3X3 XMFLOAT3X3
#define DECLARE_FLOAT4X4 SimpleMath::Matrix//glm::mat4x4
#define DEFINE_MATERIAL_CONSTANT_STRUCTURE(name, type)
#else
#define DECLARE_CONSTANT_STRUCTURE cbuffer
#ifdef BATCH
#define DEFINE_MATERIAL_CONSTANT_STRUCTURE(name, type) StructuredBuffer< type > name ;
#define DECLARE_MATERIAL_CONSTANT_STRUCTURE struct
#else
#define DEFINE_MATERIAL_CONSTANT_STRUCTURE(name, type)
#define DECLARE_MATERIAL_CONSTANT_STRUCTURE cbuffer
#endif
#define DECLARE_FLOAT float
#define DECLARE_FLOAT3 float3
#define DECLARE_FLOAT4 float4
#define DECLARE_FLOAT3X3 float3x3
#define DECLARE_FLOAT4X4 float4x4
#endif

#ifdef CPU_SIDE_CODE
#include "Extern/glm/glm.hpp"
#endif

#endif