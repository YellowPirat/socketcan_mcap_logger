#include "McapCanLogger.h"
#include "gflags/gflags.h"
#include <iostream>
#include "MyLog.h"

#include <filesystem>
#include <fmt/color.h>

DEFINE_string(config_file, "empty", "path to config.yaml file");
DEFINE_string(proto_file, "empty", "path where the output protofile is stored. MCL terminates after outputing the file");



int main(int argc, char *argv[]) {

    gflags::SetUsageMessage("mcap_can_logger");
    gflags::SetVersionString("1.0.0");
    gflags::ParseCommandLineFlags(&argc, &argv, true);


    mcl::log_msg( "Start Mcap-Can-Logger");


    mcl::McapCanLogger mcap_can_logger;
    mcap_can_logger.start(  FLAGS_config_file, 
                            FLAGS_proto_file);

    gflags::ShutDownCommandLineFlags();

    return 0;
}
