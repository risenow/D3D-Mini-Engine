#include "stdafx.h"
#include "BlurShaderVariations.h"

std::vector<GraphicsShaderMacro> BlurShaderVariations::MacroSet = { GraphicsShaderMacro("HORIZONTAL", "0"), GraphicsShaderMacro("VERTICAL", "0") };
size_t BlurShaderVariations::HORIZONTAL = 1 << 0;
size_t BlurShaderVariations::VERTICAL = 1 << 1;

std::vector<ShaderVariation> BlurShaderVariations::GetPermutations()
{
    size_t hasOneOf = HORIZONTAL | VERTICAL;
    return GetAllPermutations(MacroSet, 0, hasOneOf);
}