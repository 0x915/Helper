//
// Created by qin19 on 2022/5/15.
//

#include "core.hpp"


namespace templ {
	
	using namespace std;
	const bool quite = true;
	
	TL::TL(const std::string &basedir, const std::string &imgfile, const std::string &note,
		   vec_int32 roi5i, vec_int32 fb5i, vec_int32 param) {
		if (!param.empty()) {
			// 生成 空模板对象
			if (param[0] == 255) {
				isNull = true;
				return;
			}
		}
		
		file = basedir + "/" + imgfile;
		text = note;
		
		// 读取 模板图像
		if (this->read()) {}
		else {
			isNull = true;
			Logger.error(fformat("模板[{0}] ({1}) 图像文件读取失败", text, file));
		}
		
		// 检查 输入ROI:坐标矩形+模式/空输入
		if (roi5i.size() == 5) {
			// {X,Y,W,H,FLAG} ROI_FULL_BIT = 1(全屏) 0(区域)
			if (roi5i[4] & ROI_FULL_BIT) {
				roi = cv::Rect(1, 1, cv_width, cv_height);
				reg |= ROI_FULL_BIT;
			} else {
				roi = cv::Rect(roi5i[0], roi5i[1], roi5i[2], roi5i[3]);
				reg |= roi5i[4];
			}
		} else {
			isNull = true;
			Logger.error("传入ROI格式错误 - 仅接受 四个无符号整型{ X,Y , W,H , M} 输入");
			Logger.error("                - 矩形 起点(X,Y)宽高(W,H) 匹配模式(M)参考宏定义");
		}
		
		// 检查 输入FB:坐标矩形+模式/空输入
		if (fb5i.size() == 5) {
			if (fb5i[4] & FB_NONE_BIT) {
				fb = cv::Rect(0, 0, 0, 0);
				reg |= FB_NONE_BIT;
			} else {
				if (fb5i[1] == 0 || fb5i[2] == 0)
					fb = cv::Rect(fb5i[0], fb5i[1], width, height);
				else
					fb = cv::Rect(fb5i[0], fb5i[1], fb5i[2], fb5i[3]);
				reg |= fb5i[4];
			}
		} else {
			isNull = true;
			Logger.error("传入FB格式错误 - 仅接受 六个无符号整型{ X,Y , W,H , M } 输入");
			Logger.error("               - 矩形 起点(X,Y)宽高(W,H) 起点模式(M)参考宏定义");
		}
		
		if (!quite)
			Logger.info(fformat("定义模板[{0}] ({1}) {{{2}w{3}h {4}c}} {5}",
								text, imgfile, width, height, channel,
								isNull ? "\033[91m但不可用\033[0m" : ""));
		
		stringstream tempstr;
		tempstr << " ↑↑ ";
		if (reg & ROI_FULL_BIT) tempstr << "全屏匹配";
		else tempstr << "区域匹配";
		tempstr << fformat("({0},{1} {2}w{3}h)  ", roi.x, roi.y, roi.width, roi.height);
		if (reg & FB_NONE_BIT) {
			tempstr << "不反馈";
			tempstr << fformat("(None {0},{1} {2}w{3}h)", fb.x, fb.y, fb.width, fb.height);
		} else if (reg & FB_SELF_BIT) {
			tempstr << "相对位置反馈";
			tempstr << fformat("(Self → {0},{1} {2}w{3}h)", fb.x, fb.y, fb.width, fb.height);
		} else if (!(reg & FB_SELF_BIT)) {
			tempstr << "绝对位置反馈";
			tempstr << fformat("(Fixed {0},{1} {2}w{3}h)", fb.x, fb.y, fb.width, fb.height);
		}
		if (!quite) Logger.trace(tempstr.str());
		
		if (!quite) Logger.info("-----------------------------------------------------------------------");
		
	}
	
