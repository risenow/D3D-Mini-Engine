#pragma once
#include "ShadersCollection.h"

class BlurShaderVariations
{
public:
    static std::vector<ShaderVariation> GetPermutations();

    static std::vector<GraphicsShaderMacro> MacroSet;
    static size_t VERTICAL;
    static size_t HORIZONTAL;
};

