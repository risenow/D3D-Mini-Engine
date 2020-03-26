#pragma once

#pragma once
#include "ShadersCollection.h"

class SSAOShaderVariations
{
public:
    static std::vector<ExtendedShaderVariation> GetPermutations()
    {
        size_t optionallyHasOnlyOneOf = SAMPLES_COUNT2 | SAMPLES_COUNT4 | SAMPLES_COUNT8;
        return GetAllPermutations(MacroSet, 0, 0, optionallyHasOnlyOneOf);
    }

    static std::vector<GraphicsShaderMacro> MacroSet;
    enum
    {
        SAMPLES_COUNT2 = 1,
        SAMPLES_COUNT4 = 1 << 1,
        SAMPLES_COUNT8 = 1 << 2,
    };
};