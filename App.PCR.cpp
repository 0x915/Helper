#include <global.hpp>
#include <emulator.hpp>
#include <core.hpp>
#include <match.hpp>
#include <basic.hpp>
#include <io.h>

#include "App.PCR.hpp"

namespace AppPCR {
	
	using namespace std;
	
	namespace PRIVATE {
		map<string, vector<templ::TL>> T{};
		
		class USE {
		  public:
			vector<templ::TL> operator[](const string &key) {
				if (T.count(key) == 0) {
					Logger.critical(fformat("[AppPCR] 使用了不存在的键值(模板id) \"{0}\"", key));
					exit(0);
				}
				return T[key];
			}
		} PCR;
		
		flag::Controller l;
		
		flag::FLAG nullFlag(0, 0, "null");
		
		flag::FLAG &FlagDefault = nullFlag;
		
		bool READ(const string &xmlfile) {
			vec_int32 roi5i, fb5i;
			using namespace pugi;
			xml_node node;
			xml_document xmldoc;
			if (xmldoc.load_file(xmlfile.c_str()).status == status_ok) {
				node = xmldoc.child("pcr");
				if (!node) {
					Logger.warn("模板配置文件没有指定的根节点(pcr)");
					xmldoc.append_child("helper-config");
					return false;
				}
				string root = node.attribute("root").value();
				int32 n = 0;
				for (auto &i: node) {
					std::string id = i.attribute("id").value();
					
					std::string file = i.attribute("file").value();
					
					string path = root + "/" + file; // NOLINT(performance-inefficient-string-concatenation)
					if (_access(path.c_str(), 0) != 0) {
						Logger.error(fformat("定义模板[{0}]({1})异常 不存在的文件", id, file));
						Logger.info("-----------------------------------------------------------------------");
						continue;
					}
					if (_access(path.c_str(), 6) != 0) {
						Logger.error(fformat("定义模板[{0}]({1})异常 不可读的文件", id, file));
						Logger.info("-----------------------------------------------------------------------");
						continue;
					}
					
					std::string note = i.attribute("note").value();
					
					std::string roi = i.attribute("roi").value();
					vector<string> r = ustr::split(roi, ' ');
					if (r.size() == 1) {
						roi5i = ROI_FULL;
					} else if (r.size() == 4) {
						roi5i = {stoi(r[0]), stoi(r[1]), stoi(r[2]), stoi(r[3]), ROI_RECT_BIT};
					} else {
						Logger.error(fformat("定义模板[{0}]({1})错误 \"{2}\" 无法识别的ROI格式", id, file, roi));
						Logger.info("-----------------------------------------------------------------------");
						continue;
					}
					
					std::string fb = i.attribute("fb").value();
					vector<string> f = ustr::split(fb, ' ');
					if (f.size() == 1) {
						if (f[0] == "SELF") fb5i = FB_SELF;
						else fb5i = FB_NONE;
					} else if (f.size() == 4) {
						fb5i = {stoi(f[0]), stoi(f[1]), stoi(f[2]), stoi(f[3]), 0};
					} else if (f.size() == 5) {
						fb5i = {stoi(f[0]), stoi(f[1]), stoi(f[2]), stoi(f[3]), FB_SELF_BIT};
					} else {
						Logger.error(fformat("定义模板[{0}]({1})错误 \"{2}\" 无法识别的FB格式", id, file, fb));
						Logger.info("-----------------------------------------------------------------------");
						continue;
					}
					
					auto tl = templ::TL(root, file, note, roi5i, fb5i);
					if (tl.isNull) {
						Logger.error(fformat("定义模板[{0}]({1})失败 构造的模板对象无效", id, file));
						Logger.info("-----------------------------------------------------------------------");
						continue;
					}
					
					if (T.find(id) == T.end()) {
						T.insert({id, {tl}});
					} else {
						T[id].push_back(tl);
					}
					
					n++;
				}
				Logger.info(fformat("根据 {1} 定义 {0} 个模板对象", n, xmlfile));
				
			} else {
				Logger.warn("模板配置文件读取失败");
				return false;
			}
			return true;
		}
		
