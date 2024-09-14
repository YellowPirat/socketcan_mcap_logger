#ifndef _MCAP_LOGGER_H
#define _MCAP_LOGGER_H

#include <unordered_map>

#include "mcap/writer.hpp"

#include "ProtoCollection.h"

namespace mcl {
    class McapWriter {

        public:
            void init_mcap_logger(  const std::unordered_map<uint32_t, mydesc_t>& desc_map,
                                    const std::string& socket_name, 
                                    const std::string& mcap_file_name);


            void write_msg(std::shared_ptr<google::protobuf::Message> proto_msg);

        private:
            
            std::unique_ptr<mcap::McapWriter> _writer{std::make_unique<mcap::McapWriter>()};

        private:
            std::unordered_map<std::string, mcap::ChannelId> _name_channel_map{};

        private:
            mcap::ChannelId create_channel( const std::string& msg_name,
                                            const std::string& channel_name,
                                            const google::protobuf::Descriptor* desc);

            void create_mcap_message(   mcap::Message &mcap_msg, 
                                        const std::string msg_str, 
                                        mcap::ChannelId channel_id);
    };
}

#endif