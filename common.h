#pragma once

/*
#ifdef _DEBUG
#include <crtdbg.h>
	#define NEW  new( _NORMAL_BLOCK, __FILE__, __LINE__ )
#endif
*/

const float PI = 3.14152965f;
const float PI2 = PI * 2.f;

const bool RIGHT = false;
const bool LEFT = true;

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;


const int MW = 64, MH = 32;
const int MMX = 128;
const int MMY = 64;

extern inline float RandFloat(float r);
extern inline float RandFloat0(float r);
extern inline float RandFloat(float min, float max);

//”ÍˆÍ“à‚É‚ ‚é‚©
template<typename T1, typename T2, typename T3> 
inline bool InRange(T1 low, T2 n, T3 high) {
	return low < n && n < high;
}
template<typename T1, typename T2, typename T3> 
inline bool InRangeEq(T1 low, T2 n, T3 high) {
	return low <= n && n <= high;
}

#ifdef _DEBUG
const char WINDOW_TITLE[] = "sds[DEBUG]";
#else
const char WINDOW_TITLE[] = "sds";
#endif

typedef unsigned char byte;


#include "DebugDraw.h"
#include <cassert>
