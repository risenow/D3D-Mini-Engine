#pragma once
#include "ShadersCollection.h"

class BlurShaderVariations
{
public:
    static std::vector<ExtendedShaderVariation> GetPermutations();

    static std::vector<GraphicsShaderMacro> MacroSet;

    enum
    {
        VERTICAL = 1 << 0,
        HORIZONTAL = 1 << 1
    };
};

