#include <iostream>
#include <ostream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <string>

using namespace std;

string GetTimestamp() {
    auto now = chrono::system_clock::time_point(chrono::system_clock::now());
    auto time_t = chrono::system_clock::to_time_t(now);
    std::ostringstream oss;
    oss << put_time(localtime(&time_t), "%Y-%m-%d %H:%M:%S.%f") << endl;
    return oss.str();
}

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

LogLevel& SetLogLevel(LogLevel& current_level, const int i) {
    if ((i < static_cast<int>(LogLevel::EMERGENCY)) || (i > static_cast<int>(LogLevel::DEBUG))) {
        throw ERROR;
    }
    current_level = static_cast<LogLevel>(i);
    return current_level;
}

void ConfigurationSetDefault(LogConfig& temp_config) {
    temp_config.current_log_level = static_cast<LogLevel>(0);
    temp_config.file_size_limit = 0;
    temp_config.is_file_needed_to_archivate = false;
    temp_config.is_logging_to_file = true;
    temp_config.is_date_logging = false;
    temp_config.is_time_logging = false;
    temp_config.path_to_log_file = "";
}

void Configure(LogConfig& temp_config, const string& config_string) {
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
            cout << "\n\n" << unit << "\n\n" << size << "\n\n";
            if (unit == "K") {
                temp_config.file_size_limit = size * 1024;
            } else if (unit == "M") {
                temp_config.file_size_limit = size * 1024 * 1024; // Convert to megabytes
            } else {
                temp_config.file_size_limit = size;
            }
        } else if (key == "archive") {
            temp_config.is_file_needed_to_archivate = true;
        }
    }
}



int main() {
    LogConfig config;
    ConfigurationSetDefault(config);
    std::string str = "file=/tmp/test.log,lev=4,date,time,trunc=10M,archive";
    Configure(config, str);
    std::cout << config.is_date_logging << std::endl
        << config.is_logging_to_file << std::endl
        << config.file_size_limit << std::endl
        << config.is_file_needed_to_archivate << std::endl
        << config.path_to_log_file << std::endl
        << config.is_time_logging << std::endl
        << config.current_log_level << std::endl;



    // string current_time = GetTimestamp();
    // cout << current_time;

    // std::chrono::time_point< std::chrono::system_clock > now = std::chrono::system_clock::now();
    // auto duration = now.time_since_epoch();

    // /* UTC: -3:00 = 24 - 3 = 21 */
    // typedef std::chrono::duration< int, std::ratio_multiply< std::chrono::hours::period, std::ratio< 21 > >::type > Days;

    // Days days = std::chrono::duration_cast< Days >( duration );
    // duration -= days;

    // auto hours = std::chrono::duration_cast< std::chrono::hours >( duration );
    // duration -= hours;

    // auto minutes = std::chrono::duration_cast< std::chrono::minutes >( duration );
    // duration -= minutes;

    // auto seconds = std::chrono::duration_cast< std::chrono::seconds >( duration );
    // duration -= seconds;

    // auto milliseconds = std::chrono::duration_cast< std::chrono::milliseconds >( duration );
    // duration -= milliseconds;

    // auto microseconds = std::chrono::duration_cast< std::chrono::microseconds >( duration );
    // duration -= microseconds;

    // auto nanoseconds = std::chrono::duration_cast< std::chrono::nanoseconds >( duration );

    // time_t theTime = time(NULL);
    // struct tm *aTime = localtime(&theTime);

    // std::cout << days.count() << " days since epoch or "
    //     << days.count() / 365.2524 << " years since epoch. The time is now "
    //     << aTime->tm_hour << ":"
    //     << minutes.count() << ":"
    //     << seconds.count() << ":"
    //     << milliseconds.count() << ":"
    //     << microseconds.count() << ":"
    //     << nanoseconds.count() << std::endl;
}