		bool INIT(const uint16 &base, const string &name, vector<string> &flagMake, map<const string, flag::FLAG> &flag) {
			for (int i = 0; i < flagMake.size(); i++) {
				if (count(flagMake.begin(), flagMake.end(), flagMake[i]) > 1) {
					Logger.error(fformat("定义重复的标志({2}) 位置{1}:\"{0}\" 该标志不会被载入", flagMake[i], i, name));
				} else { flag.insert({flagMake[i], flag::FLAG(i + 1, base, flagMake[i])}); }
			}
			if (flag.size() == flagMake.size()) {
				Logger.trace(fformat("标志载入完成({0})", name));
			} else {
				Logger.error(fformat(
					"标志载入不全({2}) 定义{0}个/载入{1}个 可能会导致进程异常终止",
					flagMake.size(), flag.size(), name));
			}
			return true;
		}
	}
	
	using namespace PRIVATE;
	
	
	namespace AutoStory {
		bool init = false;
		const uint16 base = 0x00A0;
		const string name = "自动查看剧情";
		map<const string, flag::FLAG> flag;
		
		vector<string> flagMake{
			"首页", "列表", "内容", "结束",
		};
		
		bool MAIN(platform::TypeMEMU &mu, flag::FLAG *go = &nullFlag) {
			if (!init) init = INIT(base, name, flagMake, flag);
			using namespace basic;
			
			const string Title = "剧情查看";
			if (go == &nullFlag) l.push(&flag["首页"]);
			else l.push(go);
			
			while (l.isBase(base)) {
				while (l.isFlag(&flag["首页"])) {
					click(mu, PCR["剧情:首页新内容"], 0.9, {1, true}, {}, {MT_SELF, 500});
					if (exist(mu, PCR["剧情:子页面"])) l.move(&flag["列表"]);
					if (exist(mu, PCR["剧情:菜单"])) l.move(&flag["内容"]);
				}
				
				while (l.isFlag(&flag["列表"])) {
					click(mu, PCR["剧情:子页新内容"], 0.96, {1, true}, {}, {MT_SELF, 500});
					click(mu, PCR["剧情:数据下载"], 0.9, {1, true}, {}, {MT_SELF, 500});
					if (exist(mu, PCR["剧情:菜单"])) l.move(&flag["内容"]);
					
					if (!exist(mu, PCR["剧情:子页新内容"]))
						click(mu, PCR["通用:返回"], 0.9, {1, true}, {}, {MT_SELF, 500});
					if (exist(mu, PCR["剧情:主线"])) l.move(&flag["首页"]);
					
				}
				while (l.isFlag(&flag["内容"])) {
					click(mu, PCR["剧情:跳过"], 0.9, {1, true}, {}, {MT_SELF, 500});
					click(mu, PCR["剧情:跳过确认"], 0.9, {1, true}, {}, {MT_SELF, 500});
					if (!exist(mu, PCR["剧情:跳过"])) {
						click(mu, PCR["剧情:菜单"], 0.9, {1, true}, {}, {MT_SELF, 500});
					}
					click(mu, PCR["剧情:关闭"], 0.9, {1, true}, {}, {MT_SELF, 500});
					click(mu, PCR["剧情:数据下载"], 0.9, {1, true}, {}, {MT_SELF, 500});
					
				}
				while (l.isFlag(&flag["结束"])) {
				
				}
				// PASS 终止
				while (l.isFlag(&flag["结束"])) {
					Logger.info(fformat("{0} 完成于[{1}]", Title, l.getNote()));
					l.pop();
					return true;
				}
			}
			return false;
		}
	}
	
	namespace JoinLevelNull {
		bool init = false;
		const uint16 base = 0x00A0;
		const string name = "自动挑战关卡";
		map<const string, flag::FLAG> flag;
		
		vector<string> flagMake{
			"等待加载", "配置关卡", "配置队伍", "等待结束", "等待结算", "关卡完成",
		};
		
