#include <memory>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <ctime>

#include "McapCanLogger.h"
#include "MyLog.h"

std::string mcl::McapCanLogger::get_absolut_path(const std::string &path) {
    std::filesystem::path p = path;
    return std::filesystem::absolute(p);
}

std::string getDate() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm = *std::localtime(&now_time);
    std::ostringstream oss;
    oss << std::put_time(&now_tm, "/%Y-%m-%d");
    return oss.str();
}

std::string getHour() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm = *std::localtime(&now_time);
    std::ostringstream oss;
    oss << std::put_time(&now_tm, "_%H_%M_%S");
    return oss.str();
}

std::string getBasePath(std::string path) {
    int last_slash_pos = 0;
    int i = 0;

    for (auto c : path) {
        if (c == '/') {
            last_slash_pos = i;
        }
        i++;
    }

    std::string base_path;
    for (i = 0; i < last_slash_pos; i++) {
        base_path += path[i];
    }

    return base_path;
}

std::string getFileName(std::string path) {
    int last_slash_pos = 0;
    int i = 0;

    for (auto c : path) {
        if (c == '/') {
            last_slash_pos = i;
        }
        i++;
    }

    std::string file_name;
    for (i = last_slash_pos; i < path.size(); i++) {
        file_name += path[i];
    }

    return file_name;
}


std::vector<mcl::CAN_CONFIG> mcl::McapCanLogger::parse_config(YAML::Node node) {
    std::vector<CAN_CONFIG> configs;

    if(node["channels"]) {
        for (const auto& options : node["channels"]) {
            CAN_CONFIG can_config;

            can_config.socket_name = options["socket_name"].as<std::string>();
            can_config.dbc_file_name = get_absolut_path(options["dbc_file_name"].as<std::string>());
            can_config.log_file_name = getBasePath(get_absolut_path(options["log_file_name"].as<std::string>()));
            can_config.log_file_name += getDate();
            can_config.log_file_name += getFileName(get_absolut_path(options["log_file_name"].as<std::string>()));
            can_config.log_file_name += "/";
            can_config.log_file_name += getFileName(get_absolut_path(options["log_file_name"].as<std::string>()));
            can_config.log_file_name += getHour();
            can_config.log_file_name += ".mcap";
            can_config.receive_ip = options["receive_ip"].as<std::string>();
            can_config.receive_port = options["receive_port"].as<uint32_t>();
            can_config.mcap_logging = options["mcap_logging"].as<bool>();
            can_config.udp_publishing = options["udp_publishing"].as<bool>();
            can_config.ignore_msgs = options["ignore_msgs"].as<std::vector<std::string>>();

            configs.push_back(std::move(can_config));
        }
    }

    return configs;
}

std::vector<mcl::CAN_CONFIG> mcl::McapCanLogger::get_configs(std::string config_file_name) {
    YAML::Node config;

    try {
        config = YAML::LoadFile(config_file_name);
    } catch(const YAML::ParserException& ex) {
        mcl::log_error( "Yaml Import Fail", 
                        {   {"Filename", config_file_name},
                            {"Exception msg", ex.what()}});
        std::exit(EXIT_FAILURE);
    }

    return parse_config(config);
}

void mcl::McapCanLogger::create_channels(  std::vector<mcl::CAN_CONFIG> &configs) 
{
    for (const auto &channel : configs) {
        auto tmp_channel = std::make_unique<mcl::CanLoggingChannel>(_io_service);
        tmp_channel->init_channel(channel);
        _channels.push_back(std::move(tmp_channel));
    }
}

void mcl::McapCanLogger::start_asyn_loop() {
    boost::asio::signal_set signals(_io_service, SIGINT, SIGTERM);
    signals.async_wait([&](const boost::system::error_code&, int) {

        mcl::log_msg(   "Signal received, stopping io_service.");
        _io_service.stop();
    });

    _io_service.run();
}

void mcl::McapCanLogger::start_logging(const std::string &config_file_name) {


    auto configs = get_configs(config_file_name);

    create_channels(configs);

    start_asyn_loop();


}

void mcl::McapCanLogger::output_proto_file(    const std::string& config_file_name, 
                                        const std::string& proto_output_path) 
{
    if(config_file_name == "empty") {
        mcl::log_error(  "No Config file path is provided",
                        {{"Help", "Use --help to see more"}});
        std::exit(EXIT_FAILURE);
    }

    YAML::Node config = YAML::LoadFile(config_file_name);

    auto configs = parse_config(config);

    for (const auto& l_config : configs) {
        mcl::DBCCollection dbc_collection;
        mcl::ProtoCollection my_proto;
        std::ofstream file(std::string(proto_output_path + l_config.socket_name + ".proto"));

        dbc_collection.create_dbc_collection(l_config.dbc_file_name);
        my_proto.create_desc_map(   dbc_collection.get_dbc_collection(),
                                    l_config.socket_name + ".porto");
        
        file << "syntax = \"proto3\";" << std::endl << std::endl;
        file << my_proto.get_debug_string() << std::endl;
    }


}

void mcl::McapCanLogger::start(    const std::string &config_file_name,
                            const std::string &proto_output_path) {

    if(config_file_name == "empty") {
        mcl::log_error(  "No Config file path is provided",
                        {{"Help", "Use --help to see more"}});
        std::exit(EXIT_FAILURE);
    }

    if(proto_output_path == "empty") {
        start_logging(config_file_name);
    } else {
        output_proto_file(  config_file_name,
                            proto_output_path);
    }
}