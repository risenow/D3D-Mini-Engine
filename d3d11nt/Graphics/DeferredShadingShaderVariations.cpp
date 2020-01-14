#include "stdafx.h"
#include "DeferredShadingShaderVariations.h"

std::vector<GraphicsShaderMacro> DeferredShadingShaderVariations::MacroSet = { GraphicsShaderMacro("OPTIMIZED_SCHLICK", "1"),
                                                                               GraphicsShaderMacro("OVERRIDE_DIFFUSE", "1"),
                                                                               GraphicsShaderMacro("SAMPLES_COUNT", "2"),
                                                                               GraphicsShaderMacro("SAMPLES_COUNT", "4"),
                                                                               GraphicsShaderMacro("SAMPLES_COUNT", "8") };
size_t DeferredShadingShaderVariations::OPTIMIZED_SCHLICK = 1 << 0;
size_t DeferredShadingShaderVariations::OVERRIDE_DIFFUSE = 1 << 1;
size_t DeferredShadingShaderVariations::SAMPLES_COUNT2 = 1 << 2;
size_t DeferredShadingShaderVariations::SAMPLES_COUNT4 = 1 << 3;
size_t DeferredShadingShaderVariations::SAMPLES_COUNT8 = 1 << 4;

std::vector<ExtendedShaderVariation> DeferredShadingShaderVariations::GetPermutations()
{
    size_t optionallyHasOnlyOneOf = SAMPLES_COUNT2 | SAMPLES_COUNT4 | SAMPLES_COUNT8;
    return GetAllPermutations(MacroSet, 0, 0, optionallyHasOnlyOneOf);
}