
#include "match.hpp"

namespace match {
	#define MT_MASK_SUPPORT_CH_NUM 3
	Mat sc3mat[MT_MASK_SUPPORT_CH_NUM];
	Mat tl3mat[MT_MASK_SUPPORT_CH_NUM];
	f64 rate3L[MT_MASK_SUPPORT_CH_NUM];
	pt2 point3L[MT_MASK_SUPPORT_CH_NUM];
	Mat result3L[MT_MASK_SUPPORT_CH_NUM];
	#define forCH(start) for(int32 i = (start); i < MT_MASK_SUPPORT_CH_NUM; i++)
	
	void ResultDrawAndShow(Mat &mat, const vector<pt2> &pt) {
		if (mat.channels() == 3) cvtColor(mat, mat, COLOR_BGR2GRAY);
		if (mat.channels() == 1) cvtColor(mat, mat, COLOR_GRAY2BGR);
		for (pt2 p: pt) {
			circle(mat, p, 4, Scalar(0, 0, 255), FILLED, LINE_8);
		}
		imshow("debug", mat);
	}
	
	pt2 mask(Mat &screen, const typeTL &tl, f32 thh, int32 reg) {
		clock_type start = clock_now;
		
		// --------- 受入检查 ---------
		if (tl.isNull || tl.alpha.empty() || tl.image.empty() || tl.channel != 4) {
			Logger.error(fformat("MT Mask ERROR ({0}) 无效模板类/空蒙版/空模板/非透明图像", tl.text));
			return MT_FALSE_PT2;
		}
		
		bool roi_enable = !(tl.reg & ROI_FULL_BIT);
		
		if (roi_enable) screen = screen(tl.roi);
		
		if (screen.size().height <= tl.height || screen.size().width <= tl.width) {
			Logger.error(fformat("MT Rect ERROR ({0}) 背景过小", tl.text));
			return MT_FALSE_PT2;
		}
		
		// --------- 匹配初始化 ---------
		split(screen, sc3mat);
		split(tl.image, tl3mat);
		
		Mat MtResult{};
		f64 MtRate{};
		pt2 MtPoint{};
		
		// --------- 匹配 ---------
		forCH(0) {
			matchTemplate(sc3mat[i], tl3mat[i], result3L[i], TM_CCORR_NORMED, tl.alpha);
			minMaxLoc(result3L[i], nullptr, &rate3L[i], nullptr, &point3L[i]);
			if (isinf(rate3L[i])) return MT_FALSE_PT2;
		}
		
		// --------- 额外调试日志 ---------
		if (reg & MT_DEBUG_EXTRALOG) {
			stringstream tempstr;
			if (roi_enable)tempstr << "ROI" << tl.roi;
			Logger.trace(fformat("MT Mask Result ({0}) {1}", tl.text, tempstr.str()));
			
			forCH(0) cout << "                    " << point3L[i] << "    " << rate3L[i] << endl;
		}
		
		// --------- 判断结果 ---------
		if (reg & MT_MASK_MIXED) {
			MtResult = result3L[0] / MT_MASK_SUPPORT_CH_NUM;
			forCH(1) MtResult += result3L[i] / MT_MASK_SUPPORT_CH_NUM;
			minMaxLoc(MtResult, nullptr, &MtRate, nullptr, &MtPoint);
		} else {
			for (int32 i = -1; i < MT_MASK_SUPPORT_CH_NUM - 1; i++) {
				MtPoint = point3L[i == -1 ? MT_MASK_SUPPORT_CH_NUM - 1 : i] - point3L[i + 1];
				if (hypot(MtPoint.x, MtPoint.y) > 20) return MT_FALSE_PT2;
			}
			MtRate = rate3L[0];
			MtPoint = point3L[0];
			MtResult = result3L[0];
			forCH(1) {
				MtRate += rate3L[i];
				MtPoint += point3L[i];
				MtResult += result3L[i];
			}
			MtRate /= MT_MASK_SUPPORT_CH_NUM;
			MtPoint /= MT_MASK_SUPPORT_CH_NUM;
			MtResult /= MT_MASK_SUPPORT_CH_NUM;
		}
		
		if (MtRate < (thh < 0.95 ? 0.95 : thh) || MtRate > 1.0) return MT_FALSE_PT2;
		
		// --------- 返回概率 ---------
		if (reg & MT_RET_RATE) return {int32(MtRate * 1000)};
		
		// --------- 调整坐标 ---------
		if (reg & MT_RET_XYCENTER) MtPoint += pt2(tl.halfW, tl.halfH);
		
		// --------- 显示结果 ---------
		if (reg & MT_DEBUG_SHOWPT) {
			ResultDrawAndShow(screen, {MtPoint});
			waitKey(1);
		}
		
		if (roi_enable) MtPoint += tl.roi.tl();
		
		if (reg & MT_RET_XYSCALE) MtPoint = MtPoint * cv_scale;
		
		if (!(reg & MT_DISABLE_LOG)) {
			Logger.info(fformat(
				"MATCH MASK({0})={1},{2} {3}% {4}ms {5}fps {6}",
				tl.text, MtPoint.x, MtPoint.y, int32(MtRate * 100),
				difftime_ms(clock_now - start).count(),
				1000 / difftime_ms(clock_now - start).count(),
				(reg & MT_MASK_MIXED) ? "MIXED" : "DISTANCE"
			));
		}
		
		return MtPoint;
	}
	
