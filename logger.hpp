//
// Created by qin19 on 2022/5/10.
//

#ifndef HELPER_LOGGER_HPP
#define HELPER_LOGGER_HPP

#include <iostream>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/ansicolor_sink.h"

class UserANSIStyleType {
  public:
	const std::string Reset = "\033[0m";
	
	const std::string usBold = "\033[1m";
	const std::string rmBold = "\033[21m";
	
	const std::string usFaint = "\033[2m";
	const std::string rmFaint = "\033[22m";
	
	const std::string usUnderLine = "\033[4m";
	const std::string rmUnderLine = "\033[24m";
	const std::string UnderLineColor = "\033[58m;2;";
	
	const std::string usBlinkSlow = "\033[5m";
	const std::string usBlinkFast = "\033[6m";
	const std::string rmBlink = "\033[25m";
	
	const std::string usInverte = "\033[7m";
	const std::string RmInverte = "\033[27m";
	
	const std::string usHide = "\033[8m";
	const std::string rmHide = "\033[28m";
	
	const std::string usStrike = "\033[9m";
	const std::string rmStrike = "\033[29m";
	
	const std::string FgBlack = "\033[30m";
	const std::string FgRed = "\033[31m";
	const std::string FgGreen = "\033[32m";
	const std::string FgYellow = "\033[33m";
	const std::string FgBlue = "\033[34m";
	const std::string FgMagenta = "\033[35m";
	const std::string FgCyan = "\033[36m";
	const std::string FgWhite = "\033[37m";
	
	const std::string FgBlackL = "\033[90m";
	const std::string FgRedL = "\033[91m";
	const std::string FgGreenL = "\033[92m";
	const std::string FgYellowL = "\033[93m";
	const std::string FgBlueL = "\033[94m";
	const std::string FgMagentaL = "\033[95m";
	const std::string FgWhiteL = "\033[97m";
	
	const std::string BgBlack = "\033[40m";
	const std::string BgRed = "\033[41m";
	const std::string BgGreen = "\033[42m";
	const std::string BgYellow = "\033[43m";
	const std::string BgBlue = "\033[44m";
	const std::string BgMagenta = "\033[45m";
	const std::string BgCyan = "\033[46m";
	const std::string BgWhite = "\033[47m";
	
	const std::string BgBlackL = "\033[100m";
	const std::string BgRedL = "\033[101m";
	const std::string BgGreenL = "\033[102m";
	const std::string BgYellowL = "\033[103m";
	const std::string BgBlueL = "\033[104m";
	const std::string BgMagentaL = "\033[105m";
	const std::string BgCyanL = "\033[106m";
	const std::string BgWhiteL = "\033[107m";
	
	const std::string FgColor = "\033[38;2;";
	const std::string BgColor = "\033[48;2;";
	
	const std::string FgCyanL = "\033[96m";
};

extern UserANSIStyleType ANSI;

class TypeLogger {
  private:
	spdlog::logger *logger;
  public:
	TypeLogger(bool enableFileSink, bool enableConsoleSink,const std::string& filename="Helper.log");
	void trace(const std::string &str);
	void debug(const std::string &str);
	void info(const std::string &str);
	void warn(const std::string &str);
	void error(const std::string &str);
	void critical(const std::string &str);
	void fatal(const std::string &str);
};

extern TypeLogger Logger;

#endif //HELPER_LOGGER_HPP
