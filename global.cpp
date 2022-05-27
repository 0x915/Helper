
#include <global.hpp>

int32 vm_width = 0, vm_height = 0;
int32 cv_width = 0, cv_height = 0;
int32 ui_width = 0, ui_height = 0;
int32 dv_width = 0, dv_height = 0;
uint32 system_dpi = 0;
f32 ui_scale = 0.0, cv_scale = 0.0;


using namespace std;

namespace utime {
	const int8_t H = 0;
	const int8_t M = 1;
	const int8_t s = 2;
	const int8_t ms = 3;
	const int8_t us = 4;
	const int8_t ns = 5;
	
	std::string GetUnitString(int8_t u) {
		switch (u) {
			case H :
				return "hour";
			case M :
				return "min";
			case s :
				return "sec";
			case ms :
				return "ms";
			case us :
				return "us";
			case ns :
				return "ns";
			default:
				return "??";
		}
	}
	
	inline void sleep_xs(int8_t u, int32 t) {
		switch (u) {
			case s :
				sleep_sec(t);
				return;
			case ms :
				sleep_ms(t);
				return;
			case us :
				sleep_us(t);
				return;
			case ns:
				sleep_ns(t);
				return;
			default:
				return;
		}
	}
	
	inline void sleep_xs(int32 _s, int32 _ms, int32 _us, int32 _ns) {
		sleep_sec(_s);
		sleep_ms(_ms);
		sleep_us(_us);
		sleep_ns(_ns);
	}
}

namespace ustr {
	vector<string> split(string str, char ch) {
		vector<string> result;
		uint32 H = 0, E;
		for (uint32 C = 0, length = str.size(); C < length; C++) {
			if (str[C] == ch) {
				E = C;
				if (H == 0) result.push_back(str.substr(H, E - H));
				else result.push_back(str.substr(H + 1, E - H - 1));
				H = E;
			}
			if (C + 1 == length) {
				if (H == 0) result.push_back(str.substr(H, length - H));
				else result.push_back(str.substr(H + 1, length - H));
			}
		}
		return result;
	}
}


namespace random {
	
	random_device rd;
	mt19937 rde(rd());
	
	f32 get_cauchy_float(f32 x) {
		std::uniform_real_distribution<f32> rf32Y(0.1, 0.2);
		cauchy_distribution<f32> rf32{x, rf32Y(rd)};
		f32 offset;
		while (true) {
			offset = rf32(rde);
			if (offset <= 1.0f && -1.0f <= offset) break;
		}
		return offset;
	}
	
	pt2 int_cauchy_pt2(pt2 pt, int32 L, int32 T, int32 R, int32 B) {
		f32 rw = get_cauchy_float(), rh = get_cauchy_float();
		if (rw <= 0) pt.x = pt.x + int32(f32(L) * rw);
		else pt.x = pt.x + int32(f32(R) * rw);
		if (rh <= 0) pt.y = pt.y + int32(f32(T) * rh);
		else pt.y = pt.y + int32(f32(B) * rh);
		return pt;
	}
	
	pt2 int_cauchy_pt2(pt2 min, pt2 max, pt2 focus) {
		if (focus.x > max.x || min.x < focus.x) return pt2{0, 0};
		if (focus.y > max.y || min.y < focus.y) return pt2{0, 0};
		int32 l = focus.x - min.x, r = max.x - focus.x;
		int32 t = focus.y - min.y, b = max.y - focus.y;
		f32 rw = get_cauchy_float(), rh = get_cauchy_float();
		if (rw <= 0) focus.x = focus.x + int32(f32(l) * rw);
		else focus.x = focus.x + int32(f32(r) * rw);
		if (rh <= 0) focus.y = focus.y + int32(f32(t) * rh);
		else focus.y = focus.y + int32(f32(b) * rh);
		return focus;
	}
	
	int32 int_uniform(int32 min, int32 max) {
		if (min >= max) {
			Logger.error(fformat("产生随机数异常: min:{0} >= max:{1}", min, max));
			return 0;
		}
		std::uniform_int_distribution<int32> rint32(min, max);
		return rint32(rde);
	}
	
	pt2 int_uniform_pt2(pt2 min, pt2 max) {
		if (min.x >= max.x) {
			Logger.error(fformat("产生随机坐标异常: min.x:{0} >= max.x:{1}", min.x, max.x));
			return {0, 0};
		}
		if (min.y >= max.y) {
			Logger.error(fformat("产生随机坐标异常: min.y:{0} >= max.y:{1}", min.y, max.y));
			return {0, 0};
		}
		pt2 pt(int_uniform(min.x, max.x), int_uniform(min.y, max.y));
		return pt;
	}
	
	pt2 int_uniform_pt2(vec_int32 min, vec_int32 max) {
		if (min[0] >= max[0]) {
			Logger.error(fformat("产生随机坐标异常: min[0]:{0} >= max[0]:{1}", min[0], max[0]));
			return {0, 0};
		}
		if (min[1] >= max[1]) {
			Logger.error(fformat("产生随机坐标异常: min[1]:{0} >= max[1]:{1}", min[1], max[1]));
			return {0, 0};
		}
		pt2 pt(int_uniform(min[0], max[0]), int_uniform(min[1], max[1]));
		return pt;
	}
}

