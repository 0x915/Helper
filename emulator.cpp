
#include <emulator.hpp>


namespace platform {
	
	TypeMapFilter whitelist, blacklist;
	
	bool InitFliter() {
		using namespace pugi;
		xml_node node;
		xml_document xmldoc;
		
		if (xmldoc.load_file("config.xml").status == status_ok) {
			node = xmldoc.child("helper-config");
			if (!node) {
				Logger.warn("配置文件没有指定的根节点(helper-config)");
				return false;
			}
			node = node.child("windows");
			if (!node) {
				Logger.warn("配置文件没有指定的节点(helper-config\\windows)");
				return false;
			}
			node = node.child("black-list");
			if (!node) {
				Logger.warn("配置文件没有指定的节点(helper-config\\windows\\black-list)");
				return false;
			} else {
				blacklist.clear();
				//for (xml_node_iterator i = node.begin(); i != node.end(); ++i) {
				for (auto &i: node) {
					std::string value = i.attribute("VALUE").value();
					std::string eq = i.attribute("EQ").value();
					if (!eq.empty()) {
						Logger.trace(fformat("插入句柄Title/Class黑名单 {1} \"{0}\"", value, (eq == "0") ? "&" : "="));
						whitelist.insert(std::pair<std::string, uint8_t>(value, (eq == "0" ? 0 : 1)));
					}
				}
			}
			node = node.parent().child("white-list");
			if (!node) {
				Logger.warn("配置文件没有指定的节点(helper-config\\windows\\black-list)");
				
			} else {
				whitelist.clear();
				//for (xml_node_iterator i = node.begin(); i != node.end(); ++i) {
				for (auto &i: node) {
					std::string value = i.attribute("VALUE").value();
					std::string eq = i.attribute("EQ").value();
					if (!eq.empty()) {
						Logger.trace(fformat("插入句柄Title/Class白名单 {1} \"{0}\" ", value, (eq == "0") ? "&" : "="));
						whitelist.insert(std::pair<std::string, uint8_t>(value, (eq == "0" ? 0 : 1)));
					}
				}
			}
		} else {
			Logger.warn("配置文件读取失败");
			return false;
		}
		return true;
	}
	
	BOOL CALLBACK
	WEnmuProcess(HWND hwnd, LPARAM lParam) {
		char w_title[MAX_PATH], w_class[MAX_PATH];
		bool InBlackList = false;
		RECT w_pos;
		
		GetWindowText(hwnd, w_title, MAX_PATH
		);
		GetClassName(hwnd, w_class, MAX_PATH
		);
		GetWindowRect(hwnd, &w_pos
		);
		if (lParam)
			InBlackList = WEnumFilter(blacklist, w_title);
		if ((w_pos.right - w_pos.left != 0 || w_pos.bottom - w_pos.top != 0)
			&&
			WEnumFilter(whitelist, w_class
			) && !InBlackList
			) {
			MuList.
				add(hwnd, w_class, w_title, w_pos
			);
		}
		
		return (TRUE);
	}
	
	bool WEnumFilter(std::map<std::string, uint8_t> wb, const std::string &str) {
		TypeMapFilter::iterator i;
		if (str.empty())return false;
		for (i = wb.begin(); i != wb.end(); i++) {
			if (i->second) {
				if (str == i->first)return true;
			} else {
				if (str.find(i->first) != -1)return true;
			}
		}
		return false;
	}
	
	TypeWList::TypeWList() {
		wlist.clear();
	}
	
	void TypeWList::add(HWND h, const std::string &c, const std::string &t, RECT r) {
		if (t.empty()) return;
		TypeWData data{.hWnd=h, .classname=c, .titlename=t};
		wlist.push_back(data);
		Logger.info(fmt::format(
			"登记窗口 句柄({2:X}) 标题{{{1}}} \033[91m{7}\033[0m",
			nullzero, wlist.back().titlename, (uint64_t) wlist.back().hWnd,
			r.left, r.top, r.right, r.bottom,
			((r.left < 0 || r.top < 0 || r.right < 0 || r.bottom < 0) ? " 最小化 " : "  ")
		));
	}
	
