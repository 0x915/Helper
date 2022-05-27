#ifndef HELPER_DEFINE_HPP
#define HELPER_DEFINE_HPP

#define fformat fmt::format

#define nullzero 0
#define sleep_hour(t) std::this_thread::sleep_for(std::chrono::hours(t))
#define sleep_min(t) std::this_thread::sleep_for(std::chrono::minutes(t))
#define sleep_sec(t) std::this_thread::sleep_for(std::chrono::seconds(t))
#define sleep_ms(t) std::this_thread::sleep_for(std::chrono::milliseconds(t))
#define sleep_us(t) std::this_thread::sleep_for(std::chrono::microseconds(t))
#define sleep_ns(t) std::this_thread::sleep_for(std::chrono::nanoseconds(t))

#define time_now std::chrono::system_clock::now()
#define clock_now std::chrono::steady_clock::now()
#define time_type std::chrono::time_point<std::chrono::system_clock>
#define clock_type std::chrono::time_point<std::chrono::steady_clock>

#define timeout_ms(t) (difftime_ms(t - clock_now) < chrono::milliseconds(0))
#define timeout_us(t) (difftime_ms(t - clock_now) < chrono::microseconds(0))

#define difftime_hour std::chrono::duration_cast<std::chrono::hours>
#define difftime_min std::chrono::duration_cast<std::chrono::minutes>
#define difftime_sec std::chrono::duration_cast<std::chrono::seconds>
#define difftime_ms std::chrono::duration_cast<std::chrono::milliseconds>
#define difftime_us std::chrono::duration_cast<std::chrono::microseconds>
#define difftime_ns std::chrono::duration_cast<std::chrono::nanoseconds>

#define LOG_COUNT_CLOCK_MS(start) Logger.trace(fformat("finish {0} ms",difftime_ms(clock_now - (start)).count()))
#define LOG_COUNT_CLOCK_US(start) Logger.trace(fformat("finish {0} us",difftime_us(clock_now - (start)).count()))
#define LOG_COUNT_CLOCK_NS(start) Logger.trace(fformat("finish {0} ns",difftime_ns(clock_now - (start)).count()))

#endif //HELPER_DEFINE_HPP
