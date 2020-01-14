#include "stdafx.h"
#include "BlurShaderVariations.h"

std::vector<GraphicsShaderMacro> BlurShaderVariations::MacroSet = { GraphicsShaderMacro("HORIZONTAL", "0"), GraphicsShaderMacro("VERTICAL", "0") };

std::vector<ExtendedShaderVariation> BlurShaderVariations::GetPermutations()
{
    size_t hasOneOf = HORIZONTAL | VERTICAL;
    return GetAllPermutations(MacroSet, 0, hasOneOf);
}