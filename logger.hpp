#ifndef _LOGGER_HPP_
#define _LOGGER_HPP_
#include <atomic>
#include <condition_variable>
#include <iostream>
#include <stdio.h>
#include <zlib.h>
#include <chrono>
#include <exception>
#include <iomanip>
#include <cstddef>
#include <fstream>
#include <string>
#include <thread>
#include <mutex>
#include <queue>

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
            return "Invalid log level index was given";
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

    class StreamNotOpened : public LoggerException {
    public:
        virtual const char* what() const throw() {
            return "Output stream was not opened";
        }
    };

    class StreamWorkFailed : public LoggerException {
    public:
        virtual const char* what() const throw() {
            return "Output stream work failed";
        }
    };

    class InvalidLogOrZipFilename : public LoggerException {
    public:
        virtual const char* what() const throw() {
            return "Invalid log or zip filename was given as local parameter";
        }
    };

    class FileNotDeleted : public LoggerException {
    public:
        virtual const char* what() const throw() {
            return "File was no deleted, recording to stream must be suspended";
        }
    };
}

namespace logger {
    using namespace std;

    enum LogLevel
    {
        INVALID = 0, EMERGENCY, ALERT, CRITICAL, ERROR, WARNING, NOTICE, INFO, DEBUG
    };

    struct Message
    {
        LogLevel level;
        string message;
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

        void Emergency(const string& message);
        void Alert(const string& message);
        void Critical(const string& message);
        void Error(const string& message);
        void Warning(const string& message);
        void Notice(const string& message);
        void Info(const string& message);
        void Debug(const string& message);

    private:
        void Log(Message message);
        void Log2(LogLevel level, const string& message);
        void LoggerThread();

        LogLevel& SetLogLevel(LogLevel& current_level, const int i);
        void Configure(LogConfig& temp_config, const string& config);
        void ConfigurationSetDefault(LogConfig& temp_config);
        LogConfig& ConfigurationCheck(LogConfig& temp_config);

        ostream& GetOutputStream();
        string GetFilename();
        const char* GetZipName(string& base_filename);
        unsigned long file_size(const char *filename);
        int compress_one_file(const char *infilename, const char *outfilename);
        void ChangingLogFile();

        string GetTimestamp();
        string GetLogLevelString(LogLevel& level);

        ofstream file_stream_;
        unsigned int file_number_;
        LogConfig current_config_;
        mutex mutex_;
        condition_variable condition_variable_;
        queue<Message> messages_;
        atomic_bool is_logger_running = false;
        atomic_bool is_logger_closed = false;
    };
}
#endif
