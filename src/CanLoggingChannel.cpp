#include "CanLoggingChannel.h"
#include "McapCanLogger.h"

#include "MyLog.h"

#include <fmt/color.h>
#include <fmt/chrono.h>

void mcl::CanLoggingChannel::init_channel(const mcl::CAN_CONFIG &can_config)
{


    // init dbc_collection
    _dbc_collection.create_dbc_collection(can_config.dbc_file_name);
    auto dbc_col = _dbc_collection.get_dbc_collection();
    _dbc_collection.remove_messages(can_config.ignore_msgs);

    // init my_proto
    _my_proto.create_desc_map(dbc_col, can_config.log_file_name);

    // init mcap_logger
    if(can_config.mcap_logging) {
        _mcap_logger.init_mcap_logger( _my_proto.get_desc_map(),
                                        can_config.socket_name,
                                        can_config.log_file_name);
    }

    if(can_config.udp_publishing) {
        _udp_pub.init_udp_publisher(    can_config.receive_ip,
                                        can_config.receive_port);
    }

    // init listener
    _listener.setup_listener( can_config.socket_name);



    // create handler routine
    auto handler = [this, can_config](const boost::system::error_code& error, std::size_t length) {

                            if(error || length != sizeof(_listener.get_record_frame())) {
                                mcl::log_fail(  "record frame error",
                                                {   {"boost::error", (error) ? "true" : "false"},
                                                    {"length error", (length != sizeof(_listener.get_record_frame())? "true" : "false")}});
                                return;
                            }

                            auto ret = _raw_can_to_proto_msg.decode_msg(    _listener.get_record_frame().data, 
                                                                            _listener.get_record_frame().can_id, 
                                                                            _my_proto,
                                                                            _dbc_collection);
                                                           
                            if(!ret.has_value()) {
                                mcl::log_fail(  "Failed to convert raw can to proto msg ", 
                                {   {"ID", std::to_string(_listener.get_record_frame().can_id)}});
                                _listener.start_reading(); 
                                return;
                            }

                            auto msg = ret.value();


                            std::string msg_name = msg->GetDescriptor()->name();

                            msg->SerializeToString(this->logged_msg.get());
                            
                            if(can_config.mcap_logging) {
                                _mcap_logger.write_msg(this->logged_msg, msg_name);
                            }
                            
                            _listener.start_reading(); 
                    };
    
    _listener.init_handler(handler);

    _listener.start_reading();

}