		bool MAIN(platform::TypeMEMU &mu, flag::FLAG *go = &nullFlag) {
			if (!init) init = INIT(base, name, flagMake, flag);
			using namespace basic;
			
			const string Title = "关卡挑战";
			if (go == &nullFlag) l.push(&flag["配置关卡"]);
			else l.push(go);
			
			while (l.isBase(base)) {
				// PASS L1 关卡面板 画面
				while (l.isFlag(&flag["配置关卡"])) {
					// Note 寻找 {挑战按钮} -> 点击 {挑战} -> 反馈 {队伍编组面板}
					click(mu, PCR["战斗:挑战按钮"], 0.9, {1, true},
						  {PCR["战斗:队伍编组"], 1000}, {});
					
					// Note 寻找 {队伍编组面板} -> 跳转 L2 队伍配置
					if (exist(mu, PCR["战斗:队伍编组"])) {
						l.move(&flag["配置队伍"]);
						break;
					}
					
					// Note 寻找 {体力不足标志} -> 暂停等待 手动处理
					if (exist(mu, PCR["战斗:体力不足"])) {
						Logger.error("体力不足 暂停等待手动处理 在显示关卡挑战面板时 输入'\\'并回车继续");
						cout << endl;
						while (true) {
							char input;
							cin >> input;
							if (input == '\\') break;
						}
						
					}
				}
				// PASS L2 队伍配置 画面
				while (l.isFlag(&flag["配置队伍"])) {
					// Note 寻找 {空队伍标志} -> 点击 {我的队伍} -> 反馈 {呼出此编组按钮}
					if (exist(mu, PCR["战斗:空队伍"])) {
						click(mu, PCR["战斗:我的队伍"], 0.8, {1, true}, {PCR["战斗:队伍一览"], 1000}, {});
					}
					
					if (exist(mu, PCR["战斗:队伍一览"])) {
						click(mu, PCR["战斗:使用编组"], 0.8, {1, true}, {PCR["战斗:战斗开始"], 1000}, {});
					}
					
					// Note 查无 {空队伍标志} -> 寻找 {战斗开始} -> 点击 {战斗开始} -> 反馈消失
					if (not exist(mu, PCR["战斗:空队伍"])) {
						click(mu, PCR["战斗:战斗开始"], 0.8, {1, true}, {}, {MT_SELF, 1000});
					}
					
					// Note 寻找 {载入中} -> 跳转 L3 正在战斗
					if (exist(mu, PCR["通用:载入中"])) {
						l.move(&flag["等待结束"]);
						break;
					}
					
				}
				// PASS L3 正在战斗 画面
				while (l.isFlag(&flag["等待结束"])) {
					// Note 寻找 {自动战斗标志/时间沙漏} -> 保持
					if (exist(mu, PCR["战斗:自动"]) or
						exist(mu, PCR["战斗:时间沙漏"], 0.975)
						) { continue; }
					
					// Note 寻找 {战斗胜利标志} -> 跳转 L4 胜利结算
					if (exist(mu, PCR["地下城:胜利"], 0.975) or
						exist(mu, PCR["战斗:挑战胜利"], 0.975) or
						exist(mu, PCR["战斗:玩家等级"], 0.975) or
						exist(mu, PCR["战斗:好感度"], 0.975)
						) { l.move(&flag["等待结算"]); }
				}
				// PASS L4 胜利结算 画面
				while (l.isFlag(&flag["等待结算"])) {
					// Note 寻找 {结算下一步按钮} -> 点击 {下一步} -> 反馈 {自己消失}
					//      寻找 {获得道具/挑战胜利} -> 点击 {获得道具/挑战胜利}
					if (not click(mu, PCR["战斗:下一步"], 0.8, {1, true})) {
						click(mu, PCR["战斗:挑战胜利"], 0.8, {1, true, 10});
						click(mu, PCR["战斗:获得道具"], 0.8, {1, true, 10});
					}
					
					// Note 寻找 {限定商店窗口} -> 点击 {取消按钮} -> 反馈消失
					if (exist(mu, PCR["战斗:限定商店"])) {
						click(mu, PCR["战斗:商店取消"], 0.8, {1, true},
							  {}, {MT_SELF, 1000});
					}
					
					// Note 校验 出现 {载入中} -> 跳转 完成返回
					if (exist(mu, PCR["通用:载入中"])) {
						l.move(&flag["关卡完成"]);
						break;
					}
				}
				// PASS 完成返回
				while (l.isFlag(&flag["关卡完成"])) {
					Logger.info(fformat("{0} 完成于[{1}]", Title, l.getNote()));
					l.pop();
					return true;
				}
			}
			return false;
		}
	}
	
