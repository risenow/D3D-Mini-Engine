#include "stdafx.h"
#include "SSAOShaderVariations.h"

std::vector<GraphicsShaderMacro> SSAOShaderVariations::MacroSet = {            GraphicsShaderMacro("SAMPLES_COUNT", "2"),
                                                                               GraphicsShaderMacro("SAMPLES_COUNT", "4"),
                                                                               GraphicsShaderMacro("SAMPLES_COUNT", "8") };