#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <string>
#include <yaml-cpp/yaml.h>
#include <vector>
#include "CanLoggingChannel.h"


namespace mcl {
    struct CAN_CONFIG {
        std::string socket_name{};
        std::string dbc_file_name{};
        std::string log_file_name{};
        std::string receive_ip{};
        uint32_t receive_port;
        bool mcap_logging;
        bool udp_publishing;
        std::vector<std::string> ignore_msgs{};
    };

    
    class McapCanLogger {
        public:

            void start( const std::string& config_file_name, 
                        const std::string& proto_output_path);

        private:
            
            std::vector<CAN_CONFIG> parse_config(YAML::Node node);

        private:
            void start_logging( const std::string& config_file_name);
            void output_proto_file( const std::string& config_file_name, 
                                    const std::string& proto_output_path);

        private:
            std::vector<mcl::CAN_CONFIG> get_configs(const std::string config_file_name);
            void create_channels(   std::vector<mcl::CAN_CONFIG>& configs);
            void start_asyn_loop();
            std::string get_absolut_path(const std::string& path);

        private:
            boost::asio::io_service _io_service{};
            std::vector<std::unique_ptr<mcl::CanLoggingChannel>> _channels{};
    };
}

#endif