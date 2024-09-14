#include "DBCCollection.h"
#include "dbcppp/Network2Functions.h"
#include "MyLog.h"


using namespace dbcppp::Network2Human;

void mcl::DBCCollection::create_dbc_collection(const std::string &dbc_file_name) {
    mcl::log_msg(   "Create .dbc Network",
                    {{"Filename", dbc_file_name}});

    
    {
        std::ifstream idbc(dbc_file_name);
        if(!idbc) {
            mcl::log_error( "Could not open .dbc file", 
                            {{"Filename", dbc_file_name}});

            std::exit(EXIT_FAILURE);
        }

        _dbc_net = dbcppp::INetwork::LoadDBCFromIs(idbc);
        
        if(!_dbc_net) {
            mcl::log_error( "Could not import .dbc file into dbc-network",
                            {{"Filename", dbc_file_name}});
            std::exit(EXIT_FAILURE);
        }
    }

    for (const dbcppp::IMessage& msg : _dbc_net->Messages()) {
        _dbc_collection.emplace(msg.Id(), &msg);
    }

}



std::unordered_map<uint32_t, const dbcppp::IMessage*>& mcl::DBCCollection::get_dbc_collection() {

    return _dbc_collection;
}

const dbcppp::IMessage* mcl::DBCCollection::get_imessage(uint32_t id) {
    if(_dbc_collection.count(id)) {
        return _dbc_collection.at(id);
    } else {
        return {};
    }
}

bool mcl::DBCCollection::remove_message(const std::string &message_name) {

    for (const auto& [id, msg_ptr] : _dbc_collection) {
        if(message_name == msg_ptr->Name()) {
            _dbc_collection.erase(id);
            return true;
        }
    }

    return false;
}

void mcl::DBCCollection::remove_messages(std::vector<std::string> message_names) {
    for (const auto& msg_name : message_names) {
        bool ret = remove_message(msg_name);
        if(!ret) {
            mcl::log_fail(   "Cant remove message from .dbc collection", 
                            {{"Messagename", msg_name}});


        }
    }
}