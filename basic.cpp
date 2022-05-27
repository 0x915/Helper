#include <basic.hpp>

namespace flag {
	
	FLAG::FLAG(uint16 child, uint16 parent, string note) {
		id = child | parent << 16;
		text = std::move(note);
	}
	
	void Controller::pop() {
		#if FLAG_UPDATE_LOG_FULL_SHOW
		stringstream t;
		layer.pop_back();
		for (FLAG *s: layer) t << "/" << s->text;
		Logger.trace(fformat("返回[{1}] ({0})",
							 t.str().replace(t.str().find('/'), 1, ""),
							 layer.back()->text));
		#else
		Logger.trace(fformat("POP ↓ {0}", layer.back().text));
			layer.pop_back();
		#endif
		
	}
	
	void Controller::push(FLAG *newflag) {
		#if FLAG_UPDATE_LOG_FULL_SHOW
		stringstream t;
		layer.push_back(newflag);
		for (FLAG *s: layer) t << "/" << s->text;
		Logger.trace(fformat("进入[{1}] ({0})",
							 t.str().replace(t.str().find('/'), 1, ""),
							 layer.back()->text));
		#else
		Logger.trace(fformat("PUSH ↑ {0}", newflag.text));
			layer.push_back(newflag);
		#endif
	}
	
	void Controller::root(FLAG *newflag) {
		#if FLAG_UPDATE_LOG_FULL_SHOW
		stringstream t;
		layer.clear();
		layer.push_back(newflag);
		for (FLAG *s: layer) t << "/" << s->text;
		Logger.trace(fformat("设置根({0})", t.str().replace(t.str().find('/'), 1, "")));
		#else
		Logger.trace(fformat("ROOT ↓ {0}", newflag.text));
			layer.clear();
			layer.push_back(newflag);
		#endif
	}
	
	void Controller::move(FLAG *newflag) {
		#if FLAG_UPDATE_LOG_FULL_SHOW
		stringstream t;
		layer.pop_back();
		layer.push_back(newflag);
		for (FLAG *s: layer) t << "/" << s->text;
		Logger.trace(fformat("移动到[{1}] ({0})",
							 t.str().replace(t.str().find('/'), 1, ""),
							 layer.back()->text));
		#else
		Logger.trace(fformat("MOVE → {0}", newflag.text));
			layer.pop_back();
			layer.push_back(newflag);
		#endif
	}
	
	void Controller::show() {
		stringstream t;
		for (FLAG *s: layer) t << "/" << s->text;
		Logger.info(fformat("当前位于 ({0})", t.str().replace(t.str().find('/'), 1, "")));
	}
	
	bool Controller::isBase(uint16 base) { return (layer.back()->id >> 16) == base; }
	
	bool Controller::isFlag(FLAG *flag) { return flag->id == layer.back()->id; }
	
	uint32 Controller::getFlag() { return layer.back()->id; }
	
	string Controller::getNote() { return layer.back()->text; }
}

namespace basic {
	
	bool visible(platform::TypeMEMU mu, const vector<templ::TL> &tllist, f32 thh, uint32 timeout, uint32 holdtime) {
		if (timeout == 0xFFFFFFFF || tllist.empty()) return false;
		if (tllist.empty()) {
			Logger.error("disappear(): 空的模板对象列表");
			return false;
		}
		
		clock_type start_time = clock_now;
		clock_type break_time = clock_now + chrono::milliseconds(timeout);
		Logger.info(fformat("等待出现[{0}] 超时{1}ms", tllist[0].text, timeout ? string(to_string(timeout)) : "∞"));
		
		for (const templ::TL &tl: tllist) {
			while (true) {
				if (timeout != 0 && timeout_ms(break_time)) {
					Logger.warn(fformat("出现超时[{0}]", tl.text));
					return false;
				}
				mu.CaptureRender();
				if (!match::matchtl(mu.screen, tl, thh, MT_DISABLE_LOG).empty()) {
					if (holdtime) {
						sleep_ms(holdtime);
						mu.CaptureRender();
						if (match::matchtl(mu.screen, tl, thh, MT_DISABLE_LOG).empty()) {
							Logger.info(fformat("出现保持失败[{0}]", tl.text));
							return false;
						}
					}
					Logger.trace(fformat("出现完成[{0}] {1}ms", tl.text, difftime_ms(clock_now - start_time).count()));
					return true;
				}
			}
		}
		return false;
	}
	
