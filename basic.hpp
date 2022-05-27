
#include <match.hpp>
#include <emulator.hpp>
#include <list>

#ifndef HELPER_BASIC_HPP
#define HELPER_BASIC_HPP

namespace flag {
	
	#define FLAG_ROOT 1
	#define FLAG_SW 2
	#define FLAG_POP 3
	#define FLAG_PUSH 4
	
	#define FLAG_LOG_MINI false
	#define FLAG_LOG_FULL true
	
	using namespace std;
	
	/**
	 @brief \n 代码块控制标志对象
	 		\n 用于循环块内控制代码块启用，原则上作用域内唯一值
	 		\n \成员 uint32 　id 　　标志值(高16位储存父类ID)
	 		\n  string 　text 　注释值(来自构造函数参数)
	 @param uint16   　 child 　　 标志ID(父类内唯一值,1~65535)
	 @param uint16   　 parent 　　父类ID(程序内唯一值,1~65535)
	 @param string   　 note　　 　注释(将用于代码块定位显示)
	**/
	class FLAG {
	  public:
		uint32 id;
		string text;
		
		
		explicit FLAG(uint16 child = 0, uint16 parent = 0, string note = "Null");
		
	};
	
	#define FLAG_UPDATE_LOG_FULL_SHOW true
	
	/**
	 @brief \n \标志控制器对象
	 		\n 使用层级储存压入的标志对象，以判断特定代码块的启用
	 		\n \方法 pop() 返回上一层标志
	 		\n  push(const FLAG &newflag) 进入一层标志
	 		\n  root(const FLAG &newflag) 设置根标志
	 		\n  move(const FLAG &newflag) 切换同层标志
	 		\n  show() 显示当前标志位置
	 		\n  isBase(uint16 base) 判断是否属于输入父类
	 		\n  isFlag(const FLAG &flag) 判断是否属于输入标志
	 		\n  getFlag() getNote() 获取顶层标志的值或注释
	 		\n \成员 list\<FLAG\> layer 储存标志对象的列表(Private)
	**/
	class Controller {
	  private:
		list<FLAG *> layer{};
	  public:
		
		Controller() = default;
		
		void pop();
		
		void push(FLAG *newflag);
		
		void root(FLAG *newflag);
		
		void move(FLAG *newflag);
		
		void show();
		
		bool isBase(uint16 base);
		
		bool isFlag(FLAG *flag);
		
		uint32 getFlag();
		
		string getNote();
		
	};
	
}

namespace basic {
//	#define VISI true
//	#define EXIST true
//	#define notVISI false
//	#define notEXIST false
	using namespace std;
	