	bool TL::read() {
		image = cv::imread(file, cv::IMREAD_UNCHANGED);
		if (image.empty()) return false;
		width = image.size().width;
		halfW = width / 2;
		height = image.size().height;
		halfH = height / 2;
		channel = image.channels();
		if (channel == 4) {
			cv::Mat splitBGRA[4];
			cv::split(image, splitBGRA);
			cv::Mat mergeBGR[3] = {splitBGRA[0], splitBGRA[1], splitBGRA[2]};
			cv::merge(mergeBGR, 3, image);
			if (cv::countNonZero(~splitBGRA[3])) {
				alpha = splitBGRA[3];
			} else {
				channel = 3;
				Logger.warn(fformat("无效蒙版({0}) 全白Alpha通道 视为不透明图像",text));
			}
		}
		return true;
	}
	
	TL nulltl("", "", "", {}, {}, {255});
	TL &selftl = nulltl;
	
	inline vec_int32 GetOriginXY(int32 p) {
		if (p == fromSELF) return {0, 0, 0, 0, FB_SELF_BIT};
		else if (p == fromFULL) return {0, 0, 0, 0, ROI_FULL_BIT};
		else if (p == fromTL) return {1, 1};
		else if (p == fromTC) return {cv_width / 2, 1};
		else if (p == fromTR) return {cv_width, 1};
		else if (p == fromCL) return {1, cv_height / 2};
		else if (p == fromCC) return {cv_width / 2, cv_height / 2};
		else if (p == fromCR) return {cv_width, cv_height / 2};
		else if (p == fromBL) return {1, cv_height};
		else if (p == fromBC) return {cv_width / 2, cv_height};
		else if (p == fromBR) return {cv_width, cv_height};
		else {
			Logger.error("CalFBrect: 没有输入可用的预设位置 设置为不反馈");
			return {};
		}
	}
	
	vec_int32 FBfrom(int32 p, int32 x, int32 y, int32 w, int32 h) {
		vec_int32 point;
		point = GetOriginXY(p);
		if (point.empty())return FB_NONE;
		point[0] = point[0] - 1 + x;
		point[1] = point[1] - 1 + y;
		return {point[0], point[1], w, h, FB_RECT_BIT};
	}
	
	vec_int32 ROIfrom(int32 p, int32 x, int32 y, int32 w, int32 h) {
		vec_int32 point;
		point = GetOriginXY(p);
		if (point.empty())return ROI_FULL;
		point[0] = point[0] - 1 + x;
		point[1] = point[1] - 1 + y;
		return {point[0], point[1], w, h, FB_RECT_BIT};
	}
	
	
	pt2 GetFBrXY(vec_int32 xy, const templ::TL &tl) {
		// 长度2 > 结果坐标 > 计算随机
		if (xy.size() == 2) {
			if (tl.isNull) {
				Logger.error("GetFBrect: 没有有效的模板类输入 无法计算区域随机数");
				return {-1, -1};
			}
			if (tl.reg & FB_NONE_BIT) return {-1, -1};
			// 模板类范围 > 相对范围 else 绝对范围
			if (tl.reg & FB_SELF_BIT) {
				return random::int_uniform_pt2(
					pt2(xy[0] + tl.fb.x, xy[1] + tl.fb.y),
					pt2(xy[0] + tl.fb.x + tl.fb.width, xy[1] + tl.fb.y + tl.fb.height)
				);
			} else {
				return random::int_uniform_pt2(
					pt2{tl.fb.x, tl.fb.y},
					pt2{tl.fb.x + tl.fb.width, tl.fb.y + tl.fb.height}
				);
			}
		}
		// 长度4 > 自定义范围 > 绝对范围
		if (xy.size() == 4) {
			vec_int32 rectLT(xy[0], xy[2]);
			vec_int32 rectBR(xy[1], xy[3]);
			return random::int_uniform_pt2(rectLT, rectBR);
		}
		// 长度6 > 自定义范围 > 相对范围
		if (xy.size() == 6) {
			return random::int_uniform_pt2(
				vec_int32{xy[0] + xy[2], xy[1] + xy[3]},
				vec_int32{xy[0] + xy[4], xy[1] + xy[5]}
			);
		}
		
		Logger.error("GetFBXY: 无效坐标输入,首参数仅允许传入 单坐标点{X,Y} 绝对区域{X1,Y1,X2,Y2}");
		Logger.error("         相对区域{X,Y,X1,Y1,X2,Y2}(X,Y为原点,X1,Y1,X2,Y2为偏移的矩形全区域)");
		return {-1, -1};
	}
}
