#pragma once
#include "ShadersCollection.h"

class DeferredShadingShaderVariations
{
public:
    static std::vector<ExtendedShaderVariation> GetPermutations();

    static std::vector<GraphicsShaderMacro> MacroSet;
    static size_t OPTIMIZED_SCHLICK;
    static size_t OVERRIDE_DIFFUSE;
    static size_t SAMPLES_COUNT2;
    static size_t SAMPLES_COUNT4;
    static size_t SAMPLES_COUNT8;
};