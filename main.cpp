
#include <global.hpp>
#include <emulator.hpp>
#include <App.PCR.hpp>


using namespace std;

void init() {
	
	//system("cls");
	//system("chcp 65001");
	
	RECT rect;
	
	GetWindowRect(GetDesktopWindow(), &rect);
	ui_width = (rect.right - rect.left);
	ui_height = (rect.bottom - rect.top);
	
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	
	dv_width = GetDeviceCaps(GetDC(nullptr), DESKTOPHORZRES);
	dv_height = GetDeviceCaps(GetDC(nullptr), DESKTOPVERTRES);
	
	ui_scale = (float) dv_width / (float) ui_width;
	system_dpi = GetDpiForSystem();
	
	Logger.info("初始化 版本 1.0.3");
	Logger.info(fformat(
		"分辨率 逻辑[{0}x{1}] 物理[{2}x{3}] 缩放[{4:d}%] DPI感知[{5}:{6}]",
		ui_width, ui_height,
		dv_width, dv_height,
		(uint8_t) (ui_scale * 100),
		system_dpi, (system_dpi > 96 ? "HIDPI" : "NORMAL")
	));
	
	platform::InitFliter();
}

int main(int argc, char *argv[]) {
	init();
	
	Logger.trace(fformat("main start"));
	
	platform::MuList.update();
//	platform::MuList.print();
	
	cv_width = 1280;
	cv_height = 720;
	vm_width = 1920;
	vm_height = 1080;
	cv_scale = (f32) vm_height / (f32) cv_height;
	
	AppPCR::MAIN(argc, argv);

//	platform::TypeMEMU memu("127.0.0.1:21503", "VM", "/dev/input/event6", {1278, 718});

//	chrono::time_point<chrono::steady_clock> start;


//	const templ::TL A("PCR1280720", "00登录01点击屏幕.png", "登录:点击屏幕",
//					  ROI_FULL, FB_NONE);
//
//	const templ::TL B("PCR1280720", "11战斗01下一关.png", "战斗:下一关",
//					  {200, 200, 400, 300, ROI_FULL_BIT}, FB_SELF);
//
//	const templ::TL C("PCR1280720", "04导航01主页.png", "导航:主页",
//					  {11, 22, 33, 44, ROI_FULL_BIT}, templ::FBfrom(fromBR, -100, -100, 20, 20));
//	const templ::TL D("PCR1280720", "05商店10批量选框.png", "商店:批量选框",
//					  ROI_FULL, FB_SELF);
//
	//memu.SetWindow();
	//memu.InputClick({100, 100}, 1000);


//	for (int i = 0; i < 5; i++) {
//		memu.CaptureRender();
//		basic::click(memu, {D}, 0.9,
//					 {1, true, 1, 10, 50});
//	}
//
//	while (false) {
//		auto a = memu.CaptureRender();
//		cv::imshow("memu", a);
//		cv::waitKey(1);
//		//match::mask(vm, B, 0.95, MT_RET_XYCENTER | MT_DEBUG_SHOWPT);
//		//match::rect_multi(vm, D, 0.85, MT_RET_XYCENTER | MT_DEBUG_SHOWPT);
//		//basic::disappear(memu, {D}, 0.9, 5000);
//		//basic::visible(memu, {D}, 0.9, 5000);
//		//basic::exist(memu, {D}, 0.9);
//		//break;
//	}

//	const templ::TL *E;
//	map<string, const templ::TL *> pcr;
//
//	E = new templ::TL("PCR1280720", "05商店10批量选框.png", "DBG:DEBUG0", ROI_FULL, FB_SELF);
//	pcr.insert(pair<string, const templ::TL *>(E->text, E));
//	E = new templ::TL("PCR1280720", "04导航01主页.png", "DBG:DEBUG1", ROI_FULL, FB_SELF);
//	pcr.insert(pair<string, const templ::TL *>(E->text, E));
//	E = new templ::TL("PCR1280720", "00登录01点击屏幕.png", "DBG:DEBUG2", ROI_FULL, FB_SELF);
//	pcr.insert(pair<string, const templ::TL *>(E->text, E));
//	E = new templ::TL("PCR1280720", "11战斗01下一关.png", "DBG:DEBUG3", ROI_FULL, FB_SELF);
//	pcr.insert(pair<string, const templ::TL *>(E->text, E));
//	const flag::FLAG F0(1, 16, "AppPCR");
//	const flag::FLAG F1(2, 16, "登录");
//	const flag::FLAG F2(3, 16, "主界面");
//


//	auto t = templ::GetFBrect({100,100,200,200});
//	if(t.isNull()){
//		cout << fformat("{0} empty",t) <<endl;
//	}else{
//		cout << fformat("{0}",t) <<endl;
//	}



//	memu.InputClick(100,100,utime::ms,500);
//	sleep_ms(50);
//	memu.InputSlide(500, 500, 100, 100, 200);

//	while(true){
//		start  = clock_now;
//		cv::Mat a = memu.CaptureRender();
//		cv::imshow("aaaa",a);
//		//Logger.trace(fformat("CaptureRender finish {0} ms",difftime_ms(clock_now - start).count()));
//		cv::waitKey(1);
//	}

//	vector<float> zero{0, 0, 0, 0, 0, 0, 0, 0, 0};
//	vector<float> temp{0, 0, 0, 0, 0, 0, 0, 0, 0};
//	for (int i = 0; i < 1000; i++) {
//		float f = random::int_cauchy(0, 0);
//		if (f >= 0 && f <= 0.1) temp[0] += 1;
//		else if (f > 0.1 && f <= 0.2) temp[1] += 1;
//		else if (f > 0.2 && f <= 0.3) temp[2] += 1;
//		else if (f > 0.3 && f <= 0.5) temp[3] += 1;
//		else if (f > 0.5 && f <= 0.6) temp[4] += 1;
//		else if (f > 0.6 && f <= 0.7) temp[5] += 1;
//		else if (f > 0.7 && f <= 0.8) temp[6] += 1;
//		else if (f > 0.8 && f <= 0.9) temp[7] += 1;
//		else if (f > 0.9 && f <= 1.0) temp[8] += 1;
//
//		else if (f < 0.0 && f >= -0.1) zero[8] += 1;
//		else if (f < -0.1 && f >= -0.2) zero[7] += 1;
//		else if (f < -0.2 && f >= -0.3) zero[6] += 1;
//		else if (f < -0.3 && f >= -0.5) zero[5] += 1;
//		else if (f < -0.5 && f >= -0.6) zero[4] += 1;
//		else if (f < -0.6 && f >= -0.7) zero[3] += 1;
//		else if (f < -0.7 && f >= -0.8) zero[2] += 1;
//		else if (f < -0.8 && f >= -0.9) zero[1] += 1;
//		else if (f < -0.9 && f >= -1.0) zero[0] += 1;
//	}
//	cout << fformat("{1} {0}", temp, zero) << endl;
	
	Logger.trace(fformat("main return"));
//	Logger.debug(fformat("{0} -> {1}", "日志等级", "debug"));
//
//	Logger.info("info 信息");
//	Logger.warn("warn 信息");
//	Logger.error("error 信息");
//	Logger.critical("critical 信息");
//	Logger.fatal("fatal 信息");
//	while(true);
	return 0;
}