	void TypeWList::print() {
		if (wlist.empty()) {
			Logger.warn(fmt::format("空的窗口列表(0)"));
			return;
		} else {
			Logger.info(fmt::format("列表内储存了{0} 个窗口↓", wlist.size()));
		}
		
		Logger.info("    ");
		for (point = wlist.begin(); point != wlist.end(); point++) {
			if (point->hWnd == nullptr)continue;
			Logger.info(fformat(
				"    顶层窗口 句柄({2:X}) 类名[{3}] 标题{{{1}}}",
				nullzero, point->titlename, (uint64_t) point->hWnd, point->classname
			));
		}
		Logger.info("    ");
	}
	
	HWND TypeWList::find(const std::string &str) {
		for (point = wlist.begin(); point != wlist.end(); point++) {
			if (str == point->titlename) {
				return point->hWnd;
			}
		}
		Logger.warn(fmt::format("列表内没有标题为{{{0}}}的窗口", str));
		return nullptr;
	}
	
	void TypeWList::update(bool EnableBlackList) {
		wlist.clear();
		EnumWindows(WEnmuProcess, EnableBlackList);
	}
	
	TypeWList MuList;
	
	TypeMEMU::TypeMEMU(std::string a, std::string t, std::string i, pt2 r) {
		deviceId = std::move(a);
		titleText = std::move(t);
		inputDevice = std::move(i);
		runResolution = maxPosition = r;
		Logger.info(fformat("定义MEMU设备 [{1}] [{0}] [{2}] [{3}x{4}]",
							titleText, deviceId, inputDevice,
							runResolution.x, runResolution.y));
		this->Update();
	}
	
	void TypeMEMU::Update(bool quiet) {
		int8 err_count = 0;
		while (true) {
			// 更新 顶层窗体 句柄
			hWndTop = MuList.find(titleText);
			if (hWndTop == nullptr) return;
			// 判断 顶层窗体 可见
			if (IsIconic(hWndTop)) {
				Logger.warn(fformat("MU({0}) 窗口最小化 等待窗口可见...", titleText));
				Logger.warn("为保证截取画面 持续运行时请勿最小化 可使用多桌面放置窗口");
				while (true) {
					GetWindowRect(hWndTop, &rectTop);
					sleep_ms(10);
					if (rectTop.left > 0 && rectTop.right > 0 &&
						rectTop.top > 0 && rectTop.bottom > 0) {
						GetWindowRect(hWndTop, &rectTop);
						sleep_ms(10);
						Logger.info(fformat(
							"MU({1}) 窗口可见 {2},{3}→{4},{5} {6}x{7}",
							(uint64_t) hWndTop, titleText,
							rectTop.left, rectTop.top,
							rectTop.right, rectTop.bottom,
							rectTop.right - rectTop.left, rectTop.bottom - rectTop.top
						));
						break;
					}
				}
			}
			// 更新 渲染子窗 句柄
			hWndRender = FindWindowEx(hWndTop, nullptr, "Qt5QWindowIcon", "MainWindowWindow");
			hWndRender = FindWindowEx(hWndRender, nullptr, "Qt5QWindowIcon", "CenterWidgetWindow");
			hWndRender = FindWindowEx(hWndRender, nullptr, "Qt5QWindowIcon", "RenderWindowWindow");
			// 获取 顶层窗体与渲染子窗 桌面位置
			GetWindowRect(hWndTop, &rectTop);
			GetWindowRect(hWndRender, &rectRender);
			// 计算 渲染子窗距顶层窗体 边缘距离
			L = rectRender.left - rectTop.left;
			T = rectRender.top - rectTop.top;
			R = rectTop.right - rectRender.right;
			B = rectTop.bottom - rectRender.bottom;
			// 计算 顶层窗体长宽
			sizeMainWindow.x = L + runResolution.x + R;
			sizeMainWindow.y = T + runResolution.y + B;
			// 错误 重置
			if (sizeMainWindow.x > dv_width || sizeMainWindow.y > dv_height) {
				err_count++;
				if (err_count > 5) {
					Logger.error(fformat(
						"MEMU({0}) 窗体数据异常[{5:X}]已重试{8}次 L:{1} T:{2} R:{3} B:{4} WH:{6}x{7} ",
						titleText, L, T, R, B, (uint64_t) hWndRender,
						sizeMainWindow.x, sizeMainWindow.y, err_count
					));
				}
				sleep_ms(100);
				continue;
			}
			// 日志 输出
			if (!quiet)
				Logger.info(fformat(
					"MEMU({0}) 窗体数据更新[{5:X}] L:{1} T:{2} R:{3} B:{4} WH:{6}x{7} ",
					titleText, L, T, R, B, (uint64_t) hWndRender,
					sizeMainWindow.x, sizeMainWindow.y
				));
			break;
		}
	}
	
