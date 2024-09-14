#include "RawCanToProtoMsg.h"
#include "ProtoCollection.h"
#include "DBCCollection.h"
#include "MyLog.h"
#include <fmt/color.h>
#include <fmt/chrono.h>


void mcl::RawCanToProtoMsg::create_signal(   const google::protobuf::Descriptor *desc, 
                                    std::shared_ptr<google::protobuf::Message> msg, 
                                    const dbcppp::ISignal* sig,
                                    const uint8_t *buffer)
{
    const google::protobuf::FieldDescriptor* field_desc = desc->FindFieldByName(sig->Name());
    if(!field_desc) {
        mcl::log_error( "Cant find field by name",
                        {{"Name", sig->Name()}});
        std::exit(EXIT_FAILURE);
    }  


    switch(sig->ExtendedValueType()) {
        case dbcppp::ISignal::EExtendedValueType::Integer :
            if (sig->ValueType() == dbcppp::ISignal::EValueType::Signed) {
                msg->GetReflection()->SetInt64(msg.get(), field_desc, static_cast<int64_t>(sig->RawToPhys(sig->Decode(buffer))));
            } else {
                msg->GetReflection()->SetUInt64(msg.get(), field_desc, static_cast<uint64_t>(sig->RawToPhys(sig->Decode(buffer))));
            }
            break;

        case dbcppp::ISignal::EExtendedValueType::Double :
            
            break;

        case dbcppp::ISignal::EExtendedValueType::Float :
            msg->GetReflection()->SetFloat(msg.get(), field_desc, static_cast<float>(sig->RawToPhys(sig->Decode(buffer))));
            break;

        default: break;
    }

    
}


mcl::proto_msg_t mcl::RawCanToProtoMsg::decode_msg(   const uint8_t *buffer, 
                                uint32_t id, 
                                mcl::ProtoCollection& my_proto,
                                mcl::DBCCollection& dbc_col)
{
       
    auto imsg = dbc_col.get_imessage(id);

    if(!imsg) {
        return {};
    }

    const dbcppp::IMessage* msg = imsg;
    const dbcppp::ISignal* mux_sig = msg->MuxSignal();
    const google::protobuf::Descriptor* desc;
    
    int64_t mux_val = -1;

    if(mux_sig) {
        // TODO: Multiplex Nachrichten Handling 
        return {};
        mux_val = mux_sig->MultiplexerSwitchValue();
    }

    
    desc = my_proto.get_descriptor(id, mux_val);
    if(!desc) {
        return {};
    }


    std::shared_ptr<google::protobuf::Message> new_msg(_factory->GetPrototype(desc)->New()); 

 
    for (const auto& sig : msg->Signals()) {
        if( sig.MultiplexerIndicator() != dbcppp::ISignal::EMultiplexer::MuxValue || 
            (mux_sig && mux_sig->Decode(buffer) == sig.MultiplexerSwitchValue())) 
        {
            create_signal(  desc,
                            new_msg,
                            &sig,
                            buffer);   
        }
    }



    return new_msg;
}