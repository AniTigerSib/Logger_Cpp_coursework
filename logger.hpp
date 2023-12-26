#ifndef _LOGGER_HPP_
#define _LOGGER_HPP_
#include <iostream>
#include <sstream>
#include <string>
//#include <thread>
//#include <mutex>

enum LogLevel
{
    EMERGENCY = 0, ALERT, CRITICAL, ERROR, WARNING, NOTICE, INFO, DEBUG
};

struct LogConfig
{
    bool is_logging_to_file;
    std::string path_to_log_file;
    LogLevel current_log_level;
    bool is_date_logging;
    bool is_time_logging;
    size_t buffer_size;

};

class Logger
{
public:
    Logger(const std::string& config);
    ~Logger();


    //parse argv (argc, argv)

    void log(LogLevel level, const std::string& message);

    template <typename T>
    Logger& operator<<(const T& value) {
        std::ostringstream oss;
        oss << value;
        log(current_log_level_, oss.str());
        return *this;
    }

private:
    LogConfig current_config_;
    LogLevel current_log_level_;
    size_t buffer_size_;
    size_t file_size_limit_;
    bool archive_files_;

    void configure(const std::string& config);
};
#endif
