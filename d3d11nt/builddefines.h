#pragma once

#ifdef _DEBUG
#define DEBUG_ONLY(expression) expression
#else
#define DEBUG_ONLY(expression) (void)0
#endif