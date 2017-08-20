#pragma once

#ifdef _DEBUG
#define DEBUG_ONLY(expression) expression
#else
#define DEBUG_ONLY(expression) 
#endif

#ifdef _DEBUG
//#define INTEGRATE_PROFILE_SYSTEM
#endif