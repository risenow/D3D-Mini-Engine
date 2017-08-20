#pragma once

#ifdef CPU_SIDE_CODE
#define DECLARE_CONSTANT_STRUCTURE struct
#define DECLARE_FLOAT float
#define DECLARE_FLOAT3 glm::vec3
#define DECLARE_FLOAT4 glm::vec4
#define DECLARE_FLOAT4X4 glm::mat4x4
#else
#define DECLARE_CONSTANT_STRUCTURE cbuffer
#define DECLARE_FLOAT float
#define DECLARE_FLOAT3 float3
#define DECLARE_FLOAT4 float4
#define DECLARE_FLOAT4X4 float4x4
#endif
