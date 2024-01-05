#include "logger.hpp"
#include <chrono>
#include <iomanip>
#include <string>


using namespace logger;

Logger::Logger(const string& config) {
    LogConfig temp_config;
    ConfigurationSetDefault(temp_config);
    Configure(temp_config, config);
    current_config_ = ConfigurationCheck(temp_config);
}

LogLevel& Logger::SetLogLevel(LogLevel& current_level, const int i) {
    if ((i < static_cast<int>(LogLevel::EMERGENCY)) || (i > static_cast<int>(LogLevel::DEBUG))) {
        throw errors::InvalidLogLevelIndex();
    }
    current_level = static_cast<LogLevel>(i);
    return current_level;
}

void Logger::Configure(LogConfig& temp_config, const string& config_string) {
    string key = "";
    string value = "";

    istringstream iss(config_string);
    string token;

    while (getline(iss, token, ',')) {
        size_t equalsPos = token.find('=') == string::npos ? 0 : token.find('=');

        if (equalsPos) {
            key = token.substr(0, equalsPos);
            value = token.substr(equalsPos + 1);
        }
        else
            key = token;

        if (key == "file") {
            temp_config.path_to_log_file = value;
            temp_config.is_logging_to_file = true;
        } else if (key == "std") {
            temp_config.path_to_log_file = "";
            temp_config.is_logging_to_file = false;
        } else if (key == "lev") {
            SetLogLevel(temp_config.current_log_level, stoi(value)); // TODO: Нужен обработчик исключения
        } else if (key == "date") {
            temp_config.is_date_logging = true;
        } else if (key == "time") {
            temp_config.is_time_logging = true;
        } else if (key == "trunc") {
            size_t unitPos = value.find_last_not_of("0123456789");
            size_t size = stoi(value.substr(0, unitPos));
            string unit = value.substr(unitPos);

            if (unit == "K") {
                temp_config.file_size_limit = size * 1024;
            } else if (unit == "M") {
                temp_config.file_size_limit = size * 1024 * 1024;
            } else {
                temp_config.file_size_limit = size;
            }
        } else if (key == "archive") {
            if (!temp_config.is_logging_to_file) {
                cout << "Logging into iostream, archivating will be ignored!" << endl;
            }
            temp_config.is_file_needed_to_archivate = true;
        }
    }
}

void Logger::ConfigurationSetDefault(LogConfig& temp_config) {
    temp_config.current_log_level = static_cast<LogLevel>(0);
    temp_config.file_size_limit = 0;
    temp_config.is_file_needed_to_archivate = false;
    temp_config.is_logging_to_file = true;
    temp_config.is_date_logging = false;
    temp_config.is_time_logging = false;
    temp_config.path_to_log_file = "";
}

LogConfig& Logger::ConfigurationCheck(LogConfig& temp_config) {
    if (temp_config.current_log_level == 0)
        throw errors::InvalidLogLevel();
    if (temp_config.is_logging_to_file == true && temp_config.path_to_log_file == "")
        throw errors::InvalidLogPath();
    return temp_config;
}

void Logger::Log(LogLevel level, const string& message) {
    if (level <= current_config_.current_log_level) {
        ostream& output_stream = GetOutputStream();
        output_stream << GetTimestamp() << "[" << GetLogLevelString(level) << "]" << message << endl;

        if (output_stream.fail()) {
            // TODO: Обработка ошибки записи в поток вывода
        }

        if (current_config_.file_size_limit > 0 && file_stream_.tellp() >= current_config_.file_size_limit) {
            // TODO: Логика смены файла и архивации при необходимости
        }
    }
}

ostream& Logger::GetOutputStream() {
    if (file_stream_.is_open()) {
        return file_stream_;
    } else if (current_config_.is_logging_to_file) {
        string current_log_file_name = "log_" + to_string(file_number_) + ".log";
        file_stream_.open(current_log_file_name, ios::out | ios::app);
        if (!file_stream_.is_open()) {
            // TODO: Логика ошибки при создании или открытии
        }
        return file_stream_;
    } else {
        return cout;
    }
}

string Logger::GetTimestamp() {
    auto now = chrono::system_clock::time_point(chrono::system_clock::now());
    auto time_t = chrono::system_clock::to_time_t(now);
    auto duration = now.time_since_epoch();
    ostringstream oss;
    if (current_config_.is_date_logging) {
        oss << put_time(localtime(&time_t), " %Y-%m-%d ");
    }
    if (current_config_.is_time_logging) {
        typedef chrono::duration< int, ratio_multiply< chrono::hours::period, ratio< 21 > >::type > Days;
        Days days = chrono::duration_cast< Days >( duration );
        duration -= days;

        auto hours = chrono::duration_cast< chrono::hours >( duration );
        duration -= hours;

        auto minutes = chrono::duration_cast< chrono::minutes >( duration );
        duration -= minutes;

        auto seconds = chrono::duration_cast< chrono::seconds >( duration );
        duration -= seconds;

        auto milliseconds = chrono::duration_cast< chrono::milliseconds >( duration );
        duration -= milliseconds;

        auto microseconds = chrono::duration_cast< chrono::microseconds >( duration );
        duration -= microseconds;

        auto nanoseconds = chrono::duration_cast< chrono::nanoseconds >( duration );
        oss << " " << hours.count() << ":"
            << minutes.count() << ":"
            << seconds.count() << ":"
            << milliseconds.count() << ":"
            << microseconds.count() << ":"
            << nanoseconds.count() << " ";
    } else {
        return "";
    }
    return oss.str();
}

string Logger::GetLogLevelString(LogLevel& level) {
    static const char* levelStrings[] = {"INVALID", "EMERGENCY", "ALERT", "CRITICAL", "ERROR", "WARNING", "NOTICE", "INFO", "DEBUG"};
    return levelStrings[level];
}