	/**
	 @brief \n 等待模板图像出现 (超时返回)
	 	\输入异常处理
	 	timeout/holdtime时间值(无符号32位整型)
	 	不允许输入32位最大值0xFFFFFFFF，直接返回异常
	 @param TypeMEMU   　　mu 　　 　　模拟器对象
	 @param vector<templ::TL>   　　&tllist 　　模板对象矩阵
	 @param f32     　　thh 　 　 　匹配阈值(受匹配函数约束，默认值0.8)
	 @param uint32 　　timeout 　　超时值(等待timeout毫秒强制退出，默认参数为0无限等待)
	 @param uint32 　　holdtime 　 保持值(出现后等待holdtime毫秒后，再次确认目标存在)
	 @return bool　　正常true　　异常false
	**/
	bool visible(platform::TypeMEMU mu, const vector<templ::TL> &tllist, f32 thh = 0.9, uint32 timeout = 0, uint32 holdtime = 0);
	#define VISI basic::visible
	/**
	 @brief \n 等待模板图像消失 (超时返回)
	 	\输入异常处理
	 	timeout/holdtime时间值(无符号32位整型)
	 	不允许输入32位最大值0xFFFFFFFF，直接返回异常
	 	
	 @param TypeMEMU   　　mu 　　 　　模拟器对象
	 @param vector<templ::TL>   　　&tllist 　　模板对象矩阵
	 @param f32     　　thh 　 　 　匹配阈值(受匹配函数约束，默认值0.8)
	 @param uint32 　　timeout 　　超时值(等待timeout毫秒强制退出，默认参数为0无限等待)
	 @param uint32 　　holdtime 　 保持值(消失后等待holdtime毫秒后，再次确认目标消失)
	 @return bool　　正常true　　异常false
	**/
	bool disappear(platform::TypeMEMU mu, const vector<templ::TL> &tllist, f32 thh = 0.9, uint32 timeout = 0, uint32 holdtime = 0);
	#define notVISI basic::disappear
	/**
	 @brief \n click() 点击参数结构
	 \结构成员
		uint8　 　count　 　　点击次数
	 \n bool　　　random　　　区域随机点击开关
	 \n uint16　　press　 　　点击时按压时间 ClickPressTime MS
	 \n uint16　　sleep　 　　点击后延迟时间 ClickSleepTime MS
	 \n uint16　　retdelay　　函数成功返回延迟 ReturnSleepTime MS
	**/
	struct click_param {
		uint8 count = 1;
		bool random = true;
		uint16 sleep = 1;
		uint16 retdelay = 1;
		uint16 press = 1;
	};
	/**
	 @brief \n click() 响应参数结构
	 \结构成员
		vector<templ::TL>　 　tllist　 　　模板对象矩阵(输入为{}传递当前模板对象矩阵 反正传递非空模板对象矩阵)
	 \n uint32　　 timeout　　　传递的超时值 MS (默认值为0xFFFFFFFF对于调用函数为无效值 必须输入符合要求的值)
	 \n uint32　　 holdtime　 　传递的保持时间值 MS (默认值为0 不使用时无需构造)
	 \n bool　　 　enable　 　　响应使能(默认true 不使用时无需构造)
	**/
	struct click_extra {
		vector<templ::TL> tllist{};
		uint32 timeout = 0xFFFFFFFF;
		uint32 holdtime = 0;
		bool enable = true;
	};
	
	/**
	 @brief \n 点击模板图像
	 \点击结构成员_时间单位均为ms
	 	类型{ uint8, bool, uint16, uint16, uint16 }
	 \n 结构{count=1, random=true, sleep=1, retdelay=1, press=1}
	 
	 \响应结构成员_时间单位均为ms
	 \n 类型{ vector\<templ::TL\>, uint32, uint32, bool }
	 \n 结构{tllist={}, timeout=0xFFFFFFFF, holdtime=0, enable=true}
	 \n
	 \n timeout 　默认值0xFFFFFFFF 　对于被传入函数为无效值 传入其他合理值　　　
	 \n enable　　默认值true 　　　　仅用于调试用途 屏蔽该响应结构传入false
	 
	 @param TypeMEMU   　　mu 　　 　　模拟器对象
	 @param vector<templ::TL>   　　&tllist 　　模板对象矩阵
	 @param f32     　　thh 　 　 　匹配阈值(受匹配函数约束，默认值0.8)
	 @param click_param 　　param 　　　点击参数(参考点击参数结构)
	 @param click_extra 　　&visi 　　　出现响应参数(参考响应参数结构与出现函数)
	 @param click_extra 　　&disa 　　　消失响应参数(参考响应参数结构与消失函数)
	 @return bool　　成功true　　失败false
	**/
	bool click(platform::TypeMEMU mu, const vector<templ::TL> &tllist, f32 thh=0.9,
			   click_param param = {}, const click_extra &visi = {}, const click_extra &disa = {});
	#define CLICK basic::click
	/**
	 @brief \n 判断模板图像是否存在
	 @param TypeMEMU   　　mu 　　 　　模拟器对象
	 @param vector<templ::TL>   　　&tllist 　　模板对象矩阵
	 @param f32     　　thh 　 　 　匹配阈值(受匹配函数约束，默认值0.8)
	 @return bool　　存在true　　不存在false
	**/
	int32 exist(platform::TypeMEMU mu, const vector<templ::TL> &tllist, f32 thh=0.9);
	int32 notexist(platform::TypeMEMU mu, const vector<templ::TL> &tllist, f32 thh=0.9);
	#define EXIST basic::exist
	#define notEXIST basic::notexist
}

#endif //HELPER_BASIC_HPP
