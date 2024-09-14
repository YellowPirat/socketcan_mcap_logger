#ifndef _UDPPUB_H_
#define _UDPPUB_H_

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/tokenizer.h>
#include <google/protobuf/compiler/parser.h>
#include <memory>
#include <string>

using boost::asio::ip::udp;
using boost::asio::ip::address;



namespace mcl {
    class UdpPub {
        public:
            UdpPub(boost::asio::io_service& io_service) : 
                _socket(io_service) {}

            void init_udp_publisher(    const std::string& receive_ip,
                                        uint32_t port);
                      
            void publish_via_udp(   const std::string& msg);

        private:
            
            udp::socket _socket;
            udp::endpoint _endpoint;
    };
}

#endif