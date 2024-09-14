#ifndef _MY_LOG_H_
#define _MY_LOG_H_

#define MCL_DEBUG 

#include <vector>
#include <string>

namespace mcl {
    void log_error( const std::string& msg,
                    const std::vector<std::pair<std::string, std::string>>& details);

    void log_error( const std::string& msg);

    void log_fail(  const std::string& msg,
                    const std::vector<std::pair<std::string, std::string>>& details);

    void log_fail(  const std::string& msg);

    void log_msg(   const std::string& msg,
                    const std::vector<std::pair<std::string, std::string>>& details);

    void log_msg(   const std::string& msg);

    void my_print(  const std::string& id,
                    bool prio,
                    std::FILE* stream,
                    const std::string& msg,
                    const std::vector<std::pair<std::string, std::string>>& details);
}

#endif