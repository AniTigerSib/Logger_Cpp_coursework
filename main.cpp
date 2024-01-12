#include "logger.hpp"

int main(){
    std::string config = "file=/tmp/log.log,lev=6,date,time,trunc=300,archive";
    std::string config2 = "std,lev=10,date,time";
    logger::Logger* logger_main_p = new logger::Logger(config);
    logger::Logger& logger_main = *logger_main_p;
    logger::Logger* logger_sub_p = new logger::Logger(config2);
    logger::Logger& logger_sub = *logger_sub_p;
    std::cout << "Main thread: " << std::this_thread::get_id() << std::endl;
    logger_main.Alert("Halo, world!");
    logger_sub.Alert("Halo, world!");
    logger_main.Alert("Halo, world!");
    logger_sub.Alert("Halo, world!");
    logger_main.Alert("Halo, world!");
    logger_sub.Alert("Halo, world!");
    logger_main.Alert("Halo, world!");
    logger_sub.Alert("Halo, world!");
    std::this_thread::sleep_for(std::chrono::seconds(3));
    delete logger_main_p;
    delete logger_sub_p;
}
