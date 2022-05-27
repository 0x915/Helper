//
// Created by qin19 on 2022/5/8.
//

#ifndef HELPER_LOGGER_HPP
#define HELPER_LOGGER_HPP

#include <global.hpp>

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/formatting_ostream.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/attributes/value_extraction.hpp>
#include <boost/core/null_deleter.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <trivial.hpp>

#define LogLevel boost::log::trivial::trace
#define LOG(lvl) BOOST_LOG_TRIVIAL(lvl)
#define ConsoleLogTimeMS

namespace Log {
	
	std::string GetSysTime(const char *format) {
		using namespace boost::posix_time;
		using namespace std;
		auto *facet = new time_facet(format);
		stringstream stream;
		stream.imbue(locale(locale::classic(), facet));
		stream << microsec_clock::local_time();
		return stream.str();
	}
	
	void style_fileout(boost::log::record_view const &rec, boost::log::formatting_ostream &strm) {
		strm << boost::log::extract<unsigned int>("LineID", rec) << " ";
		strm << "[" << rec[boost::log::trivial::severity] << "]" << " " << rec[boost::log::expressions::smessage];
	}
	
	void style_console(boost::log::record_view const &rec, boost::log::formatting_ostream &strm) {
		#ifdef ConsoleLogTimeMS
		std::string str = GetSysTime("%H:%M:%s");
		str = str.substr(0, str.length() - 1);
//		std::string str = GetSysTime("%Y%m%d-%H%M%s");
//		str = str.substr(2, str.length() - 5);
		#else
		std::string str = GetSysTime("%H:%M:%S");
		str = str.substr(0, str.length() - 1);
//		std::string str = GetSysTime("%Y%m%d.%H%M%S");
//		str = str.substr(2, str.length() - 2);
		#endif
		strm << "\033[90m" << "[" << str.replace(str.find('.'),1," ") << "]" << " ";
		using namespace boost::posix_time;
		auto severity = rec[boost::log::trivial::severity];
		if (severity) {
			switch (severity.get()) {
				case boost::log::trivial::severity_level::trace:
					strm << "\033[95m";
					break;
				case boost::log::trivial::severity_level::debug:
					strm << "\033[90m";
					break;
				case boost::log::trivial::severity_level::info:
					strm << "\033[92m";
					break;
				case boost::log::trivial::severity_level::warning:
					strm << "\033[93m";
					break;
				case boost::log::trivial::severity_level::error:
					strm << "\033[91m";
					break;
				case boost::log::trivial::severity_level::fatal:
					strm << "\033[94m";
					break;
				case boost::log::trivial::severity_level::extra:
					strm << "\033[96m";
					break;
				default:
					strm << "\033[97m";
					break;
			}
		}
		strm << rec[boost::log::expressions::smessage] << "\033[0m";
	}
	
	void init(bool sinkfile, bool sinkconsole = true) {
		typedef boost::log::sinks::synchronous_sink<boost::log::sinks::text_ostream_backend> text_sink;
		if (sinkfile) {
//			boost::shared_ptr<text_sink> logfile = boost::make_shared<text_sink>();
//			logfile->locked_backend()->add_stream(boost::make_shared<std::ofstream>("sample.note"));
//			logfile->set_formatter(&style_fileout);
//			boost::log::core::get()->add_sink(logfile);
		}
		if (sinkconsole) {
			boost::shared_ptr<text_sink> console = boost::make_shared<text_sink>();
			boost::shared_ptr<std::ostream> stream(&std::clog, boost::null_deleter());
			console->locked_backend()->add_stream(stream);
			console->set_formatter(&style_console);
			boost::log::core::get()->add_sink(console);
		}
		boost::log::core::get()->set_filter(boost::log::trivial::severity >= LogLevel);
		
		//		boost::log::add_common_attributes();
		//		boost::log::sources::severity_logger<boost::log::trivial::severity_level> lg;
	}
}

#endif //HELPER_LOGGER_HPP
