
#ifndef HELPER_EMULATOR_HPP
#define HELPER_EMULATOR_HPP

#include <global.hpp>

#include <map>
#include <list>

#include <pugixml.hpp>

namespace platform {
	
	#define TypeMapFilter std::map<std::string, uint8_t>
	
	
	/**
	 @brief WinAPI 句柄枚举回调函数
	**/
	BOOL CALLBACK WEnmuProcess(HWND hwnd, LPARAM lParam);
	bool InitFliter();
	bool WEnumFilter(TypeMapFilter wb, const std::string &str);
	
	/**
	 @brief 窗口结构{hWnd:句柄,classname:类名,titlename:标题}
	**/
	struct TypeWData {
		HWND hWnd = nullptr;
		std::string classname;
		std::string titlename;
	};
	
	/**
	 @brief \n 窗口管理器，储存与提供过滤后的窗口信息
	 		\n \成员
	 			list\<TypeWData\> wlist 储存列表
	 		\n  list\<TypeWData\>::iterator point 列表遍历器
	 		\n \方法
	 			print() 打印信息
	 		\n	add(HWND h, const std::string &c, const std::string &t, RECT r)
	 		\n	find(const std::string &str) 返回列表内找到的句柄(HWND)或0(NULL)
	 		\n	update(bool EnableBlackList = false) 重新枚举窗口更新列表
	**/
	class TypeWList {
	  private:
		std::list<TypeWData> wlist;
		std::list<TypeWData>::iterator point;
	  public:
		TypeWList();
		void add(HWND h, const std::string &c, const std::string &t, RECT r);
		void print();
		HWND find(const std::string &str);
		void update(bool EnableBlackList = false);
	};
	
	/**
	 @brief 窗口管理器对象，原则上作用域内唯一
	**/
	extern TypeWList MuList;
	
	struct rect2p {
		int16 w = 0;
		int16 h = 0;
	};
	
	/**
	 @brief \n 安卓模拟器对象，用于截取安卓画面与虚拟光标输入
	**/
	class TypeMEMU {
	  private:
		std::string deviceId, titleText, inputDevice;
		HWND hWndTop{}, hWndRender{};
		RECT rectTop{}, rectRender{};
		uint16 L{}, T{}, R{}, B{};
	  public:
		pt2 maxPosition, runResolution, sizeMainWindow;
		cv::Mat screen;
		
		TypeMEMU(std::string a, std::string t, std::string i, pt2 r);
		void Update(bool quiet = false);
		void SetWindow();
		cv::Mat CaptureRender(bool resize = true);
		void InputClick(pt2 pt, uint16 t, int8_t u = utime::ms);
		void InputClick(cv::Point2f pt, uint16 t, int8_t u);
		void InputSlide(pt2 p1, pt2 p2, uint16 t);
		
	};
	
}

#endif //HELPER_EMULATOR_HPP