	void TypeMEMU::SetWindow() {
		this->Update(true);
		if (hWndTop == nullptr) return;
		if (SetWindowPos(hWndTop, nullptr,
						 rectTop.left, rectTop.top,
						 sizeMainWindow.x, sizeMainWindow.y,
						 SWP_NOMOVE) == false) {
			auto err = GetLastError();
			TCHAR Buf[128];
			FormatMessageA(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM ,
				nullptr,
				err,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR) &Buf,
				0, nullptr);
			Logger.error(fformat(
				" {0}:{1} SetWindowPos() Fail({2}) {3}. exit",
				__FILE__, __LINE__,err,Buf
			));
			exit(0);
		}
		GetClientRect(hWndRender, &rectRender);
	}
	
	cv::Mat TypeMEMU::CaptureRender(bool resize) {
		#ifdef MuCaptureShowClockUS
		auto start = clock_now;
		#endif
		
		GetClientRect(hWndRender, &rectRender);
		
		while (rectRender.right != runResolution.x || rectRender.bottom != runResolution.y) {
			if (resize) this->SetWindow();
			sleep_ms(10);
		}
		
		int32 &w = runResolution.x;
		int32 &h = runResolution.y;
		
		HDC hDC = GetDC(hWndRender);
		HDC hCDC = CreateCompatibleDC(hDC);
		HBITMAP hbwindow = CreateCompatibleBitmap(hDC, w, h);
		
		BITMAPINFOHEADER bh{
			bh.biSize = sizeof(BITMAPINFOHEADER),
			bh.biWidth = w, bh.biHeight = -h,
			bh.biPlanes = 1,
			bh.biBitCount = 32, bh.biCompression = BI_RGB,
			bh.biSizeImage = 0,
			bh.biXPelsPerMeter = 0, bh.biYPelsPerMeter = 0,
			bh.biClrUsed = 0, bh.biClrImportant = 0
		};
		
		SelectObject(hCDC, hbwindow);
		
		#ifdef MuCaptureUseStretchBlt
		SetStretchBltMode(hCDC, COLORONCOLOR);
				StretchBlt(hCDC, 0, 0, w, h, hDC, 0, 0, w, h, SRCCOPY);
		#else
		BitBlt(hCDC, 0, 0, w, h, hDC, 0, 0, SRCCOPY);
		#endif
		
		#ifdef MuCaptureShowClockUS
		LOG_COUNT_CLOCK_US(start);
		#endif
		
		//LOG_COUNT_CLOCK_US(start);
		cv::Mat buffer;
		buffer.create(h, w, CV_8UC4);
		GetDIBits(hCDC, hbwindow, 0, h, buffer.data, (BITMAPINFO *) &bh, DIB_RGB_COLORS);
		
		DeleteObject(hbwindow);
		DeleteDC(hCDC);
		ReleaseDC(hWndRender, hDC);
		
		cv::cvtColor(buffer, buffer, cv::COLOR_BGRA2BGR);
		screen = buffer.clone();
		return buffer;
	}
	
	void TypeMEMU::InputClick(pt2 pt, uint16 t, int8_t u) {
		if (pt.x > maxPosition.x || pt.y > maxPosition.y || pt.x < 0 || pt.y < 0) {
			Logger.error(fformat("MEMU({0}) 按压点击 {1},{2} 超出范围 {3},{4}",
								 titleText, pt.x, pt.y, maxPosition.x, maxPosition.y));
			return;
		}
		Logger.trace(fformat("MEMU({0}) 按压点击 {1},{2} {3}{4}", titleText, pt.x, pt.y, t, utime::GetUnitString(u)));
		PostMessage(hWndRender, WM_LBUTTONDOWN, VK_LBUTTON, MAKELPARAM(pt.x, pt.y));
		utime::sleep_xs(u, t);
		PostMessage(hWndRender, WM_LBUTTONUP, NULL, (pt.x | (pt.y << 16)));
	}
	
	void TypeMEMU::InputClick(cv::Point2f pt, uint16 t, int8_t u) {
		if (pt.x > 1.0 || pt.x < 0 || pt.y > 1.0 || pt.y < 0) return;
		pt2 pt2int32(int32(pt.x * float(runResolution.x)), int32(pt.y * float(runResolution.y)));
		this->InputClick(pt2int32, t, u);
	}
	
	void TypeMEMU::InputSlide(pt2 p1, pt2 p2, uint16 t) {
		if (p1.x > maxPosition.x || p1.y > maxPosition.y || p2.x > maxPosition.x || p2.y > maxPosition.y) {
			Logger.error(fformat("MEMU({0}) 滑动直线 {1},{2} → {3},{4} 超出范围 1,1 → {3},{4}",
								 titleText, p1.x, p1.y, p2.x, p2.y, maxPosition.x, maxPosition.y));
			return;
		}
		Logger.trace(fformat("MEMU({0}) 滑动直线 {1},{2} {3},{4} {5}x", titleText, p1.x, p1.y, p2.x, p2.y, t));
		// 滑动向量 正负方向
		int32_t lx = int16_t(p2.x) - int16_t(p1.x);
		int32_t ly = int16_t(p2.y) - int16_t(p1.y);
		// 滑动坐标 累加临时
		auto tx = f32(p1.x);
		auto ty = f32(p1.y);
		// 对角矩形 长边步进
		f32 dx, dy;
		if (abs(lx) > abs(ly)) {
			dx = (lx > 0) ? 1 : -1;
			dy = (f32) ly / (f32) abs(lx);
		} else {
			dy = (ly > 0) ? 1 : -1;
			dx = (f32) lx / (f32) abs(ly);
		}
		
		#ifdef MU_Slide_WINAPI_DEBUG
		std::cout << fformat("WINAPI_DEBUG 方向{0},{1} 输入{2},{3} 增量{4},{5}",lx,ly,tx,ty,dx,dy)<<std::endl;
		#endif
		
		// 滑动坐标 发送事件
		PostMessage(hWndRender, WM_LBUTTONDOWN, VK_LBUTTON, MAKELPARAM(p1.x, p1.y));
		for (int i = 0; i < ((lx > ly) ? abs(lx) : abs(ly)); i++) {
			tx += dx;
			ty += dy;
			#ifdef MU_Slide_WINAPI_DEBUG
			std::cout << fformat("WINAPI_DEBUG 进度{0} 输入{1},{2}",i+1,(uint16)(tx), (uint16)(ty))<<std::endl;
			#endif
			PostMessage(hWndRender, WM_MOUSEMOVE, VK_LBUTTON, MAKELPARAM((uint16) tx, (uint16) ty));
			sleep_ns((uint16) (1 * t));
		}
		PostMessage(hWndRender, WM_LBUTTONUP, NULL, MAKELPARAM(p2.x, p2.y));
		
		
	}
	
}
