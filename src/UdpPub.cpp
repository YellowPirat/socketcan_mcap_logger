#include "UdpPub.h"


void mcl::UdpPub::init_udp_publisher(   const std::string& receive_ip, 
                                        uint32_t port) 
{
    _endpoint = udp::endpoint(address::from_string(receive_ip), port);
    _socket.open(udp::v4());
}


void mcl::UdpPub::publish_via_udp(const std::string& str) {
    auto ret = _socket.send_to(boost::asio::buffer(str), _endpoint);
}