	namespace AutoDungeon {
		bool init = false;
		const uint16 base = 0x00FD;
		const string name = "自动地下城绿龙";
		map<const string, flag::FLAG> flag;
		
		vector<string> flagMake{
			"地下城", "地域", "一层", "二层", "三层", "四层", "五层", "报酬", "撤退", "结束"
		};
		
		bool MAIN(platform::TypeMEMU &mu, flag::FLAG *go = &nullFlag, bool ByPassBoss = true) {
			if (!init) init = INIT(base, name, flagMake, flag);
			using namespace basic;
			const string Title = "地下城";
			l.push(&flag["地下城"]);
			if (go == &nullFlag) l.push(&flag["地域"]);
			else l.push(go);
			
			while (l.isBase(base)) {
				// PASS 地下城选择与撤退
				while (l.isFlag(&flag["地域"])) {
					// Note 页面：地下城选择地域
					//      寻找 {剩余次数0/1} 跳转 -> 次数不足返回
					if (exist(mu, PCR["地下城:EX3"]) and exist(mu, PCR["地下城:次数0/1"])) {
						Logger.warn("地下城次数不足 返回");
						l.move(&flag["结束"]);
						break;
					}
					
					// Note 页面：地下城选择地域 + 确认地域
					//      寻找 {剩余次数1/1} -> 点击 {地下城EX3} -> 反馈 {区域确认按钮}
					if (exist(mu, PCR["地下城:EX3"]) and exist(mu, PCR["地下城:次数1/1"])) {
						click(mu, PCR["地下城:EX3"], 0.8, {1, true}, {PCR["地下城:确认地域"], 2000}, {});
					}
					click(mu, PCR["地下城:确认地域"], 0.8, {1, true}, {}, {MT_SELF, 2000});
					
					// Note 页面：地下城地域地图内
					//      寻找 {N/5} -> 跳转 {N层}
					if (exist(mu, PCR["地下城:撤退"])) {
						if (exist(mu, PCR["地下城:1/5"], 0.975)) {
							l.push(&flag["一层"]);
							break;
						} else if (exist(mu, PCR["地下城:2/5"], 0.975)) {
							l.push(&flag["二层"]);
							break;
						} else if (exist(mu, PCR["地下城:3/5"], 0.975)) {
							l.push(&flag["三层"]);
							break;
						} else if (exist(mu, PCR["地下城:4/5"], 0.975)) {
							l.push(&flag["四层"]);
							break;
						} else if (exist(mu, PCR["地下城:5/5"], 0.975)) {
							l.push(&flag["五层"]);
							break;
						}
					}
				}
				// PASS 地下城N层
				while (l.isFlag(&flag["一层"])) {
					click(mu, PCR["地下城:1层"], 0.8, {1, true}, {PCR["战斗:挑战按钮"], 2000}, {});
					if (exist(mu, PCR["战斗:挑战按钮"])) {
						JoinLevelNull::MAIN(mu);
						sleep_ms(1000);
						l.move(&flag["报酬"]);
					}
				}
				while (l.isFlag(&flag["二层"])) {
					click(mu, PCR["地下城:2层"], 0.8, {1, true}, {PCR["战斗:挑战按钮"], 2000}, {});
					if (exist(mu, PCR["战斗:挑战按钮"])) {
						JoinLevelNull::MAIN(mu);
						sleep_ms(1000);
						l.move(&flag["报酬"]);
					}
				}
				while (l.isFlag(&flag["三层"])) {
					click(mu, PCR["地下城:3层"], 0.8, {1, true}, {PCR["战斗:挑战按钮"], 2000}, {});
					if (exist(mu, PCR["战斗:挑战按钮"])) {
						JoinLevelNull::MAIN(mu);
						sleep_ms(1000);
						l.move(&flag["报酬"]);
					}
				}
				while (l.isFlag(&flag["四层"])) {
					click(mu, PCR["地下城:4层"], 0.8, {1, true}, {PCR["战斗:挑战按钮"], 2000}, {});
					if (exist(mu, PCR["战斗:挑战按钮"])) {
						JoinLevelNull::MAIN(mu);
						sleep_ms(1000);
						l.move(&flag["报酬"]);
					}
				}
				while (l.isFlag(&flag["五层"])) {
					if (ByPassBoss and exist(mu, PCR["地下城:5/5"])) {
						Logger.warn(fformat("{0} 五层BOSS跳过", Title));
						l.pop();
						l.move(&flag["结束"]);
//						if (exist(mu, PCR["地下城:次数1/1"], 0.9)) {
//							l.move(&flag["撤退"]);
//						} else if (exist(mu, PCR["地下城:次数0/1"])) {
//							l.move(&flag["结束"]);
//						}
					}
				}
				// PASS 地下城每层报酬
				while (l.isFlag(&flag["报酬"])) {
					visible(mu, PCR["地下城:报酬窗口"], 0.9, 0);
					while (exist(mu, PCR["地下城:报酬确认"])) {
						click(mu, PCR["地下城:报酬确认"], 0.8, {1, true}, {}, {});
					}
					l.pop();
				}
				// PASS 地下城撤退
				while (l.isFlag(&flag["撤退"])) {
					// Note 页面：地下城当前第N层 -> 撤退
					//      寻找 {撤退按钮} -> 点击 {撤退} -> 反馈 {撤退窗口}
					//      寻找 {撤退确认按钮} -> 点击 {确认} -> 反馈消失
					click(mu, PCR["地下城:撤退"], 0.8, {1, true}, {PCR["地下城:撤退窗口"], 1000}, {});
					if (exist(mu, PCR["地下城:撤退窗口"])) {
						click(mu, PCR["地下城:撤退确认"], 0.8, {1, true}, {}, {MT_SELF, 0});
						if (exist(mu, PCR["地下城:次数1/1"], 0.9)) {
						
						} else if (exist(mu, PCR["地下城:次数0/1"])) {
							l.pop();
						}
					}
				}
				// PASS 完成返回
				while (l.isFlag(&flag["结束"])) {
					Logger.info(fformat("{0} 完成于[{1}]", Title, l.getNote()));
					l.pop();
					return true;
				}
			}
			
			return false;
		}
	}
	
