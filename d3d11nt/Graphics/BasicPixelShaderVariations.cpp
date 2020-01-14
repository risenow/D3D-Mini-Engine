#include "stdafx.h"
#include "BasicPixelShaderVariations.h"

std::vector<GraphicsShaderMacro> BasicPixelShaderVariations::MacroSet = { GraphicsShaderMacro("BATCH", "1"), GraphicsShaderMacro("GBUFFER_PASS", "1"), GraphicsShaderMacro("TBN", "1"), GraphicsShaderMacro("TEXCOORD", "1") };