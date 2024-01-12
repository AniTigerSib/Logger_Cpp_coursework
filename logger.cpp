#include "logger.hpp"
#include <chrono>
#include <exception>
#include <iomanip>

using namespace logger;

Logger::Logger(const string& config) {
    LogConfig temp_config;
    ConfigurationSetDefault(temp_config);
    Configure(temp_config, config);
    current_config_ = ConfigurationCheck(temp_config);
    is_logger_running = true;
    thread t([this] {
        LoggerThread();
    });
    t.detach();
}

Logger::~Logger() {
    while (!messages_.empty()) {
        this_thread::sleep_for(chrono::seconds(3));
    }

    is_logger_running = false;
    condition_variable_.notify_all();
    while (!is_logger_closed) {
        cout << "Checked" << endl;
        this_thread::sleep_for(chrono::seconds(1));
    }
    if (file_stream_.is_open())
        file_stream_.close();
    cout << "Closed!" << endl;
}

void Logger::Log(Message message) {
    lock_guard<mutex> lock(mutex_);
    // cout << "Pushed" << endl;
    messages_.push(message);

    condition_variable_.notify_one();
}

void Logger::LoggerThread() {
    cout << "Logger started in thread: " << this_thread::get_id() << endl;
    while (true) {
        // chrono::steady_clock::time_point deadline = 
        //     chrono::steady_clock::now() + std::chrono::seconds(10); как вариант через 10 секунд без сообщений гасить поток

        unique_lock<mutex> lock(mutex_);
        // condition_variable_.wait_until(lock, deadline, [this] { return !messages_.empty() || !is_logger_running; });
        condition_variable_.wait(lock, [this] { return !messages_.empty() || !is_logger_running; });

        if (!is_logger_running) {
            break;
        }

        Message message = messages_.front();
        messages_.pop();

        Log2(message.level, message.message);
    }
    cout << "Closed from func" << endl;
    is_logger_closed = true;
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
            try {
                SetLogLevel(temp_config.current_log_level, stoi(value)); 
            } catch (exception& e) {
                cout << e.what() << ", current level is now EMERGENCY\n";
                temp_config.current_log_level = EMERGENCY;
            }
        } else if (key == "date") {
            temp_config.is_date_logging = true;
        } else if (key == "time") {
            temp_config.is_time_logging = true;
        } else if (key == "trunc") {
            if (!temp_config.is_logging_to_file) {
                cout << "Logging into iostream, size control will be ignored!" << endl;
                continue;
            }
            size_t size = stoi(value);
            size_t unit_pos = value.find_last_not_of("0123456789");
            string unit = "";
            if (unit_pos <= value.size())
                unit = value.substr(unit_pos);

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
                continue;
            }
            temp_config.is_file_needed_to_archivate = true;
        }
    }
}