	vector<pt2> mask_multi(Mat &screen, const typeTL &tl, f32 thh, int32 reg) {
		clock_type start = clock_now;
		
		// --------- 受入检查 ---------
		if (tl.isNull || tl.alpha.empty() || tl.image.empty() || tl.channel != 4) {
			Logger.error(fformat("MT Mask Multi ERROR ({0}) 无效模板类/空蒙版/空模板/非透明图像", tl.text));
			return MT_FALSE_VEC;
		}
		
		bool RoiEnable = !(tl.reg & ROI_FULL_BIT);
		
		if (RoiEnable) screen = screen(tl.roi);
		
		if (screen.size().height <= tl.height || screen.size().width <= tl.width) {
			Logger.error(fformat("MT Rect Multi ERROR ({0}) 背景过小", tl.text));
			return MT_FALSE_VEC;
		}
		
		// --------- 匹配初始化 ---------
		split(screen, sc3mat);
		split(tl.image, tl3mat);
		
		Mat MtResult{};
		f64 MtRate{};
		pt2 MtPoint{};
		vector<pt2> MtPointList{};
		
		// --------- 匹配 ---------
		forCH(0) {
			matchTemplate(sc3mat[i], tl3mat[i], result3L[i], TM_CCORR_NORMED, tl.alpha);
			minMaxLoc(result3L[i], nullptr, &rate3L[i], nullptr, &point3L[i]);
			if (isinf(rate3L[i])) return MT_FALSE_VEC;
		}
		
		// --------- 额外调试日志 ---------
		if (reg & MT_DEBUG_EXTRALOG) {
			stringstream tempstr;
			if (RoiEnable)tempstr << "ROI" << tl.roi;
			Logger.trace(fformat("MT Mask Multi Result ({0}) {1}", tl.text, tempstr.str()));
			
			forCH(0) cout << "                    " << point3L[i] << "    " << rate3L[i] << endl;
		}
		
		// --------- 判断结果 ---------
		
		MtResult = result3L[0] / MT_MASK_SUPPORT_CH_NUM;
		forCH(1) MtResult += result3L[i] / MT_MASK_SUPPORT_CH_NUM;
		
		while (true) {
			minMaxLoc(MtResult, nullptr, &MtRate, nullptr, &MtPoint);
			if (isinf(MtRate) || MtRate < thh) break;
			rectangle(MtResult, MtPoint, MtPoint + pt2(tl.width, tl.height), Scalar(0, 0, 0), 4);
			if (reg & MT_RET_XYCENTER) MtPoint = pt2(MtPoint.x + tl.width / 2, MtPoint.y + tl.height / 2);
			MtPointList.push_back(MtPoint);
		}
		
		if (MtPointList.empty()) return MT_FALSE_VEC;
		
		// --------- 成功结果 ---------
		if (!(reg & MT_DISABLE_LOG)) {
			Logger.info(fformat(
				"MATCH RECT MULTI({0}):{1} {4}ms {5}fps {6}",
				tl.text, MtPointList.size(), "", "",
				difftime_ms(clock_now - start).count(),
				1000 / difftime_ms(clock_now - start).count(),
				(reg & MT_MASK_MIXED) ? "MIXED" : "DISTANCE"
			));
		}
		
		// --------- 显示结果 ---------
		if (reg & MT_DEBUG_SHOWPT) {
			ResultDrawAndShow(screen, MtPointList);
			waitKey(1);
		}
		
		// --------- 转换结果 ---------
		for (auto pt: MtPointList) {
			if (RoiEnable) pt += tl.roi.tl();
			if (reg & MT_RET_XYSCALE) pt *= cv_scale;
		}
		
		return MtPointList;
	}
	
