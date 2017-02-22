//
// Created by lorenzodonini on 09.09.16.
//

#ifndef MOVEII_LOGGER_H
#define MOVEII_LOGGER_H

#include <iostream>
#include <ctime>
#include <string>

#define LOG_LEVEL_OFF 0
#define LOG_LEVEL_CRITICAL 2
#define LOG_LEVEL_ERROR 3
#define LOG_LEVEL_WARNING 4
#define LOG_LEVEL_INFO 6
#define LOG_LEVEL_DEBUG 7

#define LOG_LEVEL 7

extern const bool TIMESTAMP_ENABLED;
extern const std::string SUB_COMPONENT;

inline void internal_log(const std::string &logStr) {
    if (!TIMESTAMP_ENABLED) {
        std::cerr << logStr << std::endl;
        return;
    }
    auto t = std::time(nullptr);
    const auto tm = *std::localtime(&t);

    char timeStr[23];
    std::strftime(timeStr, sizeof(timeStr), "[%d.%m.%Y %H:%M:%S] ", &tm);
    std::cerr << timeStr << logStr << std::endl;
}

#if LOG_LEVEL < LOG_LEVEL_DEBUG
#define LOG_DEBUG
#else
inline void LOG_DEBUG(const std::string &logStr) {
    internal_log("<" + std::to_string(LOG_LEVEL_DEBUG) + ">" + logStr);
}
#endif

#if LOG_LEVEL < LOG_LEVEL_INFO
#define LOG_INFO
#else
inline void LOG_INFO(const std::string &logStr) {
    internal_log("<" + std::to_string(LOG_LEVEL_INFO) + ">" + logStr);
}
#endif

#if LOG_LEVEL < LOG_LEVEL_WARNING
#define LOG_WARN
#else
inline void LOG_WARN(const std::string &logStr) {
    internal_log("<" + std::to_string(LOG_LEVEL_WARNING) + ">" + logStr);
}
#endif

#if LOG_LEVEL < LOG_LEVEL_ERROR
#define LOG_ERROR
#else
inline void LOG_ERROR(int code, const std::string &logStr) {
    internal_log(std::string("<" + std::to_string(LOG_LEVEL_ERROR) + ">"
                         + std::string(SUB_COMPONENT)
                         + "-" + std::to_string(code)
                         + " " + logStr));
}
inline void LOG_ERROR(std::string logStr) {
    internal_log("<" + std::to_string(LOG_LEVEL_ERROR) + ">" + logStr);
}
#endif

#if LOG_LEVEL < LOG_LEVEL_CRITICAL
#define LOG_CRITICAL
#else
inline void LOG_CRITICAL(int code, const std::string &logStr) {
    //Format a custom string
    internal_log(std::string("<" + std::to_string(LOG_LEVEL_CRITICAL) + ">"
                         + std::string(SUB_COMPONENT)
                         + "-" + std::to_string(code)
                         + " " + logStr));
}
inline void LOG_CRITICAL(std::string logStr) {
    internal_log("<" + std::to_string(LOG_LEVEL_CRITICAL) + ">" + logStr);
}
#endif


#endif //MOVEII_LOGGER_H
