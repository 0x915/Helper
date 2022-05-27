
#ifndef HELPER_GLOBAL_HPP
#define HELPER_GLOBAL_HPP

#include <utility>
#include <iostream>
#include <ostream>
#include <csignal>
#include <chrono>
#include <thread>
#include <random>
#include <map>

#include <opencv2/opencv.hpp>
#include <windows.h>

#include <fmt/os.h>
#include <fmt/format.h>
#include <fmt/core.h>
#include <fmt/chrono.h>
#include <fmt/color.h>
#include <fmt/ranges.h>

#include <logger.hpp>
#include <define.hpp>


#define BIT0 0x1
#define BIT1 0x2
#define BIT2 0x4
#define BIT3 0x8
#define BIT4 0x10
#define BIT5 0x20
#define BIT6 0x40
#define BIT7 0x80

#define BIT8 0x100
#define BIT9 0x200
#define BIT10 0x400
#define BIT11 0x600
#define BIT12 0x1000
#define BIT13 0x2000
#define BIT14 0x4000
#define BIT15 0x8000

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float f32;
typedef double f64;

typedef cv::Point2i pt2;
typedef std::vector<int32> vec_int32;
typedef std::vector<int16> vec_int16;

extern int32 vm_width, vm_height;
extern int32 cv_width, cv_height;
extern int32 ui_width, ui_height;
extern int32 dv_width, dv_height;
extern uint32 system_dpi;
extern float ui_scale, cv_scale;

namespace utime {
	
	extern const int8_t H;
	extern const int8_t M;
	extern const int8_t s;
	extern const int8_t ms;
	extern const int8_t us;
	extern const int8_t ns;
	
	std::string GetUnitString(int8_t u);
	extern inline void sleep_xs(int8_t u, int32 t = 1);
	extern inline void sleep_xs(int32 _s = 1, int32 _ms = 0, int32 _us = 0, int32 _ns = 0);
}

namespace ustr {
	using namespace std;
	vector<string> split(string str,char ch);
}

namespace random {
	f32 get_cauchy_float(f32 x = 0);
	pt2 int_cauchy_pt2(pt2 pt, int32 L, int32 T, int32 R, int32 B);
	pt2 int_cauchy_pt2(pt2 min, pt2 max, pt2 focus);
	
	int32 int_uniform(int32 min, int32 max);
	pt2 int_uniform_pt2(pt2 min, pt2 max);
	pt2 int_uniform_pt2(vec_int32 min, vec_int32 max);
}
#endif //HELPER_GLOBAL_HPP
