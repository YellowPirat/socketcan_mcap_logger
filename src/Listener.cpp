#include <Listener.h>
#include "MyLog.h"

void mcl::Listener::setup_listener(const std::string &interface_name)
{
    mcl::log_msg(   "Init CAN-Listener Socket",
                    {{"Interfacename", interface_name}});

    int socket_file_descriptor = ::socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (socket_file_descriptor < 0) {
        mcl::log_error( "Could not create socket",
                        {{"Interfacename", interface_name}});
        std::exit(EXIT_FAILURE);
    }


    std::strcpy(ifr.ifr_name, interface_name.c_str());
    if (ioctl(socket_file_descriptor, SIOCGIFINDEX, &ifr) < 0) {
        ::close(socket_file_descriptor);
        mcl::log_error( "Could not setup socket with ioctl",
                        {{"Interfacename", interface_name}});
        std::exit(EXIT_FAILURE);
    }

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (::bind(socket_file_descriptor, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
        ::close(socket_file_descriptor);
        mcl::log_error( "Cannot bind socket",
                        {{"Interfacename", interface_name}});
        std::exit(EXIT_FAILURE);
    }

    _stream_descriptor.assign(socket_file_descriptor);
}

void mcl::Listener::init_handler(std::function<void (const boost::system::error_code &, std::size_t)> handler) {
    _handler = handler;
}

void mcl::Listener::start_reading() {
    _stream_descriptor.async_read_some(    boost::asio::buffer(&_record_frame, sizeof(_record_frame)),
                                            _handler );
}

can_frame &mcl::Listener::get_record_frame() {
    return _record_frame;
}