#ifndef _DBC_COLLECTION_
#define _DBC_COLLECTION_

#include <string>
#include <unordered_map>
#include <memory>
#include <dbcppp/Network.h>
#include <fstream>
#include <optional>
#include <vector>


namespace mcl {


    class DBCCollection{

        public:
            void create_dbc_collection(const std::string& dbc_file_name);

            std::unordered_map<uint32_t, const dbcppp::IMessage*>& get_dbc_collection();

            const dbcppp::IMessage* get_imessage(uint32_t id);

            bool remove_message(const std::string& message_name);

            void remove_messages(const std::vector<std::string> message_names);
            
        private:
            std::unique_ptr<dbcppp::INetwork> _dbc_net{};
            std::unordered_map<uint32_t, const dbcppp::IMessage*> _dbc_collection{};

    };
}

#endif