	namespace AutoNextLevel {
		bool init = false;
		const uint16 base = 0x00FE;
		const string name = "自动下一关";
		map<const string, flag::FLAG> flag;
		
		vector<string> flagMake{
			"选择关卡", "打开关卡", "关卡返回", "刷图结束",
		};
		
		bool MAIN(platform::TypeMEMU &mu, flag::FLAG *go = &nullFlag) {
			if (!init) init = INIT(base, name, flagMake, flag);
			using namespace basic;
			
			const string Title = "关卡";
			if (go == &nullFlag) l.push(&flag["选择关卡"]);
			else l.push(go);
			
			while (l.isBase(base)) {
				// PASS 关卡列表 下一关
				while (l.isFlag(&flag["选择关卡"])) {
					
					
					// Note 寻找 {下一关标志} -> 点击 {下一关-偏移} -> 反馈 {关卡挑战按钮}
					click(mu, PCR["战斗:下一关"], 0.975, {2, true},
						  {}, {MT_SELF, 500});
					
					// Note 校验 出现 {关卡挑战按钮} 跳转 -> 关卡挑战
					if (exist(mu, PCR["战斗:挑战按钮"])) {
						l.move(&flag["打开关卡"]);
						break;
					} else {
						click(mu, PCR["通告:关闭按钮"], 0.9, {1, true},
							  {}, {MT_SELF, 100});
						click(mu, PCR["战斗:商店取消"], 0.9, {1, true},
							  {}, {MT_SELF, 100});
					}
					
					// Note 寻找 {下一关备用标志} -> 点击 {备用偏移}
					if (not exist(mu, PCR["战斗:下一关"], 0.975)) {
						cv::Mat mat = mu.CaptureRender();
						vec_int32 xy = match::matchtl(mu.screen, PCR["战斗:下一关备用"], 0.9);
						if (xy.size() == 2) {
							cv::Rect crop(xy[0] - 50, xy[1] + 60, 160, 90);
							mat = mat(crop);
							if (not match::matchtl(mat, PCR["战斗:通关星"], 0.9).empty()) {
								Logger.critical(fformat("{0} : 无(未达成一星/两星/三星)未通过的关卡 终止", name));
								l.move(&flag["刷图结束"]);
							} else {
								click(mu, PCR["战斗:下一关备用"], 0.975, {1, true},
									  {PCR["战斗:挑战按钮"], 1000}, {});
							}
						}
					}
					
					// Note 寻找 {剧情弹窗标志} -> 点击 {取消/关闭} -> 反馈 {自己消失}
					if ((not exist(mu, PCR["战斗:下一关"]) and (not exist(mu, PCR["战斗:挑战按钮"])))) {
						click(mu, PCR["战斗:挑战取消"], 0.9, {1, true},
							  {}, {MT_SELF, 1000});
						click(mu, PCR["任务:关闭"], 0.9, {1, true},
							  {}, {MT_SELF, 1000});
					}
				}
				// PASS 关卡挑战
				while (l.isFlag(&flag["打开关卡"])) {
					// Note 开始挑战 等待结束 -> 关卡返回
					if (JoinLevelNull::MAIN(mu)) {
						l.move(&flag["关卡返回"]);
						break;
					}
				}
				// PASS 关卡返回
				while (l.isFlag(&flag["关卡返回"])) {
					// Note 寻找 {普通难度标志} -> 关卡列表 下一关
					if (exist(mu, PCR["主线:普通"])) {
						l.move(&flag["选择关卡"]);
						break;
					}
				}
				// PASS 终止
				while (l.isFlag(&flag["刷图结束"])) {
					Logger.info(fformat("{0} 完成于[{1}]", Title, l.getNote()));
					l.pop();
					return true;
				}
			}
			return false;
		}
	}
	
