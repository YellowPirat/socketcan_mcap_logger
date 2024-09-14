#define MCAP_IMPLEMENTATION
#include "ProtoCollection.h"

#include <bitset>
#include <algorithm>

#include "BuildFileDescriptorSet.h"
#include "MyLog.h"


std::unordered_map<uint32_t, mcl::mydesc_t>& mcl::ProtoCollection::get_desc_map() {
    return _desc_map;
}


void mcl::ProtoCollection::init_desc_proto_field(   google::protobuf::DescriptorProto *desc_proto, 
                                            const dbcppp::ISignal *sig, 
                                            int count) {

    google::protobuf::FieldDescriptorProto* field_proto = desc_proto->add_field();

    field_proto->set_name(sig->Name());
    field_proto->set_number(count);

    
    
    switch (sig->ExtendedValueType())
    {
        case dbcppp::ISignal::EExtendedValueType::Integer:
            if(sig->ValueType() == dbcppp::ISignal::EValueType::Signed) {
                field_proto->set_type(google::protobuf::FieldDescriptorProto::TYPE_INT64);
            } else {
                field_proto->set_type(google::protobuf::FieldDescriptorProto::TYPE_UINT64);
            }
            break;
        
        case dbcppp::ISignal::EExtendedValueType::Double:
            field_proto->set_type(google::protobuf::FieldDescriptorProto::TYPE_DOUBLE);
            break;

        case dbcppp::ISignal::EExtendedValueType::Float:
            field_proto->set_type(google::protobuf::FieldDescriptorProto::TYPE_FLOAT);
            break;

        default:
            mcl::log_error( "Signaltyp is not available",
                            {{"Signalname", sig->Name()}});
            std::exit(EXIT_FAILURE);
            break;
    }
    
}

void mcl::ProtoCollection::create_proto_message(    const std::string &name, 
                                            const mcl::sigcol_t &signals)
{
    google::protobuf::DescriptorProto* desc_proto = _file_desc_proto.add_message_type();
    if(!desc_proto) {
        mcl::log_error( "Can not add Messagetype");
        std::exit(EXIT_FAILURE);
    }

    desc_proto->set_name(name);
    int count = 1;
    for (const auto& sig : signals) {
        init_desc_proto_field(desc_proto, sig, count);
        count++;
    }
}

std::vector<const dbcppp::ISignal*> get_signal_vector(const dbcppp::IMessage* msg) {
    std::vector<const dbcppp::ISignal*> signals;
    for (const auto& sig : msg->Signals()) {
        signals.push_back(&sig);
    }
    return signals;
}

void mcl::ProtoCollection::create_desc_map( const mcl::dbc_collection_t &dbc_collection, 
                                    const std::string &proto_file_name)
{
    _file_desc_proto.set_name(proto_file_name);

    for (const auto&[id, msg] : dbc_collection) {
        const dbcppp::ISignal* mux_sig = msg->MuxSignal();

        if(mux_sig) {
            submsg_t sub_msg = create_mux_signal_def(msg);

            for (const auto&[mux_val, sig_col] : sub_msg) {
                std::string name = std::string(msg->Name() + "_" + std::to_string(mux_val));
                create_proto_message(   name, 
                                        sig_col);
            }
        } else {
            auto sig_col = get_signal_vector(msg);
            create_proto_message(   msg->Name(),
                                    sig_col);
        }
        
    }

    const google::protobuf::FileDescriptor* file_desc = _pool.BuildFile(_file_desc_proto);
    if(!file_desc) {
        mcl::log_error( "Can not get file descriptor from file descriptor proto",
                        {{"Name", _file_desc_proto.GetMetadata().descriptor->full_name()}});
        std::exit(EXIT_FAILURE);
    }

    for (const auto&[id, msg] : dbc_collection) {
        const dbcppp::ISignal* mux_sig = msg->MuxSignal();

        if(mux_sig) {
            submsg_t sub_msg = create_mux_signal_def(msg);
            
            mydesc_t mydesc;
            
            for (const auto&[mux_val, sig_col] : sub_msg) {
                std::string msg_name = std::string(msg->Name() + "_" + std::to_string(mux_val));

                const google::protobuf::Descriptor* desc = file_desc->FindMessageTypeByName(msg_name);

                mydesc.emplace(mux_val, desc);
            }

            _desc_map.emplace(id, std::move(mydesc));
        } else {
            
            const google::protobuf::Descriptor* desc = file_desc->FindMessageTypeByName(msg->Name());

            mydesc_t mydesc;

            mydesc.emplace(-1, desc);

            _desc_map.emplace(id, std::move(mydesc));
        }
    }


}


