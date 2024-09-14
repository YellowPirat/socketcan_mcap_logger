#ifndef _LISTENER_H_
#define _LISTENER_H_

#include <iostream>
#include <string.h>
#include <linux/can/raw.h>
#include <boost/asio.hpp>
#include <memory>
#include <functional>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <linux/can.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include "RawCanToProtoMsg.h"


namespace mcl {
    class Listener {

        public:

            Listener(boost::asio::io_service& io_service) : 
                _stream_descriptor(io_service) {
            }

            void setup_listener(const std::string& interface_name);

            
            void start_reading();

            void init_handler(std::function<void(const boost::system::error_code&, std::size_t)> handler);

            can_frame& get_record_frame();

            
        private:
            can_frame _record_frame {};
        
            std::function<void(const boost::system::error_code&, std::size_t)> _handler;

            boost::asio::posix::stream_descriptor _stream_descriptor;

            struct sockaddr_can addr{};
            struct ifreq ifr{};

            
    };
}


#endif