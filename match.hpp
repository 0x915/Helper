
#ifndef HELPER_MATCH_HPP
#define HELPER_MATCH_HPP

#include <global.hpp>
#include <core.hpp>
#include <opencv2/imgcodecs.hpp>

namespace match {
	
	// 匹配(空输入) 仅用于外部函数
	#define MT_NULL 0
	
	// 匹配(继承输入) 仅用于外部函数
	#define MT_SELF {}
	
	// 匹配(计数模式) 仅用于自动匹配函数
	#define MT_RET_COUNT BIT1
	
	// 匹配(静默模式) 关闭识别日志
	#define MT_DISABLE_LOG BIT2
	// 匹配(调试显示) 显示匹配点
	#define MT_DEBUG_EXTRALOG BIT3
	// 匹配(调试显示) 显示匹配点
	#define MT_DEBUG_SHOWPT BIT4
	
	// 矩形匹配(图像模式) 如有可能使用彩色匹配 仅用于矩形匹配函数
	#define MT_MODE_RGB BIT5
	// 矩形匹配(图像模式) 如有可能使用灰度匹配 仅用于矩形匹配函数
	#define MT_MODE_GRAY 0
	// 透明匹配(结果判断) 欧式距离/混合提取 仅用于透明匹配函数
	#define MT_MASK_MIXED BIT6
	
	// 匹配(返回概率) 返回0~1000以内的概率值
	#define MT_RET_RATE BIT7
	// 匹配(坐标转换) 转换结果坐标到模板中心
	#define MT_RET_XYCENTER BIT8
	// 匹配(坐标缩放) 缩放到虚拟机内部坐标
	#define MT_RET_XYSCALE BIT9
	
	
	// 匹配(失败返回) 空表
	#define MT_FALSE_VEC {}
	#define MT_FALSE_PT2 {-1,-1}
	
	using namespace cv;
	using namespace std;
	
	typedef templ::TL typeTL;
	typedef const std::vector<typeTL> vec_tl;
	
	/**
	 @brief \n 标记并显示结果，该函数内部没有使用cv::waitKey()
	 @param Mat   　 &mat 　 背景(CV矩阵)
	 @param Vec   　 &pt 　　坐标表(vector<cv::Point>)
	**/
	void ResultDrawAndShow(Mat &mat, const vector<pt2> &pt);
	
	/**
	 @brief \n 透明匹配：仅支持彩色匹配，要求传入含有效蒙版的模板对象
	 @param Mat   　 &screen 　 背景(三通道CV矩阵)
	 @param Obj   　 &tl 　　　 模板对象(蒙版+三通道CV矩阵)
	 @param f32   　 thh 　　　 阈值(大于0.95的32位浮点数)
	 @param int32 　 reg 　　　 配置表(参考宏定义中位定义)
	 @return std::vector\<int32\>  失败{}  成功{x,y}
	**/
	pt2 mask(Mat &screen, const typeTL &tl, f32 thh=0.95, int32 reg=0);
	
	/**
	 @brief \n 透明多目标匹配：仅支持彩色匹配，要求传入含有效蒙版的模板对象，强制混合结果计算
	 @param Mat   　 &screen 　 背景(三通道CV矩阵)
	 @param Obj   　 &tl 　　　 模板对象(蒙版+三通道CV矩阵)
	 @param f32   　 thh 　　　 阈值(大于0.95的32位浮点数)
	 @param int32 　 reg 　　　 配置表(参考宏定义中位定义)
	 @return std::vector\<int32\>  失败{}  成功{Point1,Point2,...}
	**/
	vector<pt2> mask_multi(Mat &screen, const typeTL &tl, f32 thh=0.95, int32 reg=0);
	
	/**
	 @brief \n 矩形匹配：支持彩色与灰度匹配，要求传入含有效蒙版的模板对象
	 @param Mat   　 &screen 　 背景(三通道CV矩阵)
	 @param Obj   　 &tl 　　　 模板对象(三通道CV矩阵)
	 @param f32   　 thh 　　　 阈值(32位浮点数)
	 @param int32 　 reg 　　　 配置表(参考宏定义中位定义)
	 @return std::vector\<int32\>  失败{}  成功{x,y}
	**/
	pt2 rect(Mat &screen, const typeTL &tl, f32 thh=0.8, int32 reg=0);
	
	/**
	 @brief \n 矩形多目标匹配：支持彩色与灰度匹配，要求传入含有效蒙版的模板对象
	 @param Mat   　 &screen 　 背景(三通道CV矩阵)
	 @param Obj   　 &tl 　　　 模板对象(三通道CV矩阵)
	 @param f32   　 thh 　　　 阈值(32位浮点数)
	 @param int32 　 reg 　　　 配置表(参考宏定义中位定义)
	 @return std::vector\<cv::Point\>  失败{}  成功{Point1,Point2,...}
	**/
	vector<pt2> rect_multi(Mat &screen, const typeTL &tl, f32 thh=0.8, int32 reg=0);
	
	/**
	 @brief \n 自动匹配：根据模板对象的蒙版属性，使用透明或矩形匹配函数
	 @param Mat   　 &screen 　 背景(三通道CV矩阵)
	 @param vec   　 &tl 　　　 模板对象矩阵(蒙版+三通道CV矩阵)
	 @param f32   　 thh 　　　 阈值(32位浮点数) 透明匹配需大于0.95
	 @param int32 　 reg 　　　 配置表(参考宏定义中位定义)
	 @return std::vector\<int32\>  失败{}  成功{x,y}  计数{n}
	**/
	vec_int32 matchtl(Mat &screen, vec_tl& tllist, f32 thh=0.8, int32 reg=0);
	
	/**
	 @brief \n 自动匹配：根据模板对象的蒙版属性，使用透明或矩形匹配函数
	 @param Mat   　 &screen 　 背景(三通道CV矩阵)
	 @param Obj   　 &tl 　　　 模板对象(蒙版+三通道CV矩阵)
	 @param f32   　 thh 　　　 阈值(32位浮点数) 透明匹配需大于0.95
	 @param int32 　 reg 　　　 配置表(参考宏定义中位定义)
	 @return std::vector\<int32\>  失败{}  成功{x,y}  计数{n}
	**/
	vec_int32 matchtl(Mat &screen, const templ::TL &tl, f32 thh=0.8, int32 reg=0);
}

#endif //HELPER_MATCH_HPP