const google::protobuf::Descriptor* mcl::ProtoCollection::get_descriptor(   uint32_t can_id, 
                                                                            int64_t mux_val) 
{

    if(!_desc_map.count(can_id)) {
        return nullptr;
    }

    
    auto mydesc = _desc_map.at(can_id);

    if(!mydesc.count(mux_val)) {
        return nullptr;
    }

    auto desc = mydesc.at(mux_val);

    return desc;
}



mcl::sigcol_t mcl::ProtoCollection::get_fixed_signals(const dbcppp::IMessage *msg) {

    sigcol_t fix_signals;

    for(const auto& sig : msg->Signals()) {
        if( sig.MultiplexerIndicator() == dbcppp::ISignal::EMultiplexer::MuxSwitch || 
            sig.MultiplexerIndicator() == dbcppp::ISignal::EMultiplexer::NoMux) {
                fix_signals.push_back(&sig);
            }
    }

    return fix_signals;
}

mcl::submsg_t mcl::ProtoCollection::get_submsg_definitions(const dbcppp::IMessage *msg) {

    std::map<uint32_t, sigcol_t> submsg;

    for (const auto& sig : msg->Signals()) {
        if (sig.MultiplexerIndicator() == dbcppp::ISignal::EMultiplexer::MuxValue) {
            auto it = submsg.find(sig.MultiplexerSwitchValue());
            if(it == submsg.end()) {
                submsg.emplace(sig.MultiplexerSwitchValue(), sigcol_t());
            }
        }
    }

    return submsg;
}

void mcl::ProtoCollection::insert_fixed_signals(    mcl::submsg_t &submsg, 
                                            sigcol_t &fixed_signals)
{
    for (auto& [muxval, sigvec] : submsg) {
        for (auto& sig : fixed_signals) {
            sigvec.push_back(sig);
        }
    }
}

void mcl::ProtoCollection::insert_muxval_signals(   mcl::submsg_t &submsg, 
                                                    const dbcppp::IMessage *msg) {

    for (const auto& sig : msg->Signals()) {
        if(sig.MultiplexerIndicator() == dbcppp::ISignal::EMultiplexer::MuxValue) {
            auto it = submsg.find(sig.MultiplexerSwitchValue());
            if(it == submsg.end()) {
                mcl::log_error( "Cant find SwitchValue in submsg map",
                                {{"Switchvalue", std::to_string(sig.MultiplexerSwitchValue())}});
                std::exit(EXIT_FAILURE);
            }
            auto& submsg_signals = it->second;
            submsg_signals.push_back(&sig);
        }
    }

}

mcl::submsg_t mcl::ProtoCollection::create_mux_signal_def( const dbcppp::IMessage* msg) 
{

    sigcol_t fixed_signals = get_fixed_signals(msg);
    submsg_t submsg_def = get_submsg_definitions(msg);
    insert_fixed_signals(submsg_def, fixed_signals);
    insert_muxval_signals(submsg_def, msg);

    return submsg_def;
}

std::string mcl::ProtoCollection::get_debug_string() {
    std::stringstream ss;

    for (const auto[id, mydesc] : _desc_map) {
        for (const auto[muxval, desc] : mydesc) {
            ss << desc->DebugString() << std::endl;
        }
    }

    return ss.str();
}