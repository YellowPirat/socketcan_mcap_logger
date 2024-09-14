#ifndef _MYPROTO_H_
#define _MYPROTO_H_

#include <dbcppp/Network.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/tokenizer.h>
#include <google/protobuf/compiler/parser.h>
#include <linux/can.h>
#include <unistd.h>
#include <memory>
#include <unordered_map>
#include <map>
#include <string>
#include <tuple>
#include <optional>


#include "mcap/writer.hpp"




namespace mcl {
    typedef std::unordered_map<uint32_t, const dbcppp::IMessage*> dbc_collection_t;
    typedef std::vector<const dbcppp::ISignal*> sigcol_t;
    typedef std::map<uint32_t, sigcol_t> submsg_t;
    typedef std::map<int64_t, const google::protobuf::Descriptor*> mydesc_t;

    
    class ProtoCollection {

        public:
            ProtoCollection() : 
                _file_desc_proto(),
                _pool() {

            }


            void create_desc_map(   const dbc_collection_t& dbc_collection,
                                    const std::string& proto_file_name);

            
            const google::protobuf::Descriptor* get_descriptor( uint32_t can_id,
                                                                int64_t mux_val);

            
            std::unordered_map<uint32_t, mydesc_t>& get_desc_map();

            
            std::string get_debug_string();


        private:
            std::unordered_map<uint32_t, mydesc_t> _desc_map{};


        private:

            void create_proto_message(  const std::string& name, 
                                        const std::vector<const dbcppp::ISignal*>& signals);

            submsg_t create_mux_signal_def( const dbcppp::IMessage* msg);

            sigcol_t get_fixed_signals(const dbcppp::IMessage* msg);

            submsg_t get_submsg_definitions(const dbcppp::IMessage* msg);

            void insert_fixed_signals(  submsg_t& submsg, 
                                        sigcol_t& fixed_signals);

            void insert_muxval_signals( submsg_t& submsg, 
                                        const dbcppp::IMessage* msg);

            void init_desc_proto_field( google::protobuf::DescriptorProto* desc_proto,
                                        const dbcppp::ISignal* sig, 
                                        int count);

        private:

            
            google::protobuf::FileDescriptorProto _file_desc_proto{};
            google::protobuf::DescriptorPool _pool{};
            
    };
}

#endif

