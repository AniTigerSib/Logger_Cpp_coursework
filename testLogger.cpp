#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <thread>

enum LogLevel { EMERGENCY = 0, ALERT, CRITICAL, ERROR, WARNING, NOTICE, INFO, DEBUG };

class Logger {
public:
    Logger(const std::string& config);
    ~Logger();

    void log(LogLevel level, const std::string& message);

    template <typename T>
    Logger& operator<<(const T& value) {
        std::ostringstream oss;
        oss << value;
        log(currentLogLevel, oss.str());
        return *this;
    }

private:
    void configure(const std::string& config);

    std::ostream& getOutputStream();
    void rotateLogFile();

    std::string getTimestamp();
    std::string getLogLevelString(LogLevel level);

    std::ofstream fileStream;
    LogLevel currentLogLevel;
    bool includeDate;
    size_t bufferSize;
    size_t fileSizeLimit;
    bool archiveFiles;
    std::mutex mutex;
};

Logger::Logger(const std::string& config) {
    configure(config);
}

Logger::~Logger() {
    if (fileStream.is_open()) {
        fileStream.close();
    }
}

void Logger::configure(const std::string& config) {
    // Parse configuration string and set parameters accordingly
    // Example: file=/tmp/test.log,lev=6,date,trunc=10M
}

void Logger::log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex);

    if (level <= currentLogLevel) {
        std::ostream& outputStream = getOutputStream();
        outputStream << getTimestamp() << " [" << getLogLevelString(level) << "] " << message << std::endl;

        if (outputStream.fail()) {
            // Handle failure to write to the output stream
        }

        if (fileSizeLimit > 0 && fileStream.tellp() >= fileSizeLimit) {
            rotateLogFile();
        }
    }
}

std::ostream& Logger::getOutputStream() {
    if (fileStream.is_open()) {
        return fileStream;
    } else {
        return std::cout; // Default to standard output if fileStream is not open
    }
}

void Logger::rotateLogFile() {
    if (archiveFiles) {
        // Implement file archiving logic
    }

    fileStream.close();
    fileStream.open("new_log_file.txt", std::ios::out | std::ios::app);

    if (!fileStream.is_open()) {
        // Handle failure to open the new log file
    }
}

std::string Logger::getTimestamp() {
    if (includeDate) {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return oss.str();
    } else {
        return "";
    }
}

std::string Logger::getLogLevelString(LogLevel level) {
    static const char* levelStrings[] = {"EMERGENCY", "ALERT", "CRITICAL", "ERROR", "WARNING", "NOTICE", "INFO", "DEBUG"};
    return levelStrings[level];
}

int main() {
    Logger logger("file=/tmp/test.log,lev=6,date,trunc=10M");
    logger.log(DEBUG, "This is a debug message");
    logger.log(INFO, "This is an info message");

    // Example of using the operator<< overloading
    logger << WARNING << "This is a warning message using operator<<";

    return 0;
}