void Logger::ConfigurationSetDefault(LogConfig& temp_config) {
    temp_config.current_log_level = static_cast<LogLevel>(0);
    temp_config.file_size_limit = 0;
    temp_config.is_file_needed_to_archivate = false;
    temp_config.is_logging_to_file = false;
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

void Logger::Log2(LogLevel level, const string& message) {
    if (level <= current_config_.current_log_level) {
        try {
            ostream& output_stream = GetOutputStream();
            output_stream << GetTimestamp() << "[" << GetLogLevelString(level) << "] " << message << endl;
    
            if (output_stream.fail()) {
                throw errors::StreamWorkFailed();
            }

            if (current_config_.file_size_limit > 0 && (long unsigned int) file_stream_.tellp() >= current_config_.file_size_limit) {
                ChangingLogFile();
            }
        } catch (exception& e) {
            cout << "Logger error occured: " << e.what() << endl;
            return;
        }
    }
}

ostream& Logger::GetOutputStream() {
    if (file_stream_.is_open()) {
        return file_stream_;
    } else if (current_config_.is_logging_to_file) {
        string current_log_file_name;
        if (!current_config_.is_file_needed_to_archivate)
            current_log_file_name = GetFilename();
        else
            current_log_file_name = current_config_.path_to_log_file;
        file_stream_.open(current_log_file_name, ios::out | ios::app);
        if (!file_stream_.is_open()) {
            throw errors::StreamNotOpened();
        }
        file_number_++;
        return file_stream_;
    } else {
        return cout;
    }
}

string Logger::GetFilename() {
    size_t unit_pos = current_config_.path_to_log_file.find_last_of(".");
    string unit = current_config_.path_to_log_file.substr(0, unit_pos);
    unit = unit + "_" + to_string(file_number_) + 
        current_config_.path_to_log_file.substr(unit_pos, sizeof(current_config_.path_to_log_file) - unit_pos - 1);
    return unit;
}

const char* Logger::GetZipName(string& base_filename) {
    size_t unit_pos = current_config_.path_to_log_file.find_last_of(".");
    base_filename = current_config_.path_to_log_file.substr(0, unit_pos);
    base_filename = base_filename + "_" + to_string(file_number_) + ".gz";
    return base_filename.c_str();
}

unsigned long Logger::file_size(const char *filename)
{
    FILE *pFile = fopen(filename, "rb");
    fseek (pFile, 0, SEEK_END);
    unsigned long size = ftell(pFile);
    fclose (pFile);
    return size;
}

int Logger::compress_one_file(const char *infilename, const char *outfilename)
{
    FILE *infile = fopen(infilename, "rb");
    gzFile outfile = gzopen(outfilename, "wb");
    if (!infile || !outfile) return -1;
    char inbuffer[128];
    int num_read = 0;
    unsigned long total_read = 0;
    while ((num_read = fread(inbuffer, 1, sizeof(inbuffer), infile)) > 0) {
        total_read += num_read;
        gzwrite(outfile, inbuffer, num_read);
    }
    fclose(infile);
    gzclose(outfile);

    printf("Read %ld bytes, Wrote %ld bytes, Compression factor %4.2f%%n\n", 
        total_read, file_size(outfilename),
        (1.0-file_size(outfilename)*1.0/total_read)*100.0);
    return 0;
}

void Logger::ChangingLogFile() {
    file_stream_.close();
    
    if (current_config_.is_file_needed_to_archivate) {
        const char* filename = current_config_.path_to_log_file.c_str();
        string base_filename;
        const char* zipname = GetZipName(base_filename);
        if (compress_one_file(filename, zipname))
            throw errors::InvalidLogOrZipFilename();
        if (remove(filename)) {

        }
    }
}

string Logger::GetTimestamp() {
    auto now = chrono::system_clock::time_point(chrono::system_clock::now());
    auto time_t = chrono::system_clock::to_time_t(now);
    auto duration = now.time_since_epoch();
    ostringstream oss;
    if (current_config_.is_date_logging) {
        oss << put_time(localtime(&time_t), "%Y-%m-%d ");
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
        oss << hours.count() << ":"
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

void Logger::Emergency(const string& message) {
    Message mes = {EMERGENCY, message};
    Log(mes);
    return;
}
void Logger::Alert(const string& message) {
    Message mes = {ALERT, message};
    Log(mes);
    return;
}
void Logger::Critical(const string& message) {
    Message mes = {CRITICAL, message};
    Log(mes);
    return;
}
void Logger::Error(const string& message) {
    Message mes = {ERROR, message};
    Log(mes);
    return;
}
void Logger::Warning(const string& message) {
    Message mes = {WARNING, message};
    Log(mes);
    return;
}
void Logger::Notice(const string& message) {
    Message mes = {NOTICE, message};
    Log(mes);
    return;
}
void Logger::Info(const string& message) {
    Message mes = {INFO, message};
    Log(mes);
    return;
}
void Logger::Debug(const string& message) {
    Message mes = {DEBUG, message};
    Log(mes);
    return;
}