	pt2 rect(Mat &screen, const typeTL &tl, f32 thh, int32 reg) {
		clock_type start = clock_now;
		
		if (tl.isNull || tl.image.empty()) {
			Logger.error(fformat("MT Rect ERROR ({0}) 无效模板类/空模板", tl.text));
			return MT_FALSE_PT2;
		}
		
		bool RoiEnable = !(tl.reg & ROI_FULL_BIT);
		
		Mat MtResult{};
		f64 MtRate{};
		pt2 MtPoint{};
		
		if (RoiEnable) screen = screen(tl.roi);
		
		if (screen.size().height <= tl.height || screen.size().width <= tl.width) {
			Logger.error(fformat("MT Rect ERROR ({0}) 背景过小", tl.text));
			return MT_FALSE_PT2;
		}
		
		Mat tlMat = tl.image.clone();
		
		if (!(reg & MT_MODE_RGB)) {
			if (screen.channels() != 1) cvtColor(screen, screen, COLOR_BGRA2GRAY);
			if (tl.channel != 1) cvtColor(tlMat, tlMat, COLOR_BGR2GRAY);
			if (!tl.alpha.empty()) Logger.warn(fformat("正在将透明模板({0})灰度处理,可能会导致极低的匹配率", tl.text));
		}
		
		matchTemplate(screen, tlMat, MtResult, TM_CCOEFF_NORMED);
		minMaxLoc(MtResult, nullptr, &MtRate, nullptr, &MtPoint);
		
		if (isinf(MtRate)) return MT_FALSE_PT2;
		
		if (MtRate < thh) return MT_FALSE_PT2;
		
		if (reg & MT_RET_RATE) return {int32(MtRate * 1000)};
		
		if (reg & MT_RET_XYCENTER) MtPoint = pt2(MtPoint.x + tl.width / 2, MtPoint.y + tl.height / 2);
		
		if (reg & MT_DEBUG_SHOWPT) {
			ResultDrawAndShow(screen, {MtPoint});
			waitKey(1);
		}
		
		if (RoiEnable) MtPoint += tl.roi.tl();
		
		if (reg & MT_RET_XYSCALE) MtPoint = MtPoint * cv_scale;
		
		if (!(reg & MT_DISABLE_LOG)) {
			Logger.info(fformat(
				"MATCH RECT({0})={1},{2} {3}% {4}ms {5}fps {6}",
				tl.text, MtPoint.x, MtPoint.y, int32(MtRate * 100),
				difftime_ms(clock_now - start).count(),
				1000 / difftime_ms(clock_now - start).count(),
				(!(reg & MT_MODE_RGB)) ? "GARY" : "RGB"
			));
		}
		
		return MtPoint;
	}
	