	bool disappear(platform::TypeMEMU mu, const vector<templ::TL> &tllist, f32 thh, uint32 timeout, uint32 holdtime) {
		if (timeout == 0xFFFFFFFF) return false;
		if (tllist.empty()) {
			Logger.error("disappear(): 空的模板对象列表");
			return false;
		}
		
		clock_type start_time = clock_now;
		clock_type break_time = clock_now + chrono::milliseconds(timeout);
		Logger.info(fformat("等待消失[{0}] 超时{1}ms", tllist[0].text, timeout ? string(to_string(timeout)) : "∞"));
		
		for (const templ::TL &tl: tllist) {
			while (true) {
				if (timeout != 0 && timeout_ms(break_time)) {
					Logger.warn(fformat("消失超时[{0}]", tl.text));
					return false;
				}
				
				mu.CaptureRender();
				
				if (match::matchtl(mu.screen, tl, thh, MT_DISABLE_LOG).empty()) {
					if (holdtime) {
						sleep_ms(holdtime);
						mu.CaptureRender();
						if (!match::matchtl(mu.screen, tl, thh, MT_DISABLE_LOG).empty()) {
							Logger.info(fformat("消失保持失败[{0}]", tl.text));
							return false;
						}
					}
					Logger.trace(fformat("消失完成[{0}] {1}ms", tl.text, difftime_ms(clock_now - start_time).count()));
					return true;
				}
			}
		}
		return false;
	}
	
	bool click(platform::TypeMEMU mu, const vector<templ::TL> &tllist, f32 thh,
			   click_param param, const click_extra &visi, const click_extra &disa) {
		if (param.count < 1) return false;
		for (const templ::TL &tl: tllist) {
			if (tl.reg & FB_NONE_BIT) continue;
			mu.CaptureRender();
			vec_int32 XY = match::matchtl(mu.screen, tl, thh, 0);
			if (XY.size() == 2) {
				for (int i = 0; i < param.count; i++) {
					pt2 TP(XY[0] + tl.halfW, XY[1] + tl.halfH);
					if (param.random) TP = templ::GetFBrXY(XY, tl);
					mu.InputClick(TP, param.press, utime::ms);
					sleep_ms(param.sleep);
				}
				sleep_ms(param.retdelay);
				
			} else continue;
			
			bool ret = true;
			
			if (visi.enable && visi.timeout != 0xFFFFFFFF) {
				if (visi.tllist.empty())
					ret = visible(mu, tllist, thh, visi.timeout, visi.holdtime);
				else
					ret = visible(mu, visi.tllist, thh, visi.timeout, visi.holdtime);
			}
			if (!ret) return false;
			
			if (disa.enable && disa.timeout != 0xFFFFFFFF) {
				if (disa.tllist.empty())
					ret = disappear(mu, tllist, thh, disa.timeout, disa.holdtime);
				else
					ret = disappear(mu, disa.tllist, thh, disa.timeout, disa.holdtime);
			}
			if (!ret) return false;
			
			return true;
		}
		return false;
	}
	
	int32 exist(platform::TypeMEMU mu, const vector<templ::TL> &tllist, f32 thh) {
		cv::Mat screen;
		for (const templ::TL &tl: tllist) {
			mu.CaptureRender();
			vec_int32 XY = match::matchtl(mu.screen, tl, thh, MT_RET_COUNT | MT_DISABLE_LOG);
			if (!XY.empty() && XY[0] >= 1) {
				//Logger.trace(fformat("存在[{0}] {1}个", tl.text,XY[0]));
				return XY[0];
			}
		}
		return 0;
	}
	
	int32 notexist(platform::TypeMEMU mu, const vector<templ::TL> &tllist, f32 thh) {
		cv::Mat screen;
		for (const templ::TL &tl: tllist) {
			mu.CaptureRender();
			vec_int32 XY = match::matchtl(mu.screen, tl, thh, MT_RET_COUNT | MT_DISABLE_LOG);
			if (!XY.empty() && XY[0] == 0) {
				//Logger.trace(fformat("存在[{0}] {1}个", tl.text,XY[0]));
				return 1;
			}
		}
		return 0;
	}
	
	click_param ClickParamDefault;
}
