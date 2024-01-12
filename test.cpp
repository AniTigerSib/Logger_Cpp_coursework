#include <cstdio>
#include <iostream>
#include <ostream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <string>

using namespace std;

const char* GetZipName(string unit) {
    unit = "Unit.str";
    return unit.c_str();
}


int main() {
    string unit;
    const char* chr = GetZipName(unit);
    printf("%s", chr);
    
    
    // LogConfig config;
    // ConfigurationSetDefault(config);
    // std::string str = "file=/tmp/test.log,lev=4,date,time,trunc=10M,archive";
    // Configure(config, str);
    // std::cout << config.is_date_logging << std::endl
    //     << config.is_logging_to_file << std::endl
    //     << config.file_size_limit << std::endl
    //     << config.is_file_needed_to_archivate << std::endl
    //     << config.path_to_log_file << std::endl
    //     << config.is_time_logging << std::endl
    //     << config.current_log_level << std::endl;
    


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
