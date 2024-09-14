#ifndef _DECODE_H_
#define _DECODE_H_

#include "DBCCollection.h"
#include "ProtoCollection.h"

#include <optional>


namespace mcl {

    using proto_msg_t = std::optional<std::shared_ptr<google::protobuf::Message>>;

    class RawCanToProtoMsg {

        public:

            
            proto_msg_t decode_msg( const uint8_t *buffer, 
                                    uint32_t id, 
                                    ProtoCollection& my_proto,
                                    DBCCollection& dbc_col);



        private:

            void create_signal(     const google::protobuf::Descriptor *desc, 
                                    std::shared_ptr<google::protobuf::Message> msg, 
                                    const dbcppp::ISignal* sig,
                                    const uint8_t *buffer);

        private: 
            std::unique_ptr<google::protobuf::DynamicMessageFactory> _factory{std::make_unique<google::protobuf::DynamicMessageFactory>()};
    };
}

#endif