	vector<pt2> rect_multi(Mat &screen, const typeTL &tl, f32 thh, int32 reg) {
		clock_type start = clock_now;
		
		// --------- 输入检查 ---------
		if (tl.isNull || tl.image.empty()) {
			Logger.error(fformat("MT Rect Multi ERROR ({0}) 无效模板类/空模板", tl.text));
			return MT_FALSE_VEC;
		}
		
		bool RoiEnable = !(tl.reg & ROI_FULL_BIT);
		
		if (RoiEnable) screen = screen(tl.roi);
		
		if (screen.size().height <= tl.height || screen.size().width <= tl.width) {
			Logger.error(fformat("MT Rect Multi ERROR ({0}) 背景过小", tl.text));
			return MT_FALSE_VEC;
		}
		
		// --------- 匹配初始化 ---------
		Mat MtResult{};
		f64 MtRate{};
		pt2 MtPoint{};
		vector<pt2> MtPointList{};
		Mat tlMat = tl.image.clone();
		
		if (!(reg & MT_MODE_RGB)) {
			if (screen.channels() != 1) cvtColor(screen, screen, COLOR_BGRA2GRAY);
			if (tl.channel != 1) cvtColor(tlMat, tlMat, COLOR_BGR2GRAY);
			if (!tl.alpha.empty()) Logger.warn(fformat("MT Rect Multi WARN 正在将透明模板({0})灰度处理,可能会导致极低的匹配率", tl.text));
		}
		
		// --------- 匹配 ---------
		matchTemplate(screen, tlMat, MtResult, TM_CCOEFF_NORMED);
		
		while (true) {
			minMaxLoc(MtResult, nullptr, &MtRate, nullptr, &MtPoint);
			if (isinf(MtRate) || MtRate < thh) break;
			rectangle(MtResult, MtPoint, MtPoint + pt2(tl.width, tl.height), Scalar(0, 0, 0), 4);
			if (reg & MT_RET_XYCENTER) MtPoint = pt2(MtPoint.x + tl.width / 2, MtPoint.y + tl.height / 2);
			MtPointList.push_back(MtPoint);
		}
		
		if (MtPointList.empty()) return MT_FALSE_VEC;
		
		// --------- 成功结果 ---------
		if (!(reg & MT_DISABLE_LOG)) {
			Logger.info(fformat(
				"MATCH RECT MULTI({0}):{1} {4}ms {5}fps {6}",
				tl.text, MtPointList.size(), "", "",
				difftime_ms(clock_now - start).count(),
				1000 / difftime_ms(clock_now - start).count(),
				(!(reg & MT_MODE_RGB)) ? "GARY" : "RGB"
			));
		}
		
		// --------- 显示结果 ---------
		if (reg & MT_DEBUG_SHOWPT) {
			ResultDrawAndShow(screen, MtPointList);
			waitKey(1);
		}
		
		// --------- 转换结果 ---------
		for (auto pt: MtPointList) {
			if (RoiEnable) pt += tl.roi.tl();
			if (reg & MT_RET_XYSCALE) pt *= cv_scale;
		}
		
		return MtPointList;
	}
	
	vec_int32 matchtl(Mat &screen, vec_tl &tllist, f32 thh, int32 reg) {
		for (const templ::TL &tl: tllist) {
			if (tl.alpha.empty()) {
				if (reg & MT_RET_COUNT) {
					vector<pt2> ptlist = rect_multi(screen, tl, thh, reg);
					if (ptlist.empty()) return MT_FALSE_VEC;
					return {int32(ptlist.size())};
				}
				pt2 result = rect(screen, tl, thh, reg);
				if (result.x == -1) return MT_FALSE_VEC;
				return {result.x, result.y};
			} else {
				if (reg & MT_RET_COUNT) {
					Logger.error(fformat("MatchTL({0}) 透明模板对象没有可用的计数函数 (MT_RET_COUNT)无效 ", tl.text));
					return MT_FALSE_VEC;
				}
				pt2 result = mask(screen, tl, thh, reg);
				if (result.x == -1) return MT_FALSE_VEC;
				return {result.x, result.y};
			}
		}
		return MT_FALSE_VEC;
	}
	
	vec_int32 matchtl(Mat &screen, const templ::TL &tl, f32 thh, int32 reg) {
		if (tl.alpha.empty()) {
			if (reg & MT_RET_COUNT) {
				vector<pt2> ptlist = rect_multi(screen, tl, thh, reg);
				if (ptlist.empty()) return MT_FALSE_VEC;
				return {int32(ptlist.size())};
			}
			pt2 result = rect(screen, tl, thh, reg);
			if (result.x < 0) return MT_FALSE_VEC;
			return {result.x, result.y};
		} else {
			if (reg & MT_RET_COUNT) {
				vector<pt2> ptlist = mask_multi(screen, tl, thh, reg);
				if (ptlist.empty()) return MT_FALSE_VEC;
				return {int32(ptlist.size())};
			}
			pt2 result = mask(screen, tl, thh, reg);
			if (result.x < 0) return MT_FALSE_VEC;
			return {result.x, result.y};
		}
	}
}
