#pragma once
#include "ShadersCollection.h"

class BasicPixelShaderVariations
{
public:
    static std::vector<ShaderVariation> GetPermutations();

    static std::vector<GraphicsShaderMacro> MacroSet;
    static size_t GBUFFER;
    static size_t BATCH;
};