
#include <logger.hpp>


UserANSIStyleType ANSI;

TypeLogger::TypeLogger(bool enableFileSink, bool enableConsoleSink, const std::string &filename) {
	try {
		spdlog::sinks_init_list sink_list = {};
		std::shared_ptr<spdlog::sinks::ansicolor_sink<spdlog::details::console_mutex>> console = nullptr;
		std::shared_ptr<spdlog::sinks::basic_file_sink<std::mutex>> logfile = nullptr;
		if (enableConsoleSink) {
			console = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
			console->set_level(spdlog::level::trace);
			console->set_pattern("\033[90m[%L %H:%M:%S %f]\033[0m %^%v%$");
			console->set_color(spdlog::level::trace, ANSI.FgBlueL + ANSI.BgBlack);
			console->set_color(spdlog::level::debug, ANSI.FgBlackL + ANSI.BgBlack);
			console->set_color(spdlog::level::info, ANSI.FgGreenL + ANSI.BgBlack);
			console->set_color(spdlog::level::warn, ANSI.FgYellow + ANSI.BgBlack);
			console->set_color(spdlog::level::err, ANSI.FgBlack + ANSI.BgRedL);
			console->set_color(spdlog::level::critical, ANSI.FgBlack + ANSI.BgCyan);
			console->set_color(spdlog::level::off, ANSI.FgWhiteL + ANSI.BgMagenta);
			sink_list = {console};
		}
		if (enableFileSink) {
			logfile = std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename, true);
			logfile->set_level(spdlog::level::trace);
			logfile->set_pattern("%Y-%m-%d %H:%M:%S.%F %L> %v");
			sink_list = {logfile};
		}
		if (enableConsoleSink && enableFileSink) sink_list = {console, logfile};
		
		logger = new spdlog::logger("multi_sink", sink_list.begin(), sink_list.end());
		logger->set_level(spdlog::level::trace);
		
		spdlog::set_default_logger(std::make_shared<spdlog::logger>("Sink:Console&File", sink_list));
		
		system("cls");
		logger->info(fmt::format("日志初始化成功(Logger{0}{1}{2})",
								 (console != nullptr ? " > Console" : ""),
								 (logfile != nullptr ? (" > " + filename) : ""),
								 ((logfile == nullptr) && (logfile == nullptr) ? " > NULL" : "")
		));
	} catch (const spdlog::spdlog_ex &ex) {
		std::cout << "日志初始化失败 : " << ex.what() << std::endl;
	}
}

void TypeLogger::trace(const std::string &str) {
	(logger)->log(
		spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION},
		spdlog::level::trace, str);
}

void TypeLogger::debug(const std::string &str) {
	(logger)->log(
		spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION},
		spdlog::level::debug, str);
}

void TypeLogger::info(const std::string &str) {
	(logger)->log(
		spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION},
		spdlog::level::info, str);
}

void TypeLogger::warn(const std::string &str) {
	(logger)->log(
		spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION},
		spdlog::level::warn, str);
}

void TypeLogger::error(const std::string &str) {
	(logger)->log(
		spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION},
		spdlog::level::err, " " + str + " ");
}

void TypeLogger::critical(const std::string &str) {
	(logger)->log(
		spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION},
		spdlog::level::critical, " " + str + " ");
}

void TypeLogger::fatal(const std::string &str) {
	(logger)->log(
		spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION},
		spdlog::level::off, " " + str + " ");
}

TypeLogger Logger(false, true);
