#include "MyLog.h"
#include <fmt/color.h>
#include <fmt/chrono.h>

void mcl::log_error(    const std::string &msg, 
                        const std::vector<std::pair<std::string, std::string>> &details)
{
    my_print(   "ERROR",
                true,
                stderr,
                msg,
                details);
}

void mcl::log_fail( const std::string &msg, 
                    const std::vector<std::pair<std::string, std::string>> &details)
{
    my_print(   "FAIL",
                true,
                stdout,
                msg,
                details);
}

void mcl::log_msg(  const std::string &msg, 
                    const std::vector<std::pair<std::string, std::string>> &details) {

    my_print(   "LOG",
                false,
                stdout,
                msg,
                details);
}

void mcl::my_print( const std::string &id,
                    bool prio,
                    std::FILE* stream,
                    const std::string &msg, 
                    const std::vector<std::pair<std::string, std::string>> &details) 
{
    std::time_t t = std::time(nullptr);

    #ifdef MCL_DEBUG
        if (prio) {
            fmt::print(stderr, fg(fmt::color::red), "Type: {} \n", id); 
        } else {
            fmt::print(fg(fmt::color::orange), "Type: {} \n", id);
        }

        fmt::print(fg(fmt::color::blue), "Message: ");
        fmt::print(fg(fmt::color::green), "{}\n", msg);
        fmt::print(fg(fmt::color::blue), "Time: ");
        fmt::print(fg(fmt::color::green), " {:%Y-%m-%d %H:%M:%S}\n", fmt::localtime(t));
        for (const auto& i : details) {
            fmt::print(fg(fmt::color::blue), "{} : ", std::get<0>(i));
            fmt::print(fg(fmt::color::green), "{}\n", std::get<1>(i));
        }   
    #else
        fmt::print("Type: {} \n",id);
        fmt::print("Message: ");
        fmt::print("{}\n", msg);
        fmt::print("Time: ");
        fmt::print(" {:%Y-%m-%d %H:%M:%S}\n", fmt::localtime(t));
        for (const auto& i : details) {
            fmt::print("{} : ", std::get<0>(i));
            fmt::print("{}\n", std::get<1>(i));
        } 
    #endif 

    fmt::print("--------------------------------------------------------\n");
}

void mcl::log_msg(const std::string &msg) {
    my_print(   "LOG",
                    false,
                    stdout,
                    msg,
                    {});
}

void mcl::log_fail(const std::string &msg) {
    my_print(   "FAIL",
                true,
                stdout,
                msg,
                {});
}

void mcl::log_error(const std::string &msg) {
    my_print(   "ERROR",
                true,
                stderr,
                msg,
                {});
}