	bool MAIN(int argc, char *argv[]) {
		
		platform::TypeMEMU memu("127.0.0.1:21503", "VM", "/dev/input/event6", {1278, 718});
		
		for (int i = 1; i < argc; i++) {
			if (*argv[i] == 'C') {
				CHAR sysTimeStr[13] = {};
				SYSTEMTIME systemTime;
				GetLocalTime(&systemTime);
				sprintf_s(sysTimeStr,
						  "%u%02u%02u-%03u",
						  systemTime.wHour,
						  systemTime.wMinute,
						  systemTime.wSecond,
						  systemTime.wMilliseconds);
				memu.CaptureRender();
				cv::imwrite(fformat("debug/{0:%Y%m%d}-{1}.tif", std::chrono::system_clock::now(), sysTimeStr), memu.screen);
				return true;
			}
		}
		
		AppPCR::READ("PCR1280720.xml");
		flag::FLAG LOAD(1, 1, "AppPCR");
		l.root(&LOAD);
		
		if (argc) {
			for (int i = 1; i < argc; i++) {
				
				cout << "[- --:--:-- ------] Input:" << *argv[i];
				if (*argv[i] == 'D') {
					cout << "\033[92m 地下城 \033[0m" << endl;
					AutoDungeon::MAIN(memu);
				} else if (*argv[i] == 'N') {
					cout << "\033[92m 刷图 \033[0m" << endl;
					AutoNextLevel::MAIN(memu);
				} else if (*argv[i] == 'S') {
					cout << "\033[92m 剧情 \033[0m" << endl;
					AutoStory::MAIN(memu);
				} else {
					cout << "\033[91m 未知输入 \033[0m" << endl;
				}
			}
		}
		
		//AutoXXX::MAIN(memu);
		//AutoDungeon::MAIN(memu);
		//AutoNextLevel::MAIN(memu);
		
		return false;
	}
}
