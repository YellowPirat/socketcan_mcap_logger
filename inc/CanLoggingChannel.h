#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include <string>
#include <memory>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>


#include "DBCCollection.h"
#include "ProtoCollection.h"
#include "RawCanToProtoMsg.h"
#include "Listener.h"
#include "McapWriter.h"
#include "UdpPub.h"

namespace mcl {
    struct CAN_CONFIG;

    
    class CanLoggingChannel {
        public:
            CanLoggingChannel(boost::asio::io_service& io_service) : 
                _dbc_collection(),
                _my_proto(),
                _raw_can_to_proto_msg(),
                _listener(io_service),
                _mcap_logger(),
                _udp_pub(io_service) {

            }

            void init_channel(const CAN_CONFIG& can_config);

            private:
                mcl::DBCCollection _dbc_collection{};
                mcl::ProtoCollection _my_proto{};
                mcl::RawCanToProtoMsg _raw_can_to_proto_msg{};
                mcl::Listener _listener;
                mcl::McapWriter _mcap_logger{};
                mcl::UdpPub _udp_pub;

                std::shared_ptr<std::string> logged_msg = std::make_shared<std::string>();              
    };
}

#endif