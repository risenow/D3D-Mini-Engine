#pragma once
#include "ShadersCollection.h"

class BasicPixelShaderVariations
{
public:
    static std::vector<ExtendedShaderVariation> GetPermutations()
    {
        return GetAllPermutations(MacroSet);
    }

    enum
    {
        BATCH = 1,
        GBUFFER = 1 << 1,
        TBN = 1 << 2,
        TEXCOORD = 1 << 3
    };

    static std::vector<GraphicsShaderMacro> MacroSet;
};