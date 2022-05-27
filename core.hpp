#ifndef HELPER_CORE_HPP
#define HELPER_CORE_HPP

#include <global.hpp>
#include <opencv2/imgcodecs.hpp>

#include <vector>


namespace templ {
	
	
	#define FB_RECT_BIT 0
	#define FB_SELF_BIT BIT0
	#define FB_ENABLE_BIT 0
	#define FB_NONE_BIT BIT1
	#define ROI_RECT_BIT 0
	#define ROI_FULL_BIT BIT2
	
	#define ROI_FULL {0,0,0,0,ROI_FULL_BIT}
	#define FB_NONE {0,0,0,0,FB_NONE_BIT}
	#define FB_SELF {0,0,0,0,FB_SELF_BIT}
	
	class TL {
	  public:
		std::string file{}, text{};
		cv::Rect roi{}, fb{};
		cv::Mat image{}, alpha{};
		int32 width{}, height{}, channel{}, halfW{}, halfH{};
		bool isNull = false;
		int32 reg = 0;
		
		TL(const std::string &basedir, const std::string &imgfile, const std::string &note,
		   std::vector<int32> roi5i, std::vector<int32> fb5i, std::vector<int32> param = {});
		bool read();
	};
	
	extern TL nulltl;
	extern TL& selftl;
	
	#define fromFULL 10
	#define fromSELF 0
	#define fromTL 1
	#define fromTC 2
	#define fromTR 3
	#define fromCL 4
	#define fromCC 5
	#define fromCR 6
	#define fromBL 7
	#define fromBC 8
	#define fromBR 9
	
	vec_int32 FBfrom(int32 p, int32 x, int32 y, int32 w, int32 h);
	vec_int32 ROIfrom(int32 p, int32 x, int32 y, int32 w, int32 h);
	
	/**
	 @brief \n 获取随机反馈坐标 \n\n
	 		\输入坐标格式
	 		   {x,y} 根据输入模板对象记录的矩形
	 		\n {x1,y1,x2,y2} 定义左上角(x1,y1)与右下角(x2,y2)的矩形
	 		\n {x,y,L,T,R,B} 以(X,Y)为基点定义距离基点(L,T,R,B)四边的矩形
	 @param vec_int32   　 xy 　 　输入坐标(距离值允许负值)
	 @param templ::TL   　 &tl 　　模板对象(默认值/定义FB_NONE_BIT视为异常)
	 @return cv::Point  正常{x,y}  异常{-1,-1}
	**/
	pt2 GetFBrXY(std::vector<int32> xy, const templ::TL &tl = nulltl);
}

#endif //HELPER_CORE_HPP
