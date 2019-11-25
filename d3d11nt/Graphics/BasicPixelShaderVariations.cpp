#include "stdafx.h"
#include "BasicPixelShaderVariations.h"

std::vector<GraphicsShaderMacro> BasicPixelShaderVariations::MacroSet = { GraphicsShaderMacro("BATCH", "1"), GraphicsShaderMacro("GBUFFER_PASS", "1") };
size_t BasicPixelShaderVariations::BATCH = 1;
size_t BasicPixelShaderVariations::GBUFFER = 1 << 1;

std::vector<ShaderVariation> BasicPixelShaderVariations::GetPermutations()
{
    return GetAllPermutations(MacroSet);
}