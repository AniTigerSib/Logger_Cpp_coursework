#ifndef _LOGGER_HPP_
#define _LOGGER_HPP_
#include <iostream>
#include <cstddef>
#include <sstream>
#include <fstream>
#include <string>
//#include <thread>
//#include <mutex>

namespace errors {
    class LoggerException : public std::exception {
    public:
        virtual const char* what() const throw() {
            return "Logger Exception";
        }
    };

    class InvalidLogLevelIndex : public LoggerException {
    public:
        virtual const char* what() const throw() {
            return "Invalid log level index";
        }
    };

    class InvalidLogLevel : public LoggerException {
    public:
        virtual const char* what() const throw() {
            return "Invalid log level";
        }
    };

    class InvalidLogPath : public LoggerException {
    public:
        virtual const char* what() const throw() {
            return "Invalid log path";
        }
    };
}

namespace logger {
    using namespace std;

    enum LogLevel
    {
        INVALID = 0, EMERGENCY, ALERT, CRITICAL, ERROR, WARNING, NOTICE, INFO, DEBUG
    };

    struct LogConfig
    {
        bool is_logging_to_file;
        string path_to_log_file;
        LogLevel current_log_level;
        bool is_date_logging;
        bool is_time_logging;
        size_t file_size_limit;
        bool is_file_needed_to_archivate;
    };

    class Logger
    {
    public:
        Logger(const string& config);
        ~Logger();

        void Log(LogLevel level, const string& message);

        template <typename T>
        Logger& operator<<(const T& value) {
            ostringstream oss;
            oss << value;
            Log(current_config_.current_log_level, oss.str());
            return *this;
        }

    private:
        LogLevel& SetLogLevel(LogLevel& current_level, const int i);
        void Configure(LogConfig& temp_config, const string& config);
        void ConfigurationSetDefault(LogConfig& temp_config);
        LogConfig& ConfigurationCheck(LogConfig& temp_config);

        ostream& GetOutputStream();

        string GetTimestamp();
        string GetLogLevelString(LogLevel& level);

        ofstream file_stream_;
        unsigned int file_number_;
        LogConfig current_config_;
    };
}
#endif
